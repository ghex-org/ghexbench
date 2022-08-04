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

#include <vector>
#include <array>

extern "C"
{
#include <hwcart/hwcart.h>
}

#include <ghexbench/mpi.hpp>

namespace ghexbench
{

class hw_topo_builder;

class hw_topo
{
  public:
    using arr = std::array<int, 3>;

  private:
    hwcart_topo_t               m_topo;
    hwcart_order_t              m_order;
    std::vector<int>            m_level_grid;
    std::vector<hwcart_split_t> m_levels;
    mpi_comm_holder             m_comm;
    int                         m_rank;
    int                         m_size;
    arr                         m_grid;
    bool                        m_empty = false;

  public:
    hw_topo();

    hw_topo(MPI_Comm comm, hwcart_order_t order, std::vector<int> const& level_grid,
        std::vector<hwcart_split_t> const& levels);

    ~hw_topo();

    hw_topo(hw_topo const&) = delete;

    hw_topo(hw_topo&&);

  public:
    static hw_topo_builder create(MPI_Comm comm);

  public:
    // return MPI communicator
    MPI_Comm get_comm() const;

    // return new Cartesian MPI communicator
    mpi_comm_holder make_cart_comm();

    // number of ranks
    int size() const;

    // current rank
    int rank() const;

    // spatial decompostion
    arr const & decomposition() const;

    // number of ranks within a resource
    int size(hwcart_split_t resource) const;
    int size(int l) const;

    // number of resources
    int count(hwcart_split_t resource) const;
    int count(int l) const;

    // current rank's coordinate w.r.t. the levels
    std::vector<arr> level_grid_coord() const;

    // rank from level grid coorinates
    int rank(std::vector<arr> const& c) const;

    // current rank's grid coordinate
    arr grid_coord() const;

    // rank from grid coordinate
    int rank(arr const& c) const;

    // print ranks
    void print() const;

  private:
    int get_level(hwcart_split_t resource) const;

    int size(int l, int d) const;

    int count(int l, int d) const;
};

class hw_topo_builder
{
  public:
    using arr = hw_topo::arr;

  public:
    hw_topo_builder(MPI_Comm comm)
    : m_comm{comm}
    {
    }

  private:
    MPI_Comm       m_comm;
    hwcart_order_t m_order = HWCartOrderXYZ;
    arr            m_node_d = {0, 0, 0};
    arr            m_socket_d = {0, 0, 0};
    arr            m_numa_d = {0, 0, 0};
    arr            m_l3_d = {0, 0, 0};
    arr            m_core_d = {0, 0, 0};
    arr            m_hwthread_d = {0, 0, 0};

  public:
    hw_topo_builder& order(hwcart_order_t o)
    {
        m_order = o;
        return *this;
    }
    hw_topo_builder& nodes(arr const& d)
    {
        m_node_d = d;
        return *this;
    }
    hw_topo_builder& sockets(arr const& d)
    {
        m_socket_d = d;
        return *this;
    }
    hw_topo_builder& numas(arr const& d)
    {
        m_numa_d = d;
        return *this;
    }
    hw_topo_builder& l3s(arr const& d)
    {
        m_l3_d = d;
        return *this;
    }
    hw_topo_builder& cores(arr const& d)
    {
        m_core_d = d;
        return *this;
    }
    hw_topo_builder& hwthreads(arr const& d)
    {
        m_hwthread_d = d;
        return *this;
    }

    operator hw_topo() { return build(); }

  private:
    hw_topo build();

    bool check(arr const& d);
};
} // namespace ghexbench
