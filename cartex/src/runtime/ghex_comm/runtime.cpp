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

#include <cartex/runtime/ghex_comm/runtime.hpp>
#include "../runtime_inc.cpp"

namespace cartex
{
options&
runtime::add_options(options& opts)
{
    return opts("node-local", "use non-compact node-local transport");
}

bool
runtime::check_options(options_values const&)
{
    return true;
}

runtime::impl::impl(cartex::runtime& base, options_values const& options)
: m_base{base}
, m_context{base.m_decomposition.mpi_comm(), (base.m_num_threads > 1)}
, m_halo_gen(std::array<int, 3>{0, 0, 0}, base.m_decomposition.last_domain_coord(), base.m_halos,
      std::array<bool, 3>{true, true, true})
, m_fields(base.m_num_threads)
#ifdef __CUDACC__
, m_fields_gpu(base.m_num_threads)
#endif
#ifdef CARTEX_GHEX_FIELD_BY_FIELD
, m_cos(base.m_num_threads * base.m_num_fields)
#else
, m_cos(base.m_num_threads)
#endif
, m_node_local(options.is_set("node-local"))
{
    m_local_domains.reserve(m_base.m_num_threads);
    for (auto& dom : m_base.m_domains)
    {
        m_local_domains.push_back(domain_descriptor_type{dom.id, dom.domain_coord,
            std::array<int, 3>{dom.domain_coord[0] + dom.domain_ext[0] - 1,
                dom.domain_coord[1] + dom.domain_ext[1] - 1,
                dom.domain_coord[2] + dom.domain_ext[2] - 1}});
    }
    using clock_type = std::chrono::high_resolution_clock;
    const auto start = clock_type::now();
#ifndef CARTEX_GHEX_STAGED
    m_pattern = std::unique_ptr<pattern_type>{
        new pattern_type{make_pattern<structured::grid>(m_context, m_halo_gen, m_local_domains)}};
#else
    m_pattern = structured::regular::make_staged_pattern(
        m_context, m_local_domains,
        [&base](auto id, auto const& offset) {
            const auto n = base.m_decomposition.neighbor(
                std::find_if(base.m_domains.begin(), base.m_domains.end(),
                    [id](auto const& x) { return x.id == id; })
                    ->thread,
                offset[0], offset[1], offset[2]);
            struct _neighbor
            {
                int m_id;
                int m_rank;
                int id() const noexcept { return m_id; }
                int rank() const noexcept { return m_rank; }
            };
            return _neighbor{n.id, n.rank};
        },
        std::array<int, 3>{0, 0, 0}, base.m_decomposition.last_domain_coord(), base.m_halos,
        std::array<bool, 3>{true, true, true});
#endif
    const auto end = clock_type::now();
    if (m_context.rank() == 0)
        std::cout << "setup time: "
                  << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() *
                         1.0e-9)
                  << " s" << std::endl;
}

void
runtime::impl::init(int j)
{
#ifdef __CUDACC__
    int device_id;
    cudaGetDevice(&device_id);
#endif
    const std::array<int, 3> buffer = {
        m_base.m_domains[j].domain_ext[0] + m_base.m_halos[0] + m_base.m_halos[1],
        m_base.m_domains[j].domain_ext[1] + m_base.m_halos[2] + m_base.m_halos[3],
        m_base.m_domains[j].domain_ext[2] + m_base.m_halos[4] + m_base.m_halos[5]};
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_fields[j].push_back(wrap_field<cpu, gridtools::layout_map<2, 1, 0>>(
            m_local_domains[j], m_base.m_raw_fields[j][i].host_data(), m_base.m_offset, buffer));
#ifdef __CUDACC__
        m_fields_gpu[j].push_back(
            wrap_field<gpu, gridtools::layout_map<2, 1, 0>>(m_local_domains[j],
                m_base.m_raw_fields[j][i].device_data(), m_base.m_offset, buffer, device_id));
#endif
    }

    using bco_type = bulk_communication_object<structured::rma_range_generator, pattern_type,
#ifndef __CUDACC__
        field_type
#else
        gpu_field_type
#endif
        >;

#ifndef CARTEX_GHEX_FIELD_BY_FIELD

#ifndef CARTEX_GHEX_STAGED
    bco_type bco(m_context, m_node_local);
#ifndef __CUDACC__
    for (int i = 0; i < m_base.m_num_fields; ++i)
        bco.add_field(m_pattern->operator()(m_fields[j][i]));
#else
    for (int i = 0; i < m_base.m_num_fields; ++i)
        bco.add_field(m_pattern->operator()(m_fields_gpu[j][i]));
#endif
    m_cos[j] = std::move(bco);

#else

    bco_type bco_x(m_context, m_node_local);
    bco_type bco_y(m_context, m_node_local);
    bco_type bco_z(m_context, m_node_local);
#ifndef __CUDACC__
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        bco_x.add_field(m_pattern[0]->operator()(m_fields[j][i]));
        bco_y.add_field(m_pattern[1]->operator()(m_fields[j][i]));
        bco_z.add_field(m_pattern[2]->operator()(m_fields[j][i]));
    }
#else
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        bco_x.add_field(m_pattern[0]->operator()(m_fields_gpu[j][i]));
        bco_y.add_field(m_pattern[1]->operator()(m_fields_gpu[j][i]));
        bco_z.add_field(m_pattern[2]->operator()(m_fields_gpu[j][i]));
    }
#endif
    m_cos[j][0] = std::move(bco_x);
    m_cos[j][1] = std::move(bco_y);
    m_cos[j][2] = std::move(bco_z);
#endif

#else

#ifndef CARTEX_GHEX_STAGED
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        bco_type bco(m_context, m_node_local);
#ifndef __CUDACC__
        bco.add_field(m_pattern->operator()(m_fields[j][i]));
#else
        bco.add_field(m_pattern->operator()(m_fields_gpu[j][i]));
#endif
        m_cos[j * m_base.m_num_fields + i] = std::move(bco);
    }
#else

    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        bco_type bco_x(m_context, m_node_local);
        bco_type bco_y(m_context, m_node_local);
        bco_type bco_z(m_context, m_node_local);
#ifndef __CUDACC__
        bco_x.add_field(m_pattern[0]->operator()(m_fields[j][i]));
        bco_y.add_field(m_pattern[1]->operator()(m_fields[j][i]));
        bco_z.add_field(m_pattern[2]->operator()(m_fields[j][i]));
#else
        bco_x.add_field(m_pattern[0]->operator()(m_fields_gpu[j][i]));
        bco_y.add_field(m_pattern[1]->operator()(m_fields_gpu[j][i]));
        bco_z.add_field(m_pattern[2]->operator()(m_fields_gpu[j][i]));
#endif
        m_cos[j * m_base.m_num_fields + i][0] = std::move(bco_x);
        m_cos[j * m_base.m_num_fields + i][1] = std::move(bco_y);
        m_cos[j * m_base.m_num_fields + i][2] = std::move(bco_z);
    }
#endif

#endif
}

void
runtime::impl::step(int j)
{
#ifndef CARTEX_GHEX_FIELD_BY_FIELD
#ifndef CARTEX_GHEX_STAGED
    m_cos[j].exchange().wait();
#else
    m_cos[j][0].exchange().wait();
    m_cos[j][1].exchange().wait();
    m_cos[j][2].exchange().wait();
#endif
#else
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
#ifndef CARTEX_GHEX_STAGED
        m_cos[j * m_base.m_num_fields + i].exchange().wait();
#else
        m_cos[j * m_base.m_num_fields + i][0].exchange().wait();
        m_cos[j * m_base.m_num_fields + i][1].exchange().wait();
        m_cos[j * m_base.m_num_fields + i][2].exchange().wait();
#endif
    }
#endif
}

} // namespace cartex
