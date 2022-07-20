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

#include <oomph/context.hpp>
#include <oomph/communicator.hpp>
#include <cartex/runtime/runtime.hpp>

#include <memory>
#include <vector>

#define CARTEX_MPI_MANY_BUFFERS

namespace ghexbench
{
namespace cartex
{
using namespace oomph;

class runtime::impl
{
  private:
    using domain_t = decomposition::domain_t;
    using buffer_type = message_buffer<runtime::real_type>;

    struct neighborhood
    {
        using memory_type = runtime::memory_type;
        communicator*            comm;
        int                      num_fields;
        std::array<int, 6>       m_halos;
        domain_t                 d;
        domain_t                 x_l, x_r;
        domain_t                 y_l, y_r;
        domain_t                 z_l, z_r;
        const std::array<int, 3> ext_buffer;
        int                      z_plane;
#ifdef __CUDACC__
        cudaStream_t stream;
        dim3         dims_x;
        dim3         blocks_x;
        dim3         dims_y;
        dim3         blocks_y;
#ifdef CARTEX_MPI_PACK_Z
        dim3 dims_z;
        dim3 blocks_z;
#endif
#endif

        neighborhood(int i, communicator* c, decomposition& decomp,
            std::array<int, 6> const& halos);

        //void exchange(memory_type& field, int field_id);
        void exchange(memory_type& field, std::vector<buffer_type>& send_buffers,
            std::vector<buffer_type>& recv_buffers, std::vector<buffer_type>& z_send_buffers,
            std::vector<buffer_type>& z_recv_buffers, int field_id);

      private:
        void pack_x(memory_type& field, buffer_type& buffer_left, buffer_type& buffer_right);
        void unpack_x(memory_type& field, buffer_type& buffer_left, buffer_type& buffer_right);
        void pack_y(memory_type& field, buffer_type& buffer_left, buffer_type& buffer_right);
        void unpack_y(memory_type& field, buffer_type& buffer_left, buffer_type& buffer_right);
#ifdef CARTEX_MPI_PACK_Z
        void pack_z(memory_type& field, buffer_type& buffer_left, buffer_type& buffer_right);
        void unpack_z(memory_type& field, buffer_type& buffer_left, buffer_type& buffer_right);
#endif

        int sendtag(int field_id, int dim, bool left) const noexcept;
        int recvtag(int field_id, int dim, bool left) const noexcept;
    };


  private:
    runtime&                                   m_base;
    context                                    m_context;
    std::vector<std::unique_ptr<communicator>> m_comms;
#ifdef CARTEX_MPI_MANY_BUFFERS
    std::vector<std::vector<std::vector<buffer_type>>> m_send_buffers;
    std::vector<std::vector<std::vector<buffer_type>>> m_recv_buffers;
#else
    std::vector<std::vector<buffer_type>> m_send_buffers;
    std::vector<std::vector<buffer_type>> m_recv_buffers;
#endif
    // threads x fields x directions
    // threads x directions

#ifndef CARTEX_MPI_PACK_Z
    std::vector<std::vector<std::vector<buffer_type>>> m_z_send_buffers;
    std::vector<std::vector<std::vector<buffer_type>>> m_z_recv_buffers;
#else
#ifdef CARTEX_MPI_MANY_BUFFERS
    std::vector<std::vector<std::vector<buffer_type>>> m_z_send_buffers;
    std::vector<std::vector<std::vector<buffer_type>>> m_z_recv_buffers;
#else
    std::vector<std::vector<buffer_type>> m_z_send_buffers;
    std::vector<std::vector<buffer_type>> m_z_recv_buffers;
#endif
#endif

    std::vector<neighborhood> m_neighbors;

  public:
    impl(runtime& base, options_values const& options);

    void init(int j);
    void step(int j);
    void exit(int j);

    std::string info() const
    {
#define CARTEX_STR2(var) #var
#define CARTEX_STR(var)  CARTEX_STR2(var)
        return CARTEX_STR(CARTEX_RUNTIME) " " CARTEX_STR(CARTEX_TRANSPORT);
#undef CARTEX_STR
#undef CARTEX_STR2
    }
};

} // namespace cartex
} // namespace ghexbench
