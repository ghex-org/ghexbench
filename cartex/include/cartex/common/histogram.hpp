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
#include <algorithm>
#include <iomanip>
#include <cartex/common/accumulator.hpp>

namespace cartex
{
class histogram
{
  public:
    using size_type = accumulator::size_type;
    using value_type = accumulator::value_type;

  private:
    accumulator             m_acc;
    std::vector<value_type> m_data;
    value_type              m_scale;
    size_type               m_nbins;
    value_type              m_sigma;

  public:
    histogram(value_type scale, size_type nbins = 11, value_type sigma = 2, size_type n = 128)
    : m_scale{scale}
    , m_nbins{nbins % 2 == 0 ? nbins + 1 : nbins}
    , m_sigma{sigma}
    {
        m_data.reserve(n);
    }
    histogram(const histogram&) = default;
    histogram(histogram&&) noexcept = default;
    histogram& operator=(const histogram&) = default;
    histogram& operator=(histogram&&) noexcept = default;

  public:
    inline size_type  num_samples() const noexcept { return m_acc.num_samples(); }
    inline value_type min() const noexcept { return m_acc.min() * m_scale; }
    inline value_type max() const noexcept { return m_acc.max() * m_scale; }
    inline value_type mean() const noexcept { return m_acc.mean() * m_scale; }
    inline value_type variance() const noexcept { return m_acc.variance() * (m_scale * m_scale); }
    inline value_type stddev() const noexcept { return m_acc.stddev() * m_scale; }
    inline value_type sum() const noexcept { return m_acc.sum() * m_scale; }
    inline value_type median() noexcept
    {
        std::sort(m_data.begin(), m_data.end());
        return m_data[num_samples() / 2] * m_scale;
    }
    inline value_type percentile(value_type percent) noexcept
    {
        std::sort(m_data.begin(), m_data.end());
        return m_data[(size_type)(num_samples() * (percent / 100.0))] * m_scale;
    }

  public:
    inline histogram& operator()(value_type sample) noexcept
    {
        m_data.push_back(sample);
        m_acc(sample);
        return *this;
    }

  public:
    inline void clear() noexcept
    {
        m_data.clear();
        m_acc.clear();
    }

  public:
    template<class CharT, class Traits = std::char_traits<CharT>>
    friend std::basic_ostream<CharT, Traits>& operator<<(
        std::basic_ostream<CharT, Traits>& os, const histogram& hist)
    {
        const value_type       min = hist.mean() - hist.m_sigma * hist.stddev();
        const value_type       max = hist.mean() + hist.m_sigma * hist.stddev();
        const value_type       width = 2 * hist.m_sigma * hist.stddev();
        const value_type       bin_width = width / hist.m_nbins;
        std::vector<size_type> bins(hist.m_nbins, 0);
        size_type              under = 0;
        size_type              over = 0;
        for (auto x : hist.m_data)
        {
            if (x * hist.m_scale < min) ++under;
            else if (x * hist.m_scale >= max)
                ++over;
            else
                ++bins[size_type((x * hist.m_scale - min) / bin_width)];
        }

        auto print_bar = [&os, n = hist.num_samples()](size_type x) {
            os << std::setw(6) << std::fixed << std::setprecision(2) << std::right
               << (x * 100.0) / n << "% | ";
            const int y = ((x * 1.0) / n) * 50;
            for (int j = 0; j < y; ++j) os << "o";
        };

        auto print_row = [&os, &print_bar](value_type mu, size_type x) {
            os << std::setw(11) << std::scientific << std::setprecision(4) << std::left << mu
               << "| ";
            print_bar(x);
            os << "\n";
        };

        auto print_outlier_row = [&os, &print_bar](const char* title, size_type x) {
            os << std::setw(11) << std::left << title << "| ";
            print_bar(x);
            os << "\n";
        };

        if (0.5 * bin_width + min > 0) print_outlier_row("under", under);
        os << "-----------+\n";
        for (size_type i = 0; i < hist.m_nbins; ++i)
        {
            const value_type mu = (i + 0.5) * bin_width + min;
            if (mu < 0) continue;
            print_row(mu, bins[i]);
        }
        os << "-----------+\n";
        print_outlier_row("over", over);
        return os;
    }
};
} // namespace cartex
