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
    opts("l3", "number of L3 caches", "n", {2});
    return opts;
}

benchmark::benchmark(int& argc, char**& argv)
: benchmark_base(argc, argv, (ghexbench::num_cpus() / P2P_NUM_HWTHREADS))
, m_topo{ghexbench::hw_topo::create(MPI_COMM_WORLD)
             .nodes({1, 1, 1})
             .l3s({m_options.get<int>("l3"), 1, 1})
             .cores({m_mpi_env.size / m_options.get<int>("l3"), 1, 1})}
, m_ctx{m_topo.get_comm(), m_threads > 1}
{
    auto const n_l3s = m_topo.count(HWCART_MD_L3CACHE);

    if (n_l3s < 2) abort("at least 2 L3 caches expected", m_ctx.rank() == 0);
    if (m_threads > 1 && m_threads % 2 != 0)
        abort("even number of cores per rank expected!", m_ctx.rank() == 0);
}

oomph::rank_type
benchmark::peer_rank()
{
    auto const n_l3s = m_topo.count(HWCART_MD_L3CACHE);
    auto       c = m_topo.level_grid_coord();
    auto const l3_coord = c[1][0];
    auto const peer_l3 = ((l3_coord + n_l3s) + (((l3_coord % 2) == 0) ? 1 : -1)) % n_l3s;
    c[1][0] = peer_l3;
    return m_topo.rank(c);
}

} // namespace p2p
} // namespace ghexbench
