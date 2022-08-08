/*
 * ghex-org
 *
 * Copyright (c) 2014-2022, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <chrono>
#include <iostream>
#include <ghexbench/accumulator.hpp>

namespace ghexbench
{
/** @brief timer with built-in statistics, unit: microseconds */
class timer : public accumulator
{
  private: // member types
    using base = accumulator;
    using clock_type = std::chrono::high_resolution_clock;
    using time_point = typename clock_type::time_point;
    using duration = typename clock_type::duration;

  private: // members
    time_point m_time_point = clock_type::now();

    double m_period_ms =
        (double)((clock_type::period::num * 1000000)) / clock_type::period::den; // microseconds

  private:
    double to_ms(duration const& d) const noexcept { return d.count() * m_period_ms; }

  public: // ctors
    timer() = default;
    timer(const base& b)
    : base(b)
    {
    }

    timer(base&& b)
    : base(std::move(b))
    {
    }

    timer(const timer&) noexcept = default;
    timer(timer&&) noexcept = default;
    timer& operator=(const timer&) noexcept = default;
    timer& operator=(timer&&) noexcept = default;

  public: // time functions
    /** @brief start timings */
    inline void tic() noexcept { m_time_point = clock_type::now(); }

    /** @brief stop timings */
    inline double toc() noexcept
    {
        auto const dt = plain_toc();
        this->operator()(dt);
        return dt;
    }

    /** @brief stop and start another timing period */
    inline double toc_tic() noexcept
    {
        auto const dt = plain_toc_tic();
        this->operator()(dt);
        return dt;
    }

    /** @brief stop timings without accumulating*/
    inline double plain_toc() noexcept { return to_ms(clock_type::now() - m_time_point); }

    /** @brief stop and start another timing period without accumulating*/
    inline double plain_toc_tic() noexcept
    {
        auto t2 = clock_type::now();
        auto const dt = to_ms(t2 - m_time_point);
        m_time_point = t2;
        return dt;
    }
};

} // namespace ghexbench
