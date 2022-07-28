list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(p2p_version)
make_version(GHEXBENCH_P2P 1 0 0)

project(p2p VERSION ${GHEXBENCH_P2P_VERSION} LANGUAGES CXX)

add_library(p2p INTERFACE)
target_include_directories(p2p INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(p2p INTERFACE ghexbench)


configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/p2p_config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp)

add_subdirectory(src)
