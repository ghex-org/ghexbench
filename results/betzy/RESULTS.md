# Results on Betzy (Norway)

|Node   |Memory |#C/T    |CPU VERSION             | #ACCEL  |ACCEL TYPE|
|:------|------:|-------:|:-----------------------|:--------|:---------|
|betzy  |256 GB |128/256 |AMD EPYC 7742 @ 2.25GHz |NO       |          |

## hwloc info

```
depth 0:            1 Machine (type #0)
 depth 1:           2 Package (type #1)
  depth 2:          8 Group0 (type #12)
   depth 3:         32 L3Cache (type #6)
    depth 4:        128 L2Cache (type #5)
     depth 5:       128 L1dCache (type #4)
      depth 6:      128 L1iCache (type #9)
       depth 7:     128 Core (type #2)
        depth 8:    256 PU (type #3)
Special depth -3:   8 NUMANode (type #13)
Special depth -4:   8 Bridge (type #14)
Special depth -5:   5 PCIDev (type #15)
Special depth -6:   6 OSDev (type #16)
```

## Compiler info
Benchmarks were compiled using the following compilers and MPI implementations

* OpenMPI (4.0.5) + GCC (9.3.0)  
  `CXXFLAGS="-Wall -Wextra -Wpedantic -Wno-unknown-pragmas -march=znver2 -funroll-all-loops"`
* Intel MPI + Intel C++ compiler (19.1.1.217)  
  `CXXFLAGS="-march=core-avx2 -fma -ftz -fomit-frame-pointer"`

Below, those configurations are refered to as `ompi` and `impi`, respectively. If for some results there is no explicit tag,
`ompi` was the configuration used.

## JUBE setup
The JUBE XML script is configured through the following tags

* `ompi`, `impi`: toolchain used
* `halfcores`, `allcores`: number of cores per compute node used
* `single_node`, `27_nodes` : number of nodes used
* `mpicart`, `hwcart`: Cartesian communicator implementation

The `allcores` configuration started 128 ranks per compute node, 1 rank per each physical core.
Effectively, 4 cores per L3 cache domain, 16 cores per NUMA node, and 64 cores per socket were started.
The `halfcores` configuration started 64 ranks per compute node. Every second core was used, 
hence 2 cores per L3 cache domain, 8 ranks per NUMA node, and 32 ranks per socket were started.

Within each compute node the ranks were organized into a 3D grid. The actual decomposition is mentioned
for each benchmark. Additionally, for `27_nodes` configurations the nodes were arranged into a 3D grid
with dimension `[3 3 3]`.

When using the `mpicart` configuration the rank neighborhood was determined by the `MPI_Cart_*` communicator.
In that case the rank to compute node assignment was done automatically by MPI. Running the benchmark
only required specifying the global rank grid dimensions, e.g., ``--MPICart 12 12 12``. 
With the `hwcart` configuration the ranks were remapped to a memory domain specific rank grid.
For example, `--socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 2 1 1`.

## Benchmark setup

The computational grid was periodic in all dimensions.

Single precision, 32-bit floats were used.


## Single-node tests

Results for a 64^3 domain with different halos (1,2,4,5) and different number of fields
(1 2 4 8). The in-node rank grid was `[4 4 4]` for `halfcodes` tests, and `[4 8 4]` for
`allcores` tests.

[`jube run bench.xml --tag single_node ompi halfcores hwcart`](jube_single_node_0.md)

[`jube run bench.xml --tag single_node ompi allcores hwcart`](jube_single_node_1.md)

[`jube run bench.xml --tag single_node impi halfcores hwcart`](jube_single_node_2.md)

[`jube run bench.xml --tag single_node impi allcores hwcart`](jube_single_node_3.md)

## 27-node tests

```
# short, only halo 1 fields 1
jube run bench.xml --tag 27_nodes ompi halfcores mpicart
jube run bench.xml --tag 27_nodes impi halfcores mpicart

# full runs: halo 1 2 4 5, fields 1 2 4 8
jube run bench.xml --tag 27_nodes ompi halfcores hwcart
jube run bench.xml --tag 27_nodes impi halfcores hwcart

# short: halo 1 5, fields 1 8
jube run bench.xml --tag 27_nodes ompi allcores hwcart
jube run bench.xml --tag 27_nodes impi allcores hwcart

# short, only halo 5 fields 8
jube run bench.xml --tag 27_nodes impi halfcores mpicart
jube run bench.xml --tag 27_nodes ompi halfcores mpicart

# short: halo 1 5, fields 1 8
jube run bench.xml --tag 27_nodes impi allcores mpicart
jube run bench.xml --tag 27_nodes ompi allcores mpicart
```