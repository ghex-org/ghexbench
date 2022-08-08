## `allcores` 27-nodes results

Comparison of results for 64^3 per-rank grids, `allcores` (128 ranks per compute node, `hwcart` decomposition  
`--node 3 3 3 --socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 4 1 1`.

Tables present execution time [s] of 10000 exchange calls. GHEX results for `impi` are not available (`NaN`) because of too long startup time.

### Chosen results

Chosen halo width and number of fileds: `halo (1,5) X fields (1,8)`, (`hXfY`).

Cartesian communicator numbering order: **XYZ**

```
ompi
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          2.9035    16.704    38.383    260.87
    benchmark_ghex_comm_ucx          2.9064    17.796    34.965    244.46
    benchmark_ghex_comm_xpmem_mpi    3.0652     16.56    27.863    184.54
    benchmark_ghex_comm_xpmem_ucx    2.8186    18.221    24.841    168.93
    benchmark_mpi_comm               2.5159    14.759     23.32    148.22

impi
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi             NaN       NaN       NaN       NaN
    benchmark_ghex_comm_ucx             NaN       NaN       NaN       NaN
    benchmark_ghex_comm_xpmem_mpi       NaN       NaN       NaN       NaN
    benchmark_ghex_comm_xpmem_ucx       NaN       NaN       NaN       NaN
    benchmark_mpi_comm               2.9765    14.932    25.746    142.92
```

Cartesian communicator numbering order: **ZYX**

```

ompi
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          2.8566    16.715    40.066    266.14
    benchmark_ghex_comm_ucx          3.1417     17.26    35.915    248.42
    benchmark_ghex_comm_xpmem_mpi    2.7192    16.712    27.864    180.87
    benchmark_ghex_comm_xpmem_ucx     2.869    17.311    24.847    168.96
    benchmark_mpi_comm               2.6788    15.079    23.638    141.57

impi
                                      h1f1      h5f1     h1f8      h5f8 
                                     ______    ______    _____    ______

    benchmark_ghex_comm_mpi             NaN       NaN      NaN       NaN
    benchmark_ghex_comm_ucx             NaN       NaN      NaN       NaN
    benchmark_ghex_comm_xpmem_mpi       NaN       NaN      NaN       NaN
    benchmark_ghex_comm_xpmem_ucx       NaN       NaN      NaN       NaN
    benchmark_mpi_comm               2.9857    14.974    25.53    141.13
```

### Full results

All halo widths and field numbers:  `halo (1,5) X fields (1,8)`.
Table rows are halo width. Table columns are number of fields. Table columns without a suffix
are results for the `ompi` toolchain.

Cartesian communicator numbering order: **XYZ**

```
benchmark_ghex_comm_mpi
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    2.9035    38.383      NaN         NaN   
    5    16.704    260.87      NaN         NaN   

benchmark_ghex_comm_ucx
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    2.9064    34.965      NaN         NaN   
    5    17.796    244.46      NaN         NaN   

benchmark_ghex_comm_xpmem_mpi
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    3.0652    27.863      NaN         NaN   
    5     16.56    184.54      NaN         NaN   

benchmark_ghex_comm_xpmem_ucx
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    2.8186    24.841      NaN         NaN   
    5    18.221    168.93      NaN         NaN   

benchmark_mpi_comm
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    2.5159     23.32     2.9765      25.746 
    5    14.759    148.22     14.932      142.92 
```

Cartesian communicator numbering order: **XYZ**

```
benchmark_ghex_comm_mpi
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    2.8566    40.066      NaN         NaN   
    5    16.715    266.14      NaN         NaN   

benchmark_ghex_comm_ucx
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    3.1417    35.915      NaN         NaN   
    5     17.26    248.42      NaN         NaN   

benchmark_ghex_comm_xpmem_mpi
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    2.7192    27.864      NaN         NaN   
    5    16.712    180.87      NaN         NaN   

benchmark_ghex_comm_xpmem_ucx
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1     2.869    24.847      NaN         NaN   
    5    17.311    168.96      NaN         NaN   

benchmark_mpi_comm
           1         8       1 (impi)    8 (impi)
         ______    ______    ________    ________

    1    2.6788    23.638     2.9857       25.53 
    5    15.079    141.57     14.974      141.13 
```
