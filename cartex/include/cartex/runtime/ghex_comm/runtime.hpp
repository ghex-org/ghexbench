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

#include <cartex/runtime/runtime.hpp>

namespace cartex
{
class runtime : public runtime_base<runtime>
{
  private:
    using base = runtime_base<runtime>;

  public:
    runtime(
        int num_reps_, int ext_, int halo, int num_fields_, bool check_, decomposition& decomp_);
};

} // namespace cartex

