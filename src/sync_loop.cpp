/*
 * ghex-org
 *
 * Copyright (c) 2014-2022, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <ghexbench/sync_loop.hpp>

namespace ghexbench
{
sync_loop::sync_loop(MPI_Comm comm, unsigned int num_threads) noexcept
: m_comm{comm}
, m_num_threads{num_threads}
, m_count_up{0}
, m_count_down{num_threads}
{
    MPI_Comm_rank(m_comm, &m_rank);
}

double
sync_loop::elapsed(time_point_type const& tp, unsigned int thread_id) noexcept
{
    double dt = 1.0e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(
                             sync_loop::clock_type::now() - tp)
                             .count();
    if (thread_id == 0) MPI_Bcast(&dt, 1, MPI_DOUBLE, 0, m_comm);
    bcast(dt, thread_id);
    return dt;
}

void
sync_loop::bcast(double& dt, unsigned int thread_id) noexcept
{
    if (thread_id == 0) m_dt = dt;
    auto ex_u = thread_id;
    while (!m_count_up.compare_exchange_weak(ex_u, ex_u + 1, std::memory_order_relaxed))
        ex_u = thread_id;
    if (ex_u == m_num_threads - 1) m_count_up.store(0);
    else
        while (m_count_up != 0) {}
    dt = m_dt;
    unsigned int ex_d = m_count_down;
    while (!m_count_down.compare_exchange_weak(ex_d, ex_d - 1, std::memory_order_relaxed))
        ex_d = m_count_down;
    if (ex_d == 1) m_count_down.store(m_num_threads);
    else
        while (m_count_down != m_num_threads) {}
}

} // namespace cartex
