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
#include <iostream>

#include <cartex/runtime/runtime.hpp>
#include <cartex/common/memory_view.hpp>
#include <cartex/common/accumulator.hpp>
#include <cartex/common/histogram.hpp>

namespace cartex
{
void
runtime::exchange(int j)
{
    using clock_type = std::chrono::high_resolution_clock;

    // check for correctness
    if (m_check_res)
    {
        step(j);
        //print_fields(j);
        check(j);
    }

    histogram hist(1.0e-9, 20, 2, 100000);

    auto warm_up_step = [j, this]() { step(j); };

    auto main_step = [j, this, &hist]() {
        const auto start = clock_type::now();
        step(j);
        const auto end = clock_type::now();

        const double dt = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        hist(dt);
    };

    std::size_t reps = 0;
    if (m_use_timer)
    {
        m_loop.repeat_for(warm_up_step, 0.1 * m_time, j, 50, 10);
        reps = m_loop.repeat_for(main_step, 0.9 * m_time, j, m_num_reps, 100);
    }
    else
    {
        for (int t = 0; t < 50; ++t) warm_up_step();
        for (int t = 0; t < m_num_reps; ++t) main_step();
        reps = m_num_reps;
    }

    if (j == 0)
    {
        unsigned long long int num_elements, local_num_elements = 0;
        for (int i = 0; i < m_num_threads; ++i)
        {
            local_num_elements += (m_domains[i].domain_ext[0] + m_halos[0] + m_halos[1]) *
                                      (m_domains[i].domain_ext[1] + m_halos[2] + m_halos[3]) *
                                      (m_domains[i].domain_ext[2] + m_halos[4] + m_halos[5]) -
                                  (m_domains[i].domain_ext[0]) * (m_domains[i].domain_ext[1]) *
                                      (m_domains[i].domain_ext[2]);
        }
        CARTEX_CHECK_MPI_RESULT(MPI_Reduce(&local_num_elements, &num_elements, 1,
            MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, m_decomposition.mpi_comm()));
        if (m_rank == 0)
        {
            auto print = [](char const* name, double value) {
                std::cout << std::setw(18) << std::left << name << std::scientific
                          << std::setprecision(8) << value << "\n";
            };
            const auto num_bytes = (double)num_elements * (m_num_fields * sizeof(real_type));
            const auto load = 2 * num_bytes;
            const auto load_Gb = 1.0e-9 * load;
            std::cout << "repetitions       " << reps << "\n";
            print("load (bytes)", load);
            print("elapsed (s)", hist.sum());
            print("mean (s)", hist.mean());
            print("stddev (s)", hist.stddev());
            print("median (s)", hist.median());
            print("5% centile (s)", hist.percentile(5));
            print("25% centile (s)", hist.percentile(25));
            print("75% centile (s)", hist.percentile(75));
            print("95% centile (s)", hist.percentile(95));
            print("min (s)", hist.min());
            print("max (s)", hist.max());
            print("tp. median (GB/s)", load_Gb / hist.median());
            print("tp. 5% c. (GB/s)", load_Gb / hist.percentile(95));
            print("tp. 25% c. (GB/s)", load_Gb / hist.percentile(75));
            print("tp. 75% c. (GB/s)", load_Gb / hist.percentile(25));
            print("tp. 95% c. (GB/s)", load_Gb / hist.percentile(5));
            print("tp. min (GB/s)", load_Gb / hist.max());
            print("tp. max (GB/s)", load_Gb / hist.min());
            std::cout << "\n" << hist << std::endl;
        }
    }
}

void
runtime::make_fields(int j)
{
    using view_type = memory_view<real_type, 3>;
    for (int i = 0; i < m_num_fields; ++i)
    {
        m_raw_fields[j].emplace_back((m_domains[j].domain_ext[0] + m_halos[0] + m_halos[1]) *
                                         (m_domains[j].domain_ext[1] + m_halos[2] + m_halos[3]) *
                                         (m_domains[j].domain_ext[2] + m_halos[4] + m_halos[5]),
            0);
        view_type v(&m_raw_fields[j].back(), m_domains[j].domain_ext[0] + m_halos[0] + m_halos[1],
            m_domains[j].domain_ext[1] + m_halos[2] + m_halos[3],
            m_domains[j].domain_ext[2] + m_halos[4] + m_halos[5]);
        for (int z = 0; z < m_domains[j].domain_ext[2]; ++z)
            for (int y = 0; y < m_domains[j].domain_ext[1]; ++y)
                for (int x = 0; x < m_domains[j].domain_ext[0]; ++x)
                {
                    v(x + m_halos[0], y + m_halos[2], z + m_halos[4]) =
                        ((m_domains[j].domain_coord[0] + x) +
                            (m_decomposition.last_domain_coord()[0] + 1) *
                                ((m_domains[j].domain_coord[1] + y) +
                                    (m_decomposition.last_domain_coord()[1] + 1) *
                                        ((m_domains[j].domain_coord[2] + z)))) *
                            m_num_fields +
                        i;
                }
#ifdef __CUDACC__
        m_raw_fields[j].back().clone_to_device();
#endif
    }
}

void
runtime::print_fields(int j)
{
    using view_type = memory_view<real_type, 3>;
    CARTEX_CHECK_MPI_RESULT(MPI_Barrier(m_decomposition.mpi_comm()));
    for (int r = 0; r < m_size; ++r)
    {
        if (r == m_rank)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::cout << "rank " << m_rank << ", thread " << j << std::endl;
            for (int i = 0; i < m_num_fields; ++i)
            {
                std::cout << "  field " << i << std::endl;
                view_type v(&m_raw_fields[j][i],
                    m_domains[j].domain_ext[0] + m_halos[0] + m_halos[1],
                    m_domains[j].domain_ext[1] + m_halos[2] + m_halos[3],
                    m_domains[j].domain_ext[2] + m_halos[4] + m_halos[5]);
                v.print();
                std::cout << std::endl;
            }
        }
        CARTEX_CHECK_MPI_RESULT(MPI_Barrier(m_decomposition.mpi_comm()));
    }
}

bool
runtime::check(int j)
{
    using view_type = memory_view<real_type, 3>;
    for (int i = 0; i < m_num_fields; ++i)
    {
#ifdef __CUDACC__
        m_raw_fields[j][i].clone_to_host();
#endif
        view_type v(&m_raw_fields[j][i], m_domains[j].domain_ext[0] + m_halos[0] + m_halos[1],
            m_domains[j].domain_ext[1] + m_halos[2] + m_halos[3],
            m_domains[j].domain_ext[2] + m_halos[4] + m_halos[5]);
        for (int zn = -1; zn < 2; ++zn)
            for (int yn = -1; yn < 2; ++yn)
                for (int xn = -1; xn < 2; ++xn)
                {
                    auto n = m_decomposition.neighbor(j, xn, yn, zn);

                    const int ext_z =
                        zn < 0 ? m_halos[4] : zn == 0 ? m_domains[j].domain_ext[2] : m_halos[5];
                    const int first_z =
                        zn < 0 ? 0 : zn == 0 ? m_halos[4] : m_halos[4] + m_domains[j].domain_ext[2];
                    const int first_zn = zn < 0 ? n.domain_ext[2] - m_halos[4] : 0;

                    const int ext_y =
                        yn < 0 ? m_halos[2] : yn == 0 ? m_domains[j].domain_ext[1] : m_halos[3];
                    const int first_y =
                        yn < 0 ? 0 : yn == 0 ? m_halos[2] : m_halos[2] + m_domains[j].domain_ext[1];
                    const int first_yn = yn < 0 ? n.domain_ext[1] - m_halos[2] : 0;

                    const int ext_x =
                        xn < 0 ? m_halos[0] : xn == 0 ? m_domains[j].domain_ext[0] : m_halos[1];
                    const int first_x =
                        xn < 0 ? 0 : xn == 0 ? m_halos[0] : m_halos[0] + m_domains[j].domain_ext[0];
                    const int first_xn = xn < 0 ? n.domain_ext[0] - m_halos[0] : 0;

                    for (int kk = 0; kk < ext_z; ++kk)
                    {
                        for (int jj = 0; jj < ext_y; ++jj)
                        {
                            for (int ii = 0; ii < ext_x; ++ii)
                            {
                                const real_type expected =
                                    ((n.domain_coord[0] + first_xn + ii) +
                                        (m_decomposition.last_domain_coord()[0] + 1) *
                                            ((n.domain_coord[1] + first_yn + jj) +
                                                (m_decomposition.last_domain_coord()[1] + 1) *
                                                    ((n.domain_coord[2] + first_zn + kk)))) *
                                        m_num_fields +
                                    i;
                                if (v(first_x + ii, first_y + jj, first_z + kk) != expected)
                                {
                                    std::cout << "check failed!!!!!!!!!!!!!!!!! expected "
                                              << expected << " but found "
                                              << v(first_x + ii, first_y + jj, first_z + kk)
                                              << std::endl;
                                    return false;
                                }
                            }
                        }
                    }
                }
    }
    return true;
}

} // namespace cartex
