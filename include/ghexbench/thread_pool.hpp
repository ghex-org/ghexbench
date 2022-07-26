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

#include <thread>
#include <condition_variable>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>

namespace ghexbench
{
int get_cpu() noexcept;
int num_cpus() noexcept;

class thread_pool
{
  private:
    using thread_type = std::thread;
    using function_type = std::function<void(int)>;
    using queue_type = std::queue<function_type>;
    using mutex_type = std::mutex;
    using lock_type = std::unique_lock<mutex_type>;
    using cv_type = std::condition_variable;

    struct thread_wrapper
    {
        thread_type m_thread;
        cv_type     m_cv;
        mutex_type  m_mutex;
        queue_type  m_queue;
    };

  public:
    class barrier
    {
      private:
        friend class thread_pool;
        struct counters
        {
            std::atomic<int> m_up;
            std::atomic<int> m_down;
            counters(int up, int down)
            : m_up{up}
            , m_down{down}
            {
            }
        };

      private:
        int                       m_num_threads;
        std::unique_ptr<counters> m_counters;

      public:
        barrier(int num_threads = 1);
        barrier(barrier const&) = delete;
        barrier(barrier&&) = default;

      private:
        bool count_up();
        void count_down();

      public:
        void operator()();
    };

  private:
    const int                   m_num_threads;
    std::vector<thread_wrapper> m_thread_wrapper;
    std::vector<int>            m_this_cpus;
    int                         m_num_resources;
    bool                        m_running = true;
    bool                        m_joined = false;

  public:
    thread_pool(int n);

    ~thread_pool() { join(); }

    void join();

    void sync();

    template<typename Function>
    bool schedule(int thread_id, Function&& fct)
    {
        return schedule_impl(thread_id, function_type(std::forward<Function>(fct)));
    }

    barrier make_barrier();

  private:
    bool schedule_impl(int thread_id, function_type&& fct);
};

} // namespace ghexbench
