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

#include <cartex/runtime/runtime.hpp>

#include <oomph/tensor/layout.hpp>
#include <oomph/tensor/sender.hpp>
#include <oomph/tensor/receiver.hpp>

#include <memory>
#include <vector>

namespace cartex
{
using namespace oomph;

class runtime::impl
{
  private:
    using domain_t = decomposition::domain_t;
    using layout_t = tensor::layout<2, 1, 0>;
    using map_t = tensor::map<runtime::real_type, layout_t>;
    using sender_t = tensor::sender<map_t>;
    using receiver_t = tensor::receiver<map_t>;

  private:
    runtime& m_base;
    context  m_context;
    struct field
    {
        map_t                     m_map;
        std::array<sender_t, 3>   m_senders;
        std::array<receiver_t, 3> m_receivers;
        field(map_t&& m)
        : m_map{std::move(m)}
        , m_senders{tensor::make_sender(m_map), tensor::make_sender(m_map),
              tensor::make_sender(m_map)}
        , m_receivers{tensor::make_receiver(m_map), tensor::make_receiver(m_map),
              tensor::make_receiver(m_map)}
        {
        }
    };
    std::vector<std::vector<field>> m_fields;

  public:
    impl(runtime& base, options_values const& options);

    void init(int j);
    void step(int j);
    void exit(int j);

    std::string info() const
    {
#define CARTEX_STR2(var) #var
#define CARTEX_STR(var)  CARTEX_STR2(var)
        return CARTEX_STR(CARTEX_RUNTIME) " " CARTEX_STR(CARTEX_TRANSPORT);
#undef CARTEX_STR
#undef CARTEX_STR2
    }
};

} // namespace cartex
