compile_as_cuda(benchmark.cpp)
add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE cartex)
target_compile_definitions(benchmark PRIVATE CARTEX_RUNTIME=${CARTEX_RUNTIME})
