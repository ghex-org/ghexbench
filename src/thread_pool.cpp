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

#include <chrono>
#include <algorithm>
#include <iostream>
#include <vector>

#include <ghexbench/thread_affinity.hpp>
#include <ghexbench/thread_pool.hpp>

namespace ghexbench
{

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
    auto worker_fct = [this](int thread_id, int count)
    {
        set_affinity(thread_id, count);
        //const auto cpu = get_cpu();
        //std::cout <<  "thread : " << thread_id << " on cpu " << cpu << std::endl;

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
        m_thread_wrapper[i].m_thread = thread_type(worker_fct, i, m_num_threads);
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
        std::this_thread::sleep_for(10us);
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

} // namespace ghexbench
