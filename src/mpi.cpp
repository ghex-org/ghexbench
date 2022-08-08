/*
 * ghex-org
 *
 * Copyright (c) 2014-2022, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdexcept>
#include <string>
#include <utility>
//#include <iostream>

#include <ghexbench/mpi.hpp>

namespace ghexbench
{
namespace detail
{
int
check_mpi_result(int r, const char* expr, const char* file, int line)
{
    //std::cout << "GHEX MPI " << expr << " in " << file << ":" << line << std::endl;
    if (r != MPI_SUCCESS)
        throw std::runtime_error("GHEX Error: MPI Call failed " + std::string(expr) + " in " +
                                 std::string(file) + ":" + std::to_string(line));
    return r;
}
} // namespace detail

mpi_comm_holder::mpi_comm_holder(MPI_Comm c)
: m{c}
{
}

mpi_comm_holder::mpi_comm_holder(mpi_comm_holder&& other) noexcept
: m{std::exchange(other.m, MPI_COMM_WORLD)}
{
}

mpi_comm_holder&
mpi_comm_holder::operator=(mpi_comm_holder&& other)
{
    if (m != MPI_COMM_WORLD) GHEXBENCH_CHECK_MPI_RESULT(MPI_Comm_free(&m));
    m = std::exchange(other.m, MPI_COMM_WORLD);
    return *this;
}

mpi_comm_holder::~mpi_comm_holder()
{
    if (m != MPI_COMM_WORLD) MPI_Comm_free(&m);
}

void
mpi_comm_holder::reset(MPI_Comm c)
{
    if (m != MPI_COMM_WORLD) GHEXBENCH_CHECK_MPI_RESULT(MPI_Comm_free(&m));
    m = c;
}
} // namespace ghexbench
