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

#include <cartex/common/memory.hpp>
#include <cartex/common/memory_view.hpp>
#include <cartex/decomposition/decomposition.hpp>

namespace cartex
{
template<class Derived>
class runtime_base
{
  public:
    using real_t = CARTEX_FLOAT;

  public:
    runtime_base(
        int num_reps_, int ext_, int halo, int num_fields_, bool check_, decomposition& decomp_)
    {
    }
};

} // namespace cartex
