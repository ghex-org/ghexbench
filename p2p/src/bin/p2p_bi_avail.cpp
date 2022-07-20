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
#include <iostream>
#include <string>

#include <oomph/communicator.hpp>
#include <oomph/context.hpp>

#include <ghexbench/mpi_environment.hpp>
#include <ghexbench/thread_pool.hpp>
#include <ghexbench/options.hpp>
#include <ghexbench/timer.hpp>

#include <p2p/device_map.hpp>

int abort(std::string const & msg, bool print = true)
{
    if (print)
        std::cerr << "aborting: " << msg << std::endl;
    return 1;
}

int main(int argc, char** argv) {

    /* clang-format off */
    auto opts = ghexbench::options()
        ("size",       "buffer size",                      "s", {1024})
        ("window",     "number of simultaneous inflights", "w", {1})
        ("threads",    "number of threads",                "t", {1})
        ("nrep",       "number of repetitions",            "r", {100})
        ("limit",      "execution time limit in seconds",  "l", 1)
        ("devicemap",  "rank to device map (per node) in the form i1:i2:...:iN (N=number of ranks per node)",  "d", 1)
        ; 
    /* clang-format on */

    auto const options = opts.parse(argc, argv);
    auto const threads = options.get<std::size_t>("threads");
    if (threads < 1) 
        return abort("number of threads must be at least 1");

    ghexbench::mpi_environment mpi_env(threads > 1, argc, argv);
    oomph::context ctx(MPI_COMM_WORLD, threads > 1);
    
#ifdef P2P_ENABLE_DEVICE
    auto const local_size = ctx.local_size();
    ghexbench::p2p::device_map dmap(options.get_or<std::string>("devicemap",""));
    if (dmap.size() < local_size)
        return abort("device map does not match number of processes per node", mpi_env.rank==0);
    int const device_id = dmap[ctx.local_rank()];
#endif

    auto const size   = options.get<std::size_t>("size");
    auto const window = options.get<std::size_t>("window");
    auto const nrep   = options.get<std::size_t>("nrep");
    auto const limit  = options.get_or<std::size_t>("limit", 3600);

    if (mpi_env.rank == 0)
    {
        std::cout << "running benchmark:"
            << "\nsize     " << size
            << "\nwindow   " << window
            << "\nthreads  " << threads
            << "\nnrep     " << nrep
            << "\nlimit    " << limit
#ifdef P2P_ENABLE_DEVICE
            << "\nmemory   " << "device"
#else
            << "\nmemory   " << "host"
#endif
            << std::endl;
    }

    ghexbench::thread_pool tp(threads);

    auto thread_barrier = tp.make_barrier();

    auto test_fun = [&thread_barrier](int thread_id) {
        thread_barrier();
        if (thread_id == 0)
        MPI_Barrier(MPI_COMM_WORLD);
        ghexbench::timer wall_clock;
        thread_barrier();

    };

    for (std::size_t i=0; i<threads; ++i)
        tp.schedule(i, test_fun);

    tp.join();


    return 0;
}
