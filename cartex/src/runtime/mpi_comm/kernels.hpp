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

namespace ghexbench
{
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

__global__ void
pack_x_kernel(runtime::real_type const* field, runtime::real_type* buffer_left,
    runtime::real_type* buffer_right, int halo_x_left, int halo_x_right, int halo_y_left,
    int halo_y_right, int halo_z_left, int halo_z_right, int ext_x, int ext_y, int ext_z)
{
#ifdef CARTEX_MPI_PACK_Z_LOOP_GPU
    const auto y = blockIdx.y * blockDim.y + threadIdx.y;
    if (y < ext_y)
    {
        const auto ext_x_b = ext_x + halo_x_left + halo_x_right;
        const auto ext_y_b = ext_y + halo_y_left + halo_y_right;
        if (threadIdx.x < halo_x_left)
        {
            const auto x = threadIdx.x;
            for (int z = 0; z < ext_z; ++z)
            {
                const auto offset = ext_x_b * (y + halo_y_left + (z + halo_z_left) * ext_y_b);
                const auto offset_l = halo_x_left * (y + ext_y * z);
                buffer_left[x + offset_l] = field[x + ext_x + offset];
            }
        }
        else
        {
            const auto x = threadIdx.x - halo_x_left;
            for (int z = 0; z < ext_z; ++z)
            {
                const auto offset = ext_x_b * (y + halo_y_left + (z + halo_z_left) * ext_y_b);
                const auto offset_r = halo_x_right * (y + ext_y * z);
                buffer_right[x + offset_r] = field[x + halo_x_left + offset];
            }
        }
    }
#else
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
#endif
}

__global__ void
unpack_x_kernel(runtime::real_type* field, runtime::real_type const* buffer_left,
    runtime::real_type const* buffer_right, int halo_x_left, int halo_x_right, int halo_y_left,
    int halo_y_right, int halo_z_left, int halo_z_right, int ext_x, int ext_y, int ext_z)
{
#ifdef CARTEX_MPI_PACK_Z_LOOP_GPU
    const auto y = blockIdx.y * blockDim.y + threadIdx.y;
    if (y < ext_y)
    {
        const auto ext_x_b = ext_x + halo_x_left + halo_x_right;
        const auto ext_y_b = ext_y + halo_y_left + halo_y_right;
        if (threadIdx.x < halo_x_left)
        {
            const auto x = threadIdx.x;
            for (int z = 0; z < ext_z; ++z)
            {
                const auto offset = ext_x_b * (y + halo_y_left + (z + halo_z_left) * ext_y_b);
                const auto offset_l = halo_x_left * (y + ext_y * z);
                field[x + offset] = buffer_left[x + offset_l];
            }
        }
        else
        {
            const auto x = threadIdx.x - halo_x_left;
            for (int z = 0; z < ext_z; ++z)
            {
                const auto offset = ext_x_b * (y + halo_y_left + (z + halo_z_left) * ext_y_b);
                const auto offset_r = halo_x_right * (y + ext_y * z);
                field[x + halo_x_left + ext_x + offset] = buffer_right[x + offset_r];
            }
        }
    }
#else
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
#endif
}

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

#ifdef CARTEX_MPI_PACK_Z
__global__ void
pack_z_kernel(runtime::real_type const* field, runtime::real_type* buffer_left,
    runtime::real_type* buffer_right, int halo_x_left, int halo_x_right, int halo_y_left,
    int halo_y_right, int halo_z_left, int halo_z_right, int ext_x, int ext_y, int ext_z)
{
    const auto x = blockIdx.x * blockDim.x + threadIdx.x;
    const auto y = blockIdx.y * blockDim.y + threadIdx.y;
    const auto ext_x_b = ext_x + halo_x_left + halo_x_right;
    const auto ext_y_b = ext_y + halo_y_left + halo_y_right;
    if (x < ext_x_b && y < ext_y_b)
    {
        if (threadIdx.z < halo_z_left)
        {
            const auto z = threadIdx.z;
            const auto offset = ext_x_b * (y + ext_y_b * (z + ext_z));
            const auto offset_l = ext_x_b * (y + ext_y_b * z);
            buffer_left[x + offset_l] = field[x + offset];
        }
        else
        {
            const auto z = threadIdx.z - halo_z_left;
            const auto offset = ext_x_b * (y + ext_y_b * (z + halo_z_left));
            const auto offset_r = ext_x_b * (y + ext_y_b * z);
            buffer_left[x + offset_r] = field[x + offset];
        }
    }
}

__global__ void
unpack_z_kernel(runtime::real_type* field, runtime::real_type const* buffer_left,
    runtime::real_type const* buffer_right, int halo_x_left, int halo_x_right, int halo_y_left,
    int halo_y_right, int halo_z_left, int halo_z_right, int ext_x, int ext_y, int ext_z)
{
    const auto x = blockIdx.x * blockDim.x + threadIdx.x;
    const auto y = blockIdx.y * blockDim.y + threadIdx.y;
    const auto ext_x_b = ext_x + halo_x_left + halo_x_right;
    const auto ext_y_b = ext_y + halo_y_left + halo_y_right;
    if (x < ext_x_b && y < ext_y_b)
    {
        if (threadIdx.z < halo_z_left)
        {
            const auto z = threadIdx.z;
            const auto offset = ext_x_b * (y + ext_y_b * z);
            const auto offset_l = ext_x_b * (y + ext_y_b * z);
            field[x + offset] = buffer_left[x + offset_l];
        }
        else
        {
            const auto z = threadIdx.z - halo_z_left;
            const auto offset = ext_x_b * (y + ext_y_b * (z + ext_z + halo_z_left));
            const auto offset_r = ext_x_b * (y + ext_y_b * z);
            field[x + offset] = buffer_right[x + offset_r];
        }
    }
}
#endif

#endif

} // namespace cartex
} // namespace ghexbench
