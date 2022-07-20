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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
extern "C"
{
#include <sched.h>
#include <unistd.h>
}
#include <memory>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <iostream>

#include <ghexbench/thread_pool.hpp>

namespace ghexbench
{
namespace _impl
{
static constexpr int s_num_ht = 2; //CARTEX_NUM_HT;
struct cpu_set
{
    mutable cpu_set_t m_cpuset;

    cpu_set() noexcept { CPU_ZERO(&m_cpuset); }

    cpu_set(pid_t pid)
    {
        if (0 != sched_getaffinity(pid, sizeof(cpu_set_t), &m_cpuset))
            throw std::runtime_error("could not get thread affinity mask");
    }

    bool is_set(int cpu) const noexcept { return CPU_ISSET(cpu, &m_cpuset); }

    void set(int cpu) noexcept { CPU_SET(cpu, &m_cpuset); }
};

} // namespace _impl

thread_pool::barrier::barrier(int num_threads)
: m_num_threads{num_threads}
, m_counters{new counters{0, num_threads}}
{
}

void
thread_pool::barrier::operator()()
{
    if (m_num_threads > 1)
    {
        if (!count_up())
            while (m_counters->m_down == m_num_threads) {}
        count_down();
    }
}

bool
thread_pool::barrier::count_up()
{
    int expected = m_counters->m_up;
    while (
        !m_counters->m_up.compare_exchange_weak(expected, expected + 1, std::memory_order_relaxed))
        expected = m_counters->m_up;
    if (expected == m_num_threads - 1)
    {
        m_counters->m_up.store(0);
        return true;
    }
    else
    {
        while (m_counters->m_up != 0) {}
        return false;
    }
}

void
thread_pool::barrier::count_down()
{
    int ex = m_counters->m_down;
    while (!m_counters->m_down.compare_exchange_weak(ex, ex - 1, std::memory_order_relaxed))
        ex = m_counters->m_down;
    if (ex == 1) { m_counters->m_down.store(m_num_threads); }
    else
    {
        while (m_counters->m_down != m_num_threads) {}
    }
}

thread_pool::barrier
thread_pool::make_barrier()
{
    return {m_num_threads};
}

thread_pool::thread_pool(int n)
: m_num_threads{n}
, m_thread_wrapper(n)
{
    //int            num_cpus = std::thread::hardware_concurrency();
    _impl::cpu_set m_this_cpu_set(getpid());
    //const int current_cpu = sched_getcpu();
    for (int c0 = 0; c0 < CPU_SETSIZE; ++c0)
    {
        //const int c = (current_cpu + c0) % CPU_SETSIZE;
        if (m_this_cpu_set.is_set(c0))
        {
            m_this_cpus.push_back(c0);
            //std::cout << "cpu " << c0 << std::endl;
        }
    }

    // number of cores
    m_num_resources = m_this_cpus.size();// / _impl::s_num_ht;
    //std::cout << "number of resources = " << m_num_resources << std::endl;
    if (m_num_threads > m_num_resources)
    {
    //    if (m_num_threads <= (int)m_this_cpus.size())
    //        std::cerr
    //            << "warning: more threads in thread pool than physical hardware resources: using hyperthreading"
    //            << std::endl;
    //    else
            std::cerr << "warning: more threads in thread pool than hardware resources"
                      << std::endl;
    }
    //if (m_num_threads < m_num_resources)
    //    std::cerr << "warning: less threads in thread pool than hardware resources" << std::endl;

    auto worker_fct = [this](int thread_id, int cpu_0)
    {
        for (int i = 0; i < 5; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            const auto cpu = sched_getcpu();
            if (cpu == cpu_0)
            {
                //std::cout <<  "thread : " << thread_id << " on cpu " << cpu_0 << std::endl;
                break;
            }
            if (i == 4)
                std::cerr << "warning: did not manage to set correct thread affinity" << std::endl;
        }

        while (true)
        {
            function_type job;
            {
                auto&     t = m_thread_wrapper[thread_id];
                lock_type lock(t.m_mutex);
                t.m_cv.wait(lock, [this, &t] { return !t.m_queue.empty() || !m_running; });
                if (t.m_queue.empty()) break;
                job = std::move(t.m_queue.front());
                t.m_queue.pop();
            }
            job(thread_id);
        }
    };

    for (int i = 0; i < m_num_threads; ++i)
    {
        const auto cpu_0 = m_this_cpus[i % m_this_cpus.size()];
        auto&      t = m_thread_wrapper[i];

        _impl::cpu_set s;
        s.set(cpu_0);
        t.m_thread = thread_type(worker_fct, i, cpu_0);
        auto rc =
            pthread_setaffinity_np(t.m_thread.native_handle(), sizeof(cpu_set_t), &s.m_cpuset);
        if (rc != 0) std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
}

void
thread_pool::join()
{
    if (!m_joined)
    {
        {
            std::vector<std::unique_ptr<lock_type>> locks(m_num_threads);
            for (int i = 0; i < m_num_threads; ++i)
            {
                locks[i] = std::make_unique<lock_type>(m_thread_wrapper[i].m_mutex);
            }
            m_running = false;
        }
        for (auto& t : m_thread_wrapper)
        {
            t.m_cv.notify_one();
            t.m_thread.join();
        }
        m_joined = true;
    }
}

void
thread_pool::sync()
{
    for (int j = 0; j < m_num_threads; ++j) schedule(j, [](int) {});
    while (std::count_if(m_thread_wrapper.begin(), m_thread_wrapper.end(),
        [](const auto& tw) { return tw.m_queue.size() > 0u; }))
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}

bool
thread_pool::schedule_impl(int thread_id, function_type&& fct)
{
    if (m_joined)
    {
        std::cerr << "warning: thread pool already joined" << std::endl;
        return false;
    }

    auto& t = m_thread_wrapper[thread_id];
    {
        lock_type lock(t.m_mutex);
        t.m_queue.emplace(std::move(fct));
    }
    t.m_cv.notify_one();
    return true;
}

} // namespace cartex
