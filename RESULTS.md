## Results on Ault 18

Node     Memory  #C/T     CPU VERSION              #ACCEL  ACCEL TYPE
-------  ------  ----     -----------              ------  ----------
ault18   512 GB  128/256  AMD EPYC 7742 @ 2.25GHz  NO

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
|------------------------------:|---:|-----:|-------:|------:|---------------:|-----------:|
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
