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

#include <cartex/runtime/mpi_comm/runtime.hpp>
#include "./kernels.hpp"
#include "../runtime_inc.cpp"

namespace cartex
{
options&
runtime::add_options(options& opts)
{
    return opts("MPI-datatypes", "use MPI Datatypes");
}

bool
runtime::check_options(options_values const&)
{
    return true;
}

runtime::impl::neighborhood::neighborhood(
    int i, decomposition& decomp, MPI_Datatype mpi_T, std::array<int, 6> const& halos)
: comm(decomp.mpi_comm())
, num_fields{decomp.threads_per_rank()}
, m_halos{halos}
, d{decomp.domain(i)}
, x_l{decomp.neighbor(i, -1, 0, 0)}
, x_r{decomp.neighbor(i, 1, 0, 0)}
, y_l{decomp.neighbor(i, 0, -1, 0)}
, y_r{decomp.neighbor(i, 0, 1, 0)}
, z_l{decomp.neighbor(i, 0, 0, -1)}
, z_r{decomp.neighbor(i, 0, 0, 1)}
, ext_buffer{d.domain_ext[0] + halos[0] + halos[1], d.domain_ext[1] + halos[2] + halos[3],
      d.domain_ext[2] + halos[4] + halos[5]}
, z_plane{ext_buffer[0] * ext_buffer[1]}
{
#ifdef __CUDACC__
    constexpr int max_threads = 512;   // 1024
    constexpr int max_threads_x = 512; // 1024
    constexpr int max_threads_y = 512; // 1024
    constexpr int max_threads_z = 64;
    constexpr int warp_size = 32;
    // kernel params for x dimension
    {
        const int block_dim_x = m_halos[0] + m_halos[1];
        int       remaining_threads_per_block = max_threads / block_dim_x;
        int       block_dim_y =
            std::min(std::min(d.domain_ext[1], remaining_threads_per_block), max_threads_y);
        block_dim_y = (block_dim_y / warp_size) * warp_size;
#ifdef CARTEX_MPI_PACK_Z_LOOP_GPU
        const int block_dim_z = 1;
        blocks_x = dim3(1, (d.domain_ext[1] + block_dim_y - 1) / block_dim_y, 1);
#else
        const int block_dim_z = std::min(max_threads / (block_dim_x * block_dim_y), max_threads_z);
        blocks_x = dim3(1, (d.domain_ext[1] + block_dim_y - 1) / block_dim_y,
            (d.domain_ext[2] + block_dim_z - 1) / block_dim_z);
#endif
        dims_x = dim3(block_dim_x, block_dim_y, block_dim_z);
    }
    // kernel params for y dimension
    {
        const int block_dim_y = m_halos[2] + m_halos[3];
        int       remaining_threads_per_block = max_threads / block_dim_y;
        int       block_dim_x =
            std::min(std::min(ext_buffer[0], remaining_threads_per_block), max_threads_x);
        block_dim_x = (block_dim_x / warp_size) * warp_size;
        const int block_dim_z = std::min(max_threads / (block_dim_x * block_dim_y), max_threads_z);
        dims_y = dim3(block_dim_x, block_dim_y, block_dim_z);
        blocks_y = dim3((ext_buffer[0] + block_dim_x - 1) / block_dim_x, 1,
            (d.domain_ext[2] + block_dim_z - 1) / block_dim_z);
    }
#ifdef CARTEX_MPI_PACK_Z
    // kernel params for z dimension
    {
        const int block_dim_z = m_halos[4] + m_halos[5];
        int       remaining_threads_per_block = max_threads / block_dim_z;
        int       block_dim_x =
            std::min(std::min(ext_buffer[0], remaining_threads_per_block), max_threads_x);
        block_dim_x = (block_dim_x / warp_size) * warp_size;
        const int block_dim_y = std::min(
            std::min(max_threads / (block_dim_x * block_dim_z), max_threads_y), ext_buffer[1]);
        dims_z = dim3(block_dim_x, block_dim_y, block_dim_z);
        blocks_z = dim3((ext_buffer[0] + block_dim_x - 1) / block_dim_x,
            (ext_buffer[1] + block_dim_y - 1) / block_dim_y, 1);
    }
#endif
#endif
    {
        x_recv_l = mpi_dtype_unique_ptr(new MPI_Datatype);
        x_recv_r = mpi_dtype_unique_ptr(new MPI_Datatype);
        x_send_l = mpi_dtype_unique_ptr(new MPI_Datatype);
        x_send_r = mpi_dtype_unique_ptr(new MPI_Datatype);
        const int x_recv_l_dims[3] = {halos[0], d.domain_ext[1], d.domain_ext[2]};
        const int x_recv_r_dims[3] = {halos[1], d.domain_ext[1], d.domain_ext[2]};
        const int x_recv_l_offsets[3] = {0, halos[2], halos[4]};
        const int x_recv_r_offsets[3] = {halos[0] + d.domain_ext[0], halos[2], halos[4]};
        const int x_send_l_offsets[3] = {halos[0], halos[2], halos[4]};
        const int x_send_r_offsets[3] = {halos[0] + d.domain_ext[0] - halos[1], halos[2], halos[4]};
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), x_recv_l_dims,
            x_recv_l_offsets, MPI_ORDER_FORTRAN, mpi_T, x_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), x_recv_r_dims,
            x_recv_r_offsets, MPI_ORDER_FORTRAN, mpi_T, x_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), x_recv_r_dims,
            x_send_l_offsets, MPI_ORDER_FORTRAN, mpi_T, x_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), x_recv_l_dims,
            x_send_r_offsets, MPI_ORDER_FORTRAN, mpi_T, x_send_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(x_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(x_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(x_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(x_send_r.get()));
    }
    {
        y_recv_l = mpi_dtype_unique_ptr(new MPI_Datatype);
        y_recv_r = mpi_dtype_unique_ptr(new MPI_Datatype);
        y_send_l = mpi_dtype_unique_ptr(new MPI_Datatype);
        y_send_r = mpi_dtype_unique_ptr(new MPI_Datatype);
        const int y_recv_l_dims[3] = {ext_buffer[0], halos[2], d.domain_ext[2]};
        const int y_recv_r_dims[3] = {ext_buffer[0], halos[3], d.domain_ext[2]};
        const int y_recv_l_offsets[3] = {0, 0, halos[4]};
        const int y_recv_r_offsets[3] = {0, halos[2] + d.domain_ext[1], halos[4]};
        const int y_send_l_offsets[3] = {0, halos[2], halos[4]};
        const int y_send_r_offsets[3] = {0, halos[2] + d.domain_ext[1] - halos[3], halos[4]};
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), y_recv_l_dims,
            y_recv_l_offsets, MPI_ORDER_FORTRAN, mpi_T, y_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), y_recv_r_dims,
            y_recv_r_offsets, MPI_ORDER_FORTRAN, mpi_T, y_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), y_recv_r_dims,
            y_send_l_offsets, MPI_ORDER_FORTRAN, mpi_T, y_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), y_recv_l_dims,
            y_send_r_offsets, MPI_ORDER_FORTRAN, mpi_T, y_send_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(y_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(y_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(y_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(y_send_r.get()));
    }
    {
        z_recv_l = mpi_dtype_unique_ptr(new MPI_Datatype);
        z_recv_r = mpi_dtype_unique_ptr(new MPI_Datatype);
        z_send_l = mpi_dtype_unique_ptr(new MPI_Datatype);
        z_send_r = mpi_dtype_unique_ptr(new MPI_Datatype);
        const int z_recv_l_dims[3] = {ext_buffer[0], ext_buffer[1], halos[4]};
        const int z_recv_r_dims[3] = {ext_buffer[0], ext_buffer[1], halos[5]};
        const int z_recv_l_offsets[3] = {0, 0, 0};
        const int z_recv_r_offsets[3] = {0, 0, halos[4] + d.domain_ext[2]};
        const int z_send_l_offsets[3] = {0, 0, halos[4]};
        const int z_send_r_offsets[3] = {0, 0, halos[4] + d.domain_ext[2] - halos[5]};
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), z_recv_l_dims,
            z_recv_l_offsets, MPI_ORDER_FORTRAN, mpi_T, z_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), z_recv_r_dims,
            z_recv_r_offsets, MPI_ORDER_FORTRAN, mpi_T, z_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), z_recv_r_dims,
            z_send_l_offsets, MPI_ORDER_FORTRAN, mpi_T, z_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, ext_buffer.data(), z_recv_l_dims,
            z_send_r_offsets, MPI_ORDER_FORTRAN, mpi_T, z_send_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(z_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(z_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(z_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(z_send_r.get()));
    }
}

int
runtime::impl::neighborhood::sendtag(int field_id, int dim, bool left) const noexcept
{
    // send tag encodes
    // - field id (which field):       num_fields
    // - send thread id:               num_threads
    // - dimension (x,y,z):            3
    // - left/right communication:     2
    const auto direction_tag = dim * 2 + (left ? 0 : 1);
    const auto thread_tag = d.thread;
    return field_id + num_fields * (direction_tag + 6 * thread_tag);
}

int
runtime::impl::neighborhood::recvtag(int field_id, int dim, bool left) const noexcept
{
    // recv tag encodes
    // - field id (which field):       num_fields
    // - send thread id:               num_threads
    // - dimension (x,y,z):            3
    // - left/right communication:     2
    const auto direction_tag = dim * 2 + (left ? 0 : 1);
    const auto tid_l = (dim == 0 ? x_r.thread : (dim == 1 ? y_r.thread : z_r.thread));
    const auto tid_r = (dim == 0 ? x_l.thread : (dim == 1 ? y_l.thread : z_l.thread));
    const auto thread_tag = left ? tid_l : tid_r;
    return field_id + num_fields * (direction_tag + 6 * thread_tag);
}

void
runtime::impl::neighborhood::exchange(memory_type& field, int field_id)
{
    runtime::real_type* data = field.hd_data();

    MPI_Request reqx[4];
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(data, 1, *x_recv_r, x_r.rank, recvtag(field_id, 0, true), comm, &reqx[1]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(data, 1, *x_recv_l, x_l.rank, recvtag(field_id, 0, false), comm, &reqx[3]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(data, 1, *x_send_l, x_l.rank, sendtag(field_id, 0, true), comm, &reqx[0]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(data, 1, *x_send_r, x_r.rank, sendtag(field_id, 0, false), comm, &reqx[2]));
    CARTEX_CHECK_MPI_RESULT(MPI_Waitall(4, reqx, MPI_STATUS_IGNORE));

    MPI_Request reqy[4];
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(data, 1, *y_recv_r, y_r.rank, recvtag(field_id, 1, true), comm, &reqy[1]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(data, 1, *y_recv_l, y_l.rank, recvtag(field_id, 1, false), comm, &reqy[3]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(data, 1, *y_send_l, y_l.rank, sendtag(field_id, 1, true), comm, &reqy[0]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(data, 1, *y_send_r, y_r.rank, sendtag(field_id, 1, false), comm, &reqy[2]));
    CARTEX_CHECK_MPI_RESULT(MPI_Waitall(4, reqy, MPI_STATUS_IGNORE));

    MPI_Request reqz[4];
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(data, 1, *z_recv_r, z_r.rank, recvtag(field_id, 2, true), comm, &reqz[1]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(data, 1, *z_recv_l, z_l.rank, recvtag(field_id, 2, false), comm, &reqz[3]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(data, 1, *z_send_l, z_l.rank, sendtag(field_id, 2, true), comm, &reqz[0]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(data, 1, *z_send_r, z_r.rank, sendtag(field_id, 2, false), comm, &reqz[2]));
    CARTEX_CHECK_MPI_RESULT(MPI_Waitall(4, reqz, MPI_STATUS_IGNORE));
}

void
runtime::impl::neighborhood::pack_x(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    execute_kernel(blocks_x, dims_x, pack_x_kernel, stream, field.hd_data(), buffer_left.hd_data(),
        buffer_right.hd_data(), m_halos, d.domain_ext);
#else
    for (int z = 0; z < d.domain_ext[2]; ++z)
    {
        for (int y = 0; y < d.domain_ext[1]; ++y)
        {
            const auto offset = ext_buffer[0] * (y + m_halos[2] + (z + m_halos[4]) * ext_buffer[1]);
            const auto offset_l = m_halos[0] * (y + d.domain_ext[1] * z);
            const auto offset_r = m_halos[1] * (y + d.domain_ext[1] * z);
            for (int x = 0; x < m_halos[0]; ++x)
                buffer_left[x + offset_l] = field[x + d.domain_ext[0] + offset];
            for (int x = 0; x < m_halos[1]; ++x)
                buffer_right[x + offset_r] = field[x + m_halos[0] + offset];
        }
    }
#endif
}

void
runtime::impl::neighborhood::unpack_x(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    execute_kernel(blocks_x, dims_x, unpack_x_kernel, stream, field.hd_data(),
        buffer_left.hd_data(), buffer_right.hd_data(), m_halos, d.domain_ext);
#else
    for (int z = 0; z < d.domain_ext[2]; ++z)
    {
        for (int y = 0; y < d.domain_ext[1]; ++y)
        {
            const auto offset = ext_buffer[0] * (y + m_halos[2] + (z + m_halos[4]) * ext_buffer[1]);
            const auto offset_l = m_halos[0] * (y + d.domain_ext[1] * z);
            const auto offset_r = m_halos[1] * (y + d.domain_ext[1] * z);
            for (int x = 0; x < m_halos[0]; ++x) field[x + offset] = buffer_left[x + offset_l];
            for (int x = 0; x < m_halos[1]; ++x)
                field[x + m_halos[0] + d.domain_ext[0] + offset] = buffer_right[x + offset_r];
        }
    }
#endif
}

void
runtime::impl::neighborhood::pack_y(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    execute_kernel(blocks_y, dims_y, pack_y_kernel, stream, field.hd_data(), buffer_left.hd_data(),
        buffer_right.hd_data(), m_halos, d.domain_ext);
#else
    for (int z = 0; z < d.domain_ext[2]; ++z)
    {
        for (int y = 0; y < m_halos[2]; ++y)
        {
            const auto offset =
                ext_buffer[0] * (y + d.domain_ext[1] + (z + m_halos[4]) * ext_buffer[1]);
            const auto offset_l = ext_buffer[0] * (y + m_halos[2] * z);
            for (int x = 0; x < ext_buffer[0]; ++x) buffer_left[x + offset_l] = field[x + offset];
        }
        for (int y = 0; y < m_halos[3]; ++y)
        {
            const auto offset = ext_buffer[0] * (y + m_halos[2] + (z + m_halos[4]) * ext_buffer[1]);
            const auto offset_r = ext_buffer[0] * (y + m_halos[3] * z);
            for (int x = 0; x < ext_buffer[0]; ++x) buffer_right[x + offset_r] = field[x + offset];
        }
    }
#endif
}

void
runtime::impl::neighborhood::unpack_y(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    execute_kernel(blocks_y, dims_y, unpack_y_kernel, stream, field.hd_data(),
        buffer_left.hd_data(), buffer_right.hd_data(), m_halos, d.domain_ext);
#else
    for (int z = 0; z < d.domain_ext[2]; ++z)
    {
        for (int y = 0; y < m_halos[2]; ++y)
        {
            const auto offset = ext_buffer[0] * (y + (z + m_halos[4]) * ext_buffer[1]);
            const auto offset_l = ext_buffer[0] * (y + m_halos[2] * z);
            for (int x = 0; x < ext_buffer[0]; ++x) field[x + offset] = buffer_left[x + offset_l];
        }
        for (int y = 0; y < m_halos[3]; ++y)
        {
            const auto offset = ext_buffer[0] * (y + m_halos[2] + d.domain_ext[1] +
                                                    (z + m_halos[4]) * ext_buffer[1]);
            const auto offset_r = ext_buffer[0] * (y + m_halos[3] * z);
            for (int x = 0; x < ext_buffer[0]; ++x) field[x + offset] = buffer_right[x + offset_r];
        }
    }
#endif
}

#ifdef CARTEX_MPI_PACK_Z
void
runtime::impl::neighborhood::pack_z(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    execute_kernel(blocks_z, dims_z, pack_z_kernel, stream, field.hd_data(), buffer_left.hd_data(),
        buffer_right.hd_data(), m_halos, d.domain_ext);
#else
    for (int z = 0; z < m_halos[4]; ++z)
    {
        for (int y = 0; y < ext_buffer[1]; ++y)
        {
            const auto offset = ext_buffer[0] * (y + ext_buffer[1] * (z + d.domain_ext[2]));
            const auto offset_l = ext_buffer[0] * (y + ext_buffer[1] * z);
            for (int x = 0; x < ext_buffer[0]; ++x) buffer_left[x + offset_l] = field[x + offset];
        }
    }
    for (int z = 0; z < m_halos[5]; ++z)
    {
        for (int y = 0; y < ext_buffer[1]; ++y)
        {
            const auto offset = ext_buffer[0] * (y + ext_buffer[1] * (z + m_halos[4]));
            const auto offset_r = ext_buffer[0] * (y + ext_buffer[1] * z);
            for (int x = 0; x < ext_buffer[0]; ++x) buffer_right[x + offset_r] = field[x + offset];
        }
    }
#endif
}

void
runtime::impl::neighborhood::unpack_z(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    execute_kernel(blocks_z, dims_z, unpack_z_kernel, stream, field.hd_data(),
        buffer_left.hd_data(), buffer_right.hd_data(), m_halos, d.domain_ext);
#else
    for (int z = 0; z < m_halos[4]; ++z)
    {
        for (int y = 0; y < ext_buffer[1]; ++y)
        {
            const auto offset = ext_buffer[0] * (y + ext_buffer[1] * z);
            const auto offset_l = ext_buffer[0] * (y + ext_buffer[1] * z);
            for (int x = 0; x < ext_buffer[0]; ++x) field[x + offset] = buffer_left[x + offset_l];
        }
    }
    for (int z = 0; z < m_halos[5]; ++z)
    {
        for (int y = 0; y < ext_buffer[1]; ++y)
        {
            const auto offset =
                ext_buffer[0] * (y + ext_buffer[1] * (z + d.domain_ext[2] + m_halos[4]));
            const auto offset_r = ext_buffer[0] * (y + ext_buffer[1] * z);
            for (int x = 0; x < ext_buffer[0]; ++x) field[x + offset] = buffer_right[x + offset_r];
        }
    }
#endif
}
#endif

void
runtime::impl::neighborhood::exchange(memory_type& field, std::vector<memory_type>& send_buffers,
    std::vector<memory_type>& recv_buffers, int field_id)
{
    pack_x(field, send_buffers[0], send_buffers[1]);
    MPI_Request reqx[4];
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(recv_buffers[1].hd_data(), recv_buffers[1].size() * sizeof(runtime::real_type),
            MPI_BYTE, x_r.rank, recvtag(field_id, 0, true), comm, &reqx[1]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(recv_buffers[0].hd_data(), recv_buffers[0].size() * sizeof(runtime::real_type),
            MPI_BYTE, x_l.rank, recvtag(field_id, 0, false), comm, &reqx[3]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(send_buffers[1].hd_data(), send_buffers[1].size() * sizeof(runtime::real_type),
            MPI_BYTE, x_l.rank, sendtag(field_id, 0, true), comm, &reqx[0]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(send_buffers[0].hd_data(), send_buffers[0].size() * sizeof(runtime::real_type),
            MPI_BYTE, x_r.rank, sendtag(field_id, 0, false), comm, &reqx[2]));
    CARTEX_CHECK_MPI_RESULT(MPI_Waitall(4, reqx, MPI_STATUS_IGNORE));
    unpack_x(field, recv_buffers[0], recv_buffers[1]);

    pack_y(field, send_buffers[2], send_buffers[3]);
    MPI_Request reqy[4];
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(recv_buffers[3].hd_data(), recv_buffers[3].size() * sizeof(runtime::real_type),
            MPI_BYTE, y_r.rank, recvtag(field_id, 1, true), comm, &reqy[1]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(recv_buffers[2].hd_data(), recv_buffers[2].size() * sizeof(runtime::real_type),
            MPI_BYTE, y_l.rank, recvtag(field_id, 1, false), comm, &reqy[3]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(send_buffers[3].hd_data(), send_buffers[3].size() * sizeof(runtime::real_type),
            MPI_BYTE, y_l.rank, sendtag(field_id, 1, true), comm, &reqy[0]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(send_buffers[2].hd_data(), send_buffers[2].size() * sizeof(runtime::real_type),
            MPI_BYTE, y_r.rank, sendtag(field_id, 1, false), comm, &reqy[2]));
    CARTEX_CHECK_MPI_RESULT(MPI_Waitall(4, reqy, MPI_STATUS_IGNORE));
    unpack_y(field, recv_buffers[2], recv_buffers[3]);

#ifndef CARTEX_MPI_PACK_Z
    // note that we don't need packing for z-direction since the memory is already contiguous
    // this works because we don't have padding
    const auto  z0 = field.hd_data();
    const auto  z1 = field.hd_data() + z_plane * m_halos[4];
    const auto  z2 = field.hd_data() + z_plane * d.domain_ext[2];
    const auto  z3 = field.hd_data() + z_plane * (m_halos[4] + d.domain_ext[2]);
    const auto  left_size = z_plane * m_halos[5] * sizeof(runtime::real_type);
    const auto  right_size = z_plane * m_halos[4] * sizeof(runtime::real_type);
    MPI_Request reqz[4];
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(z3, left_size, MPI_BYTE, z_r.rank, recvtag(field_id, 2, true), comm, &reqz[1]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(z0, right_size, MPI_BYTE, z_l.rank, recvtag(field_id, 2, false), comm, &reqz[3]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(z1, left_size, MPI_BYTE, z_l.rank, sendtag(field_id, 2, true), comm, &reqz[0]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(z2, right_size, MPI_BYTE, z_r.rank, sendtag(field_id, 2, false), comm, &reqz[2]));
    CARTEX_CHECK_MPI_RESULT(MPI_Waitall(4, reqz, MPI_STATUS_IGNORE));
#else
    pack_z(field, send_buffers[4], send_buffers[5]);
    MPI_Request reqz[4];
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(recv_buffers[5].hd_data(), recv_buffers[5].size() * sizeof(runtime::real_type),
            MPI_BYTE, z_r.rank, recvtag(field_id, 2, true), comm, &reqz[1]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Irecv(recv_buffers[4].hd_data(), recv_buffers[4].size() * sizeof(runtime::real_type),
            MPI_BYTE, z_l.rank, recvtag(field_id, 2, false), comm, &reqz[3]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(send_buffers[5].hd_data(), send_buffers[5].size() * sizeof(runtime::real_type),
            MPI_BYTE, z_l.rank, sendtag(field_id, 2, true), comm, &reqz[0]));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Isend(send_buffers[4].hd_data(), send_buffers[4].size() * sizeof(runtime::real_type),
            MPI_BYTE, z_r.rank, sendtag(field_id, 2, false), comm, &reqz[2]));
    CARTEX_CHECK_MPI_RESULT(MPI_Waitall(4, reqz, MPI_STATUS_IGNORE));
    unpack_z(field, recv_buffers[4], recv_buffers[5]);
#endif
}

runtime::impl::impl(cartex::runtime& base, options_values const& options)
: m_base{base}
, m_comm{m_base.m_decomposition.mpi_comm()}
, m_use_mpi_datatypes(options.is_set("MPI-datatypes"))
, m_send_buffers(m_base.m_num_threads)
, m_recv_buffers(m_base.m_num_threads)
{
    MPI_Type_match_size(MPI_TYPECLASS_REAL, sizeof(runtime::real_type), &m_mpi_T);
    m_neighbors.reserve(m_base.m_num_threads);
    for (int i = 0; i < m_base.m_num_threads; ++i)
        m_neighbors.emplace_back(i, m_base.m_decomposition, m_mpi_T, m_base.m_halos);
    CARTEX_CHECK_MPI_RESULT(MPI_Barrier(m_comm));
}

void
runtime::impl::init(int j)
{
    auto& d = m_base.m_domains[j];
#ifdef __CUDACC__
    CARTEX_CHECK_CUDA_RESULT(cudaStreamCreate(&m_neighbors[j].stream));
#endif
#ifdef CARTEX_MPI_MANY_BUFFERS
    m_recv_buffers[j].resize(m_base.m_num_fields);
    m_send_buffers[j].resize(m_base.m_num_fields);
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        auto& recv_buffers = m_recv_buffers[j][i];
        auto& send_buffers = m_send_buffers[j][i];
#else
        auto& recv_buffers = m_recv_buffers[j];
        auto& send_buffers = m_send_buffers[j];
#endif
        // x buffers
        recv_buffers.emplace_back(m_base.m_halos[0] * d.domain_ext[1] * d.domain_ext[2], 0);
        send_buffers.emplace_back(m_base.m_halos[0] * d.domain_ext[1] * d.domain_ext[2], 0);
        recv_buffers.emplace_back(m_base.m_halos[1] * d.domain_ext[1] * d.domain_ext[2], 0);
        send_buffers.emplace_back(m_base.m_halos[1] * d.domain_ext[1] * d.domain_ext[2], 0);
        // y buffers
        const auto x_ext = d.domain_ext[0] + m_base.m_halos[0] + m_base.m_halos[1];
        send_buffers.emplace_back(m_base.m_halos[2] * x_ext * d.domain_ext[2], 0);
        recv_buffers.emplace_back(m_base.m_halos[2] * x_ext * d.domain_ext[2], 0);
        send_buffers.emplace_back(m_base.m_halos[3] * x_ext * d.domain_ext[2], 0);
        recv_buffers.emplace_back(m_base.m_halos[3] * x_ext * d.domain_ext[2], 0);
#ifdef CARTEX_MPI_PACK_Z
        // z buffers
        const auto y_ext = d.domain_ext[1] + m_base.m_halos[2] + m_base.m_halos[3];
        send_buffers.emplace_back(m_base.m_halos[4] * x_ext * y_ext, 0);
        recv_buffers.emplace_back(m_base.m_halos[4] * x_ext * y_ext, 0);
        send_buffers.emplace_back(m_base.m_halos[5] * x_ext * y_ext, 0);
        recv_buffers.emplace_back(m_base.m_halos[5] * x_ext * y_ext, 0);
#endif
#ifdef CARTEX_MPI_MANY_BUFFERS
    }
#endif
}

void
runtime::impl::step(int j)
{
    if (m_use_mpi_datatypes)
        for (int i = 0; i < m_base.m_num_fields; ++i)
            m_neighbors[j].exchange(m_base.m_raw_fields[j][i], i);
    else
        for (int i = 0; i < m_base.m_num_fields; ++i)
#ifdef CARTEX_MPI_MANY_BUFFERS
            m_neighbors[j].exchange(m_base.m_raw_fields[j][i], m_send_buffers[j][i],
                m_recv_buffers[j][i], i);
#else
            m_neighbors[j].exchange(m_base.m_raw_fields[j][i], m_send_buffers[j], m_recv_buffers[j],
                i);
#endif
}

} // namespace cartex
