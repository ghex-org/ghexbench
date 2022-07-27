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

#include <oomph/communicator.hpp>

namespace ghexbench
{
namespace p2p
{

struct send_callback
{
    using communicator = oomph::communicator;
    using rank_type = oomph::rank_type;
    using tag_type = oomph::tag_type;
    using message = oomph::message_buffer<char>;

    communicator& comm;
    std::size_t   nrep;
    std::size_t&  i;

    void operator()(message& m, rank_type rank, tag_type tag)
    {
        if (++i < nrep) comm.send(m, rank, tag, send_callback{*this});
    }
};

struct recv_callback
{
    using communicator = oomph::communicator;
    using rank_type = oomph::rank_type;
    using tag_type = oomph::tag_type;
    using message = oomph::message_buffer<char>;

    communicator& comm;
    std::size_t   nrep;
    std::size_t&  i;

    void operator()(message& m, rank_type rank, tag_type tag)
    {
        if (++i < nrep) comm.recv(m, rank, tag, recv_callback{*this});
    }
};

} // namespace p2p
} // namespace ghexbench
