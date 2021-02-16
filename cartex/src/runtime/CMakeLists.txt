find_package(MPI REQUIRED COMPONENTS CXX)

add_library(runtime)
target_link_libraries(runtime PUBLIC cartex_common)
target_link_libraries(cartex PUBLIC runtime)
target_link_libraries(runtime PUBLIC MPI::MPI_CXX)
target_link_libraries(runtime PUBLIC HWCART::hwcart)

compile_as_cuda(runtime.cpp TARGET_DIRECTORY runtime)
target_sources(runtime PRIVATE runtime.cpp)

target_compile_definitions(runtime PUBLIC CARTEX_RUNTIME=${CARTEX_RUNTIME})
add_subdirectory(${CARTEX_RUNTIME})

