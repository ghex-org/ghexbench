## `mpicart` vs `hwcart` results for `halfnodes`

Comparison of results for 64^3 per-rank grids, `halfcores` (64 ranks per compute node). `hwcart` decomposition is  
`--node 3 3 3 --socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 2 1 1`. Corresponding `mpicart` decomposition is `--MPICart 12 12 12`.

Tables present execution time [s] of 10000 exchange calls. GHEX results for `impi` are not available (`NaN`) because of too long startup time.

### halo width: 1, number of fields: 1

Cartesian communicator numbering order: **XYZ**
```
                                     mpicart ompi    mpicart impi    hwcart ompi    hwcart impi
                                     ____________    ____________    ___________    ___________

    benchmark_ghex_comm_mpi             2.8893             NaN         1.5976            NaN   
    benchmark_ghex_comm_ucx             2.8815             NaN         1.6554            NaN   
    benchmark_ghex_comm_xpmem_mpi       2.9048             NaN         1.9435            NaN   
    benchmark_ghex_comm_xpmem_ucx       2.8854             NaN         2.2329            NaN   
    benchmark_mpi_comm                  2.8057          3.1499         1.7139         3.0047   
```

Cartesian communicator numbering order: **ZYX**
```
                                     mpicart ompi    mpicart impi    hwcart ompi    hwcart impi
                                     ____________    ____________    ___________    ___________

    benchmark_ghex_comm_mpi             2.9349             NaN         1.7439            NaN   
    benchmark_ghex_comm_ucx             2.8821             NaN         1.6565            NaN   
    benchmark_ghex_comm_xpmem_mpi       2.8979             NaN         1.9296            NaN   
    benchmark_ghex_comm_xpmem_ucx       2.8841             NaN          2.085            NaN   
    benchmark_mpi_comm                  2.9776          3.1544         1.7214         1.8137   
```

### halo width: 5, number of fields: 8

Cartesian communicator numbering order: **XYZ**
```
                                     mpicart ompi    mpicart impi    hwcart ompi    hwcart impi
                                     ____________    ____________    ___________    ___________

    benchmark_ghex_comm_mpi             143.87             NaN         122.37            NaN   
    benchmark_ghex_comm_ucx              147.6             NaN         109.67            NaN   
    benchmark_ghex_comm_xpmem_mpi       144.05             NaN         90.176            NaN   
    benchmark_ghex_comm_xpmem_ucx       149.97             NaN         87.174            NaN   
    benchmark_mpi_comm                  120.08          133.23         80.572         82.105   
```

Cartesian communicator numbering order: **ZYX**
```
                                     mpicart ompi    mpicart impi    hwcart ompi    hwcart impi
                                     ____________    ____________    ___________    ___________

    benchmark_ghex_comm_mpi             143.84             NaN         121.64            NaN   
    benchmark_ghex_comm_ucx              148.1             NaN         109.96            NaN   
    benchmark_ghex_comm_xpmem_mpi       143.63             NaN         93.408            NaN   
    benchmark_ghex_comm_xpmem_ucx       149.96             NaN          91.35            NaN   
    benchmark_mpi_comm                  120.44          133.44         81.429         82.063   
```

## `mpicart` vs `hwcart` results for `allnodes`

Comparison of results for 64^3 per-rank grids, `allcores` (128 ranks per compute node). `hwcart` decomposition is  
`--node 3 3 3 --socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 4 1 1`. Corresponding `mpicart` decomposition is  `--MPICart 24 12 12`.

Tables present execution time [s] of 10000 exchange calls. GHEX results for `impi` are not available (`NaN`) because of too long startup time.

### halo width: 1, number of fields: 1

Cartesian communicator numbering order: **XYZ**
```
                                     MPICart ompi    MPICart impi    HWCart ompi    HWCart impi
                                     ____________    ____________    ___________    ___________

    benchmark_ghex_comm_mpi             4.9471             NaN         2.9035            NaN   
    benchmark_ghex_comm_ucx              5.242             NaN         2.9064            NaN   
    benchmark_ghex_comm_xpmem_mpi       4.8668             NaN         3.0652            NaN   
    benchmark_ghex_comm_xpmem_ucx       5.3407             NaN         2.8186            NaN   
    benchmark_mpi_comm                  5.4453          5.9745         2.5159         2.9765   
```

Cartesian communicator numbering order: **ZYX**
```
                                     MPICart ompi    MPICart impi    HWCart ompi    HWCart impi
                                     ____________    ____________    ___________    ___________

    benchmark_ghex_comm_mpi              5.107             NaN         2.8566            NaN   
    benchmark_ghex_comm_ucx             5.3787             NaN         3.1417            NaN   
    benchmark_ghex_comm_xpmem_mpi        5.019             NaN         2.7192            NaN   
    benchmark_ghex_comm_xpmem_ucx       5.4234             NaN          2.869            NaN   
    benchmark_mpi_comm                  4.7241          4.9437         2.6788         2.9857   
```

### halo width: 5, number of fields: 8

Cartesian communicator numbering order: **XYZ**
```
                                     MPICart ompi    MPICart impi    HWCart ompi    HWCart impi
                                     ____________    ____________    ___________    ___________

    benchmark_ghex_comm_mpi             287.02            NaN          260.87            NaN   
    benchmark_ghex_comm_ucx             257.43            NaN          244.46            NaN   
    benchmark_ghex_comm_xpmem_mpi       254.23            NaN          184.54            NaN   
    benchmark_ghex_comm_xpmem_ucx        250.5            NaN          168.93            NaN   
    benchmark_mpi_comm                  197.04          206.7          148.22         142.92   
```

Cartesian communicator numbering order: **ZYX**
```
                                     MPICart ompi    MPICart impi    HWCart ompi    HWCart impi
                                     ____________    ____________    ___________    ___________

    benchmark_ghex_comm_mpi             288.06             NaN         266.14            NaN   
    benchmark_ghex_comm_ucx             257.06             NaN         248.42            NaN   
    benchmark_ghex_comm_xpmem_mpi       254.83             NaN         180.87            NaN   
    benchmark_ghex_comm_xpmem_ucx       250.31             NaN         168.96            NaN   
    benchmark_mpi_comm                  196.27          207.09         141.57         141.13   

```

