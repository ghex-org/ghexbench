set(CARTEX_MPI_PACK_Z OFF CACHE BOOL "pack/unpack z buffers instead of direct exchange")
if (CARTEX_MPI_PACK_Z)
    target_compile_definitions(runtime PRIVATE CARTEX_MPI_PACK_Z)
endif()

set(CARTEX_MPI_PACK_Z_LOOP_GPU OFF CACHE BOOL "use z-loop in kernels")
if (CARTEX_MPI_PACK_Z_LOOP_GPU)
    target_compile_definitions(runtime PRIVATE CARTEX_MPI_PACK_Z_LOOP_GPU)
endif()

target_sources(runtime PRIVATE runtime.cpp)
