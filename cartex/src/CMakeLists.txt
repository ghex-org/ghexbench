find_package(MPI REQUIRED COMPONENTS CXX)
find_package(HWLOC REQUIRED)

if(NOT _hwcart_already_fetched)
  find_package(HWCART QUIET)
endif()
if(NOT HWCART_FOUND)
  set(_hwcart_repository "git@github.com:NordicHPC/hwcart.git")
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

add_subdirectory(common)
add_subdirectory(device)
add_subdirectory(decomposition)
add_subdirectory(runtime)
add_subdirectory(bin)
