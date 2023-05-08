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
#include <iostream>
#include <exception>
#include <iomanip>
#include <numeric>

#include <p2p/benchmark.hpp>
#include <p2p/device_map.hpp>
#include <p2p/callbacks.hpp>

namespace ghexbench
{
namespace p2p
{

void
benchmark::abort(std::string const& msg, bool print)
{
    if (print) std::cerr << "aborting: " << msg << std::endl;
    std::terminate();
}

int
benchmark::set_device()
{
#ifndef P2P_ENABLE_DEVICE
    return m_device_id;
#else
    auto const local_size = m_ctx.local_size();
    device_map dmap(m_options.get_or<std::string>("devicemap", ""));
    if ((int)dmap.size() < local_size)
        abort("device map does not match number of processes per node", m_mpi_env.rank == 0);
    m_device_id = dmap[m_ctx.local_rank()];
    int const num_devices = hwmalloc::get_num_devices();
    if ((m_device_id >= num_devices) || (m_device_id < 0))
        abort("device id is out of range", m_mpi_env.rank == 0);
    hwmalloc::set_device_id(m_device_id);
    return m_device_id;
#endif
}

void
benchmark::print_locality(int thread_id)
{
    auto& comm = m_thread_states[thread_id]->comm;

    auto print_cell = [](auto item) { std::cout << std::setw(7) << item; };
    auto print_row = [&print_cell](auto... items)
    {
        ((void)print_cell(items), ...);
        std::cout << std::endl;
        std::cout.flush();
    };

    if (comm.rank() == 0 && thread_id == 0)
    {
#ifndef P2P_ENABLE_DEVICE
        print_row("rank", "peer", "thread", "cpu");
#else
        print_row("rank", "peer", "thread", "cpu", "device");
#endif
    }
    for (int r = 0; r < comm.size(); ++r)
    {
        if (thread_id == 0) MPI_Barrier(comm.mpi_comm());
        m_thread_barrier();
        if (r == comm.rank())
        {
            for (int tid = 0; tid < (int)m_threads; ++tid)
            {
                if (tid == thread_id)
                {
#ifndef P2P_ENABLE_DEVICE
                    print_row(comm.rank(), m_peer_rank, thread_id, ghexbench::get_cpu());
#else
                    print_row(comm.rank(), m_peer_rank, thread_id, ghexbench::get_cpu(),
                        m_on_device ? std::to_string(m_device_id) : "-");
#endif
                }
                m_thread_barrier();
            }
        }
        if (thread_id == 0) MPI_Barrier(comm.mpi_comm());
        m_thread_barrier();
    }
}

void
benchmark::init(int thread_id)
{
    m_thread_states[thread_id] = std::make_unique<thread_state>(m_ctx.get_communicator(), m_size,
        m_window, thread_id, m_peer_rank, m_device_id, m_on_device);
}

void
benchmark::clear(int thread_id)
{
    m_thread_states[thread_id].reset();
}

void
benchmark::send_recv(int thread_id, std::size_t n)
{
    auto& state = *m_thread_states[thread_id];
    auto& comm = state.comm;

    for (std::size_t i = 0; i < m_window; ++i)
    {
        comm.recv(state.rmsgs[i], m_peer_rank, thread_id + i * 1000,
            ghexbench::p2p::recv_callback{comm, n});
        comm.send(state.smsgs[i], m_peer_rank, thread_id + i * 1000,
            ghexbench::p2p::send_callback{comm, n});
    }
}

void
benchmark::check(int thread_id)
{
    auto& state = *m_thread_states[thread_id];
    auto& comm = state.comm;

    m_thread_barrier();
    if (thread_id == 0) MPI_Barrier(comm.mpi_comm());
    m_thread_barrier();

    send_recv(thread_id, 1);

    comm.wait_all();

    bool result = true;
    for (std::size_t i = 0; i < m_window; ++i)
    {
#ifdef P2P_ENABLE_DEVICE
        if (m_on_device) state.rmsgs[i].clone_to_host();
#endif
        for (auto c : state.rmsgs[i])
        {
            if (c != 1)
            {
                result = false;
                break;
            }
        }
        if (!result) break;
    }
    if (!result) abort("wrong result received");
}

void
benchmark::warm_up(int thread_id)
{
    auto& state = *m_thread_states[thread_id];
    auto& comm = state.comm;

    m_thread_barrier();
    if (thread_id == 0) MPI_Barrier(comm.mpi_comm());
    m_thread_barrier();

    send_recv(thread_id, 100);

    comm.wait_all();
}

void
benchmark::main_loop(int thread_id)
{
    auto& state = *m_thread_states[thread_id];
    auto& comm = state.comm;

    m_thread_barrier();
    if (thread_id == 0) MPI_Barrier(comm.mpi_comm());
    m_thread_barrier();

    state.wall_clock.tic();

    send_recv(thread_id, m_nrep);

    //comm.wait_all();
    while (!comm.is_ready())
    {
        for (std::size_t n = 0; n < 100; ++n) comm.progress();
    }

    //state.wall_clock.toc();
}

void
benchmark::run()
{
#ifdef P2P_ENABLE_DEVICE
    set_device();
#endif
    auto print_line = [](char const* description, auto value)
    {
        std::cout << std::left << std::setw(32) << description << std::right << std::setw(12)
                  << value << std::endl;
    };

    m_wall_clock.tic();
    for (std::size_t i = 0; i < m_threads; ++i)
    {
#ifdef P2P_ENABLE_DEVICE
        m_thread_pool.schedule(i, [this](int) { hwmalloc::set_device_id(m_device_id); });
#endif
        m_thread_pool.schedule(i, [this](int i) { init(i); });
        m_thread_pool.schedule(i, [this](int i) { print_locality(i); });
        m_thread_pool.schedule(i, [this](int i) { check(i); });
        m_thread_pool.schedule(i, [this](int i) { warm_up(i); });
    }
    m_thread_pool.sync();

    double elapsed_warm_up = m_wall_clock.toc();
    if (m_mpi_env.rank == 0)
    {
        std::cout << std::endl;
        print_line("memory locatlity",
            (m_on_device ? "D" : "H") + std::string(" ") + (m_peer_on_device ? "D" : "H"));
        print_line("elapsed warm up (s)", elapsed_warm_up * 1.0e-6);
    }

    for (std::size_t i = 0; i < m_threads; ++i)
    {
        m_thread_pool.schedule(i, [this](int i) { main_loop(i); });
    }

    m_thread_pool.sync();
    MPI_Barrier(m_ctx.mpi_comm());
    m_thread_states[0]->wall_clock.toc();

    std::vector<std::size_t> all_m_threads(m_ctx.size());
    MPI_Gather(&m_threads, sizeof(std::size_t), MPI_BYTE, all_m_threads.data(), sizeof(std::size_t),
        MPI_BYTE, 0, m_ctx.mpi_comm());

    if (m_mpi_env.rank == 0)
    {
        double const      elapsed = m_thread_states[0]->wall_clock.sum(); // in microseconds
        std::size_t const num_all_threads =
            std::accumulate(all_m_threads.begin(), all_m_threads.end(), std::size_t(0u));
        std::size_t const num_msgs = num_all_threads * m_window * m_nrep;
        double const      bibw = (m_size / (1000.0 * elapsed)) * num_msgs;

        print_line("elapsed (s)", elapsed * 1.0e-6);
        print_line("message size (bytes)", m_size);
        print_line("number of ranks", m_mpi_env.size);
        print_line("number of threads on rank 0", m_threads);
        print_line("number of threads total", num_all_threads);
        print_line("number of inflights", m_window);
        print_line("number of repetitions", m_nrep);
        print_line("total message count", num_msgs);
        print_line("bidirectional bandwidth (GB/s)", bibw);
        std::cout << std::endl;
    }

    for (std::size_t i = 0; i < m_threads; ++i)
        m_thread_pool.schedule(i, [this](int i) { clear(i); });

    m_thread_pool.sync();
    MPI_Barrier(m_ctx.mpi_comm());

    m_thread_pool.join();
}

} // namespace p2p
} // namespace ghexbench
