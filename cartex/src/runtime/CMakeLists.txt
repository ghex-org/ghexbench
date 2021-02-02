find_package(MPI REQUIRED COMPONENTS CXX)
find_package(HWCART REQUIRED)

add_library(runtime)
target_link_libraries(runtime PUBLIC cartex_common)
target_link_libraries(cartex PUBLIC runtime)
target_link_libraries(runtime PUBLIC MPI::MPI_CXX)
target_link_libraries(runtime PUBLIC HWCART::libhwcart)

add_subdirectory(${CARTEX_RUNTIME})

