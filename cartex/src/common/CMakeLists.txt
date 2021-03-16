compile_as_cuda(options.cpp)
add_library(options options.cpp)
target_link_libraries(options PRIVATE cartex)

compile_as_cuda(thread_pool.cpp)
add_library(thread_pool thread_pool.cpp)
target_link_libraries(thread_pool PRIVATE cartex)

compile_as_cuda(sync_loop.cpp)
add_library(sync_loop sync_loop.cpp)
target_link_libraries(sync_loop PRIVATE cartex)
target_link_libraries(sync_loop PRIVATE MPI::MPI_CXX)
