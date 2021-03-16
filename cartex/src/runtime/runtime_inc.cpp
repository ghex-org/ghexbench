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
runtime::runtime(const options_values& options, decomposition& decomp_)
: m_decomposition(decomp_)
, m_rank(m_decomposition.rank())
, m_size(m_decomposition.size())
, m_num_reps{options.has("time") ? options.get_or("nrep", 0) : options.get_or("nrep", 10)}
, m_use_timer{options.has("time")}
, m_time{options.get_or("time", 0.0)}
, m_num_threads(m_decomposition.threads_per_rank())
, m_mt(m_num_threads > 1)
, m_num_fields{options.get<int>("nfields")}
, m_check_res(options.is_set("check"))
, m_halo{options.get<int>("halo")}
, m_halos{m_halo, m_halo, m_halo, m_halo, m_halo, m_halo}
, m_offset{m_halos[0], m_halos[2], m_halos[4]}
, m_domains{decomp_.domains()}
, m_raw_fields(m_num_threads)
, m_loop(m_decomposition.mpi_comm(), m_num_threads)
, m_impl{std::make_unique<impl>(*this, options)}
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
