find_package(MPI REQUIRED COMPONENTS CXX)

set(_tausch_repository "https://github.com/luspi/tausch.git")
set(_tausch_tag        "master")
if(NOT _tausch_already_fetched)
    message(STATUS "Fetching tausch ${_tausch_tag} from ${_tausch_repository}")
endif()
include(FetchContent)
FetchContent_Declare(
    tausch
    GIT_REPOSITORY ${_tausch_repository}
    GIT_TAG        ${_tausch_tag}
)

set(TESTING OFF CACHE INTERNAL "")  # Forces the value
set(DOC OFF CACHE INTERNAL "")  # Forces the value

FetchContent_MakeAvailable(tausch)
set(_tausch_already_fetched ON CACHE INTERNAL "")

# do not build C library ctausch
set_target_properties(ctausch PROPERTIES EXCLUDE_FROM_ALL True)

# get include path
FetchContent_GetProperties(tausch SOURCE_DIR tausch_src_dir)
target_include_directories(runtime PUBLIC ${tausch_src_dir})

target_sources(runtime PRIVATE runtime.cpp)

