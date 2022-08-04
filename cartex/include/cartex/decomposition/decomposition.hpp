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

#pragma once

//#include <array>
//#include <vector>
#include <string>
//#include <iostream>

#include <ghexbench/mpi.hpp>
#include <ghexbench/hw_topo.hpp>

namespace ghexbench
{
namespace cartex
{
class decomposition
{
  public:
    using arr = hw_topo::arr;

    struct domain_t
    {
        int id;
        int rank;
        int thread;
        arr coord;
        arr domain_coord;
        arr domain_ext;
    };

  private:
//    struct hw_topo_t
//    {
//        hwcart_topo_t m;
//        hw_topo_t()
//        {
//            if (hwcart_init(&m)) throw std::runtime_error("hwcart init failed");
//        }
//        ~hw_topo_t() { hwcart_topo_free(&m); }
//        hw_topo_t(const hw_topo_t&) = delete;
//        hw_topo_t(hw_topo_t&&) = delete;
//    };
//
public:
    static hwcart_order_t parse_order(std::string const& order_str);
//    {
//        if (order_str == "XYZ") return HWCartOrderXYZ;
//        else if (order_str == "XZY")
//            return HWCartOrderXZY;
//        else if (order_str == "ZYX")
//            return HWCartOrderZYX;
//        else if (order_str == "YZX")
//            return HWCartOrderYZX;
//        else if (order_str == "ZXY")
//            return HWCartOrderZXY;
//        else if (order_str == "YXZ")
//            return HWCartOrderYXZ;
//        else
//        {
//            std::cerr << "warning: unrecognized order, using XYZ" << std::endl;
//            return HWCartOrderXYZ;
//        }
//    }
//
  private:
//    hw_topo_t                       m_hw_topo;
//    hwcart_order_t                  m_order = HWCartOrderYZX;
//    std::vector<int>                m_topo;
//    std::vector<hwcart_split_t>     m_levels;
//    MPI_Comm                        m_comm;
//    MPI_Comm                        m_cart_comm;
//    int                             m_rank;
//    int                             m_size;
//    arr                             m_coord;
    hw_topo m_hw_topo;
    mpi_comm_holder m_cart_comm;
    arr                             m_thread_decomposition;
    int                             m_threads_per_rank;
    arr                             m_global_decomposition;
    arr                             m_coord;
    int                             m_rank;
    int                             m_size;
    arr                             m_last_coord;
    arr                             m_global_domain;
    std::array<std::vector<int>, 3> m_domain_coord;
    std::array<std::vector<int>, 3> m_domain_ext;
    arr                             m_last_domain_coord;
    bool                            m_use_mpi_cart = false;
//
  private:
    void init_domain(const arr& domain_, bool local);

  public:
//    decomposition(const std::string& order, const arr& thread_d, std::vector<int>&& topo,
//        std::vector<hwcart_split_t>&& levels, const arr& domain_, bool local);
    decomposition(hw_topo_builder builder, const arr& thread_d, arr const & domain_, bool local);
//
//  public:
    decomposition(MPI_Comm comm, const arr& global_d, const arr& thread_d, const arr& domain_, bool local);

//    decomposition(const std::string& order, const arr& node_d, const arr& thread_d,
//        const arr& domain_, bool local)
//    : decomposition(order, thread_d, std::vector<int>{1, 1, 1, node_d[0], node_d[1], node_d[2]},
//          std::vector<hwcart_split_t>{HWCART_MD_CORE, HWCART_MD_NODE}, domain_, local)
//    {
//    }
//    decomposition(const std::string& order, const arr& node_d, const arr& socket_d,
//        const arr& thread_d, const arr& domain_, bool local)
//    : decomposition(order, thread_d,
//          std::vector<int>{
//              1, 1, 1, socket_d[0], socket_d[1], socket_d[2], node_d[0], node_d[1], node_d[2]},
//          std::vector<hwcart_split_t>{HWCART_MD_CORE, HWCART_MD_SOCKET, HWCART_MD_NODE}, domain_,
//          local)
//    {
//    }
//    decomposition(const std::string& order, const arr& node_d, const arr& socket_d,
//        const arr& numa_d, const arr& thread_d, const arr& domain_, bool local)
//    : decomposition(order, thread_d,
//          std::vector<int>{1, 1, 1, numa_d[0], numa_d[1], numa_d[2], socket_d[0], socket_d[1],
//              socket_d[2], node_d[0], node_d[1], node_d[2]},
//          std::vector<hwcart_split_t>{
//              HWCART_MD_CORE, HWCART_MD_NUMA, HWCART_MD_SOCKET, HWCART_MD_NODE},
//          domain_, local)
//    {
//    }
//    decomposition(const std::string& order, const arr& node_d, const arr& socket_d,
//        const arr& numa_d, const arr& l3_d, const arr& thread_d, const arr& domain_, bool local)
//    : decomposition(order, thread_d,
//          std::vector<int>{1, 1, 1, l3_d[0], l3_d[1], l3_d[2], numa_d[0], numa_d[1], numa_d[2],
//              socket_d[0], socket_d[1], socket_d[2], node_d[0], node_d[1], node_d[2]},
//          std::vector<hwcart_split_t>{
//              HWCART_MD_CORE, HWCART_MD_L3CACHE, HWCART_MD_NUMA, HWCART_MD_SOCKET, HWCART_MD_NODE},
//          domain_, local)
//    {
//    }
//    decomposition(const std::string& order, const arr& node_d, const arr& socket_d,
//        const arr& numa_d, const arr& l3_d, const arr& core_d, const arr& thread_d,
//        const arr& domain_, bool local)
//    : decomposition(order, thread_d,
//          std::vector<int>{core_d[0], core_d[1], core_d[2], l3_d[0], l3_d[1], l3_d[2], numa_d[0],
//              numa_d[1], numa_d[2], socket_d[0], socket_d[1], socket_d[2], node_d[0], node_d[1],
//              node_d[2]},
//          std::vector<hwcart_split_t>{
//              HWCART_MD_CORE, HWCART_MD_L3CACHE, HWCART_MD_NUMA, HWCART_MD_SOCKET, HWCART_MD_NODE},
//          domain_, local)
//    {
//    }
//    decomposition(const std::string& order, const arr& node_d, const arr& socket_d,
//        const arr& numa_d, const arr& l3_d, const arr& core_d, const arr& hwthread_d,
//        const arr& thread_d, const arr& domain_, bool local)
//    : decomposition(order, thread_d,
//          std::vector<int>{hwthread_d[0], hwthread_d[1], hwthread_d[2], core_d[0], core_d[1],
//              core_d[2], l3_d[0], l3_d[1], l3_d[2], numa_d[0], numa_d[1], numa_d[2], socket_d[0],
//              socket_d[1], socket_d[2], node_d[0], node_d[1], node_d[2]},
//          std::vector<hwcart_split_t>{HWCART_MD_HWTHREAD, HWCART_MD_CORE, HWCART_MD_L3CACHE,
//              HWCART_MD_NUMA, HWCART_MD_SOCKET, HWCART_MD_NODE},
//          domain_, local)
//    {
//    }
//    decomposition(const decomposition&) = delete;
//    ~decomposition();
//
    public:
    MPI_Comm              mpi_comm() const noexcept;
    MPI_Comm              mpi_cart_comm() const noexcept { return m_cart_comm; }
    int                   rank() const noexcept { return m_rank; }
    int                   size() const noexcept { return m_size; }
    arr                   coord(int thread_id) const noexcept;
    arr const &           rank_decomposition() const noexcept { return m_global_decomposition; }
    arr const &            last_coord() const noexcept { return m_last_coord; }
    arr const &            last_domain_coord() const noexcept { return m_last_domain_coord; }
    int                   threads_per_rank() const noexcept { return m_threads_per_rank; }
    int                   num_domains() const noexcept { return threads_per_rank() * size(); }
    domain_t              domain(int thread_id) const noexcept;
    domain_t              neighbor(int thread_id, int dx, int dy, int dz) const noexcept;
    std::vector<domain_t> domains() const noexcept;
    void                  print();
};

} // namespace cartex
} // namespace ghexbench 
