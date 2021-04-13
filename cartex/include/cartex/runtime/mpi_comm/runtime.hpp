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
        using memory_type = runtime::memory_type;
        MPI_Comm                 comm;
        int                      num_fields;
        std::array<int, 6>       m_halos;
        domain_t                 d;
        domain_t                 x_l, x_r;
        domain_t                 y_l, y_r;
        domain_t                 z_l, z_r;
        const std::array<int, 3> ext_buffer;
        int                      z_plane;
        mpi_dtype_unique_ptr     x_recv_l, x_recv_r;
        mpi_dtype_unique_ptr     x_send_l, x_send_r;
        mpi_dtype_unique_ptr     y_recv_l, y_recv_r;
        mpi_dtype_unique_ptr     y_send_l, y_send_r;
        mpi_dtype_unique_ptr     z_recv_l, z_recv_r;
        mpi_dtype_unique_ptr     z_send_l, z_send_r;

        neighborhood(
            int i, decomposition& decomp, MPI_Datatype mpi_T, std::array<int, 6> const& halos);

        void exchange(memory_type& field, int field_id);
        void exchange(memory_type& field, std::vector<memory_type>& send_buffers,
            std::vector<memory_type>& recv_buffers, int field_id);

      private:
        void pack_x(memory_type& field, memory_type& buffer_left, memory_type& buffer_right);
        void unpack_x(memory_type& field, memory_type& buffer_left, memory_type& buffer_right);
        void pack_y(memory_type& field, memory_type& buffer_left, memory_type& buffer_right);
        void unpack_y(memory_type& field, memory_type& buffer_left, memory_type& buffer_right);

        int sendtag(int field_id, int dim, bool left) const noexcept;
        int recvtag(int field_id, int dim, bool left) const noexcept;
    };

  private:
    runtime&                                       m_base;
    MPI_Comm                                       m_comm;
    bool                                           m_use_mpi_datatypes;
    MPI_Datatype                                   m_mpi_T;
    std::vector<std::vector<runtime::memory_type>> m_send_buffers;
    std::vector<std::vector<runtime::memory_type>> m_recv_buffers;
    std::vector<neighborhood>                      m_neighbors;

  public:
    impl(runtime& base, options_values const& options);
    void        init(int j);
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
