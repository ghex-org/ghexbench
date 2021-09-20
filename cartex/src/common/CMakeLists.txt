add_library(options options.cpp)
cartex_target_compile_options(options)
target_link_libraries(options PRIVATE cartex)


set(CARTEX_NUM_HYPERTHREADS "2" CACHE STRING "number of possible hyperthreads per core")
add_library(thread_pool thread_pool.cpp)
target_compile_definitions(thread_pool PRIVATE CARTEX_NUM_HT=${CARTEX_NUM_HYPERTHREADS})
cartex_target_compile_options(thread_pool)
target_link_libraries(thread_pool PRIVATE cartex)

add_library(sync_loop sync_loop.cpp)
cartex_target_compile_options(sync_loop)
target_link_libraries(sync_loop PRIVATE cartex)
target_link_libraries(sync_loop PRIVATE MPI::MPI_CXX)
