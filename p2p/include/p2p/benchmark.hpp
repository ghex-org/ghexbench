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
#pragma once

#include <string>
#include <memory>
#include <vector>

#include <oomph/context.hpp>
#include <oomph/communicator.hpp>

#include <ghexbench/mpi_environment.hpp>
#include <ghexbench/thread_affinity.hpp>
#include <ghexbench/thread_pool.hpp>
#include <ghexbench/options.hpp>
#include <ghexbench/timer.hpp>

namespace ghexbench
{
namespace p2p
{

class benchmark
{
  private:
    struct thread_state
    {
        oomph::communicator                      comm;
        oomph::rank_type                         peer_rank;
        oomph::tag_type                          stag;
        oomph::tag_type                          rtag;
        std::vector<oomph::message_buffer<char>> smsgs;
        std::vector<oomph::message_buffer<char>> rmsgs;
        timer                                    wall_clock;

        thread_state(oomph::communicator&& c, std::size_t size, std::size_t window, int thread_id,
            oomph::rank_type peer, [[maybe_unused]] int device_id)
        : comm{std::move(c)}
        , peer_rank{peer}
        , stag{thread_id}
        , rtag{thread_id}
        {
            smsgs.reserve(window);
            rmsgs.reserve(window);
            for (std::size_t i = 0; i < window; ++i)
            {
#ifdef P2P_ENABLE_DEVICE
                smsgs.push_back(comm.make_device_buffer<char>(size, device_id));
                rmsgs.push_back(comm.make_device_buffer<char>(size, device_id));
#else
                smsgs.push_back(comm.make_buffer<char>(size));
                rmsgs.push_back(comm.make_buffer<char>(size));
#endif
                for (auto& c : smsgs[i]) c = 1;
                for (auto& c : rmsgs[i]) c = -1;
#ifdef P2P_ENABLE_DEVICE
                smsgs[i].clone_to_device();
                rmsgs[i].clone_to_device();
#endif
            }
        }
    };

    options              m_opts;
    options_values       m_options;
    bool                 m_is_multithreaded;
    mpi_environment      m_mpi_env;
    oomph::context       m_ctx;
    std::size_t          m_threads;
    thread_pool          m_thread_pool;
    thread_pool::barrier m_thread_barrier;
    std::size_t          m_size;
    std::size_t          m_window;
    std::size_t          m_nrep;
    oomph::rank_type     m_peer_rank;
    int                  m_device_id = 0;
    timer                m_wall_clock;

    std::vector<std::unique_ptr<thread_state>> m_thread_states;

  public:
    benchmark(int& argc, char**& argv)
    : m_opts{ghexbench::options() /* clang-format off */
    ("mt", "enable multithreading")
    ("smt", "enable symmetric multithreading (hyper-threading), requires mt")
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
    , m_ctx{MPI_COMM_WORLD, m_is_multithreaded}
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
        if (m_ctx.size() % 2 != 0) abort("number of ranks not divisible by 2", m_ctx.rank() == 0);
        m_peer_rank = (m_ctx.rank() + (m_ctx.size() / 2) + m_ctx.size()) % m_ctx.size();
    }

    void run();

  private:
    int set_device();

    void init(int thread_id);

    void clear(int thread_id);

    void print_locality(int thread_id);

    void check(int thread_id);

    void warm_up(int thread_id);

    void main_loop(int thread_id);

    void send_recv(int thread_id, std::size_t n);

    static void abort(std::string const& msg, bool print = true);
};

} // namespace p2p
} // namespace ghexbench
