/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

extern "C"
{
#include <sched.h>
#include <unistd.h>
}
#include <memory>
#include <stdexcept>
#include <chrono>
#include <iostream>

#include <cartex/common/thread_pool.hpp>

namespace cartex
{
namespace _impl
{
static constexpr int s_num_ht = 2;
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

thread_pool::thread_pool(int n)
: m_num_threads{n}
, m_thread_wrapper(n)
{
    int            num_cpus = std::thread::hardware_concurrency();
    _impl::cpu_set m_this_cpu_set(getpid());
    for (int c = 0; c < num_cpus; ++c)
        if (m_this_cpu_set.is_set(c)) m_this_cpus.push_back(c);
    m_num_resources = m_this_cpus.size() / _impl::s_num_ht;
    if (m_num_threads > m_num_resources)
        std::cerr << "warning: more threads in thread pool than hardware resources" << std::endl;
    if (m_num_threads < m_num_resources)
        std::cerr << "warning: less threads in thread pool than hardware resources" << std::endl;

    auto worker_fct = [this](int thread_id, int cpu_0, int cpu_1) {
        for (int i = 0; i < 5; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            const auto cpu = sched_getcpu();
            if (cpu == cpu_0 || cpu == cpu_1) break;
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
        const auto cpu_1 = m_this_cpus[(i + m_num_resources) % m_this_cpus.size()];
        auto&      t = m_thread_wrapper[i];

        _impl::cpu_set s;
        s.set(cpu_0);
        s.set(cpu_1);
        t.m_thread = thread_type(worker_fct, i, cpu_0, cpu_1);
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
            { locks[i] = std::make_unique<lock_type>(m_thread_wrapper[i].m_mutex); }
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
