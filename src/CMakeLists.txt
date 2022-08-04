find_package(MPI REQUIRED COMPONENTS CXX)
find_package(HWLOC REQUIRED)

if(NOT _hwcart_already_fetched)
  find_package(HWCART QUIET)
endif()
if(NOT HWCART_FOUND)
  set(_hwcart_repository "https://github.com/NordicHPC/hwcart.git")
  set(_hwcart_tag        "master")
  if(NOT _hwcart_already_fetched)
    message(STATUS "Fetching HWCART ${_hwcart_tag} from ${_hwcart_repository}")
  endif()
  include(FetchContent)
  FetchContent_Declare(
    hwcart
    GIT_REPOSITORY ${_hwcart_repository}
    GIT_TAG        ${_hwcart_tag}
  )
  FetchContent_MakeAvailable(hwcart)
  set(_hwcart_already_fetched ON CACHE INTERNAL "")
endif()

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

if(GHEXBENCH_ENABLE_DEVICE)
    set(OOMPH_USE_GPU ON CACHE INTERNAL "")  # Forces the value
    set(OOMPH_GPU_TYPE ${GHEXBENCH_DEVICE_TYPE} CACHE INTERNAL "")  # Forces the value
else() 
    set(OOMPH_USE_GPU OFF CACHE INTERNAL "")  # Forces the value
    set(OOMPH_GPU_TYPE "AUTO" CACHE INTERNAL "")  # Forces the value
endif()

FetchContent_MakeAvailable(oomph)
set(_oomph_already_fetched ON CACHE INTERNAL "")

add_library(options options.cpp)
ghexbench_target_compile_options(options)
target_link_libraries(options PRIVATE ghexbench)

add_library(thread_pool thread_pool.cpp)
ghexbench_target_compile_options(thread_pool)
target_link_libraries(thread_pool PRIVATE ghexbench)

add_library(mpi_utils mpi.cpp mpi_environment.cpp hw_topo.cpp)
ghexbench_target_compile_options(mpi_utils)
target_link_libraries(mpi_utils PRIVATE ghexbench)
target_link_libraries(mpi_utils PRIVATE MPI::MPI_CXX)
target_link_libraries(mpi_utils PRIVATE HWCART::hwcart)
target_link_libraries(mpi_utils PRIVATE HWLOC::libhwloc)

add_library(sync_loop sync_loop.cpp)
ghexbench_target_compile_options(sync_loop)
target_link_libraries(sync_loop PRIVATE ghexbench)
target_link_libraries(sync_loop PRIVATE MPI::MPI_CXX)

