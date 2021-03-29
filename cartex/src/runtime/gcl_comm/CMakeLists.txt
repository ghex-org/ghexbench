if(NOT _gridtools_already_fetched)
  find_package(GridTools "2" QUIET)
endif()
if(NOT GridTools_FOUND)
  set(_gridtools_repository "https://github.com/GridTools/gridtools.git")
  set(_gridtools_tag        "release_v2.0")
  if(NOT _gridtools_already_fetched)
    message(STATUS "Fetching GridTools ${_gridtools_tag} from ${_gridtools_repository}")
  endif()
  include(FetchContent)
  FetchContent_Declare(
    gridtools
    GIT_REPOSITORY ${_gridtools_repository}
    GIT_TAG        ${_gridtools_tag}
  )
  FetchContent_MakeAvailable(gridtools)
  set(_gridtools_already_fetched ON CACHE INTERNAL "")
endif()
target_sources(runtime PRIVATE runtime.cpp)
target_link_libraries(runtime PUBLIC GridTools::gridtools)

