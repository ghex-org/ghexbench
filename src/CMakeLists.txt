find_package(MPI REQUIRED COMPONENTS CXX)

add_library(options options.cpp)
ghexbench_target_compile_options(options)
target_link_libraries(options PRIVATE ghexbench)

add_library(thread_pool thread_pool.cpp)
ghexbench_target_compile_options(thread_pool)
target_link_libraries(thread_pool PRIVATE ghexbench)

add_library(sync_loop sync_loop.cpp)
ghexbench_target_compile_options(sync_loop)
target_link_libraries(sync_loop PRIVATE ghexbench)
target_link_libraries(sync_loop PRIVATE MPI::MPI_CXX)
