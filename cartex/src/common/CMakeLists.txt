add_library(options options.cpp)
target_link_libraries(options PRIVATE cartex)

add_library(thread_pool thread_pool.cpp)
target_link_libraries(thread_pool PRIVATE cartex)

add_library(sync_loop sync_loop.cpp)
target_link_libraries(sync_loop PRIVATE cartex)
target_link_libraries(sync_loop PRIVATE MPI::MPI_CXX)
