## JUBE results

Run command:

`jube run bench.xml --tag single_node impi allcores hwcart`

Rank decomposition:

`--socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 2 1 1`

|                   executable |  L | halo | fields | order | ddindex |           mean | throughput |                 rundir|
|------------------------------|----|------|--------|-------|---------|----------------|------------|-----------------------|
|      benchmark_ghex_comm_mpi | 64 |    1 |      1 |   XYZ |       1 | 0.000234773366 | 110.576632 | 000003/000032_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      1 |   ZYX |       1 | 0.000244517937 | 106.169913 | 000003/000033_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      2 |   XYZ |       1 | 0.000558791832 | 92.9163474 | 000003/000034_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      2 |   ZYX |       1 | 0.000553199624 | 93.8556242 | 000003/000035_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      4 |   XYZ |       1 |  0.00116502102 |  89.132977 | 000003/000036_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      4 |   ZYX |       1 |   0.0011853835 |  87.601854 | 000003/000037_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      8 |   XYZ |       1 |  0.00368625376 | 56.3400128 | 000003/000038_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      8 |   ZYX |       1 |  0.00370938871 | 55.9886279 | 000003/000039_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      1 |   XYZ |       1 | 0.000585758372 | 91.4078476 | 000003/000040_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      1 |   ZYX |       1 | 0.000581568485 | 92.0663918 | 000003/000041_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      2 |   XYZ |       1 |  0.00116292435 | 92.0832247 | 000003/000042_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      2 |   ZYX |       1 |  0.00119203079 | 89.8347805 | 000003/000043_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      4 |   XYZ |       1 |  0.00266879595 | 80.2502895 | 000003/000044_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      4 |   ZYX |       1 |  0.00266386276 | 80.3989045 | 000003/000045_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      8 |   XYZ |       1 |  0.00938321049 |  45.649972 | 000003/000046_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      8 |   ZYX |       1 |  0.00937831751 | 45.6737891 | 000003/000047_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      1 |   XYZ |       1 |  0.00118917067 | 95.6721339 | 000003/000048_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      1 |   ZYX |       1 |  0.00119724899 | 95.0265961 | 000003/000049_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      2 |   XYZ |       1 |  0.00295265268 | 77.0632433 | 000003/000050_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      2 |   ZYX |       1 |  0.00294680748 | 77.2161037 | 000003/000051_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      4 |   XYZ |       1 |  0.00795040988 | 57.2400657 | 000003/000052_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      4 |   ZYX |       1 |  0.00798390577 | 56.9999192 | 000003/000053_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      8 |   XYZ |       1 |   0.0194946708 | 46.6878348 | 000003/000054_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      8 |   ZYX |       1 |   0.0193563546 | 47.0214555 | 000003/000055_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      1 |   XYZ |       1 |  0.00190330915 | 76.9785194 | 000003/000056_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      1 |   ZYX |       1 |   0.0019099506 | 76.7108426 | 000003/000057_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      2 |   XYZ |       1 |  0.00390087152 | 75.1185571 | 000003/000058_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      2 |   ZYX |       1 |  0.00386971783 | 75.7233092 | 000003/000059_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      4 |   XYZ |       1 |   0.0114167112 | 51.3331438 | 000003/000060_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      4 |   ZYX |       1 |   0.0114693151 |  51.097705 | 000003/000061_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      8 |   XYZ |       1 |   0.0260687834 | 44.9622578 | 000003/000062_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      8 |   ZYX |       1 |   0.0262332151 | 44.6804311 | 000003/000063_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      1 |   XYZ |       1 | 0.000289512095 | 89.6696492 | 000003/000096_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      1 |   ZYX |       1 |  0.00028499457 | 91.0910268 | 000003/000097_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      2 |   XYZ |       1 | 0.000536700038 | 96.7409956 | 000003/000098_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      2 |   ZYX |       1 | 0.000546018352 |   95.09002 | 000003/000099_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      4 |   XYZ |       1 |   0.0010503007 | 98.8686305 | 000003/000100_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      4 |   ZYX |       1 |  0.00105483266 | 98.4438533 | 000003/000101_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      8 |   XYZ |       1 |  0.00293832656 | 70.6809062 | 000003/000102_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      8 |   ZYX |       1 |  0.00296112961 |  70.136607 | 000003/000103_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      1 |   XYZ |       1 | 0.000479305762 | 111.709302 | 000003/000104_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      1 |   ZYX |       1 | 0.000463568345 | 115.501657 | 000003/000105_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      2 |   XYZ |       1 |   0.0010416864 | 102.800444 | 000003/000106_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      2 |   ZYX |       1 | 0.000989653376 | 108.205384 | 000003/000107_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      4 |   XYZ |       1 |  0.00226830391 | 94.4192916 | 000003/000108_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      4 |   ZYX |       1 |  0.00210594412 | 101.698638 | 000003/000109_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      8 |   XYZ |       1 |  0.00745255177 | 57.4760578 | 000003/000110_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      8 |   ZYX |       1 |  0.00741713937 | 57.7504716 | 000003/000111_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      1 |   XYZ |       1 |   0.0010808664 | 105.258611 | 000003/000112_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      1 |   ZYX |       1 |  0.00103561448 | 109.857962 | 000003/000113_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      2 |   XYZ |       1 |  0.00225832651 | 100.756463 | 000003/000114_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      2 |   ZYX |       1 |  0.00210468382 | 108.111722 | 000003/000115_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      4 |   XYZ |       1 |  0.00613336713 | 74.1977407 | 000003/000116_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      4 |   ZYX |       1 |   0.0059667127 | 76.2701352 | 000003/000117_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      8 |   XYZ |       1 |   0.0158153726 | 57.5493219 | 000003/000118_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      8 |   ZYX |       1 |   0.0158326771 | 57.4864226 | 000003/000119_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      1 |   XYZ |       1 |  0.00138486505 | 105.796532 | 000003/000120_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      1 |   ZYX |       1 |   0.0013392731 |  109.39809 | 000003/000121_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      2 |   XYZ |       1 |  0.00309946904 | 94.5413025 | 000003/000122_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      2 |   ZYX |       1 |  0.00287152768 | 102.045974 | 000003/000123_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      4 |   XYZ |       1 |  0.00903894385 | 64.8367431 | 000003/000124_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      4 |   ZYX |       1 |  0.00906513729 | 64.6493993 | 000003/000125_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      8 |   XYZ |       1 |   0.0221794972 | 52.8466156 | 000003/000126_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      8 |   ZYX |       1 |   0.0218502272 | 53.6429827 | 000003/000127_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   XYZ |       1 | 0.000271894199 | 95.4799628 | 000003/000064_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   ZYX |       1 | 0.000271796187 | 95.5143937 | 000003/000065_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      2 |   XYZ |       1 | 0.000493147146 | 105.284795 | 000003/000066_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      2 |   ZYX |       1 |  0.00048153327 | 107.824109 | 000003/000067_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      4 |   XYZ |       1 | 0.000921461305 | 112.692515 | 000003/000068_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      4 |   ZYX |       1 | 0.000917799345 | 113.142151 | 000003/000069_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      8 |   XYZ |       1 |  0.00233521279 | 88.9356142 | 000003/000070_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      8 |   ZYX |       1 |  0.00233387691 |   88.98652 | 000003/000071_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      1 |   XYZ |       1 | 0.000317976498 | 168.386382 | 000003/000072_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      1 |   ZYX |       1 | 0.000322874241 | 165.832096 | 000003/000073_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      2 |   XYZ |       1 | 0.000561073845 | 190.858699 | 000003/000074_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      2 |   ZYX |       1 | 0.000555803369 | 192.668541 | 000003/000075_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      4 |   XYZ |       1 |  0.00108217716 | 197.908121 | 000003/000076_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      4 |   ZYX |       1 |  0.00108280937 |  197.79257 | 000003/000077_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      8 |   XYZ |       1 |  0.00368383331 | 116.276514 | 000003/000078_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      8 |   ZYX |       1 |  0.00377668174 | 113.417896 | 000003/000079_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      1 |   XYZ |       1 | 0.000406971714 | 279.553817 | 000003/000080_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      1 |   ZYX |       1 | 0.000416131774 | 273.400166 | 000003/000081_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      2 |   XYZ |       1 | 0.000743479778 | 306.048663 | 000003/000082_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      2 |   ZYX |       1 | 0.000744640132 | 305.571755 | 000003/000083_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      4 |   XYZ |       1 |  0.00236607047 | 192.336615 | 000003/000084_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      4 |   ZYX |       1 |   0.0023738658 | 191.705017 | 000003/000085_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      8 |   XYZ |       1 |  0.00791779676 | 114.951671 | 000003/000086_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      8 |   ZYX |       1 |  0.00811813511 | 112.114908 | 000003/000087_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      1 |   XYZ |       1 | 0.000482699586 | 303.530238 | 000003/000088_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      1 |   ZYX |       1 | 0.000481846631 | 304.067541 | 000003/000089_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      2 |   XYZ |       1 | 0.000929448325 | 315.270717 | 000003/000090_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      2 |   ZYX |       1 | 0.000910656633 |  321.77643 | 000003/000091_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      4 |   XYZ |       1 |  0.00339132412 | 172.810283 | 000003/000092_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      4 |   ZYX |       1 |  0.00349685922 | 167.594874 | 000003/000093_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      8 |   XYZ |       1 |  0.00989100989 | 118.502698 | 000003/000094_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      8 |   ZYX |       1 |  0.00990942907 | 118.282431 | 000003/000095_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   XYZ |       1 | 0.000276064824 | 94.0375077 | 000003/000128_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   ZYX |       1 | 0.000253596684 | 102.369036 | 000003/000129_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      2 |   XYZ |       1 | 0.000486550553 | 106.712233 | 000003/000130_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      2 |   ZYX |       1 | 0.000475414042 | 109.211953 | 000003/000131_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      4 |   XYZ |       1 | 0.000913803524 | 113.636892 | 000003/000132_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      4 |   ZYX |       1 | 0.000921345247 |  112.70671 | 000003/000133_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      8 |   XYZ |       1 |  0.00234741913 | 88.4731581 | 000003/000134_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      8 |   ZYX |       1 |  0.00236405863 | 87.8504372 | 000003/000135_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      1 |   XYZ |       1 |  0.00029915702 | 178.979293 | 000003/000136_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      1 |   ZYX |       1 |  0.00031636127 | 169.246103 | 000003/000137_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      2 |   XYZ |       1 | 0.000551148446 | 194.295792 | 000003/000138_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      2 |   ZYX |       1 | 0.000545375894 | 196.352324 | 000003/000139_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      4 |   XYZ |       1 |  0.00107870274 | 198.545567 | 000003/000140_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      4 |   ZYX |       1 |   0.0010785502 | 198.573648 | 000003/000141_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      8 |   XYZ |       1 |  0.00376217496 |  113.85523 | 000003/000142_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      8 |   ZYX |       1 |  0.00381407767 | 112.305866 | 000003/000143_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      1 |   XYZ |       1 | 0.000396188619 | 287.162454 | 000003/000144_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      1 |   ZYX |       1 | 0.000405955425 | 280.253666 | 000003/000145_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      2 |   XYZ |       1 | 0.000748345046 | 304.058927 | 000003/000146_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      2 |   ZYX |       1 | 0.000755678632 | 301.108146 | 000003/000147_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      4 |   XYZ |       1 |   0.0023866766 | 190.676015 | 000003/000148_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      4 |   ZYX |       1 |  0.00242148905 | 187.934769 | 000003/000149_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      8 |   XYZ |       1 |   0.0079160468 | 114.977083 | 000003/000150_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      8 |   ZYX |       1 |  0.00815234529 | 111.644433 | 000003/000151_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      1 |   XYZ |       1 | 0.000470625438 | 311.317469 | 000003/000152_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      1 |   ZYX |       1 |  0.00048523661 | 301.943252 | 000003/000153_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      2 |   XYZ |       1 | 0.000892432444 | 328.347363 | 000003/000154_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      2 |   ZYX |       1 | 0.000916923881 | 319.577062 | 000003/000155_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      4 |   XYZ |       1 |  0.00332491987 | 176.261595 | 000003/000156_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      4 |   ZYX |       1 |  0.00344855065 | 169.942605 | 000003/000157_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      8 |   XYZ |       1 |  0.00988500086 | 118.574735 | 000003/000158_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      8 |   ZYX |       1 |  0.00988233479 | 118.606725 | 000003/000159_run/work|
|           benchmark_mpi_comm | 64 |    1 |      1 |   XYZ |       1 | 0.000216842877 | 119.720087 | 000003/000000_run/work|
|           benchmark_mpi_comm | 64 |    1 |      1 |   ZYX |       1 | 0.000207523942 | 125.096159 | 000003/000001_run/work|
|           benchmark_mpi_comm | 64 |    1 |      2 |   XYZ |       1 | 0.000424927247 | 122.187731 | 000003/000002_run/work|
|           benchmark_mpi_comm | 64 |    1 |      2 |   ZYX |       1 | 0.000426219291 |  121.81733 | 000003/000003_run/work|
|           benchmark_mpi_comm | 64 |    1 |      4 |   XYZ |       1 | 0.000874566605 | 118.735144 | 000003/000004_run/work|
|           benchmark_mpi_comm | 64 |    1 |      4 |   ZYX |       1 | 0.000853753951 | 121.629647 | 000003/000005_run/work|
|           benchmark_mpi_comm | 64 |    1 |      8 |   XYZ |       1 |  0.00197124563 | 105.356522 | 000003/000006_run/work|
|           benchmark_mpi_comm | 64 |    1 |      8 |   ZYX |       1 |  0.00198152474 | 104.809988 | 000003/000007_run/work|
|           benchmark_mpi_comm | 64 |    2 |      1 |   XYZ |       1 | 0.000318760895 | 167.972022 | 000003/000008_run/work|
|           benchmark_mpi_comm | 64 |    2 |      1 |   ZYX |       1 |  0.00030632953 | 174.788608 | 000003/000009_run/work|
|           benchmark_mpi_comm | 64 |    2 |      2 |   XYZ |       1 | 0.000637201864 | 168.056357 | 000003/000010_run/work|
|           benchmark_mpi_comm | 64 |    2 |      2 |   ZYX |       1 |   0.0006115586 | 175.103128 | 000003/000011_run/work|
|           benchmark_mpi_comm | 64 |    2 |      4 |   XYZ |       1 |  0.00130601883 | 163.988177 | 000003/000012_run/work|
|           benchmark_mpi_comm | 64 |    2 |      4 |   ZYX |       1 |   0.0012428474 | 172.323367 | 000003/000013_run/work|
|           benchmark_mpi_comm | 64 |    2 |      8 |   XYZ |       1 |     0.00374229 | 114.460209 | 000003/000014_run/work|
|           benchmark_mpi_comm | 64 |    2 |      8 |   ZYX |       1 |  0.00371321734 | 115.356376 | 000003/000015_run/work|
|           benchmark_mpi_comm | 64 |    4 |      1 |   XYZ |       1 | 0.000564605107 | 201.504546 | 000003/000016_run/work|
|           benchmark_mpi_comm | 64 |    4 |      1 |   ZYX |       1 |  0.00050390539 | 225.777494 | 000003/000017_run/work|
|           benchmark_mpi_comm | 64 |    4 |      2 |   XYZ |       1 |  0.00113384893 | 200.680167 | 000003/000018_run/work|
|           benchmark_mpi_comm | 64 |    4 |      2 |   ZYX |       1 |  0.00101530817 | 224.110273 | 000003/000019_run/work|
|           benchmark_mpi_comm | 64 |    4 |      4 |   XYZ |       1 |  0.00254062803 | 179.121846 | 000003/000020_run/work|
|           benchmark_mpi_comm | 64 |    4 |      4 |   ZYX |       1 |  0.00245271219 |  185.54235 | 000003/000021_run/work|
|           benchmark_mpi_comm | 64 |    4 |      8 |   XYZ |       1 |  0.00727168113 |  125.16555 | 000003/000022_run/work|
|           benchmark_mpi_comm | 64 |    4 |      8 |   ZYX |       1 |  0.00727169698 | 125.165277 | 000003/000023_run/work|
|           benchmark_mpi_comm | 64 |    5 |      1 |   XYZ |       1 | 0.000724917129 | 202.111268 | 000003/000024_run/work|
|           benchmark_mpi_comm | 64 |    5 |      1 |   ZYX |       1 | 0.000681306582 | 215.048443 | 000003/000025_run/work|
|           benchmark_mpi_comm | 64 |    5 |      2 |   XYZ |       1 |  0.00151236009 | 193.755338 | 000003/000026_run/work|
|           benchmark_mpi_comm | 64 |    5 |      2 |   ZYX |       1 |  0.00138114782 | 212.162547 | 000003/000027_run/work|
|           benchmark_mpi_comm | 64 |    5 |      4 |   XYZ |       1 |  0.00434363097 |  134.92299 | 000003/000028_run/work|
|           benchmark_mpi_comm | 64 |    5 |      4 |   ZYX |       1 |  0.00426475859 | 137.418254 | 000003/000029_run/work|
|           benchmark_mpi_comm | 64 |    5 |      8 |   XYZ |       1 |   0.0103987845 | 112.716189 | 000003/000030_run/work|
|           benchmark_mpi_comm | 64 |    5 |      8 |   ZYX |       1 |   0.0102275935 |  114.60285 | 000003/000031_run/work|

