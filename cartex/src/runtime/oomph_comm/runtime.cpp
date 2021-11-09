/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <chrono>
#include <algorithm>
#include <iostream>

#include <cartex/runtime/oomph_comm/runtime.hpp>
#include "../runtime_inc.cpp"

namespace cartex
{
options&
runtime::add_options(options& opts)
{
    return opts;
}

bool
runtime::check_options(options_values const&)
{
    return true;
}

runtime::impl::impl(cartex::runtime& base, options_values const& /*options*/)
: m_base{base}
, m_context{base.m_decomposition.mpi_comm(), (base.m_num_threads > 1)}
, m_fields(base.m_num_threads)
{
}

void
runtime::impl::init(int j)
{
    auto& d = m_base.m_domains[j];

    std::size_t const x = d.domain_ext[0];
    std::size_t const y = d.domain_ext[1];
    std::size_t const z = d.domain_ext[2];

    std::size_t const hx_l = m_base.m_halos[0];
    std::size_t const hx_r = m_base.m_halos[1];
    std::size_t const hy_l = m_base.m_halos[2];
    std::size_t const hy_r = m_base.m_halos[3];
    std::size_t const hz_l = m_base.m_halos[4];
    std::size_t const hz_r = m_base.m_halos[5];

    auto const x_l = m_base.m_decomposition.neighbor(j, -1, 0, 0);
    auto const x_r = m_base.m_decomposition.neighbor(j, 1, 0, 0);
    auto const y_l = m_base.m_decomposition.neighbor(j, 0, -1, 0);
    auto const y_r = m_base.m_decomposition.neighbor(j, 0, 1, 0);
    auto const z_l = m_base.m_decomposition.neighbor(j, 0, 0, -1);
    auto const z_r = m_base.m_decomposition.neighbor(j, 0, 0, 1);

    std::size_t const x_ext = x + hx_l + hx_r;
    std::size_t const y_ext = y + hy_l + hy_r;
    std::size_t const z_ext = z + hz_l + hz_r;

    const auto send_x_l_range = tensor::range<3>({hx_l, hy_l, hz_l}, {hx_l, y, z});
    const auto recv_x_r_range = tensor::range<3>({hx_l + x, hy_l, hz_l}, {hx_l, y, z});
    const auto send_x_r_range = tensor::range<3>({hx_l + x - hx_r, hy_l, hz_l}, {hy_r, y, z});
    const auto recv_x_l_range = tensor::range<3>({0, hy_l, hz_l}, {hy_r, y, z});

    const auto send_y_l_range = tensor::range<3>({0, hy_l, hz_l}, {x_ext, hy_l, z});
    const auto recv_y_r_range = tensor::range<3>({0, hy_l + y, hz_l}, {x_ext, hy_l, z});
    const auto send_y_r_range = tensor::range<3>({0, hy_l + y - hy_r, hz_l}, {x_ext, hy_r, z});
    const auto recv_y_l_range = tensor::range<3>({0, 0, hz_l}, {x_ext, hy_r, z});

    const auto send_z_l_range = tensor::range<3>({0, 0, hz_l}, {x_ext, y_ext, hz_l});
    const auto recv_z_r_range = tensor::range<3>({0, 0, hz_l + z}, {x_ext, y_ext, hz_l});
    const auto send_z_r_range = tensor::range<3>({0, 0, hz_l + z - hz_r}, {x_ext, y_ext, hz_r});
    const auto recv_z_l_range = tensor::range<3>({0, 0, 0}, {x_ext, y_ext, hz_r});

    auto send_tag = [&d, n = m_base.m_num_fields](int field_id, int dim, bool left)
    {
        // send tag encodes
        // - field id (which field):       num_fields
        // - send thread id:               num_threads
        // - dimension (x,y,z):            3
        // - left/right communication:     2
        const auto direction_tag = dim * 2 + (left ? 0 : 1);
        const auto thread_tag = d.thread;
        return field_id + n * (direction_tag + 6 * thread_tag);
    };

    auto recv_tag = [&x_l, &x_r, &y_l, &y_r, &z_l, &z_r, n = m_base.m_num_fields](int field_id,
                        int dim, bool left)
    {
        // recv tag encodes
        // - field id (which field):       num_fields
        // - send thread id:               num_threads
        // - dimension (x,y,z):            3
        // - left/right communication:     2
        const auto direction_tag = dim * 2 + (left ? 0 : 1);
        const auto tid_l = (dim == 0 ? x_r.thread : (dim == 1 ? y_r.thread : z_r.thread));
        const auto tid_r = (dim == 0 ? x_l.thread : (dim == 1 ? y_l.thread : z_l.thread));
        const auto thread_tag = left ? tid_l : tid_r;
        return field_id + n * (direction_tag + 6 * thread_tag);
    };

#ifndef CARTEX_OOMPH_NO_BUFFER_REUSE
    tensor::buffer_cache<runtime::real_type> send_cache;
    tensor::buffer_cache<runtime::real_type> recv_cache;
#endif
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        auto m = m_context.map_tensor<layout_t>({x_ext, y_ext, z_ext},
            m_base.m_raw_fields[j][i].hd_data(),
            m_base.m_raw_fields[j][i].hd_data() + (x_ext * y_ext * z_ext - 1));

#ifdef CARTEX_OOMPH_NO_BUFFER_REUSE
        m_fields[j].push_back(field{std::move(m)});
#else
        m_fields[j].push_back(field{std::move(m), send_cache, recv_cache});
#endif

        auto& f = m_fields[j].back();
        f.m_sender.add_dst(send_x_l_range, x_l.rank, send_tag(i, 0, true), 0)
            .add_dst(send_x_r_range, x_r.rank, send_tag(i, 0, false), 0);
        f.m_receiver.add_src(recv_x_r_range, x_r.rank, recv_tag(i, 0, true), 0)
            .add_src(recv_x_l_range, x_l.rank, recv_tag(i, 0, false), 0);

        f.m_sender.add_dst(send_y_l_range, y_l.rank, send_tag(i, 1, true), 1)
            .add_dst(send_y_r_range, y_r.rank, send_tag(i, 1, false), 1);
        f.m_receiver.add_src(recv_y_r_range, y_r.rank, recv_tag(i, 1, true), 1)
            .add_src(recv_y_l_range, y_l.rank, recv_tag(i, 1, false), 1);

        f.m_sender.add_dst(send_z_l_range, z_l.rank, send_tag(i, 2, true), 2)
            .add_dst(send_z_r_range, z_r.rank, send_tag(i, 2, false), 2);
        f.m_receiver.add_src(recv_z_r_range, z_r.rank, recv_tag(i, 2, true), 2)
            .add_src(recv_z_l_range, z_l.rank, recv_tag(i, 2, false), 2);

        f.m_sender.connect();
        f.m_receiver.connect();
    }
}

void
runtime::impl::step(int j)
{
#ifdef CARTEX_OOMPH_NO_BUFFER_REUSE
    for (unsigned int d = 0; d < 3; ++d)
    {
        for (int i = 0; i < m_base.m_num_fields; ++i)
        {
            auto& f = m_fields[j][i];
            f.m_recv_handle = f.m_receiver.recv(d);
        }
        for (int i = 0; i < m_base.m_num_fields; ++i)
        {
            auto& f = m_fields[j][i];
            f.m_sender.pack(d).wait();
            f.m_send_handle = f.m_sender.send(d);
        }
        for (int i = 0; i < m_base.m_num_fields; ++i)
        {
            auto& f = m_fields[j][i];
            auto& rh = f.m_recv_handle;
            auto& sh = f.m_send_handle;
            while (true)
            {
                if (!rh.is_ready()) rh.progress();
                if (!sh.is_ready()) sh.progress();
                if (rh.is_ready()) break;
            }
            f.m_receiver.unpack(d).wait();
            sh.wait();
        }
    }
#else
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        auto& f = m_fields[j][i];

        for (unsigned int d = 0; d < 3; ++d)
        {
            auto rh = f.m_receiver.recv(d);
            f.m_sender.pack(d).wait();
            auto sh = f.m_sender.send(d);
            while (true)
            {
                if (!rh.is_ready()) rh.progress();
                if (!sh.is_ready()) sh.progress();
                if (rh.is_ready() && sh.is_ready()) break;
            }
            f.m_receiver.unpack(d).wait();
        }
    }
#endif
}

void
runtime::impl::exit(int j)
{
    m_fields[j].clear();
}

} // namespace cartex
