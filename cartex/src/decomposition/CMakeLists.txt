add_library(decomposition decomposition.cpp)
target_link_libraries(decomposition PRIVATE cartex)
target_link_libraries(decomposition PRIVATE MPI::MPI_CXX)
target_link_libraries(decomposition PRIVATE HWCART::hwcart)
target_link_libraries(decomposition PRIVATE HWLOC::libhwloc)

