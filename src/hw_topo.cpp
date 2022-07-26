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

#include <stdexcept>
#include <algorithm>
#include <iterator>

#include <ghexbench/hw_topo.hpp>

namespace ghexbench
{
hw_topo::hw_topo(MPI_Comm comm, hwcart_order_t order, std::vector<int> const& level_grid,
    std::vector<hwcart_split_t> const& levels)
: m_order{order}
, m_level_grid{level_grid}
, m_levels{levels}
{
    if (hwcart_init(&m_topo)) throw std::runtime_error("hwcart init failed");

    MPI_Comm new_comm;
    if (hwcart_create(m_topo, comm, m_levels.size(), m_levels.data(), m_level_grid.data(), m_order,
            &new_comm))
        throw std::runtime_error("hwcart create failed");
    m_comm.reset(new_comm);

    for (int i = 0; i < 3; ++i)
    {
        m_grid[i] = 1;
        for (unsigned int j = 0; j < m_levels.size(); ++j) m_grid[i] *= m_level_grid[j * 3 + i];
    }
    GHEXBENCH_CHECK_MPI_RESULT(MPI_Comm_rank(m_comm, &m_rank));
    GHEXBENCH_CHECK_MPI_RESULT(MPI_Comm_size(m_comm, &m_size));
}

hw_topo::~hw_topo() { hwcart_topo_free(&m_topo); }

hw_topo_builder
hw_topo::create(MPI_Comm comm)
{
    return {comm};
}

// return MPI communicator
MPI_Comm
hw_topo::get_comm() const
{
    return m_comm;
}

// number of ranks
int
hw_topo::size() const
{
    return m_size;
}

// current rank
int
hw_topo::rank() const
{
    return m_rank;
}

// number of ranks within a resource
int
hw_topo::size(hwcart_split_t resource) const
{
    return size(get_level(resource));
}

int
hw_topo::size(int l) const
{
    int count = 1;
    for (int j = 0; j < l; ++j)
        for (int i = 0; i < 3; ++i) count *= m_level_grid[j * 3 + i];
    return count;
}

int
hw_topo::size(int l, int d) const
{
    int count = 1;
    for (int j = 0; j < l; ++j) count *= m_level_grid[j * 3 + d];
    return count;
}

// number of resources
int
hw_topo::count(hwcart_split_t resource) const
{
    return count(get_level(resource));
}

int
hw_topo::count(int l) const
{
    int count = 1;
    for (unsigned int j = l; j < m_levels.size(); ++j)
        for (int i = 0; i < 3; ++i) count *= m_level_grid[j * 3 + i];
    return count;
}

int
hw_topo::count(int l, int d) const
{
    int count = 1;
    for (unsigned int j = l; j < m_levels.size(); ++j) count *= m_level_grid[j * 3 + d];
    return count;
}

// current rank's coordinate w.r.t. the levels
std::vector<hw_topo::arr>
hw_topo::level_grid_coord() const
{
    std::vector<hw_topo::arr> res;
    arr                       coord = grid_coord();

    for (int l = (int)m_levels.size() - 1; l >= 0; --l)
    {
        auto const nx = size(l, 0);
        auto const ny = size(l, 1);
        auto const nz = size(l, 2);
        //std::cout << nx << ", " << ny << ", " << nz << std::endl;
        arr larr{coord[0] / nx, coord[1] / ny, coord[2] / nz};
        coord[0] -= larr[0] * nx;
        coord[1] -= larr[1] * ny;
        coord[2] -= larr[2] * nz;
        res.push_back(larr);
    }

    return res;
}

// rank from level grid coorinates
int
hw_topo::rank(std::vector<hw_topo::arr> const& c) const
{
    arr coord{0, 0, 0};

    for (unsigned int l = 0; l < m_levels.size(); ++l)
    {
        coord[0] += c[m_levels.size() - 1 - l][0] * size(l, 0);
        coord[1] += c[m_levels.size() - 1 - l][1] * size(l, 1);
        coord[2] += c[m_levels.size() - 1 - l][2] * size(l, 2);
    }

    return rank(coord);
}

// current rank's grid coordinate
hw_topo::arr
hw_topo::grid_coord() const
{
    arr coord;
    hwcart_rank2coord(m_comm, const_cast<int*>(m_grid.data()), m_rank, m_order, coord.data());
    return coord;
}

// rank from grid coordinate
int
hw_topo::rank(hw_topo::arr const& coord) const
{
    int res = 0;
    int periodic[3] = {1, 1, 1};
    hwcart_coord2rank(m_comm, const_cast<int*>(m_grid.data()), periodic,
        const_cast<int*>(&coord[0]), m_order, &res);
    return res;
}

int
hw_topo::get_level(hwcart_split_t resource) const
{
    auto it = std::find(m_levels.begin(), m_levels.end(), resource);
    if (it == m_levels.end()) throw std::runtime_error("resource cannot be found");
    return std::distance(m_levels.begin(), it);
}

hw_topo
hw_topo_builder::build()
{
    std::vector<int>            level_grid;
    std::vector<hwcart_split_t> levels;

    if (check(m_hwthread_d))
    {
        level_grid.insert(level_grid.end(), m_hwthread_d.begin(), m_hwthread_d.end());
        levels.push_back(HWCART_MD_HWTHREAD);
    }
    if (check(m_core_d))
    {
        level_grid.insert(level_grid.end(), m_core_d.begin(), m_core_d.end());
        levels.push_back(HWCART_MD_CORE);
    }
    if (check(m_l3_d))
    {
        level_grid.insert(level_grid.end(), m_l3_d.begin(), m_l3_d.end());
        levels.push_back(HWCART_MD_L3CACHE);
    }
    if (check(m_numa_d))
    {
        level_grid.insert(level_grid.end(), m_numa_d.begin(), m_numa_d.end());
        levels.push_back(HWCART_MD_NUMA);
    }
    if (check(m_socket_d))
    {
        level_grid.insert(level_grid.end(), m_socket_d.begin(), m_socket_d.end());
        levels.push_back(HWCART_MD_SOCKET);
    }
    if (check(m_node_d))
    {
        level_grid.insert(level_grid.end(), m_node_d.begin(), m_node_d.end());
        levels.push_back(HWCART_MD_NODE);
    }

    return {m_comm, m_order, level_grid, levels};
}

bool
hw_topo_builder::check(arr const& d)
{
    return ((d[0] > 0) && (d[1] > 0) && (d[2] > 0));
}

} // namespace ghexbench
