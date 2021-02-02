
find_package(MPI REQUIRED COMPONENTS CXX)
find_package(HWLOC REQUIRED)
find_package(HWCART REQUIRED)


compile_as_cuda(decomposition.cpp TARGET_DIRECTORY cartex)
target_sources(cartex PRIVATE decomposition.cpp)
target_link_libraries(cartex PUBLIC MPI::MPI_CXX)
target_link_libraries(cartex PUBLIC HWCART::libhwcart)
target_link_libraries(cartex PUBLIC HWLOC::libhwloc)

