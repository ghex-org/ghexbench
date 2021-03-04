## `halfcores` 27-nodes results

Comparison of results for 64^3 per-rank grids, `halfcores` (64 ranks per compute node, `hwcart` decomposition  
`--node 3 3 3 --socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 2 1 1`.

Tables present execution time [s] of 10000 exchange calls. GHEX results for `impi` are not available (`NaN`) because of too long startup time.

### Plots

Currently results for `impi` are not available, except for the raw MPI version.

![ompi vs. impi](betzy_ompi_vs_impi.png)

### Chosen results

Chosen halo width and number of fileds: `halo (1,5) X fields (1,8)`, (`hXfY`).

Cartesian communicator numbering order: **XYZ**

```
ompi
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.5976    10.052    13.459    122.37
    benchmark_ghex_comm_ucx          1.6554      10.2    13.793    109.67
    benchmark_ghex_comm_xpmem_mpi    1.9435    9.8813    13.958    90.176
    benchmark_ghex_comm_xpmem_ucx    2.2329    10.383      14.9    87.174
    benchmark_mpi_comm               1.7139    9.2246     13.96    80.572

impi
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi             NaN       NaN       NaN       NaN
    benchmark_ghex_comm_ucx             NaN       NaN       NaN       NaN
    benchmark_ghex_comm_xpmem_mpi       NaN       NaN       NaN       NaN
    benchmark_ghex_comm_xpmem_ucx       NaN       NaN       NaN       NaN
    benchmark_mpi_comm               3.0047    9.2818    14.699    82.105
```

Cartesian communicator numbering order: **ZYX**

```
ompi
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.7439    10.025    13.591    121.64
    benchmark_ghex_comm_ucx          1.6565    10.401    13.924    109.96
    benchmark_ghex_comm_xpmem_mpi    1.9296    10.329     13.96    93.408
    benchmark_ghex_comm_xpmem_ucx     2.085    10.637    14.933     91.35
    benchmark_mpi_comm               1.7214    9.4107    13.811    81.429

impi
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi             NaN       NaN       NaN       NaN
    benchmark_ghex_comm_ucx             NaN       NaN       NaN       NaN
    benchmark_ghex_comm_xpmem_mpi       NaN       NaN       NaN       NaN
    benchmark_ghex_comm_xpmem_ucx       NaN       NaN       NaN       NaN
    benchmark_mpi_comm               1.8137    9.2487    14.732    82.063
```

### Full results

All halo widths and field numbers:  `halo (1,2,4,5) X fields (1,2,4,8)`.
Table rows are halo width. Table columns are number of fields. Table columns without a suffix
are results for the `ompi` toolchain.

Cartesian communicator numbering order: **XYZ**

```
benchmark_ghex_comm_mpi
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    1.5976    3.2381    6.7091    13.459      NaN         NaN         NaN         NaN   
    2    3.5137    6.8926    14.266     29.99      NaN         NaN         NaN         NaN   
    4    7.5734    15.505    31.842     86.66      NaN         NaN         NaN         NaN   
    5    10.052    20.493    46.479    122.37      NaN         NaN         NaN         NaN   

benchmark_ghex_comm_ucx
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    1.6554    3.4909    6.6519    13.793      NaN         NaN         NaN         NaN   
    2    3.6501    7.0191    14.425    32.337      NaN         NaN         NaN         NaN   
    4    7.8034     15.88    31.568    77.212      NaN         NaN         NaN         NaN   
    5      10.2     21.33     44.34    109.67      NaN         NaN         NaN         NaN   

benchmark_ghex_comm_xpmem_mpi
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    1.9435    3.3638    6.9371    13.958      NaN         NaN         NaN         NaN   
    2    3.3461    6.9653    14.064    28.604      NaN         NaN         NaN         NaN   
    4    7.5947    15.249     31.06    68.201      NaN         NaN         NaN         NaN   
    5    9.8813    20.222    40.574    90.176      NaN         NaN         NaN         NaN   

benchmark_ghex_comm_xpmem_ucx
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    2.2329    3.7053    7.1567      14.9      NaN         NaN         NaN         NaN   
    2    3.5212    7.3316    14.792     30.66      NaN         NaN         NaN         NaN   
    4    7.9126    16.548    32.397    68.148      NaN         NaN         NaN         NaN   
    5    10.383    22.044    44.592    87.174      NaN         NaN         NaN         NaN   

benchmark_mpi_comm
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    1.7139    3.4549    7.0464     13.96     3.0047      3.5989      7.5332      14.699 
    2    3.2473    6.5883    13.562    27.735     3.4782       6.992      14.167      28.191 
    4    7.0731    14.156    28.708    59.611     7.1708      14.374      29.114      61.365 
    5    9.2246    19.173    38.321    80.572     9.2818      18.521      37.598      82.105 
```

Cartesian communicator numbering order: **ZYX**

```
benchmark_ghex_comm_mpi
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    1.7439    3.5113     6.849    13.591      NaN         NaN         NaN         NaN   
    2    3.4449    7.0367    14.083    30.392      NaN         NaN         NaN         NaN   
    4     7.555    15.486    31.966    86.296      NaN         NaN         NaN         NaN   
    5    10.025     20.19    44.048    121.64      NaN         NaN         NaN         NaN   

benchmark_ghex_comm_ucx
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    1.6565    3.3942    6.9144    13.924      NaN         NaN         NaN         NaN   
    2    3.4683    6.9887    14.629    30.325      NaN         NaN         NaN         NaN   
    4    7.8342    18.306    31.889    77.613      NaN         NaN         NaN         NaN   
    5    10.401    21.084    43.295    109.96      NaN         NaN         NaN         NaN   

benchmark_ghex_comm_xpmem_mpi
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    1.9296    3.4924     6.812     13.96      NaN         NaN         NaN         NaN   
    2    3.2955    6.7848    14.103    28.308      NaN         NaN         NaN         NaN   
    4    7.3693     15.18    31.176    68.601      NaN         NaN         NaN         NaN   
    5    10.329    20.025    41.342    93.408      NaN         NaN         NaN         NaN   

benchmark_ghex_comm_xpmem_ucx
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1     2.085    3.7308    7.2144    14.933      NaN         NaN         NaN         NaN   
    2    3.4691    7.3449    15.756    30.823      NaN         NaN         NaN         NaN   
    4     8.047    16.543    34.235    68.931      NaN         NaN         NaN         NaN   
    5    10.637    22.898    44.369     91.35      NaN         NaN         NaN         NaN   

benchmark_mpi_comm
           1         2         4         8       1 (impi)    2 (impi)    4 (impi)    8 (impi)
         ______    ______    ______    ______    ________    ________    ________    ________

    1    1.7214     3.547    7.0231    13.811     1.8137      3.6371      7.3448      14.732 
    2    3.2622    6.5042    13.443    26.199     3.5399      7.1765      14.017      28.379 
    4    7.2012    14.229    30.519    58.745     7.1933      14.462      29.043      61.388 
    5    9.4107    18.589     37.31    81.429     9.2487      18.533      37.551      82.063 
```
