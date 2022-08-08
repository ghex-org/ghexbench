/*
 * ghex-org
 *
 * Copyright (c) 2014-2022, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <ghexbench/mpi.hpp>

namespace ghexbench
{
struct mpi_environment
{
    int size = 1;
    int rank = 0;
    bool is_thread_safe;

    mpi_environment(bool thread_safe, int& argc, char**& argv);

    mpi_environment(mpi_environment const&) = delete;

    ~mpi_environment();
};

} // namespace ghexbench
