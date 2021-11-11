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

#include <vector>

#include <cartex/runtime/runtime.hpp>
#include <gridtools/gcl/GCL.hpp>
#include <gridtools/gcl/halo_exchange.hpp>

namespace cartex
{
namespace gt = gridtools;

class runtime::impl
{
  private:
    using pattern_type = gt::gcl::halo_exchange_dynamic_ut<gt::layout_map<2, 1, 0>,
        gt::layout_map<0, 1, 2>, runtime::real_type,
#ifdef __CUDACC__
        gt::gcl::gpu
#else
        gt::gcl::cpu
#endif
        >;

  private:
    runtime&                         m_base;
    pattern_type                     m_pattern;
    std::vector<runtime::real_type*> m_field_ptrs;

  public:
    impl(runtime& base, options_values const& options);

    void        init(int);
    void        step(int j);
    void        exit(int j);
    std::string info() const
    {
#define CARTEX_STR2(var) #var
#define CARTEX_STR(var)  CARTEX_STR2(var)
        return CARTEX_STR(CARTEX_RUNTIME);
#undef CARTEX_STR
#undef CARTEX_STR2
    }
};

} // namespace cartex
