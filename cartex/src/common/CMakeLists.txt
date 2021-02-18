compile_as_cuda(options.cpp)
add_library(options options.cpp)
target_link_libraries(options PRIVATE cartex)

compile_as_cuda(thread_pool.cpp)
add_library(thread_pool thread_pool.cpp)
target_link_libraries(thread_pool PRIVATE cartex)
