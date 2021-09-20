## `halfcores` results: staged vs. standard algorithm

The following tables present execution times of the standard algorithm, in which each rank exchanges explicit
messages with each of it's 26 neighbors, with the staged algorithm, in which the halos are exchanged in 3
one-dimentional steps, with a synchronization point between each step / dimension. The staged algorithm
exchanges only 6 messages, while the standard algorithm exchanges 26 messages.

Comparison of results for 64^3 per-rank grids, `halfcores` (64 ranks per compute node, `hwcart` decomposition  
`--node 3 3 3 --socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 2 1 1`.

Tables present execution time [s] of 10000 exchange calls. GHEX results for `impi` are not available (`NaN`) because of too long startup time.

### Plots

![compact vs. staged](betzy_compact_vs_stages.png)

### Chosen results

Chosen halo width and number of fileds: `halo (1,5) X fields (1,8)`, (`hXfY`).

Cartesian communicator numbering order: **XYZ**

```
standard
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.5976    10.052    13.459    122.37
    benchmark_ghex_comm_ucx          1.6554      10.2    13.793    109.67
    benchmark_ghex_comm_xpmem_mpi    1.9435    9.8813    13.958    90.176
    benchmark_ghex_comm_xpmem_ucx    2.2329    10.383      14.9    87.174
    benchmark_mpi_comm               1.7139    9.2246     13.96    80.572

staged
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.5867    8.6193    13.645    109.09
    benchmark_ghex_comm_ucx          1.7317    9.1967    13.846    100.74
    benchmark_ghex_comm_xpmem_mpi    1.8931     8.434    15.888    80.417
    benchmark_ghex_comm_xpmem_ucx    2.0101    8.7648    15.572    76.691
    benchmark_mpi_comm               1.7139    9.2554     14.08    80.728
```

Cartesian communicator numbering order: **ZYX**

```
standard
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.7439    10.025    13.591    121.64
    benchmark_ghex_comm_ucx          1.6565    10.401    13.924    109.96
    benchmark_ghex_comm_xpmem_mpi    1.9296    10.329     13.96    93.408
    benchmark_ghex_comm_xpmem_ucx     2.085    10.637    14.933     91.35
    benchmark_mpi_comm               1.7214    9.4107    13.811    81.429

staged
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.5703    8.6046    13.357    108.13
    benchmark_ghex_comm_ucx          1.8575    9.3907    13.894    100.57
    benchmark_ghex_comm_xpmem_mpi    1.8649    8.4979    15.501    77.462
    benchmark_ghex_comm_xpmem_ucx    1.9733     8.939    15.201    76.803
    benchmark_mpi_comm               1.7105    9.3312        14    80.154
```

### Full results

All halo widths and field numbers:  `halo (1,2,4,5) X fields (1,2,4,8)`.
Table rows are halo width. Table columns are number of fields. Table columns without a suffix
are results for the `ompi` toolchain.

Cartesian communicator numbering order: **XYZ**

```
benchmark_ghex_comm_mpi
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.5976    3.2381    6.7091    13.459      1.5867        3.1556        6.5764        13.645  
    2    3.5137    6.8926    14.266     29.99      3.1971        6.4097        13.011        28.901  
    4    7.5734    15.505    31.842     86.66      6.7937        13.361        28.256        79.868  
    5    10.052    20.493    46.479    122.37      8.6193        17.239        41.876        109.09  

benchmark_ghex_comm_ucx
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.6554    3.4909    6.6519    13.793      1.7317        3.4014        6.8605        13.846  
    2    3.6501    7.0191    14.425    32.337      3.3334        6.5344        13.006        28.775  
    4    7.8034     15.88    31.568    77.212      7.0299        14.025        28.371        76.943  
    5      10.2     21.33     44.34    109.67      9.1967        18.337        40.419        100.74  

benchmark_ghex_comm_xpmem_mpi
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.9435    3.3638    6.9371    13.958      1.8931        3.6783        7.4527        15.888  
    2    3.3461    6.9653    14.064    28.604      3.2038        6.3921        12.956         26.87  
    4    7.5947    15.249     31.06    68.201       6.566        13.335        26.947         63.74  
    5    9.8813    20.222    40.574    90.176       8.434        16.945         36.69        80.417  

benchmark_ghex_comm_xpmem_ucx
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    2.2329    3.7053    7.1567      14.9      2.0101        3.7026        7.6002        15.572  
    2    3.5212    7.3316    14.792     30.66       3.363        6.4977        12.792        26.151  
    4    7.9126    16.548    32.397    68.148      6.8682         13.43        26.816        57.235  
    5    10.383    22.044    44.592    87.174      8.7648        17.292        34.839        76.691  

benchmark_mpi_comm
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.7139    3.4549    7.0464     13.96      1.7139        3.4346        7.0597         14.08  
    2    3.2473    6.5883    13.562    27.735      3.3799         6.649        13.422        26.773  
    4    7.0731    14.156    28.708    59.611       7.134        14.318        28.863        58.701  
    5    9.2246    19.173    38.321    80.572      9.2554         18.68        37.436        80.728  
```

Cartesian communicator numbering order: **ZYX**

```
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.7439    3.5113     6.849    13.591      1.5703         3.351         6.481        13.357  
    2    3.4449    7.0367    14.083    30.392      3.1496          6.39         13.39        29.411  
    4     7.555    15.486    31.966    86.296      6.7565        13.681        28.479        77.695  
    5    10.025     20.19    44.048    121.64      8.6046        17.372        42.326        108.13  

benchmark_ghex_comm_ucx
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.6565    3.3942    6.9144    13.924      1.8575        3.3285        6.9323        13.894  
    2    3.4683    6.9887    14.629    30.325      3.4215        6.7949        13.254        28.525  
    4    7.8342    18.306    31.889    77.613      7.1655        14.488        29.307         73.03  
    5    10.401    21.084    43.295    109.96      9.3907        18.745        39.666        100.57  

benchmark_ghex_comm_xpmem_mpi
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.9296    3.4924     6.812     13.96      1.8649        3.8207        7.5007        15.501  
    2    3.2955    6.7848    14.103    28.308      3.1916        6.4123        12.939        26.853  
    4    7.3693     15.18    31.176    68.601      6.5447        13.261        27.007        62.716  
    5    10.329    20.025    41.342    93.408      8.4979        17.058        36.569        77.462  

benchmark_ghex_comm_xpmem_ucx
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1     2.085    3.7308    7.2144    14.933      1.9733        3.8474        7.3518        15.201  
    2    3.4691    7.3449    15.756    30.823      3.2693        6.4528        12.847        26.065  
    4     8.047    16.543    34.235    68.931      6.7305        13.522        26.902        57.248  
    5    10.637    22.898    44.369     91.35       8.939        17.569        34.655        76.803  

benchmark_mpi_comm
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.7214     3.547    7.0231    13.811      1.7105        3.4686        6.9062            14  
    2    3.2622    6.5042    13.443    26.199      3.2903        6.6359        13.589        26.536  
    4    7.2012    14.229    30.519    58.745      7.1416        14.276        28.611        58.745  
    5    9.4107    18.589     37.31    81.429      9.3312          18.6        37.258        80.154  
```
