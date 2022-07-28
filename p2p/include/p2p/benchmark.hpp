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

#include <string>
#include <memory>
#include <vector>

#include <oomph/context.hpp>
#include <oomph/communicator.hpp>

#include <ghexbench/mpi_environment.hpp>
#include <ghexbench/hw_topo.hpp>
#include <ghexbench/thread_pool.hpp>
#include <ghexbench/options.hpp>
#include <ghexbench/timer.hpp>

namespace ghexbench
{
namespace p2p
{

template<typename Derived>
class benchmark_base
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
            oomph::rank_type peer);
    };

  protected:
    options              m_opts;
    options_values       m_options;
    std::size_t          m_threads;
    mpi_environment      m_mpi_env;
    thread_pool          m_thread_pool;
    thread_pool::barrier m_thread_barrier;
    std::size_t          m_size;
    std::size_t          m_window;
    std::size_t          m_nrep;
    oomph::rank_type     m_peer_rank = 0;
    int                  m_device_id = 0;
    timer                m_wall_clock;

    std::vector<std::unique_ptr<thread_state>> m_thread_states;

  protected:
    static void abort(std::string const& msg, bool print = true);

  public:
    benchmark_base(int& argc, char**& argv, std::size_t num_threads_);

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
};

class benchmark : public benchmark_base<benchmark>
{
  private:
    friend class benchmark_base<benchmark>;
    using base = benchmark_base<benchmark>;
    using base::abort;

  private:
    hw_topo        m_topo;
    oomph::context m_ctx;

  public:
    benchmark(int& argc, char**& argv);

  private:
    static options& add_options(options& opts);

  private:
    oomph::rank_type peer_rank();
};

} // namespace p2p
} // namespace ghexbench
