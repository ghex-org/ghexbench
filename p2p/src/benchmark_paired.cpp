/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <ghexbench/thread_affinity.hpp>

#include <p2p/benchmark.hpp>

namespace ghexbench
{
namespace p2p
{
benchmark::benchmark(int& argc, char**& argv)
: m_opts{ghexbench::options() /* clang-format off */
    ("mt", "enable multithreading")
    ("smt", "enable symmetric multithreading (hyper-threading), requires mt")
    ("size", "buffer size", "s", {1024})
    ("inflights","number of simultaneous inflights per thread", "i", {1})
    ("nrep", "number of messages per thread", "n", {100})
#ifdef P2P_ENABLE_DEVICE
    ("devicemap", "rank to device map (per node) in the form i1:i2:...:iN (N=number of ranks per node)", "d", 1)
#endif
    ("node", "number of nodes", "n", {1})
    ("socket", "number of sockets", "s", {1})
    ("numa", "number of numa nodes", "u", {1})
    ("l3", "number of L3 caches", "l", {1})
  } /* clang-format on */
, m_options{m_opts.parse(argc, argv)}
, m_is_multithreaded{m_options.is_set("mt")}
, m_mpi_env{m_is_multithreaded, argc, argv}
, m_topo{ghexbench::hw_topo::create(MPI_COMM_WORLD)
             .nodes({m_options.get<int>("node"), 1, 1})
             .sockets({m_options.get<int>("socket"), 1, 1})
             .numas({m_options.get<int>("numa"), 1, 1})
             .l3s({m_options.get<int>("l3"), 1, 1})
             .cores(
                 {m_mpi_env.size / (m_options.get<int>("l3") * m_options.get<int>("numa") *
                                       m_options.get<int>("socket") * m_options.get<int>("node")),
                     1, 1})}
, m_ctx{m_topo.get_comm(), m_is_multithreaded}
, m_threads{(std::size_t)(
      m_is_multithreaded
          ? (m_options.is_set("smt") ? hardware_resources().second : hardware_resources().first)
          : 1)}
, m_thread_pool{(int)m_threads}
, m_thread_barrier{m_thread_pool.make_barrier()}
, m_size{m_options.get<std::size_t>("size")}
, m_window{m_options.get<std::size_t>("inflights")}
, m_nrep{m_options.get<std::size_t>("nrep")}
, m_thread_states{m_threads}
{
    if (m_ctx.size() == 1) abort("at least 2 ranks expected!", m_ctx.rank() == 0);
    if (m_ctx.size() % 2 != 0) abort("even numer of ranks expected!", m_ctx.rank() == 0);

    auto       c = m_topo.level_grid_coord();
    auto const n_nodes = m_topo.count(HWCART_MD_NODE);
    auto const n_sockets = m_topo.count(HWCART_MD_SOCKET);
    auto const n_numas = m_topo.count(HWCART_MD_NUMA);
    auto const n_l3s = m_topo.count(HWCART_MD_L3CACHE);
    auto const n_cores = m_topo.count(HWCART_MD_CORE);

    if (n_nodes > 1)
    {
        if (n_nodes % 2 != 0) abort("even numer of nodes expected!", m_ctx.rank() == 0);
        auto const node_coord = c[0][0];
        auto const peer = ((node_coord + n_nodes) + (((node_coord % 2) == 0) ? 1 : -1)) % n_nodes;
        c[0][0] = peer;
    }
    else if (n_sockets > 1)
    {
        if (n_sockets % 2 != 0) abort("even numer of sockets expected!", m_ctx.rank() == 0);
        auto const socket_coord = c[1][0];
        auto const peer =
            ((socket_coord + n_sockets) + (((socket_coord % 2) == 0) ? 1 : -1)) % n_sockets;
        c[1][0] = peer;
    }
    else if (n_numas > 1)
    {
        if (n_numas % 2 != 0) abort("even numer of numa nodes expected!", m_ctx.rank() == 0);
        auto const numa_coord = c[2][0];
        auto const peer = ((numa_coord + n_numas) + (((numa_coord % 2) == 0) ? 1 : -1)) % n_numas;
        c[2][0] = peer;
    }
    else if (n_l3s > 1)
    {
        if (n_l3s % 2 != 0) abort("even numer of l3 caches expected!", m_ctx.rank() == 0);
        auto const l3_coord = c[3][0];
        auto const peer_l3 = ((l3_coord + n_l3s) + (((l3_coord % 2) == 0) ? 1 : -1)) % n_l3s;
        c[3][0] = peer_l3;
    }
    else if (n_cores > 1)
    {
        if (n_cores % 2 != 0) abort("even numer of cores expected!", m_ctx.rank() == 0);
        auto const core_coord = c[4][0];
        auto const peer = ((core_coord + n_cores) + (((core_coord % 2) == 0) ? 1 : -1)) % n_cores;
        c[4][0] = peer;
    }
    m_peer_rank = m_topo.rank(c);
}

} // namespace p2p
} // namespace ghexbench
