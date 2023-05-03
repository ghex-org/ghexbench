/*
 * ghex-org
 *
 * Copyright (c) 2014-2023, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#pragma once

#include <utility>

namespace ghexbench
{

// returns available resources for the current process: [physical cores, logical processors]
std::pair<int, int> hardware_resources();

// sets the affinity of a thread with index `tid` within a group of `count` threads
void set_affinity(int tid, int count);

// get the current logical processor index
int get_cpu();

} // namespace ghexbench
