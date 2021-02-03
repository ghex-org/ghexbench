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

#include <cartex/runtime/ghex_comm/runtime.hpp>

namespace cartex
{
runtime::impl::impl(cartex::runtime& base)
: m_base{base}
, m_context_ptr{tl::context_factory<transport>::create(base.m_decomposition.mpi_comm())}
, m_context{*m_context_ptr}
, m_halo_gen(base.m_g_first, base.m_g_last, base.m_halos, std::array<bool, 3>{true, true, true})
, m_fields(base.m_num_threads)
#ifdef __CUDACC__
, m_fields_gpu(base.m_num_threads)
#endif
, m_comm{m_context.get_serial_communicator()}
, m_comms(base.m_num_threads, m_comm)
, m_cos(base.m_num_threads)
{
    m_local_domains.reserve(m_base.m_num_threads);
    for (int j = 0; j < m_base.m_num_threads; ++j)
    {
        const auto coord = m_base.m_decomposition.coord(j);
        int        x = coord[0] * m_base.m_ext[0];
        int        y = coord[1] * m_base.m_ext[1];
        int        z = coord[2] * m_base.m_ext[2];
        m_local_domains.push_back(domain_descriptor_type{
            m_context.rank() * m_base.m_num_threads + j, std::array<int, 3>{x, y, z},
            std::array<int, 3>{
                x + m_base.m_ext[0] - 1, y + m_base.m_ext[1] - 1, z + m_base.m_ext[2] - 1}});
    }
    m_pattern = std::unique_ptr<pattern_type>{
        new pattern_type{make_pattern<structured::grid>(m_context, m_halo_gen, m_local_domains)}};
}

void
runtime::impl::init(int j)
{
    m_comms[j] = m_context.get_communicator();
    for (int i = 0; i < m_base.m_num_fields; ++i)
    {
        m_fields[j].push_back(wrap_field<cpu, 2, 1, 0>(m_local_domains[j],
            m_base.m_raw_fields[j][i].host_data(), m_base.m_offset, m_base.m_ext_buffer));
#ifdef __CUDACC__
        m_fields_gpu[j].push_back(wrap_field<gpu, 2, 1, 0>(m_local_domains[j],
            m_base.m_raw_fields[j][i].device_data(), m_base.m_offset, m_base.m_ext_buffer));
#endif
    }

    auto bco = bulk_communication_object<structured::rma_range_generator, pattern_type,
#ifndef __CUDACC__
        field_type
#else
        gpu_field_type
#endif
        >(m_comms[j]);
#ifndef __CUDACC__
    for (int i = 0; i < m_base.m_num_fields; ++i)
        bco.add_field(m_pattern->operator()(m_fields[j][i]));
#else
    for (int i = 0; i < m_base.m_num_fields; ++i)
        bco.add_field(m_pattern->operator()(m_fields_gpu[j][i]));
#endif
    m_cos[j] = std::move(bco);
}

void
runtime::impl::step(int j)
{
    m_cos[j].exchange().wait();
}

} // namespace cartex
