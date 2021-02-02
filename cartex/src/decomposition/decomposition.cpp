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

#include <string>
#include <stdexcept>
#include <mpi.h>

#include <iostream>

#include <cartex/decomposition/decomposition.hpp>

namespace cartex
{
decomposition::decomposition(const arr& global_d, const arr& thread_d)
: m_mpi_cart{true}
, m_hw_topo()
, m_order{HWCartOrderXYZ}
, m_thread_decomposition(thread_d)
, m_topo{global_d[0], global_d[1], global_d[2]}
, m_levels{HWCART_MD_NODE}
, m_global_decomposition{global_d}
, m_threads_per_rank{thread_d[0] * thread_d[1] * thread_d[2]}
{
    const int periods[3] = {1, 1, 1};
    CARTEX_CHECK_MPI_RESULT(
        MPI_Cart_create(MPI_COMM_WORLD, 3, global_d.data(), periods, 1, &m_comm));
    for (int i = 0; i < 3; ++i)
        m_last_coord[i] = m_global_decomposition[i] * m_thread_decomposition[i] - 1;
    CARTEX_CHECK_MPI_RESULT(MPI_Comm_rank(m_comm, &m_rank));
    CARTEX_CHECK_MPI_RESULT(MPI_Comm_size(m_comm, &m_size));
    CARTEX_CHECK_MPI_RESULT(MPI_Cart_coords(m_comm, m_rank, 3, m_coord.data()));
    m_coord[0] *= m_thread_decomposition[0];
    m_coord[1] *= m_thread_decomposition[1];
    m_coord[2] *= m_thread_decomposition[2];
}

void
decomposition::print()
{
    if (!m_mpi_cart)
    {
        hwcart_print_rank_topology(
            m_hw_topo.m, m_comm, m_levels.size(), m_levels.data(), m_topo.data(), m_order);
    }
    else
    {
        if (m_rank == 0) std::cout << "cannot print topology for MPI_Cart" << std::endl;
    }
}

decomposition::arr
decomposition::coord(int thread_id) const noexcept
{
    arr res(m_coord);
    res[0] += thread_id % m_thread_decomposition[0];
    thread_id /= m_thread_decomposition[0];
    res[1] += thread_id % m_thread_decomposition[1];
    thread_id /= m_thread_decomposition[1];
    res[2] += thread_id;
    return res;
}

decomposition::domain_t
decomposition::domain(int thread_id) const noexcept
{
    const auto c = coord(thread_id);
    return {c[0] + m_global_decomposition[0] * m_thread_decomposition[0] *
                       (c[1] + m_global_decomposition[1] * m_thread_decomposition[1] * c[2]),
        m_rank, thread_id, c};
}

decomposition::domain_t
decomposition::neighbor(int thread_id, int dx, int dy, int dz) const noexcept
{
    auto c = coord(thread_id);
    c[0] += dx;
    c[1] += dy;
    c[2] += dz;

    for (int i = 0; i < 3; ++i)
    {
        if (c[i] > m_last_coord[i]) c[i] -= m_last_coord[i] + 1;
        if (c[i] < 0) c[i] += m_last_coord[i] + 1;
    }
    const int id = c[0] + m_global_decomposition[0] * m_thread_decomposition[0] *
                              (c[1] + m_global_decomposition[1] * m_thread_decomposition[1] * c[2]);

    arr ct = c;
    arr c0;
    for (int i = 0; i < 3; ++i)
    {
        c0[i] = c[i] / m_thread_decomposition[i];
        ct[i] = c[i] - c0[i] * m_thread_decomposition[i];
    }
    const int t_id =
        ct[0] + m_thread_decomposition[0] * (ct[1] + m_thread_decomposition[1] * ct[2]);

    if (c0[0] == m_coord[0] && c0[1] == m_coord[1] && c0[2] == m_coord[2])
    { return {id, m_rank, t_id, c}; }
    else
    {
        int n_rank;
        int periodic[3] = {1, 1, 1};
        int dims[3] = {
            m_global_decomposition[0], m_global_decomposition[1], m_global_decomposition[2]};
        if (m_mpi_cart) { CARTEX_CHECK_MPI_RESULT(MPI_Cart_rank(m_comm, c0.data(), &n_rank)); }
        else
        {
            hwcart_coord2rank(m_comm, dims, periodic, c0.data(), m_order, &n_rank);
        }
        return {id, n_rank, t_id, c};
    }
}

} // namespace cartex
