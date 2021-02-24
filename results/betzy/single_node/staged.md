## `halfcores` results: staged vs. standard algorithm

The following tables present execution times of the standard algorithm, in which each rank exchanges explicit
messages with each of it's 26 neighbors, with the staged algorithm, in which the halos are exchanged in 3
one-dimentional stages, with a synchronization point between each step / dimension. The staged algorithm
exchanges only 6 messages, while the standard algorithm exchanges 26 messages.

Comparison of results for 64^3 per-rank grids, `halfcores` (64 ranks per compute node, `hwcart` decomposition  
`--socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 2 1 1`.

Tables present execution time [s] of 10000 exchange calls.

### Chosen results

Chosen halo width and number of fileds: `halo (1,5) X fields (1,8)`, (`hXfY`).

Cartesian communicator numbering order: **XYZ**

```
standard
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.4594     5.278     10.22    98.682
    benchmark_ghex_comm_ucx          1.6214    5.3684    10.059    97.127
    benchmark_ghex_comm_xpmem_mpi     2.419     5.087    15.367    58.717
    benchmark_ghex_comm_xpmem_ucx    2.4194    5.0158    15.294    58.881
    benchmark_mpi_comm               1.4823    6.8061    12.069    69.365

staged
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.3701    5.4031    9.8879    88.217
    benchmark_ghex_comm_ucx          1.3687    5.5023    9.8844    88.615
    benchmark_ghex_comm_xpmem_mpi    2.3533    3.5435    14.723    42.005
    benchmark_ghex_comm_xpmem_ucx    2.4004    3.5273    14.675    42.024
    benchmark_mpi_comm               1.4796    6.9864    12.103    69.494
```

Cartesian communicator numbering order: **ZYX**

```
standard
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.5331    5.2716     10.13    98.578
    benchmark_ghex_comm_ucx           1.517    5.3212    10.001    96.462
    benchmark_ghex_comm_xpmem_mpi    2.4552    5.1894    15.133    59.959
    benchmark_ghex_comm_xpmem_ucx    2.3787    5.1484    15.193     60.61
    benchmark_mpi_comm               1.4763    6.1309    11.957    63.083

staged
                                      h1f1      h5f1      h1f8      h5f8 
                                     ______    ______    ______    ______

    benchmark_ghex_comm_mpi          1.3819    5.3945    9.9054    87.855
    benchmark_ghex_comm_ucx          1.3638    5.4346    9.8286    88.596
    benchmark_ghex_comm_xpmem_mpi     2.126    3.6372    14.524    43.061
    benchmark_ghex_comm_xpmem_ucx    2.1207    3.6533    14.441    42.998
    benchmark_mpi_comm               1.4859     6.172    11.958    63.567
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

    1    1.4594    2.7123     5.042     10.22      1.3701        2.5823        5.0766        9.8879  
    2    2.2972    4.2544    8.2552    21.885      2.1562        4.2177        7.9576        22.001  
    4    4.1459    8.2193    18.995    66.354      4.1534        8.0364        18.183        63.607  
    5     5.278    10.934    31.569    98.682      5.4031        10.169        32.014        88.217  

benchmark_ghex_comm_ucx
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.6214    2.8277    5.2091    10.059      1.3687        2.5939         5.153        9.8844  
    2    2.2671    4.2491    8.2915    21.015      2.1936        4.2552        8.3428        21.903  
    4    4.2167    8.2483     18.09    67.615      4.3066        8.2874         18.26        63.055  
    5    5.3684     10.95    31.561    97.127      5.5023        10.513        31.927        88.615  

benchmark_ghex_comm_xpmem_mpi
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1     2.419    4.3668    7.6539    15.367      2.3533        4.1838        7.5864        14.723  
    2    2.5861    4.8721    8.5641    18.361      2.2086        4.3302        7.9798        16.112  
    4    3.8194    7.2749    13.528     41.88      2.9128        5.4833        9.6176        28.124  
    5     5.087     9.132    19.176    58.717      3.5435        6.4516        12.656        42.005  

benchmark_ghex_comm_xpmem_ucx
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    2.4194    4.1905    7.6265    15.294      2.4004        4.1859        7.4859        14.675  
    2    2.5641    4.5776    8.5026    18.475      2.2264        4.2801        7.9471        16.258  
    4    3.8386    7.1215    13.578    41.956      2.9157        5.2256        9.6373        28.026  
    5    5.0158    9.0417    19.061    58.881      3.5273        6.3404        12.553        42.024  

benchmark_mpi_comm
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.4823      2.97    5.9675    12.069      1.4796        3.1371        5.9937        12.103  
    2    2.4599    4.9366     10.09    20.281       2.518        5.2191        10.187        20.348  
    4    5.2103    10.463    20.979    47.002      5.2499        10.501        21.028        47.098  
    5    6.8061    13.704     27.57    69.365      6.9864         13.86        27.806        69.494  
```

Cartesian communicator numbering order: **ZYX**
```benchmark_ghex_comm_mpi
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.5331    2.6973    5.1448     10.13      1.3819        2.5936        4.9329        9.9054  
    2    2.2582    4.2051    8.2989    21.969       2.191        4.2357        8.0174        22.162  
    4    4.2074    8.1639    18.697    66.028      4.1913         8.082        18.526         62.97  
    5    5.2716     10.62    31.446    98.578      5.3945        10.527        30.952        87.855  

benchmark_ghex_comm_ucx
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1     1.517     2.722     5.264    10.001      1.3638        2.6917        5.1764        9.8286  
    2    2.2166    4.1917    8.2557    21.576       2.196        4.2007        8.1768        21.551  
    4    4.1785    8.2998    17.986    66.433      4.2865        8.2689        17.901        63.652  
    5    5.3212    10.905    31.125    96.462      5.4346        10.296        30.707        88.596  

benchmark_ghex_comm_xpmem_mpi
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    2.4552    4.1226    7.3871    15.133       2.126        3.9161        7.3657        14.524  
    2    2.8885     4.778    8.3851    18.843      2.1714        4.3996        8.0044        16.563  
    4    3.8942    7.2467    13.854    43.141      2.9275        5.4528        9.7299        28.515  
    5    5.1894    9.2097    19.204    59.959      3.6372        6.5669        12.786        43.061  

benchmark_ghex_comm_xpmem_ucx
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    2.3787    3.9916     7.347    15.193      2.1207          3.89        7.2665        14.441  
    2    2.5109     4.463     8.386    18.501      2.1619        4.3194        7.8411        16.503  
    4    3.8917    7.1059    14.146    43.243      2.9565        5.2619        9.6801        28.498  
    5    5.1484    9.1448    19.411     60.61      3.6533        6.3986        12.705        42.998  

benchmark_mpi_comm
           1         2         4         8       1 (staged)    2 (staged)    4 (staged)    8 (staged)
         ______    ______    ______    ______    __________    __________    __________    __________

    1    1.4763    2.9468    5.9185    11.957      1.4859         2.951        5.9217        11.958  
    2    2.3846    4.8089    9.6341    19.219      2.3855        4.8154        9.6139        19.503  
    4    4.7087    9.4188    18.867    42.327       4.695        9.4207        18.984        42.405  
    5    6.1309    12.291    25.098    63.083       6.172         12.38        24.941        63.567   
```