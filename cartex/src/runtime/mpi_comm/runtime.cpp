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
runtime::impl::impl(cartex::runtime& base)
: m_base{base}
, m_comm{m_base.m_decomposition.mpi_comm()}
{
    for (int i = 0; i < m_base.m_num_threads; ++i)
    {
        m_neighbors.push_back(
            {m_base.m_decomposition.domain(i), m_base.m_decomposition.neighbor(i, -1, 0, 0),
                m_base.m_decomposition.neighbor(i, 1, 0, 0),
                m_base.m_decomposition.neighbor(i, 0, -1, 0),
                m_base.m_decomposition.neighbor(i, 0, 1, 0),
                m_base.m_decomposition.neighbor(i, 0, 0, -1),
                m_base.m_decomposition.neighbor(i, 0, 0, 1)});
    }
    MPI_Type_match_size(MPI_TYPECLASS_REAL, sizeof(runtime::real_type), &m_mpi_T);
    {
        x_recv_l = mpi_dtype_unique_ptr(new MPI_Datatype);
        x_recv_r = mpi_dtype_unique_ptr(new MPI_Datatype);
        x_send_l = mpi_dtype_unique_ptr(new MPI_Datatype);
        x_send_r = mpi_dtype_unique_ptr(new MPI_Datatype);
        const int x_recv_l_dims[3] = {m_base.m_halos[0], m_base.m_ext[1], m_base.m_ext[2]};
        const int x_recv_r_dims[3] = {m_base.m_halos[1], m_base.m_ext[1], m_base.m_ext[2]};
        const int x_recv_l_offsets[3] = {0, m_base.m_halos[2], m_base.m_halos[4]};
        const int x_recv_r_offsets[3] = {
            m_base.m_halos[0] + m_base.m_ext[0], m_base.m_halos[2], m_base.m_halos[4]};
        const int x_send_l_offsets[3] = {m_base.m_halos[0], m_base.m_halos[2], m_base.m_halos[4]};
        const int x_send_r_offsets[3] = {m_base.m_halos[0] + m_base.m_ext[0] - m_base.m_halos[1],
            m_base.m_halos[2], m_base.m_halos[4]};
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            x_recv_l_dims, x_recv_l_offsets, MPI_ORDER_FORTRAN, m_mpi_T, x_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            x_recv_r_dims, x_recv_r_offsets, MPI_ORDER_FORTRAN, m_mpi_T, x_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            x_recv_r_dims, x_send_l_offsets, MPI_ORDER_FORTRAN, m_mpi_T, x_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            x_recv_l_dims, x_send_r_offsets, MPI_ORDER_FORTRAN, m_mpi_T, x_send_r.get()));
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
        const int y_recv_l_dims[3] = {m_base.m_ext_buffer[0], m_base.m_halos[2], m_base.m_ext[2]};
        const int y_recv_r_dims[3] = {m_base.m_ext_buffer[0], m_base.m_halos[3], m_base.m_ext[2]};
        const int y_recv_l_offsets[3] = {0, 0, m_base.m_halos[4]};
        const int y_recv_r_offsets[3] = {0, m_base.m_halos[2] + m_base.m_ext[1], m_base.m_halos[4]};
        const int y_send_l_offsets[3] = {0, m_base.m_halos[2], m_base.m_halos[4]};
        const int y_send_r_offsets[3] = {
            0, m_base.m_halos[2] + m_base.m_ext[1] - m_base.m_halos[3], m_base.m_halos[4]};
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            y_recv_l_dims, y_recv_l_offsets, MPI_ORDER_FORTRAN, m_mpi_T, y_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            y_recv_r_dims, y_recv_r_offsets, MPI_ORDER_FORTRAN, m_mpi_T, y_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            y_recv_r_dims, y_send_l_offsets, MPI_ORDER_FORTRAN, m_mpi_T, y_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            y_recv_l_dims, y_send_r_offsets, MPI_ORDER_FORTRAN, m_mpi_T, y_send_r.get()));
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
        const int z_recv_l_dims[3] = {
            m_base.m_ext_buffer[0], m_base.m_ext_buffer[1], m_base.m_halos[4]};
        const int z_recv_r_dims[3] = {
            m_base.m_ext_buffer[0], m_base.m_ext_buffer[1], m_base.m_halos[5]};
        const int z_recv_l_offsets[3] = {0, 0, 0};
        const int z_recv_r_offsets[3] = {0, 0, m_base.m_halos[4] + m_base.m_ext[2]};
        const int z_send_l_offsets[3] = {0, 0, m_base.m_halos[4]};
        const int z_send_r_offsets[3] = {
            0, 0, m_base.m_halos[4] + m_base.m_ext[2] - m_base.m_halos[5]};
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            z_recv_l_dims, z_recv_l_offsets, MPI_ORDER_FORTRAN, m_mpi_T, z_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            z_recv_r_dims, z_recv_r_offsets, MPI_ORDER_FORTRAN, m_mpi_T, z_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            z_recv_r_dims, z_send_l_offsets, MPI_ORDER_FORTRAN, m_mpi_T, z_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_create_subarray(3, m_base.m_ext_buffer.data(),
            z_recv_l_dims, z_send_r_offsets, MPI_ORDER_FORTRAN, m_mpi_T, z_send_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(z_recv_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(z_recv_r.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(z_send_l.get()));
        CARTEX_CHECK_MPI_RESULT(MPI_Type_commit(z_send_r.get()));
    }
    CARTEX_CHECK_MPI_RESULT(MPI_Barrier(m_comm));
}

void
runtime::impl::step(int j)
{
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        CARTEX_CHECK_MPI_RESULT(
            MPI_Sendrecv(m_base.m_raw_fields[j][i].hd_data(), 1, *x_send_l, m_neighbors[j].x_l.rank,
                i + m_base.m_num_fields * 0 + m_base.m_num_fields * 6 * m_neighbors[j].d.thread,
                m_base.m_raw_fields[j][i].hd_data(), 1, *x_recv_r, m_neighbors[j].x_r.rank,
                i + m_base.m_num_fields * 0 + m_base.m_num_fields * 6 * m_neighbors[j].x_r.thread,
                m_comm, MPI_STATUS_IGNORE));
        CARTEX_CHECK_MPI_RESULT(
            MPI_Sendrecv(m_base.m_raw_fields[j][i].hd_data(), 1, *x_send_r, m_neighbors[j].x_r.rank,
                i + m_base.m_num_fields * 1 + m_base.m_num_fields * 6 * m_neighbors[j].d.thread,
                m_base.m_raw_fields[j][i].hd_data(), 1, *x_recv_l, m_neighbors[j].x_l.rank,
                i + m_base.m_num_fields * 1 + m_base.m_num_fields * 6 * m_neighbors[j].x_l.thread,
                m_comm, MPI_STATUS_IGNORE));

        CARTEX_CHECK_MPI_RESULT(
            MPI_Sendrecv(m_base.m_raw_fields[j][i].hd_data(), 1, *y_send_l, m_neighbors[j].y_l.rank,
                i + m_base.m_num_fields * 2 + m_base.m_num_fields * 6 * m_neighbors[j].d.thread,
                m_base.m_raw_fields[j][i].hd_data(), 1, *y_recv_r, m_neighbors[j].y_r.rank,
                i + m_base.m_num_fields * 2 + m_base.m_num_fields * 6 * m_neighbors[j].y_r.thread,
                m_comm, MPI_STATUS_IGNORE));
        CARTEX_CHECK_MPI_RESULT(
            MPI_Sendrecv(m_base.m_raw_fields[j][i].hd_data(), 1, *y_send_r, m_neighbors[j].y_r.rank,
                i + m_base.m_num_fields * 3 + m_base.m_num_fields * 6 * m_neighbors[j].d.thread,
                m_base.m_raw_fields[j][i].hd_data(), 1, *y_recv_l, m_neighbors[j].y_l.rank,
                i + m_base.m_num_fields * 3 + m_base.m_num_fields * 6 * m_neighbors[j].y_l.thread,
                m_comm, MPI_STATUS_IGNORE));

        CARTEX_CHECK_MPI_RESULT(
            MPI_Sendrecv(m_base.m_raw_fields[j][i].hd_data(), 1, *z_send_l, m_neighbors[j].z_l.rank,
                i + m_base.m_num_fields * 4 + m_base.m_num_fields * 6 * m_neighbors[j].d.thread,
                m_base.m_raw_fields[j][i].hd_data(), 1, *z_recv_r, m_neighbors[j].z_r.rank,
                i + m_base.m_num_fields * 4 + m_base.m_num_fields * 6 * m_neighbors[j].z_r.thread,
                m_comm, MPI_STATUS_IGNORE));
        CARTEX_CHECK_MPI_RESULT(
            MPI_Sendrecv(m_base.m_raw_fields[j][i].hd_data(), 1, *z_send_r, m_neighbors[j].z_r.rank,
                i + m_base.m_num_fields * 5 + m_base.m_num_fields * 6 * m_neighbors[j].d.thread,
                m_base.m_raw_fields[j][i].hd_data(), 1, *z_recv_l, m_neighbors[j].z_l.rank,
                i + m_base.m_num_fields * 5 + m_base.m_num_fields * 6 * m_neighbors[j].z_l.thread,
                m_comm, MPI_STATUS_IGNORE));
    }
}

} // namespace cartex
