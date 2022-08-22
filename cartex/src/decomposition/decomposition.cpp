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

//#include <string>
//#include <stdexcept>
//#include <mpi.h>

#include <iostream>

#include <cartex/decomposition/decomposition.hpp>

namespace ghexbench
{
namespace cartex
{

hwcart_order_t decomposition::parse_order(std::string const& order_str)
{
    if (order_str == "XYZ") return HWCartOrderXYZ;
    else if (order_str == "XZY")
        return HWCartOrderXZY;
    else if (order_str == "ZYX")
        return HWCartOrderZYX;
    else if (order_str == "YZX")
        return HWCartOrderYZX;
    else if (order_str == "ZXY")
        return HWCartOrderZXY;
    else if (order_str == "YXZ")
        return HWCartOrderYXZ;
    else
    {
        std::cerr << "warning: unrecognized order, using XYZ" << std::endl;
        return HWCartOrderXYZ;
    }
}

//decomposition::~decomposition()
//{
//    hwcart_comm_free(&m_comm);
//    if (!m_mpi_cart) MPI_Comm_free(&m_cart_comm);
//}
//
void
decomposition::init_domain(const arr& domain_, bool local)
{
    //auto coord = m_hw_topo.grid_coord();
    //auto global_decomposition = m_hw_topo.decomposition();
    //auto coord = m_coord;
    for (int d = 0; d < 3; ++d)
    {
        m_coord[d] *= m_thread_decomposition[d];
        const int dim = m_global_decomposition[d] * m_thread_decomposition[d];
        m_global_domain[d] = (local ? dim * domain_[d] : domain_[d]);
        const int ext = (double)m_global_domain[d] / dim;
        m_domain_coord[d].resize(dim + 1, 0);
        m_domain_ext[d].resize(dim, 0);
        const int j = m_global_domain[d] - dim * ext;
        for (int i = 0; i < dim; ++i)
        {
            m_domain_coord[d][i + 1] = m_domain_coord[d][i] + ext + (i < j ? 1 : 0);
            m_domain_ext[d][i] = ext + (i < j ? 1 : 0);
        }
        m_last_domain_coord[d] = m_domain_coord[d].back() - 1;
    }
}
//
//decomposition::decomposition(const std::string& order, const arr& thread_d, std::vector<int>&& topo,
//    std::vector<hwcart_split_t>&& levels, const arr& domain_, bool local)
//: m_hw_topo()
//, m_order{parse_order(order)}
//, m_thread_decomposition(thread_d)
//, m_topo{topo}
//, m_levels{levels}
//, m_threads_per_rank{thread_d[0] * thread_d[1] * thread_d[2]}
//{
//    if (hwcart_create(m_hw_topo.m, MPI_COMM_WORLD, m_levels.size(), m_levels.data(), m_topo.data(),
//            m_order, &m_comm))
//        throw std::runtime_error("hwcart create failed");
//    for (int i = 0; i < 3; ++i)
//    {
//        m_global_decomposition[i] = 1;
//        for (unsigned int j = 0; j < m_levels.size(); ++j)
//            m_global_decomposition[i] *= m_topo[j * 3 + i];
//        m_last_coord[i] = m_global_decomposition[i] * m_thread_decomposition[i] - 1;
//    }
//    CARTEX_CHECK_MPI_RESULT(MPI_Comm_rank(m_comm, &m_rank));
//    CARTEX_CHECK_MPI_RESULT(MPI_Comm_size(m_comm, &m_size));
//    hwcart_rank2coord(m_comm, m_global_decomposition.data(), m_rank, m_order, m_coord.data());
//    int periodic[3] = {1, 1, 1};
//    hwcart2mpicart(m_comm, m_levels.size(), m_topo.data(), periodic, m_order, &m_cart_comm);
//    init_domain(domain_, local);
//}

decomposition::decomposition(hw_topo_builder builder, const arr& thread_d, arr const & domain_, bool local)
: m_hw_topo{builder}
, m_cart_comm{m_hw_topo.make_cart_comm()}
, m_thread_decomposition{thread_d}
, m_threads_per_rank{thread_d[0] * thread_d[1] * thread_d[2]}
, m_global_decomposition{m_hw_topo.decomposition()}
, m_coord{m_hw_topo.grid_coord()}
, m_rank{m_hw_topo.rank()}
, m_size{m_hw_topo.size()}
{
    for (int i = 0; i < 3; ++i)
    {
        m_last_coord[i] = m_global_decomposition[i] * m_thread_decomposition[i] - 1;
    }

    init_domain(domain_, local);
}

decomposition::decomposition(MPI_Comm comm,
    const arr& global_d, const arr& thread_d, const arr& domain_, bool local)
: m_thread_decomposition{thread_d}
, m_threads_per_rank{thread_d[0] * thread_d[1] * thread_d[2]}
, m_global_decomposition{global_d}
, m_use_mpi_cart{true}
{
    const int periods[3] = {1, 1, 1};
    MPI_Comm new_comm;
    GHEXBENCH_CHECK_MPI_RESULT(
        MPI_Cart_create(comm, 3, global_d.data(), periods, 1, &new_comm));
    for (int i = 0; i < 3; ++i)
        m_last_coord[i] = m_global_decomposition[i] * m_thread_decomposition[i] - 1;
    GHEXBENCH_CHECK_MPI_RESULT(MPI_Comm_rank(new_comm, &m_rank));
    GHEXBENCH_CHECK_MPI_RESULT(MPI_Comm_size(new_comm, &m_size));
    GHEXBENCH_CHECK_MPI_RESULT(MPI_Cart_coords(new_comm, m_rank, 3, m_coord.data()));

    m_cart_comm = mpi_comm_holder{new_comm};
//    m_cart_comm = m_comm;
    init_domain(domain_, local);
}

MPI_Comm
decomposition::mpi_comm() const noexcept
{
    if(!m_use_mpi_cart)
        return m_hw_topo.get_comm();
    else
        return m_cart_comm;
}

//
//decomposition::decomposition(
//    const arr& global_d, const arr& thread_d, const arr& domain_, bool local)
//: m_mpi_cart{true}
//, m_hw_topo()
//, m_order{HWCartOrderXYZ}
//, m_thread_decomposition(thread_d)
//, m_topo{global_d[0], global_d[1], global_d[2]}
//, m_levels{HWCART_MD_NODE}
//, m_global_decomposition{global_d}
//, m_threads_per_rank{thread_d[0] * thread_d[1] * thread_d[2]}
//{
//    const int periods[3] = {1, 1, 1};
//    CARTEX_CHECK_MPI_RESULT(
//        MPI_Cart_create(MPI_COMM_WORLD, 3, global_d.data(), periods, 1, &m_comm));
//    for (int i = 0; i < 3; ++i)
//        m_last_coord[i] = m_global_decomposition[i] * m_thread_decomposition[i] - 1;
//    CARTEX_CHECK_MPI_RESULT(MPI_Comm_rank(m_comm, &m_rank));
//    CARTEX_CHECK_MPI_RESULT(MPI_Comm_size(m_comm, &m_size));
//    CARTEX_CHECK_MPI_RESULT(MPI_Cart_coords(m_comm, m_rank, 3, m_coord.data()));
//    m_cart_comm = m_comm;
//    init_domain(domain_, local);
//}
//
void
decomposition::print()
{
    if(!m_use_mpi_cart)
    {
        m_hw_topo.print();
//        hwcart_print_rank_topology(
//            m_hw_topo.m, m_comm, m_levels.size(), m_levels.data(), m_topo.data(), m_order);
    }
    else
    {
        if (m_rank == 0) std::cout << "cannot print topology for MPI_Cart" << std::endl;
    }
}

decomposition::arr
decomposition::coord(int thread_id) const noexcept
{
//    //arr res = m_hw_topo.grid_coord();
//    auto res = m_coord;
//    for (unsigned i=0; i<3; ++i)
//    {
//        res[i] *= m_thread_decomposition[i];
//        res[i] += thread_id % m_thread_decomposition[i];
//        thread_id /= m_thread_decomposition[i];
//    }
//    return res;
    arr res(m_coord);
    res[0] += thread_id % m_thread_decomposition[0];
    thread_id /= m_thread_decomposition[0];
    res[1] += thread_id % m_thread_decomposition[1];
    thread_id /= m_thread_decomposition[1];
    res[2] += thread_id;
    //std::cout << "thread coord = " << res[0] << ", " << res[1] << ", " << res[2] << std::endl;
    return res;
}

decomposition::domain_t
decomposition::domain(int thread_id) const noexcept
{
    const auto  c = coord(thread_id);
    //auto const& global_decomposition = m_hw_topo.decomposition();
    //
    return {c[0] + m_global_decomposition[0] * m_thread_decomposition[0] *
                       (c[1] + m_global_decomposition[1] * m_thread_decomposition[1] * c[2]),
        m_rank, thread_id, c,
        {m_domain_coord[0][c[0]], m_domain_coord[1][c[1]], m_domain_coord[2][c[2]]},
        {m_domain_ext[0][c[0]], m_domain_ext[1][c[1]], m_domain_ext[2][c[2]]}};
}

decomposition::domain_t
decomposition::neighbor(int thread_id, int dx, int dy, int dz) const noexcept
{
    //auto coord = m_hw_topo.grid_coord();
    auto c = coord(thread_id);
    c[0] += dx;
    c[1] += dy;
    c[2] += dz;

    for (int i = 0; i < 3; ++i)
    {
        if (c[i] > m_last_coord[i]) c[i] -= m_last_coord[i] + 1;
        if (c[i] < 0) c[i] += m_last_coord[i] + 1;
    }
    //auto const& global_decomposition = m_hw_topo.decomposition();
    const int id = c[0] + m_global_decomposition[0] * m_thread_decomposition[0] *
                              (c[1] + m_global_decomposition[1] * m_thread_decomposition[1] * c[2]);

    const arr domain_coord{
        m_domain_coord[0][c[0]], m_domain_coord[1][c[1]], m_domain_coord[2][c[2]]};
    const arr domain_ext{m_domain_ext[0][c[0]], m_domain_ext[1][c[1]], m_domain_ext[2][c[2]]};

    arr ct = c;
    arr c0;
    for (int i = 0; i < 3; ++i)
    {
        c0[i] = c[i] / m_thread_decomposition[i];
        ct[i] = c[i] - c0[i] * m_thread_decomposition[i];
    }
    const int t_id =
        ct[0] + m_thread_decomposition[0] * (ct[1] + m_thread_decomposition[1] * ct[2]);

    if (c0[0] * m_thread_decomposition[0] == m_coord[0] &&
        c0[1] * m_thread_decomposition[1] == m_coord[1] &&
        c0[2] * m_thread_decomposition[2] == m_coord[2])
    { return {id, m_rank, t_id, c, domain_coord, domain_ext}; }
    else
    {
        int n_rank = 0;
        if(m_use_mpi_cart)
        {
            GHEXBENCH_CHECK_MPI_RESULT(MPI_Cart_rank(m_cart_comm, c0.data(), &n_rank));
        }
        else
        {
           n_rank = m_hw_topo.rank(c0);
        }

//        int n_rank;
//        int periodic[3] = {1, 1, 1};
//        int dims[3] = {
//            m_global_decomposition[0], m_global_decomposition[1], m_global_decomposition[2]};
//        if (m_mpi_cart) { CARTEX_CHECK_MPI_RESULT(MPI_Cart_rank(m_comm, c0.data(), &n_rank)); }
//        else
//        {
//            hwcart_coord2rank(m_comm, dims, periodic, c0.data(), m_order, &n_rank);
//        }
        return {id, n_rank, t_id, c, domain_coord, domain_ext};
    }
}

std::vector<decomposition::domain_t>
decomposition::domains() const noexcept
{
    std::vector<domain_t> res;
    res.reserve(threads_per_rank());
    for (int i = 0; i < threads_per_rank(); ++i) res.push_back(domain(i));
    return res;
}

} // namespace cartex
} // namespace ghexbench
