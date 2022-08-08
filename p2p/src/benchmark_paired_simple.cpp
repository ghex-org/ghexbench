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

#include <iostream>

#include <p2p/benchmark.hpp>

namespace ghexbench
{
namespace p2p
{

options&
benchmark::add_options(options& opts)
{
    return opts;
}

benchmark::benchmark(int& argc, char**& argv)
: benchmark_base(argc, argv, (ghexbench::num_cpus() / P2P_NUM_HWTHREADS))
, m_topo{ghexbench::hw_topo::create(MPI_COMM_WORLD)
    .nodes({2, 1, 1})
    .cores({m_mpi_env.size / 2, 1, 1})}
, m_ctx{m_topo.get_comm(), m_threads > 1}
{
}

oomph::rank_type
benchmark::peer_rank()
{
    auto       c = m_topo.level_grid_coord();
    auto const node_coord = c[0][0];
    auto const peer = (node_coord==0? 1 : 0);
    c[0][0] = peer;
    return m_topo.rank(c);
}

} // namespace p2p
} // namespace ghexbench
