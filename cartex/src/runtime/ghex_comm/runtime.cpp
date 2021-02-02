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

#include <cartex/runtime/ghex_comm/runtime.hpp>

namespace cartex
{
runtime::runtime(
    int num_reps_, int ext_, int halo, int num_fields_, bool check_, decomposition& decomp_)
: base(num_reps_, ext_, halo, num_fields_, check_, decomp_)
{
}

} // namespace cartex
