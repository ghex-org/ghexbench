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
#include <cartex/decomposition/decomposition.hpp>

namespace cartex
{
class runtime
{
  public:
    using real_type = CARTEX_FLOAT;
    using memory_type = memory<real_type>;

  private:
    decomposition&                        m_decomposition;
    const int                             m_rank;
    const int                             m_size;
    const int                             m_num_reps;
    const int                             m_num_threads;
    const bool                            m_mt;
    const int                             m_num_fields;
    const bool                            m_check_res;
    const std::array<int, 3>              m_ext;
    const std::array<int, 3>              m_g_first;
    const std::array<int, 3>              m_g_last;
    const std::array<int, 3>              m_offset;
    const std::array<int, 6>              m_halos;
    const std::array<int, 3>              m_ext_buffer;
    const int                             m_max_memory;
    std::vector<std::vector<memory_type>> m_raw_fields;
    std::mutex                            m_mutex;
    class impl;
    friend class impl;
    std::unique_ptr<impl> m_impl;

  public:
    runtime(int num_reps, const std::array<int, 3>& ext, int halo, int num_fields, bool check_res,
        decomposition& decomp_);
    ~runtime();
    runtime(const runtime&) = delete;
    void        exchange(int j);
    std::string info() const;

  private:
    void init(int);
    void step(int);
    void make_fields(int j);
    void print_fields(int j);
    bool check(int jj);
};

} // namespace cartex
