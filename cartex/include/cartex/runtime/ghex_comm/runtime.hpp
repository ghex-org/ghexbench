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

#include <memory>
#include <vector>
#include <type_traits>
#include <sstream>

#ifndef GHEX_USE_UCP
#include <ghex/transport_layer/mpi/context.hpp>
using transport = gridtools::ghex::tl::mpi_tag;
//#pragma message "using mpi for transport"
#else
#include <ghex/transport_layer/ucx/context.hpp>
using transport = gridtools::ghex::tl::ucx_tag;
//#pragma message "using ucp for transport"
#endif
#include <ghex/bulk_communication_object.hpp>
#include <ghex/structured/pattern.hpp>
#include <ghex/structured/rma_range_generator.hpp>
#include <ghex/structured/regular/domain_descriptor.hpp>
#include <ghex/structured/regular/field_descriptor.hpp>
#include <ghex/structured/regular/halo_generator.hpp>

#include <cartex/runtime/runtime.hpp>

namespace cartex
{
using namespace gridtools::ghex;

class runtime::impl
{
  private:
    using context_type = typename tl::context_factory<transport>::context_type;
    using context_ptr_type = std::unique_ptr<context_type>;
    using domain_descriptor_type = structured::regular::domain_descriptor<int, 3>;
    using halo_generator_type = structured::regular::halo_generator<int, 3>;
    template<typename Arch, int... Is>
    using field_descriptor_type = structured::regular::field_descriptor<runtime::real_type, Arch,
        domain_descriptor_type, Is...>;
    using field_type = field_descriptor_type<cpu, 2, 1, 0>;
#ifdef __CUDACC__
    using gpu_field_type = field_descriptor_type<gpu, 2, 1, 0>;
#endif

  private:
    runtime&                             m_base;
    context_ptr_type                     m_context_ptr;
    context_type&                        m_context;
    halo_generator_type                  m_halo_gen;
    std::vector<domain_descriptor_type>  m_local_domains;
    std::vector<std::vector<field_type>> m_fields;
#ifdef __CUDACC__
    std::vector<std::vector<gpu_field_type>> m_fields_gpu;
#endif
    typename context_type::communicator_type              m_comm;
    std::vector<typename context_type::communicator_type> m_comms;
    std::vector<generic_bulk_communication_object>        m_cos;

    using pattern_type = std::remove_reference_t<decltype(
        make_pattern<structured::grid>(m_context, m_halo_gen, m_local_domains))>;
    std::unique_ptr<pattern_type> m_pattern;

  public:
    impl(runtime& base);
    void        init(int j);
    void        step(int j);
    std::string info() const
    {
#define CARTEX_STR2(var) #var
#define CARTEX_STR(var)  CARTEX_STR2(var)
        return CARTEX_STR(CARTEX_RUNTIME) " " CARTEX_STR(CARTEX_TRANSPORT) " " CARTEX_STR(
            CARTEX_RMA);
#undef CARTEX_STR
#undef CARTEX_STR2
    }
};

#include "../runtime.inc"

} // namespace cartex
