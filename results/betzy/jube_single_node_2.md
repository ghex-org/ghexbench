## JUBE results

Run command:

`jube run bench.xml --tag single_node impi halfcores hwcart`

Rank decomposition:

`--socket 1 1 2 --numa 1 2 2 --l3 2 2 1 --core 2 1 1`

64^3 grid points per rank

|                   executable |  L | halo | fields | order | ddindex |           mean | throughput |                 rundir|
|------------------------------|----|------|--------|-------|---------|----------------|------------|-----------------------|
|      benchmark_ghex_comm_mpi | 64 |    1 |      1 |   XYZ |       1 | 0.000194492777 | 66.7388486 | 000002/000001_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      1 |   ZYX |       1 | 0.000195611192 | 66.3572665 | 000002/000006_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      2 |   XYZ |       1 | 0.000395033371 | 65.7171012 | 000002/000011_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      2 |   ZYX |       1 | 0.000415430381 | 62.4904898 | 000002/000016_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      4 |   XYZ |       1 | 0.000791696936 | 65.5817822 | 000002/000021_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      4 |   ZYX |       1 | 0.000788519809 | 65.8460262 | 000002/000026_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      8 |   XYZ |       1 |  0.00162518769 | 63.8952613 | 000002/000031_run/work|
|      benchmark_ghex_comm_mpi | 64 |    1 |      8 |   ZYX |       1 |  0.00160718163 | 64.6111119 | 000002/000036_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      1 |   XYZ |       1 | 0.000306277582 | 87.4091268 | 000002/000041_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      1 |   ZYX |       1 | 0.000314852642 | 85.0285258 | 000002/000046_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      2 |   XYZ |       1 |  0.00059739706 | 89.6270095 | 000002/000051_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      2 |   ZYX |       1 | 0.000592920276 | 90.3037291 | 000002/000056_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      4 |   XYZ |       1 |  0.00120651244 | 88.7565023 | 000002/000061_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      4 |   ZYX |       1 |  0.00119759999 | 89.4170216 | 000002/000066_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      8 |   XYZ |       1 |  0.00292189885 | 73.2987892 | 000002/000071_run/work|
|      benchmark_ghex_comm_mpi | 64 |    2 |      8 |   ZYX |       1 |  0.00280053631 | 76.4752261 | 000002/000076_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      1 |   XYZ |       1 | 0.000578371815 |  98.354115 | 000002/000081_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      1 |   ZYX |       1 | 0.000567856776 | 100.175344 | 000002/000086_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      2 |   XYZ |       1 |   0.0013672078 |  83.213756 | 000002/000091_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      2 |   ZYX |       1 |  0.00133950905 | 84.9344734 | 000002/000096_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      4 |   XYZ |       1 |  0.00295105898 |  77.104861 | 000002/000101_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      4 |   ZYX |       1 |  0.00299364916 | 76.0079021 | 000002/000106_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      8 |   XYZ |       1 |    0.008214935 | 55.3969063 | 000002/000111_run/work|
|      benchmark_ghex_comm_mpi | 64 |    4 |      8 |   ZYX |       1 |   0.0081211077 |  56.036935 | 000002/000116_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      1 |   XYZ |       1 | 0.000946870177 | 77.3674805 | 000002/000121_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      1 |   ZYX |       1 | 0.000950708609 | 77.0551137 | 000002/000126_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      2 |   XYZ |       1 |  0.00179452059 | 81.6451595 | 000002/000131_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      2 |   ZYX |       1 |  0.00175876763 | 83.3048763 | 000002/000136_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      4 |   XYZ |       1 |  0.00424298439 | 69.0617294 | 000002/000141_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      4 |   ZYX |       1 |  0.00423489031 | 69.1937261 | 000002/000146_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      8 |   XYZ |       1 |   0.0115591121 | 50.7007522 | 000002/000151_run/work|
|      benchmark_ghex_comm_mpi | 64 |    5 |      8 |   ZYX |       1 |   0.0115858045 | 50.5839434 | 000002/000156_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      1 |   XYZ |       1 | 0.000228669868 | 56.7640333 | 000002/000003_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      1 |   ZYX |       1 | 0.000229073941 | 56.6639049 | 000002/000008_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      2 |   XYZ |       1 | 0.000418682503 | 62.0050941 | 000002/000013_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      2 |   ZYX |       1 | 0.000429786948 | 60.4030628 | 000002/000018_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      4 |   XYZ |       1 | 0.000812451163 | 63.9064825 | 000002/000023_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      4 |   ZYX |       1 | 0.000803459783 | 64.6216489 | 000002/000028_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      8 |   XYZ |       1 |  0.00157939334 |  65.747898 | 000002/000033_run/work|
|      benchmark_ghex_comm_ucx | 64 |    1 |      8 |   ZYX |       1 |  0.00157985123 | 65.7288421 | 000002/000038_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      1 |   XYZ |       1 | 0.000284147105 | 94.2168881 | 000002/000043_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      1 |   ZYX |       1 | 0.000286451655 |  93.458898 | 000002/000048_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      2 |   XYZ |       1 | 0.000547209203 | 97.8472433 | 000002/000053_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      2 |   ZYX |       1 | 0.000544345562 | 98.3619887 | 000002/000058_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      4 |   XYZ |       1 |  0.00109385295 | 97.8978248 | 000002/000063_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      4 |   ZYX |       1 |  0.00107688717 | 99.4401524 | 000002/000068_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      8 |   XYZ |       1 |  0.00256790077 | 83.4033971 | 000002/000073_run/work|
|      benchmark_ghex_comm_ucx | 64 |    2 |      8 |   ZYX |       1 |  0.00254251421 | 84.2361656 | 000002/000078_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      1 |   XYZ |       1 | 0.000558397644 | 101.872292 | 000002/000083_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      1 |   ZYX |       1 | 0.000552973939 | 102.871481 | 000002/000088_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      2 |   XYZ |       1 |  0.00114199248 | 99.6245581 | 000002/000093_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      2 |   ZYX |       1 |  0.00114437981 | 99.4167274 | 000002/000098_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      4 |   XYZ |       1 |  0.00255408389 | 89.0890831 | 000002/000103_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      4 |   ZYX |       1 |  0.00257391899 | 88.4025461 | 000002/000108_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      8 |   XYZ |       1 |  0.00647964715 | 70.2325256 | 000002/000113_run/work|
|      benchmark_ghex_comm_ucx | 64 |    4 |      8 |   ZYX |       1 |   0.0064837991 | 70.1875516 | 000002/000118_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      1 |   XYZ |       1 | 0.000732791572 | 99.9697087 | 000002/000123_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      1 |   ZYX |       1 | 0.000719809559 | 101.772697 | 000002/000128_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      2 |   XYZ |       1 |   0.0016263568 | 90.0871937 | 000002/000133_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      2 |   ZYX |       1 |  0.00156142045 | 93.8337394 | 000002/000138_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      4 |   XYZ |       1 |  0.00341529033 | 85.7988082 | 000002/000143_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      4 |   ZYX |       1 |  0.00350349732 | 83.6386653 | 000002/000148_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      8 |   XYZ |       1 |  0.00944039564 | 62.0795677 | 000002/000153_run/work|
|      benchmark_ghex_comm_ucx | 64 |    5 |      8 |   ZYX |       1 |  0.00945359525 | 61.9928889 | 000002/000158_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   XYZ |       1 | 0.000254687023 | 50.9653921 | 000002/000002_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      1 |   ZYX |       1 |  0.00024068917 | 53.9294061 | 000002/000007_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      2 |   XYZ |       1 | 0.000468105336 | 55.4585603 | 000002/000012_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      2 |   ZYX |       1 | 0.000448438421 | 57.8907756 | 000002/000017_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      4 |   XYZ |       1 | 0.000839354828 | 61.8581013 | 000002/000022_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      4 |   ZYX |       1 | 0.000816053891 | 63.6243471 | 000002/000027_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      8 |   XYZ |       1 |  0.00164435681 |  63.150401 | 000002/000032_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    1 |      8 |   ZYX |       1 |  0.00162710541 | 63.8199537 | 000002/000037_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      1 |   XYZ |       1 | 0.000278360836 | 96.1753686 | 000002/000042_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      1 |   ZYX |       1 | 0.000281551359 | 95.0855151 | 000002/000047_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      2 |   XYZ |       1 | 0.000523615648 | 102.256134 | 000002/000052_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      2 |   ZYX |       1 | 0.000540128289 | 99.1299902 | 000002/000057_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      4 |   XYZ |       1 | 0.000945219217 | 113.292051 | 000002/000062_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      4 |   ZYX |       1 |  0.00093963457 | 113.965394 | 000002/000067_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      8 |   XYZ |       1 |    0.001980348 | 108.148491 | 000002/000072_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    2 |      8 |   ZYX |       1 |  0.00203775747 | 105.101638 | 000002/000077_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      1 |   XYZ |       1 | 0.000344886741 | 164.938924 | 000002/000082_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      1 |   ZYX |       1 | 0.000347992047 | 163.467092 | 000002/000087_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      2 |   XYZ |       1 | 0.000666152569 | 170.787446 | 000002/000092_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      2 |   ZYX |       1 | 0.000675015184 | 168.545092 | 000002/000097_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      4 |   XYZ |       1 |  0.00127311767 | 178.727386 | 000002/000102_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      4 |   ZYX |       1 |  0.00127022014 | 179.135084 | 000002/000107_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      8 |   XYZ |       1 |  0.00402748924 | 112.993966 | 000002/000112_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    4 |      8 |   ZYX |       1 |  0.00399309767 | 113.967156 | 000002/000117_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      1 |   XYZ |       1 | 0.000430194092 | 170.288159 | 000002/000122_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      1 |   ZYX |       1 | 0.000439763949 | 166.582459 | 000002/000127_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      2 |   XYZ |       1 | 0.000792054164 | 184.979673 | 000002/000132_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      2 |   ZYX |       1 | 0.000801439249 | 182.813507 | 000002/000137_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      4 |   XYZ |       1 |  0.00167523015 |  174.91796 | 000002/000142_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      4 |   ZYX |       1 |  0.00167912329 | 174.512403 | 000002/000147_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      8 |   XYZ |       1 |  0.00550334495 | 106.490813 | 000002/000152_run/work|
|benchmark_ghex_comm_xpmem_mpi | 64 |    5 |      8 |   ZYX |       1 |  0.00555060957 | 105.584022 | 000002/000157_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   XYZ |       1 | 0.000256007711 | 50.7024728 | 000002/000004_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      1 |   ZYX |       1 | 0.000244628093 | 53.0610521 | 000002/000009_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      2 |   XYZ |       1 | 0.000458722885 | 56.5928774 | 000002/000014_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      2 |   ZYX |       1 | 0.000438312538 | 59.2281665 | 000002/000019_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      4 |   XYZ |       1 | 0.000829233091 | 62.6131501 | 000002/000024_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      4 |   ZYX |       1 | 0.000807631668 | 64.2878407 | 000002/000029_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      8 |   XYZ |       1 |  0.00163905277 | 63.3547581 | 000002/000034_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    1 |      8 |   ZYX |       1 |  0.00162112434 | 64.0554147 | 000002/000039_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      1 |   XYZ |       1 | 0.000278727611 | 96.0488123 | 000002/000044_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      1 |   ZYX |       1 | 0.000278555392 | 96.1081952 | 000002/000049_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      2 |   XYZ |       1 | 0.000513200554 |  104.33136 | 000002/000054_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      2 |   ZYX |       1 | 0.000523128574 | 102.351343 | 000002/000059_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      4 |   XYZ |       1 | 0.000946924443 | 113.088034 | 000002/000064_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      4 |   ZYX |       1 | 0.000942444074 | 113.625654 | 000002/000069_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      8 |   XYZ |       1 |  0.00196883032 | 108.781161 | 000002/000074_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    2 |      8 |   ZYX |       1 |  0.00199674441 | 107.260422 | 000002/000079_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      1 |   XYZ |       1 | 0.000353179534 | 161.066094 | 000002/000084_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      1 |   ZYX |       1 |  0.00035494819 | 160.263525 | 000002/000089_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      2 |   XYZ |       1 |  0.00066774739 | 170.379544 | 000002/000094_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      2 |   ZYX |       1 | 0.000665214615 | 171.028257 | 000002/000099_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      4 |   XYZ |       1 |  0.00128383031 | 177.236033 | 000002/000104_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      4 |   ZYX |       1 |   0.0012794692 | 177.840148 | 000002/000109_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      8 |   XYZ |       1 |  0.00388889858 |  117.02079 | 000002/000114_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    4 |      8 |   ZYX |       1 |  0.00397537965 |   114.4751 | 000002/000119_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      1 |   XYZ |       1 | 0.000448371097 | 163.384662 | 000002/000124_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      1 |   ZYX |       1 |  0.00045557702 | 160.800385 | 000002/000129_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      2 |   XYZ |       1 | 0.000796326416 | 183.987266 | 000002/000134_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      2 |   ZYX |       1 | 0.000796298679 | 183.993675 | 000002/000139_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      4 |   XYZ |       1 |  0.00170003439 | 172.365831 | 000002/000144_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      4 |   ZYX |       1 |  0.00169509365 | 172.868231 | 000002/000149_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      8 |   XYZ |       1 |  0.00539818481 | 108.565323 | 000002/000154_run/work|
|benchmark_ghex_comm_xpmem_ucx | 64 |    5 |      8 |   ZYX |       1 |  0.00562461192 | 104.194865 | 000002/000159_run/work|
|           benchmark_mpi_comm | 64 |    1 |      1 |   XYZ |       1 | 0.000138134403 | 93.9680754 | 000002/000000_run/work|
|           benchmark_mpi_comm | 64 |    1 |      1 |   ZYX |       1 | 0.000140154659 | 92.6135748 | 000002/000005_run/work|
|           benchmark_mpi_comm | 64 |    1 |      2 |   XYZ |       1 | 0.000278341471 | 93.2683438 | 000002/000010_run/work|
|           benchmark_mpi_comm | 64 |    1 |      2 |   ZYX |       1 | 0.000278295024 | 93.2839101 | 000002/000015_run/work|
|           benchmark_mpi_comm | 64 |    1 |      4 |   XYZ |       1 | 0.000562050251 |  92.377676 | 000002/000020_run/work|
|           benchmark_mpi_comm | 64 |    1 |      4 |   ZYX |       1 | 0.000559421562 | 92.8117533 | 000002/000025_run/work|
|           benchmark_mpi_comm | 64 |    1 |      8 |   XYZ |       1 |  0.00113223596 | 91.7139146 | 000002/000030_run/work|
|           benchmark_mpi_comm | 64 |    1 |      8 |   ZYX |       1 |  0.00112216098 | 92.5373395 | 000002/000035_run/work|
|           benchmark_mpi_comm | 64 |    2 |      1 |   XYZ |       1 | 0.000192571746 | 139.020685 | 000002/000040_run/work|
|           benchmark_mpi_comm | 64 |    2 |      1 |   ZYX |       1 | 0.000190632549 | 140.434864 | 000002/000045_run/work|
|           benchmark_mpi_comm | 64 |    2 |      2 |   XYZ |       1 | 0.000388234883 | 137.913707 | 000002/000050_run/work|
|           benchmark_mpi_comm | 64 |    2 |      2 |   ZYX |       1 | 0.000440734126 | 121.485741 | 000002/000055_run/work|
|           benchmark_mpi_comm | 64 |    2 |      4 |   XYZ |       1 | 0.000773222661 | 138.492868 | 000002/000060_run/work|
|           benchmark_mpi_comm | 64 |    2 |      4 |   ZYX |       1 | 0.000767824825 | 139.466478 | 000002/000065_run/work|
|           benchmark_mpi_comm | 64 |    2 |      8 |   XYZ |       1 |  0.00159012592 | 134.688483 | 000002/000070_run/work|
|           benchmark_mpi_comm | 64 |    2 |      8 |   ZYX |       1 |  0.00157248693 | 136.199318 | 000002/000075_run/work|
|           benchmark_mpi_comm | 64 |    4 |      1 |   XYZ |       1 | 0.000304393441 | 186.880663 | 000002/000080_run/work|
|           benchmark_mpi_comm | 64 |    4 |      1 |   ZYX |       1 | 0.000303236209 |  187.59385 | 000002/000085_run/work|
|           benchmark_mpi_comm | 64 |    4 |      2 |   XYZ |       1 | 0.000756954614 | 150.300287 | 000002/000090_run/work|
|           benchmark_mpi_comm | 64 |    4 |      2 |   ZYX |       1 | 0.000757792048 | 150.134191 | 000002/000095_run/work|
|           benchmark_mpi_comm | 64 |    4 |      4 |   XYZ |       1 |  0.00124304612 | 183.051125 | 000002/000100_run/work|
|           benchmark_mpi_comm | 64 |    4 |      4 |   ZYX |       1 |  0.00155028125 | 146.774008 | 000002/000105_run/work|
|           benchmark_mpi_comm | 64 |    4 |      8 |   XYZ |       1 |  0.00304428063 | 149.487527 | 000002/000110_run/work|
|           benchmark_mpi_comm | 64 |    4 |      8 |   ZYX |       1 |  0.00306633304 | 148.412445 | 000002/000115_run/work|
|           benchmark_mpi_comm | 64 |    5 |      1 |   XYZ |       1 | 0.000388292961 | 188.664146 | 000002/000120_run/work|
|           benchmark_mpi_comm | 64 |    5 |      1 |   ZYX |       1 | 0.000386246901 | 189.663554 | 000002/000125_run/work|
|           benchmark_mpi_comm | 64 |    5 |      2 |   XYZ |       1 | 0.000989639577 | 148.047757 | 000002/000130_run/work|
|           benchmark_mpi_comm | 64 |    5 |      2 |   ZYX |       1 | 0.000987568834 | 148.358185 | 000002/000135_run/work|
|           benchmark_mpi_comm | 64 |    5 |      4 |   XYZ |       1 |  0.00158349999 | 185.050737 | 000002/000140_run/work|
|           benchmark_mpi_comm | 64 |    5 |      4 |   ZYX |       1 |  0.00158790362 | 184.537547 | 000002/000145_run/work|
|           benchmark_mpi_comm | 64 |    5 |      8 |   XYZ |       1 |   0.0052953714 | 110.673197 | 000002/000150_run/work|
|           benchmark_mpi_comm | 64 |    5 |      8 |   ZYX |       1 |   0.0049293129 | 118.891962 | 000002/000155_run/work|

