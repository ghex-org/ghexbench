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

#pragma once

#include <mpi.h>

#ifdef NDEBUG

#define GHEXBENCH_CHECK_MPI_RESULT(x) x;

#else

#define GHEXBENCH_CHECK_MPI_RESULT(x)                                                              \
    ::ghexbench::detail::check_mpi_result(x, #x, __FILE__, __LINE__);
namespace ghexbench
{
namespace detail
{
int check_mpi_result(int r, const char* expr, const char* file, int line);
} // namespace detail
} // namespace ghexbench

#endif

namespace ghexbench
{
class mpi_comm_holder
{
  private:
    MPI_Comm m = MPI_COMM_WORLD;

  public:
    mpi_comm_holder() {}

    mpi_comm_holder(MPI_Comm c);

    mpi_comm_holder(mpi_comm_holder const&) = delete;
    mpi_comm_holder& operator=(mpi_comm_holder const&) = delete;

    mpi_comm_holder(mpi_comm_holder&&) noexcept;
    mpi_comm_holder& operator=(mpi_comm_holder&&);

    ~mpi_comm_holder();

  public:
    void reset(MPI_Comm c);

    MPI_Comm get() const noexcept { return m; }

    operator MPI_Comm() const noexcept { return m; };
};
} // namespace ghexbench
