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
#include "../runtime_inc.cpp"

namespace cartex
{
#ifdef __CUDACC__
template<class Kernel, class... Args>
void
launch_and_wait(
    dim3 const& blocks, dim3 const& threads, Kernel kernel, cudaStream_t stream, Args... args)
{
    kernel<<<blocks, threads, 0, stream>>>(std::move(args)...);
    CARTEX_CHECK_CUDA_RESULT(cudaGetLastError());
    CARTEX_CHECK_CUDA_RESULT(cudaStreamSynchronize(stream));
}

template<class Kernel>
void
execute_kernel(dim3 const& blocks, dim3 const& threads, Kernel kernel, cudaStream_t stream,
    runtime::real_type* field, runtime::real_type* buffer_left, runtime::real_type* buffer_right,
    std::array<int, 6> const& halos, std::array<int, 3> const& dims)
{
    launch_and_wait(blocks, threads, kernel, stream, field, buffer_left, buffer_right, halos[0],
        halos[1], halos[2], halos[3], halos[4], halos[5], dims[0], dims[1], dims[2]);
}
#endif

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

#ifdef __CUDACC__
__global__ void
pack_x_kernel(runtime::real_type const* field, runtime::real_type* buffer_left,
    runtime::real_type* buffer_right, int halo_x_left, int halo_x_right, int halo_y_left,
    int halo_y_right, int halo_z_left, int halo_z_right, int ext_x, int ext_y, int ext_z)
{
    const auto y = blockIdx.y * blockDim.y + threadIdx.y;
    const auto z = blockIdx.z * blockDim.z + threadIdx.z;
    if (y < ext_y && z < ext_z)
    {
        const auto ext_x_b = ext_x + halo_x_left + halo_x_right;
        const auto ext_y_b = ext_y + halo_y_left + halo_y_right;
        const auto offset = ext_x_b * (y + halo_y_left + (z + halo_z_left) * ext_y_b);
        if (threadIdx.x < halo_x_left)
        {
            const auto x = threadIdx.x;
            const auto offset_l = halo_x_left * (y + ext_y * z);
            buffer_left[x + offset_l] = field[x + ext_x + offset];
        }
        else
        {
            const auto x = threadIdx.x - halo_x_left;
            const auto offset_r = halo_x_right * (y + ext_y * z);
            buffer_right[x + offset_r] = field[x + halo_x_left + offset];
        }
    }
}
#endif

void
runtime::impl::neighborhood::pack_x(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    constexpr int block_dim_z = 4;
    constexpr int block_dim_y = 64;
    const dim3    block_dims(m_halos[0] + m_halos[1], block_dim_y, block_dim_z);
    const dim3    num_blocks(1, (d.domain_ext[1] + block_dim_y - 1) / block_dim_y,
        (d.domain_ext[2] + block_dim_z - 1) / block_dim_z);
    execute_kernel(num_blocks, block_dims, pack_x_kernel, stream, field.hd_data(),
        buffer_left.hd_data(), buffer_right.hd_data(), m_halos, d.domain_ext);
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

#ifdef __CUDACC__
__global__ void
unpack_x_kernel(runtime::real_type* field, runtime::real_type const* buffer_left,
    runtime::real_type const* buffer_right, int halo_x_left, int halo_x_right, int halo_y_left,
    int halo_y_right, int halo_z_left, int halo_z_right, int ext_x, int ext_y, int ext_z)
{
    const auto y = blockIdx.y * blockDim.y + threadIdx.y;
    const auto z = blockIdx.z * blockDim.z + threadIdx.z;
    if (y < ext_y && z < ext_z)
    {
        const auto ext_x_b = ext_x + halo_x_left + halo_x_right;
        const auto ext_y_b = ext_y + halo_y_left + halo_y_right;
        const auto offset = ext_x_b * (y + halo_y_left + (z + halo_z_left) * ext_y_b);
        if (threadIdx.x < halo_x_left)
        {
            const auto x = threadIdx.x;
            const auto offset_l = halo_x_left * (y + ext_y * z);
            field[x + offset] = buffer_left[x + offset_l];
        }
        else
        {
            const auto x = threadIdx.x - halo_x_left;
            const auto offset_r = halo_x_right * (y + ext_y * z);
            field[x + halo_x_left + ext_x + offset] = buffer_right[x + offset_r];
        }
    }
}
#endif

void
runtime::impl::neighborhood::unpack_x(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    constexpr int block_dim_z = 4;
    constexpr int block_dim_y = 64;
    const dim3    block_dims(m_halos[0] + m_halos[1], block_dim_y, block_dim_z);
    const dim3    num_blocks(1, (d.domain_ext[1] + block_dim_y - 1) / block_dim_y,
        (d.domain_ext[2] + block_dim_z - 1) / block_dim_z);
    execute_kernel(num_blocks, block_dims, unpack_x_kernel, stream, field.hd_data(),
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

#ifdef __CUDACC__
__global__ void
pack_y_kernel(runtime::real_type const* field, runtime::real_type* buffer_left,
    runtime::real_type* buffer_right, int halo_x_left, int halo_x_right, int halo_y_left,
    int halo_y_right, int halo_z_left, int halo_z_right, int ext_x, int ext_y, int ext_z)
{
    const auto x = blockIdx.x * blockDim.x + threadIdx.x;
    const auto z = blockIdx.z * blockDim.z + threadIdx.z;
    const auto ext_x_b = ext_x + halo_x_left + halo_x_right;
    if (x < ext_x_b && z < ext_z)
    {
        const auto ext_y_b = ext_y + halo_y_left + halo_y_right;
        if (threadIdx.y < halo_y_left)
        {
            const auto y = threadIdx.y;
            const auto offset = ext_x_b * (y + ext_y + (z + halo_z_left) * ext_y_b);
            const auto offset_l = ext_x_b * (y + halo_y_left * z);
            buffer_left[x + offset_l] = field[x + offset];
        }
        else
        {
            const auto y = threadIdx.y - halo_y_left;
            const auto offset = ext_x_b * (y + halo_y_left + (z + halo_z_left) * ext_y_b);
            const auto offset_r = ext_x_b * (y + halo_y_right * z);
            buffer_right[x + offset_r] = field[x + offset];
        }
    }
}
#endif

void
runtime::impl::neighborhood::pack_y(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    constexpr int block_dim_z = 4;
    constexpr int block_dim_x = 64;
    const dim3    block_dims(block_dim_x, m_halos[2] + m_halos[3], block_dim_z);
    const dim3    num_blocks((ext_buffer[0] + block_dim_x - 1) / block_dim_x, 1,
        (d.domain_ext[2] + block_dim_z - 1) / block_dim_z);
    execute_kernel(num_blocks, block_dims, pack_y_kernel, stream, field.hd_data(),
        buffer_left.hd_data(), buffer_right.hd_data(), m_halos, d.domain_ext);
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

#ifdef __CUDACC__
__global__ void
unpack_y_kernel(runtime::real_type* field, runtime::real_type const* buffer_left,
    runtime::real_type const* buffer_right, int halo_x_left, int halo_x_right, int halo_y_left,
    int halo_y_right, int halo_z_left, int halo_z_right, int ext_x, int ext_y, int ext_z)
{
    const auto x = blockIdx.x * blockDim.x + threadIdx.x;
    const auto z = blockIdx.z * blockDim.z + threadIdx.z;
    const auto ext_x_b = ext_x + halo_x_left + halo_x_right;
    if (x < ext_x_b && z < ext_z)
    {
        const auto ext_y_b = ext_y + halo_y_left + halo_y_right;
        if (threadIdx.y < halo_y_left)
        {
            const auto y = threadIdx.y;
            const auto offset = ext_x_b * (y + (z + halo_z_left) * ext_y_b);
            const auto offset_l = ext_x_b * (y + halo_y_left * z);
            field[x + offset] = buffer_left[x + offset_l];
        }
        else
        {
            const auto y = threadIdx.y - halo_y_left;
            const auto offset = ext_x_b * (y + halo_y_left + ext_y + (z + halo_z_left) * ext_y_b);
            const auto offset_r = ext_x_b * (y + halo_y_right * z);
            field[x + offset] = buffer_right[x + offset_r];
        }
    }
}
#endif

void
runtime::impl::neighborhood::unpack_y(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
#ifdef __CUDACC__
    constexpr int block_dim_z = 4;
    constexpr int block_dim_x = 64;
    const dim3    block_dims(block_dim_x, m_halos[2] + m_halos[3], block_dim_z);
    const dim3    num_blocks((ext_buffer[0] + block_dim_x - 1) / block_dim_x, 1,
        (d.domain_ext[2] + block_dim_z - 1) / block_dim_z);
    execute_kernel(num_blocks, block_dims, unpack_y_kernel, stream, field.hd_data(),
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

    // note that we don't need packing for z-direction since the memory is already contiguous
    // this works because we don't have padding
    const auto z0 = field.hd_data();
    const auto z1 = field.hd_data() + z_plane * m_halos[4];
    const auto z2 = field.hd_data() + z_plane * d.domain_ext[2];
    const auto z3 = field.hd_data() + z_plane * (m_halos[4] + d.domain_ext[2]);
    const auto left_size = z_plane * m_halos[5] * sizeof(runtime::real_type);
    const auto right_size = z_plane * m_halos[4] * sizeof(runtime::real_type);
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
    // x buffers
    m_recv_buffers[j].emplace_back(m_base.m_halos[0] * d.domain_ext[1] * d.domain_ext[2], 0);
    m_send_buffers[j].emplace_back(m_base.m_halos[0] * d.domain_ext[1] * d.domain_ext[2], 0);
    m_recv_buffers[j].emplace_back(m_base.m_halos[1] * d.domain_ext[1] * d.domain_ext[2], 0);
    m_send_buffers[j].emplace_back(m_base.m_halos[1] * d.domain_ext[1] * d.domain_ext[2], 0);
    // y buffers
    const auto x_ext = d.domain_ext[0] + m_base.m_halos[0] + m_base.m_halos[1];
    m_send_buffers[j].emplace_back(m_base.m_halos[2] * x_ext * d.domain_ext[2], 0);
    m_recv_buffers[j].emplace_back(m_base.m_halos[2] * x_ext * d.domain_ext[2], 0);
    m_send_buffers[j].emplace_back(m_base.m_halos[3] * x_ext * d.domain_ext[2], 0);
    m_recv_buffers[j].emplace_back(m_base.m_halos[3] * x_ext * d.domain_ext[2], 0);
}

void
runtime::impl::step(int j)
{
    if (m_use_mpi_datatypes)
        for (int i = 0; i < m_base.m_num_fields; ++i)
            m_neighbors[j].exchange(m_base.m_raw_fields[j][i], i);
    else
        for (int i = 0; i < m_base.m_num_fields; ++i)
            m_neighbors[j].exchange(
                m_base.m_raw_fields[j][i], m_send_buffers[j], m_recv_buffers[j], i);
}

} // namespace cartex
