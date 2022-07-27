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
             .nodes({1, 1, 1})
             .l3s({1, 1, 1})
             .cores({m_mpi_env.size, 1, 1})}
, m_ctx{m_topo.get_comm(), m_threads > 1}
{
    auto const n_cores = m_topo.size(HWCART_MD_L3CACHE);

    if (n_cores > 1 && n_cores % 2 != 0) abort("even number of ranks per L3 cache expected!", m_ctx.rank() == 0);
    if (m_threads > 1 && m_threads % 2 != 0)
        abort("even number of cores per rank expected!", m_ctx.rank() == 0);
}

oomph::rank_type
benchmark::peer_rank()
{
    auto const n_cores = m_topo.size(HWCART_MD_L3CACHE);
    auto       c = m_topo.level_grid_coord();
    auto const core_coord = c[2][0];
    auto const peer_core = ((core_coord + n_cores) + (((core_coord % 2) == 0) ? 1 : -1)) % n_cores;
    c[2][0] = peer_core;
    return m_topo.rank(c);
}

} // namespace p2p
} // namespace ghexbench
