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

#pragma once

#include <mpi.h>
#include <cstddef>
#include <algorithm>
#include <chrono>
#include <atomic>

namespace cartex
{
class sync_loop
{
  private:
    using clock_type = std::chrono::high_resolution_clock;
    using time_point_type = std::chrono::time_point<clock_type>;

  private:
    MPI_Comm                  m_comm;
    int                       m_rank;
    unsigned int              m_num_threads;
    std::atomic<unsigned int> m_count_up;
    std::atomic<unsigned int> m_count_down;
    double                    m_dt;

  public:
    sync_loop(MPI_Comm comm, unsigned int num_threads = 1u) noexcept;
    sync_loop(sync_loop const&) = delete;
    sync_loop(sync_loop&&) = delete;

    template<typename Function>
    std::size_t repeat_for(Function&& f, double seconds, unsigned int thread_id = 0,
        std::size_t min_iterations = 1, std::size_t increment = 1) noexcept
    {
        const auto tp = clock_type::now();
        increment = std::max((std::size_t)1, increment);
        std::size_t i = 0;
        do
        {
            for (std::size_t j = 0; j < increment; ++j) f();
            i += increment;
        } while (i < min_iterations || elapsed(tp, thread_id) < seconds);
        return i;
    }

  private:
    double elapsed(time_point_type const& tp, unsigned int thread_id) noexcept;
    void   bcast(double& dt, unsigned int thread_id) noexcept;
};
} // namespace cartex
