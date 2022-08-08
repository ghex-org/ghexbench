# GHEX-BENCH

This repository is a collection of benchmarking tools for the
[GHEX](https://github.com/ghex-org/GHEX) and 
[OOMPH](https://github.com/ghex-org/oomph)
library. It contains

* Cartex, a halo-exchange [benchmark](https://github.com/GridTools/GHEX-BENCH/tree/main/cartex) for 3D
  Cartesian grids and configuration files for large-scale benchmarks for different clusters
* P2P, a transport level [benchmark](https://github.com/GridTools/GHEX-BENCH/tree/main/p2p) for point to
  point bandwidth measurements.

## Dependencies

* [numactl](https://github.com/numactl/numactl) (minimum version 2.0.12)
* [hwloc](https://github.com/open-mpi/hwloc) (minimum version 2.0)
* [boost](https://github.com/boostorg/boost) header only, minimum version 1.75)
* MPI implentation, for example [openmpi](https://github.com/open-mpi/ompi)

### Optional dependencies

* [Unified Communication X (UCX)](https://github.com/openucx/ucx) (minimun version 1.8.0)
* [LIBFABRIC](https://github.com/ofiwg/libfabric)



## Building

The benchmarks can be built using [CMake](https://gitlab.kitware.com/cmake/cmake) (minimum version
3.17). Once the external dependencies are installed, CMake can be used to configure the build as
follows:
```console
$ cd /PATH/TO/GHEXBENCH-SOURCE
$ mkdir build && cd build
$ cmake ..
```

The different benchmarks can be selected by setting the respective CMake option when configuring:
```console
cmake -DGHEXBENCH_BUILD_CARTEX ..
cmake -DGHEXBENCH_BUILD_P2P ..
```

## Acknowledgments

This work was financially supported by the PRACE project funded in part by the EU's Horizon 2020
Research and Innovation programme (2014-2020) under grant agreement 823767.
