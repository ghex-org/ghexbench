find_package(MPI REQUIRED COMPONENTS CXX)

add_library(runtime)
target_link_libraries(runtime PRIVATE cartex)
target_link_libraries(runtime PRIVATE sync_loop)
target_link_libraries(runtime PRIVATE MPI::MPI_CXX)
target_link_libraries(runtime PRIVATE HWCART::hwcart)
target_compile_definitions(runtime PUBLIC CARTEX_RUNTIME=${CARTEX_RUNTIME})

compile_as_cuda(runtime.cpp)
target_sources(runtime PRIVATE runtime.cpp)

compile_as_cuda(${CARTEX_RUNTIME}/runtime.cpp)
add_subdirectory(${CARTEX_RUNTIME})
