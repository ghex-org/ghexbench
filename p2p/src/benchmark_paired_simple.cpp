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
    ("size", "buffer size", "s", {1024})
    ("inflights","number of simultaneous inflights per thread", "i", {1})
    ("nrep", "number of messages per thread", "n", {100})
#ifdef P2P_ENABLE_DEVICE
    ("devicemap", "rank to device map (per node) in the form i1:i2:...:iN (N=number of ranks per node)", "d", 1)
#endif
  } /* clang-format on */
, m_options{m_opts.parse(argc, argv)}
, m_is_multithreaded{m_options.is_set("mt")}
, m_mpi_env{m_is_multithreaded, argc, argv}
, m_topo{ghexbench::hw_topo::create(MPI_COMM_WORLD)
             .nodes({2, 1, 1})
             .cores({m_mpi_env.size / 2, 1, 1})}
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
    auto       c = m_topo.level_grid_coord();
    auto const node_coord = c[0][0];
    auto const peer = (node_coord == 0 ? 1 : 0);
    c[0][0] = peer;
    m_peer_rank = m_topo.rank(c);
}

} // namespace p2p
} // namespace ghexbench
