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
    return field_id + num_fields * (dim * 2 + (left ? 0 : 1)) + num_fields * 6 * d.thread;
}

int
runtime::impl::neighborhood::recvtag(int field_id, int dim, bool left) const noexcept
{
    const auto tid_l = (dim == 0 ? x_r.thread : (dim == 1 ? y_r.thread : z_r.thread));
    const auto tid_r = (dim == 0 ? x_l.thread : (dim == 1 ? y_l.thread : z_l.thread));
    return field_id + num_fields * (dim * 2 + (left ? 0 : 1)) +
           num_fields * 6 * (left ? tid_l : tid_r);
}

void
runtime::impl::neighborhood::exchange(memory_type& field, int field_id)
{
    runtime::real_type* data = field.hd_data();

    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *x_send_l, x_l.rank, sendtag(field_id, 0, true),
        data, 1, *x_recv_r, x_r.rank, recvtag(field_id, 0, true), comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *x_send_r, x_r.rank, sendtag(field_id, 0, false),
        data, 1, *x_recv_l, x_l.rank, recvtag(field_id, 0, false), comm, MPI_STATUS_IGNORE));

    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *y_send_l, y_l.rank, sendtag(field_id, 1, true),
        data, 1, *y_recv_r, y_r.rank, recvtag(field_id, 1, true), comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *y_send_r, y_r.rank, sendtag(field_id, 1, false),
        data, 1, *y_recv_l, y_l.rank, recvtag(field_id, 1, false), comm, MPI_STATUS_IGNORE));

    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *z_send_l, z_l.rank, sendtag(field_id, 2, true),
        data, 1, *z_recv_r, z_r.rank, recvtag(field_id, 2, true), comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *z_send_r, z_r.rank, sendtag(field_id, 2, false),
        data, 1, *z_recv_l, z_l.rank, recvtag(field_id, 2, false), comm, MPI_STATUS_IGNORE));
}

void
runtime::impl::neighborhood::pack_x(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
    for (int z = 0; z < d.domain_ext[2]; ++z)
    {
        for (int y = 0; y < d.domain_ext[1]; ++y)
        {
            const auto offset =
                (m_halos[0] + m_halos[1] + d.domain_ext[0]) *
                (y + m_halos[2] + (z + m_halos[4]) * (m_halos[2] + m_halos[3] + d.domain_ext[1]));
            const auto offset_l = m_halos[0] * (y + d.domain_ext[1] * z);
            const auto offset_r = m_halos[1] * (y + d.domain_ext[1] * z);
            for (int x = 0; x < m_halos[0]; ++x)
                buffer_left[x + offset_l] = field[x + d.domain_ext[0] + offset];
            for (int x = 0; x < m_halos[1]; ++x)
                buffer_right[x + offset_r] = field[x + m_halos[0] + offset];
        }
    }
}

void
runtime::impl::neighborhood::unpack_x(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
    for (int z = 0; z < d.domain_ext[2]; ++z)
    {
        for (int y = 0; y < d.domain_ext[1]; ++y)
        {
            const auto offset =
                (m_halos[0] + m_halos[1] + d.domain_ext[0]) *
                (y + m_halos[2] + (z + m_halos[4]) * (m_halos[2] + m_halos[3] + d.domain_ext[1]));
            const auto offset_l = m_halos[0] * (y + d.domain_ext[1] * z);
            const auto offset_r = m_halos[1] * (y + d.domain_ext[1] * z);
            for (int x = 0; x < m_halos[0]; ++x) field[x + offset] = buffer_left[x + offset_l];
            for (int x = 0; x < m_halos[1]; ++x)
                field[x + m_halos[0] + d.domain_ext[0] + offset] = buffer_right[x + offset_r];
        }
    }
}

void
runtime::impl::neighborhood::pack_y(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
    for (int z = 0; z < d.domain_ext[2]; ++z)
    {
        for (int y = 0; y < m_halos[2]; ++y)
        {
            const auto offset = (m_halos[0] + m_halos[1] + d.domain_ext[0]) *
                                (y + d.domain_ext[2] +
                                    (z + m_halos[4]) * (m_halos[2] + m_halos[3] + d.domain_ext[1]));
            const auto offset_l =
                (m_halos[0] + m_halos[1] + d.domain_ext[0]) * (y + m_halos[2] * z);
            for (int x = 0; x < d.domain_ext[0] + m_halos[0] + m_halos[1]; ++x)
                buffer_left[x + offset_l] = field[x + offset];
        }
        for (int y = 0; y < m_halos[3]; ++y)
        {
            const auto offset =
                (m_halos[0] + m_halos[1] + d.domain_ext[0]) *
                (y + m_halos[2] + (z + m_halos[4]) * (m_halos[2] + m_halos[3] + d.domain_ext[1]));
            const auto offset_r =
                (m_halos[0] + m_halos[1] + d.domain_ext[0]) * (y + m_halos[3] * z);
            for (int x = 0; x < d.domain_ext[0] + m_halos[0] + m_halos[1]; ++x)
                buffer_right[x + offset_r] = field[x + offset];
        }
    }
}

void
runtime::impl::neighborhood::unpack_y(
    memory_type& field, memory_type& buffer_left, memory_type& buffer_right)
{
    for (int z = 0; z < d.domain_ext[2]; ++z)
    {
        for (int y = 0; y < m_halos[2]; ++y)
        {
            const auto offset =
                (m_halos[0] + m_halos[1] + d.domain_ext[0]) *
                (y + (z + m_halos[4]) * (m_halos[2] + m_halos[3] + d.domain_ext[1]));
            const auto offset_l =
                (m_halos[0] + m_halos[1] + d.domain_ext[0]) * (y + m_halos[2] * z);
            for (int x = 0; x < d.domain_ext[0] + m_halos[0] + m_halos[1]; ++x)
                field[x + offset] = buffer_left[x + offset_l];
        }
        for (int y = 0; y < m_halos[3]; ++y)
        {
            const auto offset = (m_halos[0] + m_halos[1] + d.domain_ext[0]) *
                                (y + m_halos[2] + d.domain_ext[1] +
                                    (z + m_halos[4]) * (m_halos[2] + m_halos[3] + d.domain_ext[1]));
            const auto offset_r =
                (m_halos[0] + m_halos[1] + d.domain_ext[0]) * (y + m_halos[3] * z);
            for (int x = 0; x < d.domain_ext[0] + m_halos[0] + m_halos[1]; ++x)
                field[x + offset] = buffer_right[x + offset_r];
        }
    }
}

void
runtime::impl::neighborhood::exchange(memory_type& field, std::vector<memory_type>& send_buffers,
    std::vector<memory_type>& recv_buffers, int field_id)
{
    pack_x(field, send_buffers[0], send_buffers[1]);
    CARTEX_CHECK_MPI_RESULT(
        MPI_Sendrecv(send_buffers[1].hd_data(), send_buffers[1].size() * sizeof(runtime::real_type),
            MPI_BYTE, x_l.rank, sendtag(field_id, 0, true), recv_buffers[1].hd_data(),
            recv_buffers[1].size() * sizeof(runtime::real_type), MPI_BYTE, x_r.rank,
            recvtag(field_id, 0, true), comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Sendrecv(send_buffers[0].hd_data(), send_buffers[0].size() * sizeof(runtime::real_type),
            MPI_BYTE, x_r.rank, sendtag(field_id, 0, false), recv_buffers[0].hd_data(),
            recv_buffers[0].size() * sizeof(runtime::real_type), MPI_BYTE, x_l.rank,
            recvtag(field_id, 0, false), comm, MPI_STATUS_IGNORE));
    unpack_x(field, recv_buffers[0], recv_buffers[1]);

    pack_y(field, send_buffers[2], send_buffers[3]);
    CARTEX_CHECK_MPI_RESULT(
        MPI_Sendrecv(send_buffers[3].hd_data(), send_buffers[3].size() * sizeof(runtime::real_type),
            MPI_BYTE, y_l.rank, sendtag(field_id, 1, true), recv_buffers[3].hd_data(),
            recv_buffers[3].size() * sizeof(runtime::real_type), MPI_BYTE, y_r.rank,
            recvtag(field_id, 1, true), comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Sendrecv(send_buffers[2].hd_data(), send_buffers[2].size() * sizeof(runtime::real_type),
            MPI_BYTE, y_r.rank, sendtag(field_id, 1, false), recv_buffers[2].hd_data(),
            recv_buffers[2].size() * sizeof(runtime::real_type), MPI_BYTE, y_l.rank,
            recvtag(field_id, 1, false), comm, MPI_STATUS_IGNORE));
    unpack_y(field, recv_buffers[2], recv_buffers[3]);

    const auto z0 = field.hd_data();
    const auto z1 = field.hd_data() + z_plane * m_halos[4];
    const auto z2 = field.hd_data() + z_plane * d.domain_ext[2];
    const auto z3 = field.hd_data() + z_plane * (m_halos[4] + d.domain_ext[2]);
    const auto left_size = z_plane * m_halos[5] * sizeof(runtime::real_type);
    const auto right_size = z_plane * m_halos[4] * sizeof(runtime::real_type);
    CARTEX_CHECK_MPI_RESULT(
        MPI_Sendrecv(z1, left_size, MPI_BYTE, z_l.rank, sendtag(field_id, 2, true), z3, left_size,
            MPI_BYTE, z_r.rank, recvtag(field_id, 2, true), comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(
        MPI_Sendrecv(z2, right_size, MPI_BYTE, z_r.rank, sendtag(field_id, 2, false), z0,
            right_size, MPI_BYTE, z_l.rank, recvtag(field_id, 2, false), comm, MPI_STATUS_IGNORE));
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
