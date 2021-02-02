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

#include <array>
#include <vector>
extern "C"
{
#include <hwcart.h>
}
#ifdef NDEBUG
#define CARTEX_CHECK_MPI_RESULT(x) x;
#else
#include <string>
#include <stdexcept>
#define CARTEX_CHECK_MPI_RESULT(x)                                                                 \
    if (x != MPI_SUCCESS)                                                                          \
        throw std::runtime_error("GHEX Error: MPI Call failed " + std::string(#x) + " in " +       \
                                 std::string(__FILE__) + ":" + std::to_string(__LINE__));
#endif

namespace cartex
{
class decomposition
{
  public:
    using arr = std::array<int, 3>;

    struct domain_t
    {
        int id;
        int rank;
        int thread;
        arr coord;
    };

  private:
    struct hw_topo_t
    {
        hwcart_topo_t m;
        hw_topo_t()
        {
            if (hwcart_init(&m)) throw std::runtime_error("hwcart init failed");
        }
    };

  private:
    bool                        m_mpi_cart = false;
    hw_topo_t                   m_hw_topo;
    hwcart_order_t              m_order = HWCartOrderYZX;
    arr                         m_thread_decomposition;
    std::vector<int>            m_topo;
    std::vector<hwcart_split_t> m_levels;
    MPI_Comm                    m_comm;
    arr                         m_global_decomposition;
    arr                         m_last_coord;
    int                         m_threads_per_rank;
    int                         m_rank;
    int                         m_size;
    arr                         m_coord;

  public:
    decomposition(const arr& global_d, const arr& thread_d);
    decomposition(const decomposition&) = delete;
    ~decomposition() { hwcart_free(&m_hw_topo.m, &m_comm); }
    MPI_Comm   mpi_comm() const noexcept { return m_comm; }
    int        rank() const noexcept { return m_rank; }
    int        size() const noexcept { return m_size; }
    arr        coord(int thread_id) const noexcept;
    const arr& last_coord() const noexcept { return m_last_coord; }
    int        threads_per_rank() const noexcept { return m_threads_per_rank; }
    int        num_domains() const noexcept { return threads_per_rank() * size(); }
    domain_t   domain(int thread_id) const noexcept;
    domain_t   neighbor(int thread_id, int dx, int dy, int dz) const noexcept;
    void       print();
};

} // namespace cartex
