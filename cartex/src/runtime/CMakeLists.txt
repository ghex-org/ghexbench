find_package(MPI REQUIRED COMPONENTS CXX)

add_library(runtime)
target_link_libraries(runtime PRIVATE cartex)
target_link_libraries(runtime PRIVATE MPI::MPI_CXX)
target_link_libraries(runtime PRIVATE HWCART::hwcart)

compile_as_cuda(runtime.cpp)
target_sources(runtime PRIVATE runtime.cpp)

target_compile_definitions(runtime PUBLIC CARTEX_RUNTIME=${CARTEX_RUNTIME})
add_subdirectory(${CARTEX_RUNTIME})

