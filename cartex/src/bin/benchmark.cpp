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

#include <thread>
#include <iostream>

#include <ghexbench/options.hpp>
#include <cartex/device/set_device.hpp>
#include <ghexbench/thread_pool.hpp>
#include <cartex/runtime/runtime.hpp>

int
main(int argc, char** argv)
{
    /* clang-format off */
    auto opts = ghexbench::options()
        ("domain",        "local domain size (default: 64 64 64)", "X Y Z",    3)
        ("global-domain", "global domain size",                    "X Y Z",    3)
        ("nrep",          "number of repetitions (default: 10)",   "r",        1)
        ("time",          "execution time",                        "t",        1)
        ("nfields",       "number of fields",                      "n",        {1})
        ("halo",          "halo size",                             "h",        {1})
        ("MPICart",       "MPI cartesian global grid",             "NX NY NZ", 3)
        ("order",         "cartesian order (default: XYZ)",        "IJK",      1)
        ("node",          "node grid",                             "NX NY NZ", 3)
        ("socket",        "socket grid",                           "NX NY NZ", 3)
        ("numa",          "numa-node grid",                        "NX NY NZ", 3)
        ("l3",            "l3-cache grid",                         "NX NY NZ", 3)
        ("core",          "core grid",                             "NX NY NZ", 3)
        ("hwthread",      "hardware-thread grid",                  "NX NY NZ", 3)
        ("thread",        "software-thread grid",                  "NX NY NZ", {1,1,1})
        ("print",         "print decomposition")
        ("check",         "check results")
        ("config",        "print configuration");
    /* clang-format on */
    const auto options = ghexbench::cartex::runtime::add_options(opts).parse(argc, argv);
    if (!ghexbench::cartex::runtime::check_options(options)) { std::terminate(); }

    const auto threads = options.get<std::array<int, 3>>("thread");
    const auto num_threads = threads[0] * threads[1] * threads[2];

    int required = num_threads > 1 ? MPI_THREAD_MULTIPLE : MPI_THREAD_SINGLE;
    int provided;
    int init_result = MPI_Init_thread(&argc, &argv, required, &provided);
    if (init_result == MPI_ERR_OTHER)
    {
        std::cerr << "MPI init failed\n";
        std::terminate();
    }
    if (provided < required)
    {
        std::cerr << "MPI does not support required threading level\n";
        std::terminate();
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    const auto device_id = ghexbench::cartex::setup_device(rank);

    if (options.is_set("config"))
    {
        if (rank==0) ghexbench::cartex::print_config();
        return 0;
    }

    {
        std::unique_ptr<ghexbench::cartex::decomposition> decomp_ptr;

        const bool global = options.has("global-domain");
        if (global && options.has("domain"))
            if (rank == 0) std::cerr << "Warning: ignoring argument --domain" << std::endl;
        try
        {
            if (options.has("MPICart"))
            {
                if (options.has("order"))
                    if (rank == 0) std::cerr << "Warning: ignoring argument --order" << std::endl;
                if (options.has("node"))
                    if (rank == 0) std::cerr << "Warning: ignoring argument --node" << std::endl;
                if (options.has("socket"))
                    if (rank == 0) std::cerr << "Warning: ignoring argument --socket" << std::endl;
                if (options.has("numa"))
                    if (rank == 0) std::cerr << "Warning: ignoring argument --numa" << std::endl;
                if (options.has("l3"))
                    if (rank == 0) std::cerr << "Warning: ignoring argument --l3" << std::endl;
                if (options.has("core"))
                    if (rank == 0) std::cerr << "Warning: ignoring argument --core" << std::endl;
                if (options.has("hwthread"))
                    if (rank == 0)
                        std::cerr << "Warning: ignoring argument --hwthread" << std::endl;
                const auto dims = options.get<std::array<int, 3>>("MPICart");
                int        cart_size = dims[0] * dims[1] * dims[2];
                if (cart_size != size)
                    throw std::runtime_error("cart size is not equal to world size");
                //decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(dims, threads,
                //    (global ? options.get<std::array<int, 3>>("global-domain")
                //            : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
                //    !global);
                decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(MPI_COMM_WORLD, dims, threads,
                    (global ? options.get<std::array<int, 3>>("global-domain")
                            : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
                    !global);
            }
            else
            {
                auto builder = ghexbench::hw_topo_builder(MPI_COMM_WORLD);
                builder.order(ghexbench::cartex::decomposition::parse_order(
                    options.get_or("order", std::string("XYZ"))));
                if (options.has("node")) builder.nodes(options.get<std::array<int, 3>>("node"));
                if (options.has("socket"))
                    builder.sockets(options.get<std::array<int, 3>>("socket"));
                if (options.has("numa")) builder.numas(options.get<std::array<int, 3>>("numa"));
                if (options.has("l3")) builder.l3s(options.get<std::array<int, 3>>("l3"));
                if (options.has("core")) builder.cores(options.get<std::array<int, 3>>("core"));
                if (options.has("hwthread"))
                    builder.hwthreads(options.get<std::array<int, 3>>("hwthread"));
                decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(builder, threads,
                    (global ? options.get<std::array<int, 3>>("global-domain")
                            : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
                    !global);
            }
            /* clang-format off */
            //else if (options.has("hwthread"))
            //{
            //    decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(
            //        options.get_or("order",  std::string("XYZ")),
            //        options.get_or("node",   std::array<int,3>{1,1,1}),
            //        options.get_or("socket", std::array<int,3>{1,1,1}),
            //        options.get_or("numa",   std::array<int,3>{1,1,1}),
            //        options.get_or("l3",     std::array<int,3>{1,1,1}),
            //        options.get_or("core",   std::array<int,3>{1,1,1}),
            //        options.get<std::array<int,3>>("hwthread"),
            //        threads, 
            //        (global ? options.get<std::array<int,3>>("global-domain")
            //                : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
            //        !global);
            //}
            //else if (options.has("core"))
            //    decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(
            //        options.get_or("order",  std::string("XYZ")),
            //        options.get_or("node",   std::array<int,3>{1,1,1}),
            //        options.get_or("socket", std::array<int,3>{1,1,1}),
            //        options.get_or("numa",   std::array<int,3>{1,1,1}),
            //        options.get_or("l3",     std::array<int,3>{1,1,1}),
            //        options.get<std::array<int,3>>("core"),
            //        threads,
            //        (global ? options.get<std::array<int,3>>("global-domain")
            //                : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
            //        !global);
            //else if (options.has("l3"))
            //    decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(
            //        options.get_or("order",  std::string("XYZ")),
            //        options.get_or("node",   std::array<int,3>{1,1,1}),
            //        options.get_or("socket", std::array<int,3>{1,1,1}),
            //        options.get_or("numa",   std::array<int,3>{1,1,1}),
            //        options.get<std::array<int,3>>("l3"),
            //        threads,
            //        (global ? options.get<std::array<int,3>>("global-domain")
            //                : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
            //        !global);
            //else if (options.has("numa"))
            //    decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(
            //        options.get_or("order",  std::string("XYZ")),
            //        options.get_or("node",   std::array<int,3>{1,1,1}),
            //        options.get_or("socket", std::array<int,3>{1,1,1}),
            //        options.get<std::array<int,3>>("numa"),
            //        threads,
            //        (global ? options.get<std::array<int,3>>("global-domain")
            //                : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
            //        !global);
            //else if (options.has("socket"))
            //    decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(
            //        options.get_or("order",  std::string("XYZ")),
            //        options.get_or("node",   std::array<int,3>{1,1,1}),
            //        options.get<std::array<int,3>>("socket"),
            //        threads,
            //        (global ? options.get<std::array<int,3>>("global-domain")
            //                : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
            //        !global);
            //else 
            //    decomp_ptr = std::make_unique<ghexbench::cartex::decomposition>(
            //        options.get_or("order",  std::string("XYZ")),
            //        options.get_or("node",   std::array<int,3>{1,1,1}),
            //        threads,
            //        (global ? options.get<std::array<int,3>>("global-domain")
            //                : options.get_or("domain", std::array<int, 3>{64, 64, 64})),
            //        !global);
        }
        /* clang-format on */
        catch (...)
        {
            if (rank == 0) std::cout << "could not create decomposition" << std::endl;
            MPI_Finalize();
            return 0;
        }

        if (options.is_set("print"))
        {
            GHEXBENCH_CHECK_MPI_RESULT(MPI_Barrier(decomp_ptr->mpi_comm()));
            decomp_ptr->print();
            GHEXBENCH_CHECK_MPI_RESULT(MPI_Barrier(decomp_ptr->mpi_comm()));
            decomp_ptr.release();
            GHEXBENCH_CHECK_MPI_RESULT(MPI_Barrier(MPI_COMM_WORLD));
            MPI_Finalize();
            return 0;
        }

        GHEXBENCH_CHECK_MPI_RESULT(MPI_Barrier(decomp_ptr->mpi_comm()));

        ghexbench::cartex::runtime r(options, *decomp_ptr);
        if (rank == 0)
        {
            std::cout << r.info() << std::endl;
            std::cout << "running on " << size << " ranks" << std::endl;
        }

        {
            ghexbench::thread_pool tp(num_threads);
            auto                b = tp.make_barrier();
            for (int j = 0; j < num_threads; ++j)
                tp.schedule(j,
                    [&r, device_id](int j)
                    {
                        ghexbench::cartex::set_device(device_id);
                        r.init(j);
                    });
            tp.sync();
            GHEXBENCH_CHECK_MPI_RESULT(MPI_Barrier(decomp_ptr->mpi_comm()));
            for (int j = 0; j < num_threads; ++j)
            {
                tp.schedule(j,
                    [&r, device_id, &b](int j)
                    {
                        ghexbench::cartex::set_device(device_id);
                        r.exchange(j, b);
                    });
                tp.schedule(j, [&r](int j) { r.exit(j); });
            }
            tp.join();
        }

        GHEXBENCH_CHECK_MPI_RESULT(MPI_Barrier(decomp_ptr->mpi_comm()));
    }
    MPI_Finalize();
    return 0;
}
