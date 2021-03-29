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

#include <vector>
//#include <boost/variant.hpp>

#include <cartex/runtime/runtime.hpp>
#include <gridtools/gcl/GCL.hpp>
#include <gridtools/gcl/halo_exchange.hpp>

namespace cartex
{
namespace gt = gridtools;

class runtime::impl
{
  private:
    //    template<unsigned int I, unsigned int J, unsigned int K>
    //    using pt = gt::gcl::halo_exchange_dynamic_ut<gt::layout_map<2, 1, 0>, gt::layout_map<I, J, K>,
    //        runtime::real_type,
    //#ifdef __CUDACC__
    //        gt::gcl::gpu
    //#else
    //        gt::gcl::cpu
    //#endif
    //        >;

    //template<typename T>
    //static std::unique_ptr<T> make_pattern_impl(MPI_Comm comm)
    //{
    //    return std::make_unique<T>(typename T::grid_type::period_type{true, true, true}, comm);
    //}
    //
    //static pattern_variant_t make_pattern(options_values const & options, MPI_Comm comm)
    //{
    //    const auto order = options.get_or("order",  std::string("XYZ"));
    //    if (options.has("MPICart"))  return pattern_variant_t(make_pattern_impl<pt<0,1,2>>(comm));
    //    else   if (order == "XYZ")   return pattern_variant_t(make_pattern_impl<pt<2,1,0>>(comm));
    //    else   if (order == "XZY")   return pattern_variant_t(make_pattern_impl<pt<2,0,1>>(comm));
    //    else   if (order == "YXZ")   return pattern_variant_t(make_pattern_impl<pt<1,2,0>>(comm));
    //    else   if (order == "YZX")   return pattern_variant_t(make_pattern_impl<pt<0,2,1>>(comm));
    //    else   if (order == "ZXY")   return pattern_variant_t(make_pattern_impl<pt<1,0,2>>(comm));
    //    else /*if (order == "ZYX")*/ return pattern_variant_t(make_pattern_impl<pt<0,1,2>>(comm));
    //}
    //
    //using pattern_variant_t = boost::variant<std::unique_ptr<pt<0, 1, 2>>,
    //    std::unique_ptr<pt<0, 2, 1>>, std::unique_ptr<pt<1, 0, 2>>, std::unique_ptr<pt<2, 0, 1>>,
    //    std::unique_ptr<pt<1, 2, 0>>, std::unique_ptr<pt<2, 1, 0>>>;

    using pattern_type = gt::gcl::halo_exchange_dynamic_ut<gt::layout_map<2, 1, 0>,
        gt::layout_map<0, 1, 2>, runtime::real_type,
#ifdef __CUDACC__
        gt::gcl::gpu
#else
        gt::gcl::cpu
#endif
        >;

  private:
    runtime&     m_base;
    pattern_type m_pattern;
    //pattern_variant_t                m_pattern_v;
    std::vector<runtime::real_type*> m_field_ptrs;

  public:
    impl(runtime& base, options_values const& options);

    void        init(int);
    void        step(int j);
    std::string info() const
    {
#define CARTEX_STR2(var) #var
#define CARTEX_STR(var)  CARTEX_STR2(var)
        return CARTEX_STR(CARTEX_RUNTIME);
#undef CARTEX_STR
#undef CARTEX_STR2
    }
};

} // namespace cartex
