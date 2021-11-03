set(CARTEX_TRANSPORT_BACKEND "MPI" CACHE STRING "Choose the backend type: MPI | UCX | LIBFABRIC")
set_property(CACHE CARTEX_TRANSPORT_BACKEND PROPERTY STRINGS "MPI" "UCX" "LIBFABRIC")

#define 
set(CARTEX_OOMPH_NO_BUFFER_REUSE OFF CACHE BOOL "no buffer reuse among fields, but overlap")
if (CARTEX_OOMPH_NO_BUFFER_REUSE)
    target_compile_definitions(runtime PRIVATE CARTEX_OOMPH_NO_BUFFER_REUSE)
endif()

#set(_oomph_repository "https://github.com/ghex-org/oomph.git")
#set(_oomph_tag        "main")
set(_oomph_repository "https://github.com/boeschf/oomph.git")
set(_oomph_tag        "tensor")

if(NOT _oomph_already_fetched)
    message(STATUS "Fetching oomph ${_oomph_tag} from ${_oomph_repository}")
endif()
include(FetchContent)
FetchContent_Declare(
    oomph
    GIT_REPOSITORY ${_oomph_repository}
    GIT_TAG        ${_oomph_tag}
)

if (CARTEX_TRANSPORT_BACKEND STREQUAL "LIBFABRIC")
    set(OOMPH_WITH_MPI OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_UCX OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_LIBFABRIC ON CACHE INTERNAL "")  # Forces the value
    target_link_libraries(runtime PUBLIC oomph::libfabric)
    target_compile_definitions(runtime PUBLIC CARTEX_TRANSPORT=LIBFABRIC)
elseif (CARTEX_TRANSPORT_BACKEND STREQUAL "UCX")
    set(OOMPH_WITH_MPI OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_UCX ON CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_LIBFABRIC OFF CACHE INTERNAL "")  # Forces the value
    target_link_libraries(runtime PUBLIC oomph::ucx)
    target_compile_definitions(runtime PUBLIC CARTEX_TRANSPORT=UCX)
else()
    set(OOMPH_WITH_MPI ON CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_UCX OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_WITH_LIBFABRIC OFF CACHE INTERNAL "")  # Forces the value
    target_link_libraries(runtime PUBLIC oomph::mpi)
    target_compile_definitions(runtime PUBLIC CARTEX_TRANSPORT=MPI)
endif()

if(_cartex_cuda_enabled)
    set(OOMPH_USE_GPU ON CACHE INTERNAL "")  # Forces the value
    set(OOMPH_GPU_TYPE "NVIDIA" CACHE INTERNAL "")  # Forces the value
else() 
    set(OOMPH_USE_GPU OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_GPU_TYPE "AUTO" CACHE INTERNAL "")  # Forces the value
endif()

FetchContent_MakeAvailable(oomph)
set(_oomph_already_fetched ON CACHE INTERNAL "")

target_sources(runtime PRIVATE runtime.cpp)
#target_compile_definitions(runtime PUBLIC CARTEX_RMA=${CARTEX_RMA})
