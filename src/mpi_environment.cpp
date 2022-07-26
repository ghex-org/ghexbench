/*
 * ghex-org
 *
 * Copyright (c) 2014-2022, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>

#include <ghexbench/mpi_environment.hpp>

namespace ghexbench
{
mpi_environment::mpi_environment(bool thread_safe, int& argc, char**& argv)
: is_thread_safe{thread_safe}
{
    int mode;
    if (thread_safe)
    {
        MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &mode);
        if (mode != MPI_THREAD_MULTIPLE)
        {
            std::cerr << "MPI_THREAD_MULTIPLE not supported by MPI, aborting\n";
            std::terminate();
        }
    }
    else { MPI_Init(&argc, &argv); }
    GHEXBENCH_CHECK_MPI_RESULT(MPI_Comm_size(MPI_COMM_WORLD, &size));
    GHEXBENCH_CHECK_MPI_RESULT(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
}

mpi_environment::~mpi_environment()
{
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}

} // namespace ghexbench
