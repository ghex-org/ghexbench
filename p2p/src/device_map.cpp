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
#include <sstream>
#include <p2p/device_map.hpp>

namespace ghexbench
{
namespace p2p
{
device_map::device_map(std::string const& s)
{
    std::stringstream ss(s);
    std::string       id;
    while (std::getline(ss, id, ':'))
    {
        std::istringstream value_stream(id);
        int                result;
        value_stream >> result;
        m_devices.push_back(result); 
    }
}
} // namespace p2p
} // namespace ghexbench
