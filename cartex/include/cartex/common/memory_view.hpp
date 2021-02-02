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

#include <array>
#include <numeric>
#include <cassert>
#include <iostream>
#include <iomanip>

#include <cartex/common/memory.hpp>

namespace cartex
{
template<typename T, unsigned int Dim>
class memory_view
{
  public:
    using memory_type = memory<T>;
    using array_type = std::array<unsigned int, Dim>;

  private:
    memory_type*         m;
    array_type           m_dims;
    array_type           m_strides;
    static constexpr int s_w = 6;

  public:
    memory_view(memory_type* mem, array_type dims)
    : m{mem}
    , m_dims{dims}
    {
        std::partial_sum(
            dims.begin(), dims.end(), m_strides.begin(), std::multiplies<unsigned int>());
        assert(m_strides[Dim - 1] == m->size());
    }

    template<typename... D>
    memory_view(memory_type* mem, D... dims)
    : memory_view(mem, array_type{(unsigned int)dims...})
    {
    }

    const T& operator()(array_type const& coord) const noexcept
    {
        unsigned int i = coord[0];
        for (unsigned int d = 1; d < Dim; ++d) i += coord[d] * m_strides[d - 1];
        return m->operator[](i);
    }

    template<typename... I>
    const T& operator()(I... i) const noexcept
    {
        return this->operator()(array_type{(unsigned int)i...});
    }

    T& operator()(array_type const& coord) noexcept
    {
        unsigned int i = coord[0];
        for (unsigned int d = 1; d < Dim; ++d) i += coord[d] * m_strides[d - 1];
        return m->operator[](i);
    }

    template<typename... I>
    T& operator()(I... i) noexcept
    {
        return this->operator()(array_type{(unsigned int)i...});
    }

    const memory_type& get() const noexcept { return *m; }

    memory_type& get() noexcept { return *m; }

    void print() const { print(Dim - 1, array_type{}); }

  private:
    void print(unsigned int d, array_type coord) const
    {
        if (d == 0)
        {
            const int y_s = (Dim > 1 ? coord[1] + 1 : 1);
            std::cout << std::setw(y_s) << " ";
            for (coord[0] = 0; coord[0] < m_dims[0]; ++coord[0])
                std::cout << std::setw(s_w) << std::right << this->operator()(coord);
        }
        else if (d == 1)
        {
            for (unsigned int i = 0; i < m_dims[1]; ++i)
            {
                coord[1] = m_dims[1] - 1 - i;
                print(0, coord);
                std::cout << "\n";
            }
        }
        else
        {
            for (unsigned int i = 0; i < m_dims[d]; ++i)
            {
                coord[d] = m_dims[d] - 1 - i;
                print(d - 1, coord);
                std::cout << "\n";
            }
        }
    };
};

} // namespace cartex
