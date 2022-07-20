#find_package(Threads REQUIRED)

set(P2P_ENABLE_DEVICE "OFF" CACHE BOOL "use gpu")

set(P2P_TRANSPORT_BACKEND "MPI" CACHE STRING "Choose the backend type: MPI | UCX | LIBFABRIC")
set_property(CACHE P2P_TRANSPORT_BACKEND PROPERTY STRINGS "MPI" "UCX" "LIBFABRIC")

#find_package(Threads REQUIRED)
#compile_as_cuda(benchmark.cpp)
#add_executable(benchmark benchmark.cpp)

#set(_oomph_repository "https://github.com/ghex-org/oomph.git")
#set(_oomph_tag        "main")
set(_oomph_repository "https://github.com/boeschf/oomph.git")
set(_oomph_tag        "refactoring")

if(NOT _oomph_already_fetched)
    message(STATUS "Fetching oomph ${_oomph_tag} from ${_oomph_repository}")
endif()
include(FetchContent)
FetchContent_Declare(
    oomph
    GIT_REPOSITORY ${_oomph_repository}
    GIT_TAG        ${_oomph_tag}
)

if (P2P_TRANSPORT_BACKEND STREQUAL "LIBFABRIC")
    set(OOMPH_WITH_MPI OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_UCX OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_LIBFABRIC ON CACHE INTERNAL "")  # Forces the value
    #target_link_libraries(runtime PUBLIC oomph::libfabric)
    #target_compile_definitions(runtime PUBLIC P2P_TRANSPORT=LIBFABRIC)
elseif (P2P_TRANSPORT_BACKEND STREQUAL "UCX")
    set(OOMPH_WITH_MPI OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_UCX ON CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_LIBFABRIC OFF CACHE INTERNAL "")  # Forces the value
    #target_link_libraries(runtime PUBLIC oomph::ucx)
    #target_compile_definitions(runtime PUBLIC P2P_TRANSPORT=UCX)
else()
    set(OOMPH_WITH_MPI ON CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_UCX OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_LIBFABRIC OFF CACHE INTERNAL "")  # Forces the value
    #target_link_libraries(runtime PUBLIC oomph::mpi)
    #target_compile_definitions(runtime PUBLIC P2P_TRANSPORT=MPI)
endif()

if(P2P_ENABLE_DEVICE)
    set(OOMPH_USE_GPU ON CACHE INTERNAL "")  # Forces the value
    set(OOMPH_GPU_TYPE "NVIDIA" CACHE INTERNAL "")  # Forces the value
else() 
    set(OOMPH_USE_GPU OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_GPU_TYPE "AUTO" CACHE INTERNAL "")  # Forces the value
endif()

FetchContent_MakeAvailable(oomph)
set(_oomph_already_fetched ON CACHE INTERNAL "")

#target_sources(runtime PRIVATE runtime.cpp)
#target_compile_definitions(runtime PUBLIC P2P_RMA=${P2P_RMA})

add_library(p2p_helper)
target_link_libraries(p2p_helper PRIVATE p2p)
ghexbench_target_compile_options(p2p_helper)
target_sources(p2p_helper PRIVATE device_map.cpp)


add_subdirectory(bin)
