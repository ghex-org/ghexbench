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

namespace cartex
{
runtime::runtime(int num_reps, int halo, int num_fields, bool check_res, decomposition& decomp_)
: m_decomposition(decomp_)
, m_rank(m_decomposition.rank())
, m_size(m_decomposition.size())
, m_num_reps{num_reps}
, m_num_threads(m_decomposition.threads_per_rank())
, m_mt(m_num_threads > 1)
, m_num_fields{num_fields}
, m_check_res(check_res)
, m_halos{halo, halo, halo, halo, halo, halo}
, m_offset{m_halos[0], m_halos[2], m_halos[4]}
, m_domains{decomp_.domains()}
, m_raw_fields(m_num_threads)
, m_impl{std::make_unique<impl>(*this)}
{
}

runtime::~runtime() = default;

std::string
runtime::info() const
{
    return m_impl->info();
}

void
runtime::init(int j)
{
    m_impl->init(j);
}

void
runtime::step(int j)
{
    m_impl->step(j);
}
} // namespace cartex
