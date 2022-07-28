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

#include <p2p/benchmark.hpp>
//#include <p2p/debug.hpp>

int
main(int argc, char** argv)
{
    using namespace ghexbench::p2p;

    benchmark bm(argc,argv);

    //debug_wait();

    bm.run();

    return 0;
}
