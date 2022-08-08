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
    opts("numa", "number of numa nodes", "n", {1});
    opts("l3", "number of L3 caches", "l", {1});
    return opts;
}

benchmark::benchmark(int& argc, char**& argv)
: benchmark_base(argc, argv, (ghexbench::num_cpus() / P2P_NUM_HWTHREADS))
, m_topo{ghexbench::hw_topo::create(MPI_COMM_WORLD)
             .nodes({1, 1, 1})
             .numas({m_options.get<int>("numa"), 1, 1})
             .l3s({m_options.get<int>("l3"), 1, 1})
             .cores(
                 {m_mpi_env.size / (m_options.get<int>("l3") * m_options.get<int>("numa")), 1, 1})}
, m_ctx{m_topo.get_comm(), m_threads > 1}
{
    //auto const n_l3s = m_topo.count(HWCART_MD_L3CACHE);
    //if (n_l3s < 2) abort("at least 2 L3 caches expected", m_ctx.rank() == 0);
    //if (m_threads > 1 && m_threads % 2 != 0)
    //    abort("even number of cores per rank expected!", m_ctx.rank() == 0);
    //if (m_mpi_env.size > 1
    auto const n_numas = m_topo.count(HWCART_MD_NUMA);
    auto const n_l3s = m_topo.count(HWCART_MD_L3CACHE);
    auto const n_cores = m_topo.count(HWCART_MD_CORE);
    //if (n_numas > 1 && n_numas %2 != 0) 
}

oomph::rank_type
benchmark::peer_rank()
{
    auto       c = m_topo.level_grid_coord();
    auto const n_numas = m_topo.count(HWCART_MD_NUMA);
    auto const n_l3s = m_topo.count(HWCART_MD_L3CACHE);
    auto const n_cores = m_topo.count(HWCART_MD_CORE);
    if (n_numas > 1)
    {
        auto const numa_coord = c[1][0];
        auto const peer = ((numa_coord + n_numas) + (((numa_coord % 2) == 0) ? 1 : -1)) % n_numas;
        c[1][0] = peer;
    }
    else if (n_l3s > 1)
    {
        auto const l3_coord = c[2][0];
        auto const peer_l3 = ((l3_coord + n_l3s) + (((l3_coord % 2) == 0) ? 1 : -1)) % n_l3s;
        c[2][0] = peer_l3;
    }
    else
    {
        auto const core_coord = c[3][0];
        auto const peer = ((core_coord + n_cores) + (((core_coord % 2) == 0) ? 1 : -1)) % n_cores;
        c[3][0] = peer;
    }
    return m_topo.rank(c);
}

} // namespace p2p
} // namespace ghexbench
