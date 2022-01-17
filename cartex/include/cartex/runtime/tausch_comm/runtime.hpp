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
#ifdef __CUDACC__
#define TAUSCH_CUDA
#endif
#include <tausch.h>

namespace cartex
{
class runtime::impl
{
  private:
    runtime&                 m_base;
    MPI_Comm                 m_comm;
    Tausch                   m_tausch;
    std::vector<MPI_Request> m_reqs;
    std::size_t              s_x_l, s_x_r, r_x_l, r_x_r;
    std::size_t              s_y_l, s_y_r, r_y_l, r_y_r;
    std::size_t              s_z_l, s_z_r, r_z_l, r_z_r;

  public:
    impl(runtime& base, options_values const& options);
    void        init(int j);
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
    std::size_t add_send_halo(std::size_t x_l, std::size_t x_r, std::size_t y_l, std::size_t y_r,
        std::size_t z_l, std::size_t z_r, std::array<int, 3> ext_buffer, int rank);
    std::size_t add_recv_halo(std::size_t x_l, std::size_t x_r, std::size_t y_l, std::size_t y_r,
        std::size_t z_l, std::size_t z_r, std::array<int, 3> ext_buffer, int rank);
};

} // namespace cartex
