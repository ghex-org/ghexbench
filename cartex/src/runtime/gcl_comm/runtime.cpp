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

#include <array>
#include <iostream>

#include <cartex/runtime/gcl_comm/runtime.hpp>
#include "../runtime_inc.cpp"

namespace cartex
{
options&
runtime::add_options(options& opts)
{
    return opts;
}

bool
runtime::check_options(options_values const& opts)
{
    // no threading
    const auto threads = opts.get<std::array<int, 3>>("thread");
    const auto num_threads = threads[0] * threads[1] * threads[2];
    if (num_threads > 1)
    {
        std::cerr << "GCL backend does not support threading" << std::endl;
        return false;
    }
    return true;
}

runtime::impl::impl(runtime& base, options_values const& options)
: m_base(base)
, m_pattern(typename pattern_type::grid_type::period_type{true, true, true},
      base.m_decomposition.mpi_cart_comm())
{
    const auto halo = options.get<int>("halo");
    m_pattern.add_halo<0>(halo, halo, halo, m_base.m_domains[0].domain_ext[0] + halo - 1,
        m_base.m_domains[0].domain_ext[0] + 2 * halo);
    m_pattern.add_halo<1>(halo, halo, halo, m_base.m_domains[0].domain_ext[1] + halo - 1,
        m_base.m_domains[0].domain_ext[1] + 2 * halo);
    m_pattern.add_halo<2>(halo, halo, halo, m_base.m_domains[0].domain_ext[2] + halo - 1,
        m_base.m_domains[0].domain_ext[2] + 2 * halo);
}

void
runtime::impl::init(int)
{
    for (auto& f : m_base.m_raw_fields[0]) m_field_ptrs.push_back(f.hd_data());
    m_pattern.setup(m_base.m_num_fields);
}

void
runtime::impl::step(int)
{
    m_pattern.pack(m_field_ptrs);
    m_pattern.exchange();
    m_pattern.unpack(m_field_ptrs);
}

void
runtime::impl::exit(int)
{
}

} // namespace cartex
