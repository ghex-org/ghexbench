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

namespace cartex
{
void
runtime::exchange(int j)
{
    using clock_type = std::chrono::high_resolution_clock;

    make_fields(j);

    init(j);

    if (m_check_res)
    {
        step(j);
        //print_fields(j);
        check(j);
    }

    // warm up
    for (int t = 0; t < 50; ++t) step(j);

    const auto start = clock_type::now();
    for (int t = 0; t < m_num_reps; ++t) step(j);
    const auto                          end = clock_type::now();
    const std::chrono::duration<double> elapsed_seconds = end - start;

    if (m_rank == 0 && j == 0)
    {
        const auto num_elements =
            m_ext_buffer[0] * m_ext_buffer[1] * m_ext_buffer[2] - m_ext[0] * m_ext[1] * m_ext[2];
        const auto   num_bytes = num_elements * sizeof(real_type);
        const double load = 2 * m_size * m_num_threads * m_num_fields * num_bytes;
        const auto   GB_per_s = m_num_reps * load / (elapsed_seconds.count() * 1.0e9);
        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
        std::cout << "GB/s : " << GB_per_s << std::endl;
    }
}

void
runtime::make_fields(int j)
{
    using view_type = memory_view<real_type, 3>;
    for (int i = 0; i < m_num_fields; ++i)
    {
        m_raw_fields[j].emplace_back(m_max_memory, 0);
        view_type    v(&m_raw_fields[j].back(), m_ext_buffer[0], m_ext_buffer[1], m_ext_buffer[2]);
        unsigned int c = m_decomposition.domain(j).id + i + 1;
        for (int z = 0; z < m_ext[2]; ++z)
            for (int y = 0; y < m_ext[1]; ++y)
                for (int x = 0; x < m_ext[0]; ++x)
                {
                    v(x + m_halos[0], y + m_halos[2], z + m_halos[4]) = c;
                    ++c;
                }
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
                view_type v(&m_raw_fields[j][i], m_ext_buffer[0], m_ext_buffer[1], m_ext_buffer[2]);
                v.print();
                std::cout << std::endl;
            }
        }
        CARTEX_CHECK_MPI_RESULT(MPI_Barrier(m_decomposition.mpi_comm()));
    }
}

bool
runtime::check(int jj)
{
    using view_type = memory_view<real_type, 3>;
    for (int zn = -1; zn < 2; ++zn)
        for (int yn = -1; yn < 2; ++yn)
            for (int xn = -1; xn < 2; ++xn)
            {
                int offset = m_decomposition.neighbor(jj, xn, yn, zn).id;

                const int ext_z = zn < 0 ? m_halos[4] : zn == 0 ? m_ext[2] : m_halos[5];
                const int first_z = zn < 0 ? 0 : zn == 0 ? m_halos[4] : m_halos[4] + m_ext[2];
                const int first_zn = zn < 0 ? m_ext[2] : zn == 0 ? m_halos[4] : m_halos[4];

                const int ext_y = yn < 0 ? m_halos[2] : yn == 0 ? m_ext[1] : m_halos[3];
                const int first_y = yn < 0 ? 0 : yn == 0 ? m_halos[2] : m_halos[2] + m_ext[1];
                const int first_yn = yn < 0 ? m_ext[1] : yn == 0 ? m_halos[2] : m_halos[2];

                const int ext_x = xn < 0 ? m_halos[0] : xn == 0 ? m_ext[0] : m_halos[1];
                const int first_x = xn < 0 ? 0 : xn == 0 ? m_halos[0] : m_halos[0] + m_ext[0];
                const int first_xn = xn < 0 ? m_ext[0] : xn == 0 ? m_halos[0] : m_halos[0];

                for (int ii = 0; ii < m_num_fields; ++ii)
                {
                    view_type v(
                        &m_raw_fields[jj][ii], m_ext_buffer[0], m_ext_buffer[1], m_ext_buffer[2]);
                    for (int k = 0; k < ext_z; ++k)
                        for (int j = 0; j < ext_y; ++j)
                            for (int i = 0; i < ext_x; ++i)
                            {
                                const auto x = first_x + i;
                                const auto xn = first_xn + i - m_halos[0];
                                const auto y = first_y + j;
                                const auto yn = first_yn + j - m_halos[2];
                                const auto z = first_z + k;
                                const auto zn = first_zn + k - m_halos[4];

                                const unsigned int expected =
                                    offset + ii + 1 + xn + m_ext[0] * (yn + zn * m_ext[1]);
                                if (v(x, y, z) != (real_type)expected)
                                {
                                    std::cout << "check failed!!!!!!!!!!!!!!!!! expected "
                                              << (real_type)expected << " but found " << v(x, y, z)
                                              << std::endl;
                                    return false;
                                }
                            }
                }
            }
    return true;
}

} // namespace cartex
