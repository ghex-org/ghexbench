set(P2P_TRANSPORT_BACKEND "MPI" CACHE STRING "Choose the backend type: MPI | UCX | LIBFABRIC")
set_property(CACHE P2P_TRANSPORT_BACKEND PROPERTY STRINGS "MPI" "UCX" "LIBFABRIC")

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

#set(_oomph_repository "https://github.com/ghex-org/oomph.git")
#set(_oomph_tag        "main")
set(_oomph_repository "https://github.com/boeschf/oomph.git")
set(_oomph_tag        "refactoring2")

if(NOT _oomph_already_fetched)
    message(STATUS "Fetching oomph ${_oomph_tag} from ${_oomph_repository}")
endif()
include(FetchContent)
FetchContent_Declare(
    oomph
    GIT_REPOSITORY ${_oomph_repository}
    GIT_TAG        ${_oomph_tag}
)

if(GHEXBENCH_ENABLE_DEVICE)
    set(OOMPH_USE_GPU ON CACHE INTERNAL "")  # Forces the value
    set(OOMPH_GPU_TYPE ${GHEXBENCH_DEVICE_TYPE} CACHE INTERNAL "")  # Forces the value
else() 
    set(OOMPH_USE_GPU OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_GPU_TYPE "AUTO" CACHE INTERNAL "")  # Forces the value
endif()
set(OOMPH_GIT_SUBMODULE OFF CACHE INTERNAL "")  # Forces the value
set(OOMPH_USE_BUNDLED_LIBS ON CACHE INTERNAL "")  # Forces the value

FetchContent_MakeAvailable(oomph)
set(_oomph_already_fetched ON CACHE INTERNAL "")


set(benchmarks
    #benchmark_paired
    #benchmark_paired_simple
    benchmark
)

function(compile_benchmark b_)
    set(b ${b_}.x)
    add_executable(${b})

    target_sources(${b} PRIVATE main.cpp)
    target_sources(${b} PRIVATE ${b_}.cpp)
    target_sources(${b} PRIVATE device_map.cpp)

    ghexbench_target_compile_options(${b})
    if (GHEXBENCH_ENABLE_DEVICE)
        target_compile_definitions(${b} PRIVATE P2P_ENABLE_DEVICE)
    endif()

    target_link_libraries(${b} PRIVATE p2p)
    target_link_libraries(${b} PRIVATE thread_pool)
    target_link_libraries(${b} PRIVATE options)
    target_link_libraries(${b} PRIVATE mpi_utils)
    target_link_libraries(${b} PRIVATE HWCART::hwcart)
    if (P2P_TRANSPORT_BACKEND STREQUAL "LIBFABRIC")
        target_link_libraries(${b} PUBLIC oomph::libfabric)
    elseif (P2P_TRANSPORT_BACKEND STREQUAL "UCX")
        target_link_libraries(${b} PUBLIC oomph::ucx)
    else()
        target_link_libraries(${b} PUBLIC oomph::mpi)
    endif()

    install(TARGETS ${b}
        CONFIGURATIONS Debug
        RUNTIME DESTINATION Debug/bin)
    install(TARGETS ${b}
        CONFIGURATIONS Release
        RUNTIME DESTINATION Release/bin)

endfunction()

foreach(b ${benchmarks})
    compile_benchmark(${b})
endforeach()
