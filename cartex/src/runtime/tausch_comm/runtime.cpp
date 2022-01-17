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

#include <cartex/runtime/tausch_comm/runtime.hpp>
#include "../runtime_inc.cpp"
#include <vector>

namespace cartex
{
void
print_runtime_config()
{
}

options&
runtime::add_options(options& opts)
{
    return opts;
}

bool
runtime::check_options(options_values const& opts)
{
    // no threading
    const auto threads = opts.get<std::array<int, 3>>("thread");
    const auto num_threads = threads[0] * threads[1] * threads[2];
    if (num_threads > 1)
    {
        std::cerr << "GCL backend does not support threading" << std::endl;
        return false;
    }
    return true;
}

std::size_t
runtime::impl::add_send_halo(std::size_t x_l, std::size_t x_r, std::size_t y_l, std::size_t y_r,
    std::size_t z_l, std::size_t z_r, std::array<int, 3> ext_buffer, int rank)
{
    static std::vector<int> idx;
    idx.clear();
    for (std::size_t z = z_l; z < z_r; ++z)
        for (std::size_t y = y_l; y < y_r; ++y)
            for (std::size_t x = x_l; x < x_r; ++x)
                idx.push_back(x + y * (ext_buffer[0]) + z * (ext_buffer[0] * ext_buffer[1]));
    return m_tausch.addSendHaloInfos(idx, sizeof(runtime::real_type), m_base.m_num_fields, rank);
}

std::size_t
runtime::impl::add_recv_halo(std::size_t x_l, std::size_t x_r, std::size_t y_l, std::size_t y_r,
    std::size_t z_l, std::size_t z_r, std::array<int, 3> ext_buffer, int rank)
{
    static std::vector<int> idx;
    idx.clear();
    for (std::size_t z = z_l; z < z_r; ++z)
        for (std::size_t y = y_l; y < y_r; ++y)
            for (std::size_t x = x_l; x < x_r; ++x)
                idx.push_back(x + y * (ext_buffer[0]) + z * (ext_buffer[0] * ext_buffer[1]));
    return m_tausch.addRecvHaloInfos(idx, sizeof(runtime::real_type), m_base.m_num_fields, rank);
}

runtime::impl::impl(cartex::runtime& base, options_values const&)
: m_base{base}
, m_comm{m_base.m_decomposition.mpi_comm()}
, m_tausch{m_comm, false}
, m_reqs(4 * m_base.m_num_fields)
{
    auto                     d = m_base.m_decomposition.domain(0);
    const auto               halos = m_base.m_halos;
    const std::array<int, 3> ext_buffer = {d.domain_ext[0] + halos[0] + halos[1],
        d.domain_ext[1] + halos[2] + halos[3], d.domain_ext[2] + halos[4] + halos[5]};

    r_x_l = add_recv_halo(0, halos[0], halos[2], d.domain_ext[1] + halos[3], halos[4],
        d.domain_ext[2] + halos[5], ext_buffer, m_base.m_decomposition.neighbor(0, -1, 0, 0).rank);
    r_x_r = add_recv_halo(halos[0] + d.domain_ext[0], ext_buffer[0], halos[2],
        d.domain_ext[1] + halos[3], halos[4], d.domain_ext[2] + halos[5], ext_buffer,
        m_base.m_decomposition.neighbor(0, 1, 0, 0).rank);

    r_y_l = add_recv_halo(0, ext_buffer[0], 0, halos[2], halos[4], d.domain_ext[2] + halos[5],
        ext_buffer, m_base.m_decomposition.neighbor(0, 0, -1, 0).rank);
    r_y_r = add_recv_halo(0, ext_buffer[0], halos[2] + d.domain_ext[1], ext_buffer[1], halos[4],
        d.domain_ext[2] + halos[5], ext_buffer, m_base.m_decomposition.neighbor(0, 0, 1, 0).rank);

    r_z_l = add_recv_halo(0, ext_buffer[0], 0, ext_buffer[1], 0, halos[4], ext_buffer,
        m_base.m_decomposition.neighbor(0, 0, 0, -1).rank);
    r_z_r = add_recv_halo(0, ext_buffer[0], 0, ext_buffer[1], halos[4] + d.domain_ext[2],
        ext_buffer[2], ext_buffer, m_base.m_decomposition.neighbor(0, 0, 0, 1).rank);

    s_x_l = add_send_halo(halos[0], halos[0] + halos[1], halos[2], d.domain_ext[1] + halos[3],
        halos[4], d.domain_ext[2] + halos[5], ext_buffer,
        m_base.m_decomposition.neighbor(0, -1, 0, 0).rank);
    s_x_r = add_send_halo(d.domain_ext[0], halos[0] + d.domain_ext[0], halos[2],
        d.domain_ext[1] + halos[3], halos[4], d.domain_ext[2] + halos[5], ext_buffer,
        m_base.m_decomposition.neighbor(0, 1, 0, 0).rank);

    s_y_l = add_send_halo(0, ext_buffer[0], halos[2], halos[2] + halos[3], halos[4],
        d.domain_ext[2] + halos[5], ext_buffer, m_base.m_decomposition.neighbor(0, 0, -1, 0).rank);
    s_y_r = add_send_halo(0, ext_buffer[0], d.domain_ext[1], halos[2] + d.domain_ext[1], halos[4],
        d.domain_ext[2] + halos[5], ext_buffer, m_base.m_decomposition.neighbor(0, 0, 1, 0).rank);

    s_z_l = add_send_halo(0, ext_buffer[0], 0, ext_buffer[1], halos[4], halos[4] + halos[5],
        ext_buffer, m_base.m_decomposition.neighbor(0, 0, 0, -1).rank);
    s_z_r = add_send_halo(0, ext_buffer[0], 0, ext_buffer[1], d.domain_ext[2],
        halos[4] + d.domain_ext[2], ext_buffer, m_base.m_decomposition.neighbor(0, 0, 0, 1).rank);

    //enum Communication {
    //    Default = 1,
    //    TryDirectCopy = 2,
    //    DerivedMpiDatatype = 4,
    //    CUDAAwareMPI = 8,
    //    MPIPersistent = 16,
    //    GPUMultiCopy = 32
    //};

    Tausch::Communication strategy =
#ifdef __CUDACC__
        Tausch::Communication::CUDAAwareMPI
#else
        Tausch::TryDirectCopy
#endif
        ;

    m_tausch.setSendCommunicationStrategy(s_x_l, strategy);
    m_tausch.setSendCommunicationStrategy(s_x_r, strategy);
    m_tausch.setSendCommunicationStrategy(s_y_l, strategy);
    m_tausch.setSendCommunicationStrategy(s_y_r, strategy);
    m_tausch.setSendCommunicationStrategy(s_z_l, strategy);
    m_tausch.setSendCommunicationStrategy(s_z_r, strategy);

    m_tausch.setRecvCommunicationStrategy(r_x_l, strategy);
    m_tausch.setRecvCommunicationStrategy(r_x_r, strategy);
    m_tausch.setRecvCommunicationStrategy(r_y_l, strategy);
    m_tausch.setRecvCommunicationStrategy(r_y_r, strategy);
    m_tausch.setRecvCommunicationStrategy(r_z_l, strategy);
    m_tausch.setRecvCommunicationStrategy(r_z_r, strategy);

    CARTEX_CHECK_MPI_RESULT(MPI_Barrier(m_comm));
}

void
runtime::impl::init(int)
{
    //setSendHaloBuffer(haloId,bufferId,unsigned char ptr);
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_tausch.setSendHaloBuffer(s_x_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setSendHaloBuffer(s_x_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setRecvHaloBuffer(r_x_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setRecvHaloBuffer(r_x_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));

        m_tausch.setSendHaloBuffer(s_y_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setSendHaloBuffer(s_y_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setRecvHaloBuffer(r_y_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setRecvHaloBuffer(r_y_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));

        m_tausch.setSendHaloBuffer(s_z_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setSendHaloBuffer(s_z_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setRecvHaloBuffer(r_z_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.setRecvHaloBuffer(r_z_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
    }
}

void
runtime::impl::step(int)
{
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_tausch.packSendBuffer(s_x_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.packSendBuffer(s_x_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
    }
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_reqs[4 * i + 0] =
            m_tausch.send(s_x_l, 0, m_base.m_decomposition.neighbor(0, -1, 0, 0).rank, i);
        m_reqs[4 * i + 1] =
            m_tausch.send(s_x_r, 1, m_base.m_decomposition.neighbor(0, 1, 0, 0).rank, i);
        m_reqs[4 * i + 2] =
            m_tausch.recv(r_x_r, 0, m_base.m_decomposition.neighbor(0, 1, 0, 0).rank, i);
        m_reqs[4 * i + 3] =
            m_tausch.recv(r_x_l, 1, m_base.m_decomposition.neighbor(0, -1, 0, 0).rank, i);
    }
    MPI_Waitall(m_reqs.size(), m_reqs.data(), MPI_STATUS_IGNORE);
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_tausch.unpackRecvBuffer(r_x_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.unpackRecvBuffer(r_x_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
    }

    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_tausch.packSendBuffer(s_y_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.packSendBuffer(s_y_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
    }
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_reqs[4 * i + 0] =
            m_tausch.send(s_y_l, 0, m_base.m_decomposition.neighbor(0, 0, -1, 0).rank, i);
        m_reqs[4 * i + 1] =
            m_tausch.send(s_y_r, 1, m_base.m_decomposition.neighbor(0, 0, 1, 0).rank, i);
        m_reqs[4 * i + 2] =
            m_tausch.recv(r_y_r, 0, m_base.m_decomposition.neighbor(0, 0, 1, 0).rank, i);
        m_reqs[4 * i + 3] =
            m_tausch.recv(r_y_l, 1, m_base.m_decomposition.neighbor(0, 0, -1, 0).rank, i);
    }
    MPI_Waitall(m_reqs.size(), m_reqs.data(), MPI_STATUS_IGNORE);
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_tausch.unpackRecvBuffer(r_y_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.unpackRecvBuffer(r_y_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
    }

    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_tausch.packSendBuffer(s_z_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.packSendBuffer(s_z_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
    }
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_reqs[4 * i + 0] =
            m_tausch.send(s_z_l, 0, m_base.m_decomposition.neighbor(0, 0, 0, -1).rank, i);
        m_reqs[4 * i + 1] =
            m_tausch.send(s_z_r, 1, m_base.m_decomposition.neighbor(0, 0, 0, 1).rank, i);
        m_reqs[4 * i + 2] =
            m_tausch.recv(r_z_r, 0, m_base.m_decomposition.neighbor(0, 0, 0, 1).rank, i);
        m_reqs[4 * i + 3] =
            m_tausch.recv(r_z_l, 1, m_base.m_decomposition.neighbor(0, 0, 0, -1).rank, i);
    }
    MPI_Waitall(m_reqs.size(), m_reqs.data(), MPI_STATUS_IGNORE);
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_tausch.unpackRecvBuffer(r_z_l, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
        m_tausch.unpackRecvBuffer(r_z_r, i, (unsigned char*)(m_base.m_raw_fields[0][i].hd_data()));
    }
}

void
runtime::impl::exit(int)
{
}

} // namespace cartex
