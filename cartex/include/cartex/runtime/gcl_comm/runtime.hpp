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
#include <gridtools/gcl/GCL.hpp>
#include <gridtools/gcl/halo_exchange.hpp>

namespace cartex
{
namespace gt = gridtools;

class runtime::impl
{
  private:
    using pattern_type = gt::gcl::halo_exchange_dynamic_ut<gt::layout_map<2, 1, 0>,
        gt::layout_map<0, 1, 2>, runtime::real_type,
#ifdef __CUDACC__
        gt::gcl::gpu
#else
        gt::gcl::cpu
#endif
        >;

    struct mpi_cart_holder
    {
        bool     m_create = false;
        bool     m_reorder = false;
        MPI_Comm m_comm;
        MPI_Comm m_reorder_comm;
        mpi_cart_holder(options_values const& options, decomposition const& d)
        {
            if (!options.has("MPICart"))
            {
                m_create = true;
                const auto order = options.get_or("order", std::string("XYZ"));
                if (order != "ZYX")
                {
                    m_reorder = true;
                    const auto c = d.coord(0);
                    const int  key = c[2] + d.rank_decomposition()[2] *
                                               (c[1] + d.rank_decomposition()[1] * c[0]);
                    MPI_Comm_split(d.mpi_comm(), 0, key, &m_reorder_comm);
                }
                else
                    m_reorder_comm = d.mpi_comm();
                const int periods[3] = {1, 1, 1};
                MPI_Cart_create(m_reorder_comm, 3, &d.rank_decomposition()[0], periods, 0, &m_comm);
            }
            else
            {
                m_comm = d.mpi_comm();
            }
        }
        mpi_cart_holder(const mpi_cart_holder&) = delete;
        mpi_cart_holder(mpi_cart_holder&& other)
        : m_create(std::exchange(other.m_create, false))
        , m_reorder(std::exchange(other.m_reorder, false))
        , m_comm(other.m_comm)
        , m_reorder_comm(other.m_reorder_comm)
        {
        }
        ~mpi_cart_holder()
        {
            if (m_create)
            {
                MPI_Comm_free(&m_comm);
                if (m_reorder) MPI_Comm_free(&m_reorder_comm);
            }
        }
    };

  private:
    runtime&                         m_base;
    mpi_cart_holder                  m_cart;
    pattern_type                     m_pattern;
    std::vector<runtime::real_type*> m_field_ptrs;

  public:
    impl(runtime& base, options_values const& options);

    void        init(int);
    void        step(int j);
    std::string info() const
    {
#define CARTEX_STR2(var) #var
#define CARTEX_STR(var)  CARTEX_STR2(var)
        return CARTEX_STR(CARTEX_RUNTIME);
#undef CARTEX_STR
#undef CARTEX_STR2
    }
};

} // namespace cartex
