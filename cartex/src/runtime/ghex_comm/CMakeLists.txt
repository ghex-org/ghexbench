set(CARTEX_RMA "none" CACHE STRING "select rma option")
set_property(CACHE CARTEX_RMA PROPERTY STRINGS "none" "threads" "xpmem")
  
set(CARTEX_STAGED_EXCHANGE OFF CACHE BOOL "use 6-way exchange")
if (CARTEX_STAGED_EXCHANGE)
    target_compile_definitions(runtime PRIVATE CARTEX_GHEX_STAGED)
endif()

set(CARTEX_FIELD_BY_FIELD OFF CACHE BOOL "exchange field by field")
if (CARTEX_FIELD_BY_FIELD)
  target_compile_definitions(runtime PRIVATE CARTEX_GHEX_FIELD_BY_FIELD)
endif()

set(CARTEX_TRANSPORT_BACKEND "MPI" CACHE STRING "Choose the backend type: MPI | UCX | LIBFABRIC")
set_property(CACHE CARTEX_TRANSPORT_BACKEND PROPERTY STRINGS "MPI" "UCX" "LIBFABRIC")

#set(_ghex_repository "https://github.com/GridTools/GHEX.git")
set(_ghex_repository "https://github.com/boeschf/GHEX.git")
set(_ghex_tag        "benchmarking_to_master")
if(NOT _ghex_already_fetched)
    message(STATUS "Fetching GHEX ${_ghex_tag} from ${_ghex_repository}")
endif()
include(FetchContent)
FetchContent_Declare(
    ghex
    GIT_REPOSITORY ${_ghex_repository}
    GIT_TAG        ${_ghex_tag})


if((NOT CARTEX_RMA STREQUAL "xpmem") AND (NOT CARTEX_RMA STREQUAL "threads"))
    set(GHEX_NO_RMA ON CACHE INTERNAL "")  # Forces the value
else()
    set(GHEX_NO_RMA OFF CACHE INTERNAL "")  # Forces the value
endif()

if(CARTEX_RMA STREQUAL "xpmem")
    set(GHEX_USE_XPMEM ON CACHE INTERNAL "")  # Forces the value
else()
    set(GHEX_USE_XPMEM OFF CACHE INTERNAL "")  # Forces the value
endif()

if (CARTEX_TRANSPORT_BACKEND STREQUAL "LIBFABRIC")
    set (GHEX_TRANSPORT_BACKEND "LIBFABRIC" CACHE INTERNAL "") # Forces the value
    target_compile_definitions(runtime PUBLIC CARTEX_TRANSPORT=LIBFABRIC)
elseif (CARTEX_TRANSPORT_BACKEND STREQUAL "UCX")
    set (GHEX_TRANSPORT_BACKEND "UCX" CACHE INTERNAL "") # Forces the value
    target_compile_definitions(runtime PUBLIC CARTEX_TRANSPORT=UCX)
else()
    set (GHEX_TRANSPORT_BACKEND "MPI" CACHE INTERNAL "") # Forces the value
    target_compile_definitions(runtime PUBLIC CARTEX_TRANSPORT=MPI)
endif()

if(_cartex_cuda_enabled)
    set (GHEX_USE_GPU ON CACHE INTERNAL "") # Forces the value
    set (GHEX_GPU_TYPE "NVIDIA" CACHE INTERNAL "") # Forces the value
else() 
    set (GHEX_USE_GPU OFF CACHE INTERNAL "") # Forces the value
    set (GHEX_GPU_TYPE "AUTO" CACHE INTERNAL "") # Forces the value
endif()

FetchContent_MakeAvailable(ghex)
set(_ghex_already_fetched ON CACHE INTERNAL "")

target_sources(runtime PRIVATE runtime.cpp)
target_link_libraries(runtime PUBLIC GHEX::lib)
target_compile_definitions(runtime PUBLIC CARTEX_RMA=${CARTEX_RMA})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ghex_cmake_config.inc.in 
    ${CMAKE_BINARY_DIR}/include/ghex_cmake_config.inc)

