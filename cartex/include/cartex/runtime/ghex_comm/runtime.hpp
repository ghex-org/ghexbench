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

#include <ghex/config.hpp>
#include <ghex/context.hpp>
#include <ghex/bulk_communication_object.hpp>
#include <ghex/structured/pattern.hpp>
#include <ghex/structured/rma_range_generator.hpp>
#include <ghex/structured/regular/domain_descriptor.hpp>
#include <ghex/structured/regular/field_descriptor.hpp>
#include <ghex/structured/regular/halo_generator.hpp>
#ifdef CARTEX_GHEX_STAGED
#include <ghex/structured/regular/make_pattern.hpp>
#endif

#include <cartex/runtime/runtime.hpp>

#include <memory>
#include <vector>
#include <type_traits>
#include <sstream>

namespace ghexbench
{
namespace cartex
{
using namespace ghex;

class runtime::impl
{
  private:
    using domain_descriptor_type =
        structured::regular::domain_descriptor<int, std::integral_constant<int, 3>>;
    using halo_generator_type =
        structured::regular::halo_generator<int, std::integral_constant<int, 3>>;
    template<typename Arch, int... Is>
    using field_descriptor_type = structured::regular::field_descriptor<runtime::real_type, Arch,
        domain_descriptor_type, gridtools::layout_map<Is...>>;
    using field_type = field_descriptor_type<cpu, 2, 1, 0>;
#ifdef __CUDACC__
    using gpu_field_type = field_descriptor_type<gpu, 2, 1, 0>;
#endif

  private:
    runtime&                             m_base;
    context                              m_context;
    halo_generator_type                  m_halo_gen;
    std::vector<domain_descriptor_type>  m_local_domains;
    std::vector<std::vector<field_type>> m_fields;
#ifdef __CUDACC__
    std::vector<std::vector<gpu_field_type>> m_fields_gpu;
#endif
#ifndef CARTEX_GHEX_STAGED
    std::vector<generic_bulk_communication_object> m_cos;
#else
    std::vector<std::array<generic_bulk_communication_object, 3>> m_cos;
#endif
    bool m_node_local;

    using pattern_type = std::remove_reference_t<decltype(
        make_pattern<structured::grid>(m_context, m_halo_gen, m_local_domains))>;
#ifndef CARTEX_GHEX_STAGED
    std::unique_ptr<pattern_type> m_pattern;
#else
    std::array<std::unique_ptr<pattern_type>, 3>                  m_pattern;
#endif

  public:
    impl(runtime& base, options_values const& options);
    void        init(int j);
    void        step(int j);
    void        exit(int j);
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

} // namespace cartex
} // namespace ghexbench
