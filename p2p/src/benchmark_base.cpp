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
#include <exception>
#include <iomanip>

#include <p2p/benchmark.hpp>
#include <p2p/device_map.hpp>
#include <p2p/callbacks.hpp>

namespace ghexbench
{
namespace p2p
{

template<typename Derived>
benchmark_base<Derived>::thread_state::thread_state(oomph::communicator&& c, std::size_t size,
    std::size_t window, int thread_id, oomph::rank_type peer)
: comm{std::move(c)}
, peer_rank{peer}
, stag{thread_id}
, rtag{thread_id}
{
    smsgs.reserve(window);
    rmsgs.reserve(window);
    for (unsigned i = 0; i < window; ++i)
    {
#ifdef P2P_ENABLE_DEVICE
        smsgs.push_back(comm.make_device_buffer<char>(size));
        rmsgs.push_back(comm.make_device_buffer<char>(size));
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

template<typename Derived>
benchmark_base<Derived>::benchmark_base(int& argc, char**& argv, std::size_t num_threads_)
: m_opts{ ghexbench::options()
    ("size",      "buffer size",                                 "s", {1024})
    ("inflights", "number of simultaneous inflights per thread", "i", {1})
    ("nrep",      "number of messages per thread",               "n", {100})
#ifdef P2P_ENABLE_DEVICE
    ("devicemap",  "rank to device map (per node) in the form i1:i2:...:iN (N=number of ranks per node)",  "d", 1)
#endif
    } /* clang-format on */
, m_options{Derived::add_options(m_opts).parse(argc, argv)}
, m_threads{num_threads_}
, m_mpi_env{m_threads > 1, argc, argv}
, m_thread_pool{(int)num_threads_}
, m_thread_barrier{m_thread_pool.make_barrier()} /* clang-format off */
, m_size{m_options.get<std::size_t>("size")}
, m_window{m_options.get<std::size_t>("inflights")}
, m_nrep{m_options.get<std::size_t>("nrep")}
, m_thread_states{num_threads_}
{
}

template<typename Derived>
void
benchmark_base<Derived>::abort(std::string const& msg, bool print)
{
    if (print) std::cerr << "aborting: " << msg << std::endl;
    std::terminate();
}

template<typename Derived>
int
benchmark_base<Derived>::set_device()
{
#ifndef P2P_ENABLE_DEVICE
    return m_device_id;
#else
    auto& ctx = static_cast<Derived*>(this)->m_ctx;
    auto const local_size = ctx.local_size();
    device_map dmap(m_options.get_or<std::string>("devicemap", ""));
    if (dmap.size() < local_size)
        abort("device map does not match number of processes per node", m_mpi_env.rank == 0);
    m_device_id = dmap[ctx.local_rank()];
    int const num_devices = hwmalloc::get_num_devices();
    if ((m_device_id >= num_devices) || (m_device_id < 0))
        abort("device id is out of range", m_mpi_env.rank == 0);
    hwmalloc::set_device_id(m_device_id);
    return m_device_id;
#endif
}

template<typename Derived>
void
benchmark_base<Derived>::print_locality(int thread_id)
{
    auto const node = static_cast<Derived*>(this)->m_topo.level_grid_coord()[0][0];
    auto&      comm = m_thread_states[thread_id]->comm;
    int const  w = 7;

    for (int r = 0; r < comm.size(); ++r)
    {
        if (r == comm.rank())
        {
            for (int tid = 0; tid < (int)m_threads; ++tid)
            {
                if (tid == thread_id)
                {
                    if (r == 0 && tid == 0)
                        std::cout << std::setw(w) << "node" << std::setw(w) << "rank"
                                  << std::setw(w) << "peer" << std::setw(w) << "thread"
                                  << std::setw(w) << "cpu"
#ifdef P2P_ENABLE_DEVICE
                                  << std::setw(w) << "device"
#endif
                                  << std::endl;

                    std::cout << std::setw(w) << node << std::setw(w) << comm.rank() << std::setw(w)
                              << m_peer_rank << std::setw(w) << thread_id << std::setw(w)
                              << ghexbench::get_cpu()
#ifdef P2P_ENABLE_DEVICE
                              << std::setw(w) << device_id
#endif
                              << std::endl;
                }
                m_thread_barrier();
            }
        }
        if (thread_id == 0) MPI_Barrier(comm.mpi_comm());
    }
}

template<typename Derived>
void
benchmark_base<Derived>::init(int thread_id)
{
    auto& ctx = static_cast<Derived*>(this)->m_ctx;
    m_thread_states[thread_id] = std::make_unique<thread_state>(ctx.get_communicator(), m_size,
        m_window, thread_id, m_peer_rank);
}

template<typename Derived>
void
benchmark_base<Derived>::clear(int thread_id)
{
    m_thread_states[thread_id].reset();
}

template<typename Derived>
void
benchmark_base<Derived>::warm_up(int thread_id)
{
    auto& state = *m_thread_states[thread_id];
    auto& comm = state.comm;

    m_thread_barrier();
    if (thread_id == 0) MPI_Barrier(comm.mpi_comm());
    m_thread_barrier();

    for (unsigned i = 0; i < m_window; ++i)
    {
        comm.recv(state.rmsgs[i], m_peer_rank, thread_id + i * 1000,
            ghexbench::p2p::recv_callback{comm, 100});
        comm.send(state.smsgs[i], m_peer_rank, thread_id + i * 1000,
            ghexbench::p2p::send_callback{comm, 100});
    }

    comm.wait_all();
}

template<typename Derived>
void
benchmark_base<Derived>::main_loop(int thread_id)
{
    auto& state = *m_thread_states[thread_id];
    auto& comm = state.comm;

    m_thread_barrier();
    if (thread_id == 0) MPI_Barrier(comm.mpi_comm());
    m_thread_barrier();

    state.wall_clock.tic();

    for (unsigned i = 0; i < m_window; ++i)
    {
        comm.recv(state.rmsgs[i], m_peer_rank, thread_id + i * 1000,
            ghexbench::p2p::recv_callback{comm, (unsigned)m_nrep});
        comm.send(state.smsgs[i], m_peer_rank, thread_id + i * 1000,
            ghexbench::p2p::send_callback{comm, (unsigned)m_nrep});
    }

    comm.wait_all();

    state.wall_clock.toc();
}

template<typename Derived>
void
benchmark_base<Derived>::run()
{
    m_peer_rank = static_cast<Derived*>(this)->peer_rank();

#ifndef P2P_ENABLE_DEVICE
    set_device();
#endif

    for (std::size_t i = 0; i < m_threads; ++i)
    {
        m_thread_pool.schedule(i, [this](int i) { init(i); });
        m_thread_pool.schedule(i, [this](int i) { print_locality(i); });
        m_thread_pool.schedule(i, [this](int i) { warm_up(i); });
        m_thread_pool.schedule(i, [this](int i) { main_loop(i); });
    }

    m_thread_pool.sync();

    if (m_mpi_env.rank == 0)
    {
        double const      elapsed = m_thread_states[0]->wall_clock.sum(); // in microseconds
        std::size_t const num_msgs_per_rank = m_threads * m_window * m_nrep;
        std::size_t const num_msgs = num_msgs_per_rank * m_mpi_env.size;
        double const      bibw = (m_size / elapsed) * (num_msgs_per_rank * m_mpi_env.size * 2);

        auto print_line = [](char const* description, auto value)
        {
            std::cout << std::left << std::setw(32) << description << std::right << std::setw(12)
                      << value << std::endl;
        };

        std::cout << std::endl;
        print_line("elapsed (s)", elapsed * 1.0e-6);
        print_line("message size (bytes)", m_size);
        print_line("number of ranks", m_mpi_env.size);
        print_line("number of threads", m_threads);
        print_line("number of inflights", m_window);
        print_line("number of repetitions", m_nrep);
        print_line("total message count", num_msgs);
        print_line("bidirectional bandwidth (GB/s)", bibw);
        std::cout << std::endl;
    }

    for (std::size_t i = 0; i < m_threads; ++i)
        m_thread_pool.schedule(i, [this](int i) { clear(i); });

    m_thread_pool.join();
}

template class benchmark_base<benchmark>;

} // namespace p2p
} // namespace ghexbench
