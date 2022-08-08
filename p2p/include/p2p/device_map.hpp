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
#include <string>
#include <vector>

namespace ghexbench
{
namespace p2p
{
class device_map
{
  private:
    std::vector<int> m_devices;

  public:
    device_map(std::string const& s);

    device_map(device_map const&) = default;

  public:
    std::size_t size() const noexcept { return m_devices.size(); }

    int operator[](unsigned i) const noexcept { return m_devices[i]; }
};

} // namespace p2p
} // namespace ghexbench
