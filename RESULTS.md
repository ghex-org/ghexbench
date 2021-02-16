## Results on Ault 18

|Node   |Memory |#C/T    |CPU VERSION             | #ACCEL  |ACCEL TYPE|
|:------|------:|-------:|:-----------------------|:--------|:---------|
|ault18 |512 GB |128/256 |AMD EPYC 7742 @ 2.25GHz |NO       |          |

### hwloc info

    depth 0:           1 Machine (type #0)
     depth 1:          2 Package (type #1)
      depth 2:         32 L3Cache (type #6)
       depth 3:        128 L2Cache (type #5)
        depth 4:       128 L1dCache (type #4)
         depth 5:      128 L1iCache (type #9)
          depth 6:     128 Core (type #2)
           depth 7:    256 PU (type #3)
    Special depth -3:  2 NUMANode (type #13)
    Special depth -4:  12 Bridge (type #14)
    Special depth -5:  8 PCIDev (type #15)
    Special depth -6:  7 OSDev (type #16)

### case 1

* fields = 8
* halo = 1
* domain size L = 64^3
* nodes = 1
* tasks per node = 128
* decomposition: --socket 1 1 2 --numa 1 1 1 --l3 1 4 4 --core 4 1 1

|                    executable |  L | halo | fields | order |           mean | throughput |
|:------------------------------|---:|-----:|-------:|------:|:---------------|:-----------|
|       benchmark_ghex_comm_mpi | 64 |    1 |      1 |   XYZ | 0.000167897762 | 154.620572 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      1 |   XZY | 0.000170886602 | 151.916228 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      1 |   YXZ | 0.000169793498 | 152.894241 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      1 |   YZX | 0.000170130942 | 152.590985 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      1 |   ZXY | 0.000170902794 | 151.901835 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      1 |   ZYX | 0.000170749146 | 152.038523 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      1 |   XYZ | 0.000173698416 | 149.457022 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      1 |   XZY |  0.00017637082 | 147.192421 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      1 |   YXZ | 0.000176192056 | 147.341762 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      1 |   YZX | 0.000172793812 | 150.239454 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      1 |   ZXY | 0.000176188746 |  147.34453 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      1 |   ZYX | 0.000172211958 | 150.747069 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   XYZ |    0.000129235 | 200.877843 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   XZY |  0.00013044207 | 199.018982 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   YXZ | 0.000127895448 | 202.981798 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   YZX | 0.000125716676 | 206.499637 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   ZXY | 0.000127753444 | 203.207422 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   ZYX | 0.000125115748 |  207.49145 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   XYZ | 0.000127695586 | 203.299494 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   XZY | 0.000135781222 | 191.193212 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   YXZ | 0.000133315178 | 194.729875 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   YZX | 0.000126754824 | 204.808363 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   ZXY | 0.000133946752 | 193.811702 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   ZYX | 0.000135182372 | 192.040187 |
|            benchmark_mpi_comm | 64 |    1 |      1 |   XYZ |  0.00018765435 |  138.34184 |
|            benchmark_mpi_comm | 64 |    1 |      1 |   XZY |  0.00019064823 | 136.169363 |
|            benchmark_mpi_comm | 64 |    1 |      1 |   YXZ | 0.000188912746 | 137.420309 |
|            benchmark_mpi_comm | 64 |    1 |      1 |   YZX | 0.000185721866 | 139.781322 |
|            benchmark_mpi_comm | 64 |    1 |      1 |   ZXY |  0.00019020949 | 136.483453 |
|            benchmark_mpi_comm | 64 |    1 |      1 |   ZYX | 0.000190872806 | 136.009149 |

### case 2

* fields = 1,2,3,4,5,6,7,8
* halo = 1,2,3,4,5
* domain size L = 64^3
* nodes = 1
* tasks per node = 128
* decomposition: --socket 1 1 2 --numa 1 1 1 --l3 1 4 4 --core 4 1 1

|                    executable |  L | halo | fields | order |           mean | throughput |
|:------------------------------|---:|-----:|-------:|------:|:---------------|:-----------|
|       benchmark_ghex_comm_mpi | 64 |    1 |      1 |   XYZ | 0.000170420854 | 152.331404 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      2 |   XYZ | 0.000324388958 | 160.057532 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      3 |   XYZ | 0.000482019186 |  161.57312 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      4 |   XYZ |  0.00063752754 | 162.882049 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      5 |   XYZ | 0.000875898962 | 148.193166 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      6 |   XYZ |  0.00132967471 | 117.143454 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      7 |   XYZ |  0.00200990789 | 90.4136638 |
|       benchmark_ghex_comm_mpi | 64 |    1 |      8 |   XYZ |  0.00292018474 | 71.1200155 |
|       benchmark_ghex_comm_mpi | 64 |    2 |      1 |   XYZ | 0.000277359304 | 193.045307 |
|       benchmark_ghex_comm_mpi | 64 |    2 |      2 |   XYZ | 0.000559645973 | 191.345653 |
|       benchmark_ghex_comm_mpi | 64 |    2 |      3 |   XYZ | 0.000944364704 | 170.091846 |
|       benchmark_ghex_comm_mpi | 64 |    2 |      4 |   XYZ |   0.0017788654 | 120.397894 |
|       benchmark_ghex_comm_mpi | 64 |    2 |      5 |   XYZ |  0.00287599583 | 93.0858651 |
|       benchmark_ghex_comm_mpi | 64 |    2 |      6 |   XYZ |  0.00450056644 | 71.3815642 |
|       benchmark_ghex_comm_mpi | 64 |    2 |      7 |   XYZ |  0.00627357764 | 59.7426868 |
|       benchmark_ghex_comm_mpi | 64 |    2 |      8 |   XYZ |   0.0080256687 | 53.3716644 |
|       benchmark_ghex_comm_mpi | 64 |    3 |      1 |   XYZ |  0.00043362313 | 190.941254 |
|       benchmark_ghex_comm_mpi | 64 |    3 |      2 |   XYZ | 0.000958021186 | 172.849088 |
|       benchmark_ghex_comm_mpi | 64 |    3 |      3 |   XYZ |  0.00231887856 | 107.116274 |
|       benchmark_ghex_comm_mpi | 64 |    3 |      4 |   XYZ |  0.00455581966 | 72.6951901 |
|       benchmark_ghex_comm_mpi | 64 |    3 |      5 |   XYZ |  0.00706068744 |   58.63207 |
|       benchmark_ghex_comm_mpi | 64 |    3 |      6 |   XYZ |  0.00944486731 | 52.5978023 |
|       benchmark_ghex_comm_mpi | 64 |    3 |      7 |   XYZ |   0.0117608164 | 49.2802359 |
|       benchmark_ghex_comm_mpi | 64 |    3 |      8 |   XYZ |   0.0140697553 | 47.0777451 |
|       benchmark_ghex_comm_mpi | 64 |    4 |      1 |   XYZ | 0.000606692611 | 187.525765 |
|       benchmark_ghex_comm_mpi | 64 |    4 |      2 |   XYZ |  0.00164828994 | 138.046703 |
|       benchmark_ghex_comm_mpi | 64 |    4 |      3 |   XYZ |  0.00372195752 | 91.7021451 |
|       benchmark_ghex_comm_mpi | 64 |    4 |      4 |   XYZ |   0.0065368711 | 69.6177081 |
|       benchmark_ghex_comm_mpi | 64 |    4 |      5 |   XYZ |  0.00916330087 | 62.0794284 |
|       benchmark_ghex_comm_mpi | 64 |    4 |      6 |   XYZ |   0.0117268949 | 58.2100362 |
|       benchmark_ghex_comm_mpi | 64 |    4 |      7 |   XYZ |     0.01408982 | 56.5226152 |
|       benchmark_ghex_comm_mpi | 64 |    4 |      8 |   XYZ |   0.0165062173 | 55.1406754 |
|       benchmark_ghex_comm_mpi | 64 |    5 |      1 |   XYZ |  0.00077462351 | 189.142103 |
|       benchmark_ghex_comm_mpi | 64 |    5 |      2 |   XYZ |  0.00271516326 | 107.922733 |
|       benchmark_ghex_comm_mpi | 64 |    5 |      3 |   XYZ |  0.00635473682 | 69.1675788 |
|       benchmark_ghex_comm_mpi | 64 |    5 |      4 |   XYZ |  0.00976391304 | 60.0226239 |
|       benchmark_ghex_comm_mpi | 64 |    5 |      5 |   XYZ |   0.0129932687 |  56.380701 |
|       benchmark_ghex_comm_mpi | 64 |    5 |      6 |   XYZ |   0.0161965396 | 54.2760084 |
|       benchmark_ghex_comm_mpi | 64 |    5 |      7 |   XYZ |   0.0196409598 | 52.2172771 |
|       benchmark_ghex_comm_mpi | 64 |    5 |      8 |   XYZ |   0.0229557271 | 51.0596486 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      1 |   XYZ | 0.000175163351 | 148.207076 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      2 |   XYZ | 0.000332578532 | 156.116198 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      3 |   XYZ | 0.000492909752 | 158.003252 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      4 |   XYZ | 0.000672037427 |  154.51787 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      5 |   XYZ | 0.000912293234 |  142.28127 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      6 |   XYZ |  0.00139795551 | 111.421778 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      7 |   XYZ |   0.0020909654 | 86.9087244 |
|       benchmark_ghex_comm_ucx | 64 |    1 |      8 |   XYZ |  0.00295911321 | 70.1843996 |
|       benchmark_ghex_comm_ucx | 64 |    2 |      1 |   XYZ | 0.000272820238 | 196.257112 |
|       benchmark_ghex_comm_ucx | 64 |    2 |      2 |   XYZ | 0.000553037938 | 193.631967 |
|       benchmark_ghex_comm_ucx | 64 |    2 |      3 |   XYZ | 0.000913979786 | 175.746486 |
|       benchmark_ghex_comm_ucx | 64 |    2 |      4 |   XYZ |  0.00182021927 | 117.662554 |
|       benchmark_ghex_comm_ucx | 64 |    2 |      5 |   XYZ |  0.00286305732 | 93.5065317 |
|       benchmark_ghex_comm_ucx | 64 |    2 |      6 |   XYZ |   0.0043534366 | 73.7939935 |
|       benchmark_ghex_comm_ucx | 64 |    2 |      7 |   XYZ |  0.00593609896 | 63.1391738 |
|       benchmark_ghex_comm_ucx | 64 |    2 |      8 |   XYZ |  0.00755057542 | 56.7298877 |
|       benchmark_ghex_comm_ucx | 64 |    3 |      1 |   XYZ | 0.000422755983 | 195.849491 |
|       benchmark_ghex_comm_ucx | 64 |    3 |      2 |   XYZ | 0.000937215846 |  176.68618 |
|       benchmark_ghex_comm_ucx | 64 |    3 |      3 |   XYZ |  0.00228542045 | 108.684436 |
|       benchmark_ghex_comm_ucx | 64 |    3 |      4 |   XYZ |   0.0043933651 | 75.3832583 |
|       benchmark_ghex_comm_ucx | 64 |    3 |      5 |   XYZ |   0.0066663928 | 62.0999591 |
|       benchmark_ghex_comm_ucx | 64 |    3 |      6 |   XYZ |  0.00891918822 | 55.6978115 |
|       benchmark_ghex_comm_ucx | 64 |    3 |      7 |   XYZ |   0.0111737267 | 51.8695169 |
|       benchmark_ghex_comm_ucx | 64 |    3 |      8 |   XYZ |   0.0135247546 | 48.9748147 |
|       benchmark_ghex_comm_ucx | 64 |    4 |      1 |   XYZ | 0.000594975062 | 191.218932 |
|       benchmark_ghex_comm_ucx | 64 |    4 |      2 |   XYZ |  0.00164135673 | 138.629822 |
|       benchmark_ghex_comm_ucx | 64 |    4 |      3 |   XYZ |  0.00363457814 | 93.9067686 |
|       benchmark_ghex_comm_ucx | 64 |    4 |      4 |   XYZ |  0.00620056818 |  73.393594 |
|       benchmark_ghex_comm_ucx | 64 |    4 |      5 |   XYZ |  0.00863618211 | 65.8685137 |
|       benchmark_ghex_comm_ucx | 64 |    4 |      6 |   XYZ |   0.0110414848 | 61.8234765 |
|       benchmark_ghex_comm_ucx | 64 |    4 |      7 |   XYZ |   0.0134619962 | 59.1586462 |
|       benchmark_ghex_comm_ucx | 64 |    4 |      8 |   XYZ |   0.0159125578 | 57.1978422 |
|       benchmark_ghex_comm_ucx | 64 |    5 |      1 |   XYZ | 0.000732769329 | 199.945487 |
|       benchmark_ghex_comm_ucx | 64 |    5 |      2 |   XYZ |  0.00268572536 | 109.105661 |
|       benchmark_ghex_comm_ucx | 64 |    5 |      3 |   XYZ |  0.00609270269 | 72.1423286 |
|       benchmark_ghex_comm_ucx | 64 |    5 |      4 |   XYZ |  0.00925291797 | 63.3373906 |
|       benchmark_ghex_comm_ucx | 64 |    5 |      5 |   XYZ |   0.0123637245 | 59.2515305 |
|       benchmark_ghex_comm_ucx | 64 |    5 |      6 |   XYZ |   0.0155096907 | 56.6796293 |
|       benchmark_ghex_comm_ucx | 64 |    5 |      7 |   XYZ |   0.0187232196 |  54.776767 |
|       benchmark_ghex_comm_ucx | 64 |    5 |      8 |   XYZ |   0.0219469391 | 53.4065982 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   XYZ | 0.000126587661 | 205.078819 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      2 |   XYZ | 0.000205161044 | 253.073854 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      3 |   XYZ | 0.000281258903 |  276.90268 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      4 |   XYZ | 0.000373250564 | 278.209337 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      5 |   XYZ | 0.000466215987 | 278.416536 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      6 |   XYZ |  0.00057254713 | 272.052168 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      7 |   XYZ | 0.000738854185 | 245.952638 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      8 |   XYZ |  0.00106278515 | 195.414458 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      1 |   XYZ |  0.00014069901 | 380.549316 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      2 |   XYZ |  0.00023217201 | 461.234858 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      3 |   XYZ |  0.00033253984 | 483.036066 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      4 |   XYZ | 0.000454729416 |  470.98701 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      5 |   XYZ | 0.000606665676 | 441.288457 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      6 |   XYZ | 0.000928615909 | 345.953013 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      7 |   XYZ |  0.00162031332 | 231.313524 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      8 |   XYZ |   0.0025990608 | 164.806955 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    3 |      1 |   XYZ | 0.000181195055 |  456.94704 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    3 |      2 |   XYZ | 0.000333972589 | 495.828381 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    3 |      3 |   XYZ |  0.00050738032 | 489.553146 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    3 |      4 |   XYZ | 0.000922981218 | 358.822227 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    3 |      5 |   XYZ |  0.00190103416 | 217.767113 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    3 |      6 |   XYZ |  0.00328368124 | 151.287298 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    3 |      7 |   XYZ |  0.00458490604 | 126.409528 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    3 |      8 |   XYZ |  0.00563747335 | 117.494542 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      1 |   XYZ | 0.000227824141 |  499.37858 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      2 |   XYZ | 0.000340394108 | 668.463368 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      3 |   XYZ | 0.000653786095 | 522.053758 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      4 |   XYZ |  0.00161456974 | 281.859603 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      5 |   XYZ |  0.00305920764 | 185.947653 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      6 |   XYZ |  0.00449491209 | 151.865701 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      7 |   XYZ |  0.00564299717 | 141.129518 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      8 |   XYZ |  0.00656261593 | 138.689202 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      1 |   XYZ | 0.000210439253 | 696.229044 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      2 |   XYZ | 0.000376939432 | 777.387068 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      3 |   XYZ |  0.00111620596 | 393.781951 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      4 |   XYZ |  0.00302142759 | 193.966482 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      5 |   XYZ |  0.00488740641 | 149.889233 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      6 |   XYZ |  0.00626900833 | 140.226887 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      7 |   XYZ |    0.007496597 |   136.8084 |
| benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      8 |   XYZ |  0.00867695129 | 135.083317 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   XYZ | 0.000134571201 |  192.91236 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      2 |   XYZ | 0.000209064051 |  248.34923 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      3 |   XYZ | 0.000292983176 | 265.821898 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      4 |   XYZ | 0.000388494729 | 267.292666 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      5 |   XYZ | 0.000478927489 | 271.026915 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      6 |   XYZ | 0.000584164698 | 266.641734 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      7 |   XYZ | 0.000761427851 | 238.661005 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      8 |   XYZ |  0.00106408192 | 195.176311 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      1 |   XYZ | 0.000141594054 | 378.143788 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      2 |   XYZ |  0.00022716918 | 471.392396 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      3 |   XYZ | 0.000345663045 | 464.697451 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      4 |   XYZ | 0.000460271544 | 465.315857 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      5 |   XYZ | 0.000619684409 | 432.017582 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      6 |   XYZ | 0.000947250591 | 339.147291 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      7 |   XYZ |  0.00163089964 | 229.812046 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      8 |   XYZ |  0.00259444103 | 165.100417 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    3 |      1 |   XYZ | 0.000196596497 |  421.14964 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    3 |      2 |   XYZ | 0.000333031936 | 497.228854 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    3 |      3 |   XYZ | 0.000509105787 | 487.893947 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    3 |      4 |   XYZ | 0.000937213388 | 353.373287 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    3 |      5 |   XYZ |  0.00193878289 | 213.527117 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    3 |      6 |   XYZ |  0.00331658955 | 149.786175 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    3 |      7 |   XYZ |  0.00463961018 | 124.919074 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    3 |      8 |   XYZ |  0.00565676994 | 117.093741 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      1 |   XYZ | 0.000209549309 | 542.929473 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      2 |   XYZ | 0.000337645698 | 673.904609 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      3 |   XYZ | 0.000645957338 | 528.380851 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      4 |   XYZ |  0.00160724783 | 283.143629 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      5 |   XYZ |   0.0030834862 |  184.48355 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      6 |   XYZ |  0.00449102922 | 151.997002 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      7 |   XYZ |   0.0056358268 | 141.309075 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      8 |   XYZ |  0.00657304504 | 138.469151 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      1 |   XYZ | 0.000253854288 | 577.157554 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      2 |   XYZ | 0.000380049706 | 771.025041 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      3 |   XYZ |  0.00112570019 | 390.460767 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      4 |   XYZ |  0.00301920563 |  194.10923 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      5 |   XYZ |  0.00488189488 | 150.058454 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      6 |   XYZ |  0.00624698557 | 140.721234 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      7 |   XYZ |  0.00749182091 | 136.895616 |
| benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      8 |   XYZ |  0.00865252267 | 135.464697 |
|            benchmark_mpi_comm | 64 |    1 |      1 |   XYZ | 0.000188209522 | 137.933765 |
|            benchmark_mpi_comm | 64 |    1 |      2 |   XYZ | 0.000375670731 | 138.208521 |
|            benchmark_mpi_comm | 64 |    1 |      3 |   XYZ | 0.000561865732 |  138.61202 |
|            benchmark_mpi_comm | 64 |    1 |      4 |   XYZ | 0.000752027494 | 138.082441 |
|            benchmark_mpi_comm | 64 |    1 |      5 |   XYZ | 0.000950111578 | 136.617891 |
|            benchmark_mpi_comm | 64 |    1 |      6 |   XYZ |  0.00118531192 | 131.410716 |
|            benchmark_mpi_comm | 64 |    1 |      7 |   XYZ |   0.0014992653 | 121.208125 |
|            benchmark_mpi_comm | 64 |    1 |      8 |   XYZ |  0.00194618353 | 106.713257 |
|            benchmark_mpi_comm | 64 |    2 |      1 |   XYZ | 0.000305574707 | 175.220366 |
|            benchmark_mpi_comm | 64 |    2 |      2 |   XYZ | 0.000607891822 | 176.159343 |
|            benchmark_mpi_comm | 64 |    2 |      3 |   XYZ | 0.000920075541 | 174.582117 |
|            benchmark_mpi_comm | 64 |    2 |      4 |   XYZ |    0.001243419 | 172.244149 |
|            benchmark_mpi_comm | 64 |    2 |      5 |   XYZ |  0.00169531696 | 157.914164 |
|            benchmark_mpi_comm | 64 |    2 |      6 |   XYZ |  0.00242665171 | 132.387137 |
|            benchmark_mpi_comm | 64 |    2 |      7 |   XYZ |  0.00339053841 | 110.543028 |
|            benchmark_mpi_comm | 64 |    2 |      8 |   XYZ |  0.00447538771 |   95.71088 |
|            benchmark_mpi_comm | 64 |    3 |      1 |   XYZ | 0.000442628113 | 187.056677 |
|            benchmark_mpi_comm | 64 |    3 |      2 |   XYZ | 0.000895949161 | 184.824201 |
|            benchmark_mpi_comm | 64 |    3 |      3 |   XYZ |  0.00145566656 | 170.636352 |
|            benchmark_mpi_comm | 64 |    3 |      4 |   XYZ |  0.00236232333 | 140.195109 |
|            benchmark_mpi_comm | 64 |    3 |      5 |   XYZ |  0.00364957489 | 113.433135 |
|            benchmark_mpi_comm | 64 |    3 |      6 |   XYZ |  0.00493874145 | 100.588231 |
|            benchmark_mpi_comm | 64 |    3 |      7 |   XYZ |  0.00612643474 | 94.6024617 |
|            benchmark_mpi_comm | 64 |    3 |      8 |   XYZ |  0.00718462301 | 92.1930561 |
|            benchmark_mpi_comm | 64 |    4 |      1 |   XYZ | 0.000644254742 | 176.592408 |
|            benchmark_mpi_comm | 64 |    4 |      2 |   XYZ |  0.00133244302 | 170.769773 |
|            benchmark_mpi_comm | 64 |    4 |      3 |   XYZ |   0.0021286942 | 160.338431 |
|            benchmark_mpi_comm | 64 |    4 |      4 |   XYZ |  0.00337371352 | 134.890524 |
|            benchmark_mpi_comm | 64 |    4 |      5 |   XYZ |   0.0048457985 | 117.390865 |
|            benchmark_mpi_comm | 64 |    4 |      6 |   XYZ |  0.00635446712 |  107.42411 |
|            benchmark_mpi_comm | 64 |    4 |      7 |   XYZ |    0.007695606 | 103.486778 |
|            benchmark_mpi_comm | 64 |    4 |      8 |   XYZ |  0.00875546294 | 103.953837 |
|            benchmark_mpi_comm | 64 |    5 |      1 |   XYZ | 0.000798614733 | 183.460076 |
|            benchmark_mpi_comm | 64 |    5 |      2 |   XYZ |  0.00169440316 | 172.938677 |
|            benchmark_mpi_comm | 64 |    5 |      3 |   XYZ |  0.00324404887 |  135.49172 |
|            benchmark_mpi_comm | 64 |    5 |      4 |   XYZ |  0.00524259231 | 111.787384 |
|            benchmark_mpi_comm | 64 |    5 |      5 |   XYZ |  0.00714798738 | 102.486135 |
|            benchmark_mpi_comm | 64 |    5 |      6 |   XYZ |  0.00876081058 | 100.342715 |
|            benchmark_mpi_comm | 64 |    5 |      7 |   XYZ |   0.0102292763 | 100.260997 |
|            benchmark_mpi_comm | 64 |    5 |      8 |   XYZ |   0.0117425145 | 99.8177488 |
