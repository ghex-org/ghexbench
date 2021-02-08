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

namespace cartex
{
runtime::impl::neighborhood::neighborhood(
    int i, decomposition& decomp, MPI_Datatype mpi_T, std::array<int, 6> const& halos)
: comm(decomp.mpi_comm())
, num_fields{decomp.threads_per_rank()}
, d{decomp.domain(i)}
, x_l{decomp.neighbor(i, -1, 0, 0)}
, x_r{decomp.neighbor(i, 1, 0, 0)}
, y_l{decomp.neighbor(i, 0, -1, 0)}
, y_r{decomp.neighbor(i, 0, 1, 0)}
, z_l{decomp.neighbor(i, 0, 0, -1)}
, z_r{decomp.neighbor(i, 0, 0, 1)}
, ext_buffer{d.domain_ext[0] + halos[0] + halos[1], d.domain_ext[1] + halos[2] + halos[3],
      d.domain_ext[2] + halos[4] + halos[5]}
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

void
runtime::impl::neighborhood::neighborhood::exchange(void* data, int field_id)
{
    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *x_send_l, x_l.rank,
        field_id + num_fields * 0 + num_fields * 6 * d.thread, data, 1, *x_recv_r, x_r.rank,
        field_id + num_fields * 0 + num_fields * 6 * x_r.thread, comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *x_send_r, x_r.rank,
        field_id + num_fields * 1 + num_fields * 6 * d.thread, data, 1, *x_recv_l, x_l.rank,
        field_id + num_fields * 1 + num_fields * 6 * x_l.thread, comm, MPI_STATUS_IGNORE));

    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *y_send_l, y_l.rank,
        field_id + num_fields * 2 + num_fields * 6 * d.thread, data, 1, *y_recv_r, y_r.rank,
        field_id + num_fields * 2 + num_fields * 6 * y_r.thread, comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *y_send_r, y_r.rank,
        field_id + num_fields * 3 + num_fields * 6 * d.thread, data, 1, *y_recv_l, y_l.rank,
        field_id + num_fields * 3 + num_fields * 6 * y_l.thread, comm, MPI_STATUS_IGNORE));

    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *z_send_l, z_l.rank,
        field_id + num_fields * 4 + num_fields * 6 * d.thread, data, 1, *z_recv_r, z_r.rank,
        field_id + num_fields * 4 + num_fields * 6 * z_r.thread, comm, MPI_STATUS_IGNORE));
    CARTEX_CHECK_MPI_RESULT(MPI_Sendrecv(data, 1, *z_send_r, z_r.rank,
        field_id + num_fields * 5 + num_fields * 6 * d.thread, data, 1, *z_recv_l, z_l.rank,
        field_id + num_fields * 5 + num_fields * 6 * z_l.thread, comm, MPI_STATUS_IGNORE));
}

runtime::impl::impl(cartex::runtime& base)
: m_base{base}
, m_comm{m_base.m_decomposition.mpi_comm()}
{
    MPI_Type_match_size(MPI_TYPECLASS_REAL, sizeof(runtime::real_type), &m_mpi_T);
    m_neighbors.reserve(m_base.m_num_threads);
    for (int i = 0; i < m_base.m_num_threads; ++i)
        m_neighbors.emplace_back(i, m_base.m_decomposition, m_mpi_T, m_base.m_halos);
    CARTEX_CHECK_MPI_RESULT(MPI_Barrier(m_comm));
}

void
runtime::impl::step(int j)
{
    for (int i = 0; i < m_base.m_num_fields; ++i)
        m_neighbors[j].exchange(m_base.m_raw_fields[j][i].hd_data(), i);
}

} // namespace cartex
