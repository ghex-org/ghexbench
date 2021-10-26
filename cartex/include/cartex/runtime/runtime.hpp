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

#pragma once

#include <array>
#include <mutex>
#include <memory>
#include <string>

#include <cartex/common/memory.hpp>
#include <cartex/common/options.hpp>
#include <cartex/decomposition/decomposition.hpp>
#include <cartex/common/sync_loop.hpp>
#include <cartex/common/thread_pool.hpp>

namespace cartex
{
class runtime
{
  public:
    using real_type = CARTEX_FLOAT;
    using memory_type = memory<real_type>;
    using domain_type = decomposition::domain_t;

  private:
    decomposition&                        m_decomposition;
    const int                             m_rank;
    const int                             m_size;
    const int                             m_num_reps;
    bool                                  m_use_timer;
    double                                m_time;
    const int                             m_num_threads;
    const bool                            m_mt;
    const int                             m_num_fields;
    const bool                            m_check_res;
    const int                             m_halo;
    const std::array<int, 6>              m_halos;
    const std::array<int, 3>              m_offset;
    std::vector<domain_type>              m_domains;
    std::vector<std::vector<memory_type>> m_raw_fields;
    std::mutex                            m_mutex;
    sync_loop                             m_loop;
    class impl;
    friend class impl;
    std::unique_ptr<impl> m_impl;

  public:
    static options& add_options(options& opts);
    static bool     check_options(options_values const& opts);

  public:
    runtime(const options_values& options, decomposition& decomp_);
    ~runtime();
    runtime(const runtime&) = delete;
    void        init(int);
    void        exit(int);
    void        exchange(int j, thread_pool::barrier& b);
    std::string info() const;

  private:
    void step(int);
    void make_fields(int j);
    void print_fields(int j);
    bool check(int j);
};

} // namespace cartex
