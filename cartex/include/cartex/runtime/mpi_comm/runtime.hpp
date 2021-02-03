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

namespace cartex
{
class runtime::impl
{
  private:
    struct mpi_dtype_deleter
    {
        void operator()(MPI_Datatype* type)
        {
            MPI_Type_free(type);
            delete type;
        }
    };

    using mpi_dtype_unique_ptr = std::unique_ptr<MPI_Datatype, mpi_dtype_deleter>;
    using domain_t = decomposition::domain_t;

    struct neighborhood
    {
        domain_t d;
        domain_t x_l, x_r;
        domain_t y_l, y_r;
        domain_t z_l, z_r;
    };

  private:
    runtime&                  m_base;
    MPI_Comm                  m_comm;
    MPI_Datatype              m_mpi_T;
    mpi_dtype_unique_ptr      x_recv_l, x_recv_r;
    mpi_dtype_unique_ptr      x_send_l, x_send_r;
    mpi_dtype_unique_ptr      y_recv_l, y_recv_r;
    mpi_dtype_unique_ptr      y_send_l, y_send_r;
    mpi_dtype_unique_ptr      z_recv_l, z_recv_r;
    mpi_dtype_unique_ptr      z_send_l, z_send_r;
    std::vector<neighborhood> m_neighbors;

  public:
    impl(runtime& base);
    void init(int) {}
    void step(int j);
    std::string info() const
    {
#define CARTEX_STR2(var) #var
#define CARTEX_STR(var)  CARTEX_STR2(var)
        return CARTEX_STR(CARTEX_RUNTIME);
#undef CARTEX_STR
#undef CARTEX_STR2
    }
};

#include "../runtime.inc"

} // namespace cartex
