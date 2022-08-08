cmake_minimum_required(VERSION 3.17)

project(ghexbench VERSION 0.1 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE) # AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ghexbench_compile_options)

add_library(ghexbench INTERFACE)
set_target_properties(ghexbench PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
target_include_directories(ghexbench INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set(GHEXBENCH_ENABLE_DEVICE "OFF" CACHE BOOL "use gpu")
set(GHEXBENCH_DEVICE_TYPE "NVIDIA" CACHE STRING "Choose the device type:  NVIDIA | AMD")
set_property(CACHE GHEXBENCH_DEVICE_TYPE PROPERTY STRINGS "NVIDIA" "AMD")

add_subdirectory(src)

set(GHEXBENCH_BUILD_CARTEX "OFF" CACHE BOOL "cartex benchmark")
if (GHEXBENCH_BUILD_CARTEX)
    add_subdirectory(cartex)
endif()

set(GHEXBENCH_BUILD_P2P "OFF" CACHE BOOL "p2p benchmark")
if (GHEXBENCH_BUILD_P2P)
    add_subdirectory(p2p)
endif()
