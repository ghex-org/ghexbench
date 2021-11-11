cmake_minimum_required(VERSION 3.14.5)

file(STRINGS "version.txt" _cartex_version)
project(CartEx VERSION ${_cartex_version} LANGUAGES CXX)
unset(_cartex_version)

if(NOT CMAKE_BUILD_TYPE) # AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  set_property(CACHE CMAKE_CXX_FLAGS PROPERTY VALUE "-Wall -Wextra -Wpedantic -Wno-unknown-pragmas")
  set_property(CACHE CMAKE_CXX_FLAGS_RELEASE PROPERTY VALUE "-O3 -DNDEBUG -Wno-unused-parameter")
endif()
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


# User options
set(CARTEX_FLOAT "float" CACHE STRING "Floating-point type")
set_property(CACHE CARTEX_FLOAT PROPERTY STRINGS "float" "double")
set(CARTEX_RUNTIME "mpi_comm" CACHE STRING "Runtime")
set_property(CACHE CARTEX_RUNTIME PROPERTY STRINGS "mpi_comm" "gcl_comm" "ghex_comm")
set(CARTEX_ON_GPU "OFF" CACHE BOOL "use gpu")

# Set up cuda if needed
set(_cartex_cuda_enabled OFF)
if(CARTEX_ON_GPU)
  include(CheckLanguage)
  check_language(CUDA)
  if (CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 14)
    set(CMAKE_CUDA_EXTENSIONS OFF)
    set(_cartex_cuda_enabled ON)
    # WORKAROUND - enable cuda dynamic linking:
    if ( CMAKE_CUDA_HOST_IMPLICIT_LINK_LIBRARIES )
      list( REMOVE_ITEM CMAKE_CUDA_HOST_IMPLICIT_LINK_LIBRARIES "cudart_static" )
      list( REMOVE_ITEM CMAKE_CUDA_HOST_IMPLICIT_LINK_LIBRARIES "cudadevrt" )
      list( APPEND CMAKE_CUDA_HOST_IMPLICIT_LINK_LIBRARIES "cudart" )
    endif()
    if ( CMAKE_CUDA_IMPLICIT_LINK_LIBRARIES )
      list( REMOVE_ITEM CMAKE_CUDA_IMPLICIT_LINK_LIBRARIES "cudart_static" )
      list( REMOVE_ITEM CMAKE_CUDA_IMPLICIT_LINK_LIBRARIES "cudadevrt" )
      list( APPEND CMAKE_CUDA_IMPLICIT_LINK_LIBRARIES "cudart" )
    endif()
  endif()
endif()

function(compile_as_cuda)
  if(_cartex_cuda_enabled)
    set_source_files_properties(${ARGN} PROPERTIES LANGUAGE CUDA)
  endif()
endfunction()

# ---------------------------------------------------------------------
# generate config file
# ---------------------------------------------------------------------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_config.inc.in 
    ${CMAKE_CURRENT_BINARY_DIR}/include/cmake_config.inc)

add_library(cartex INTERFACE)
add_library(CartEx::cartex ALIAS cartex)
target_include_directories(cartex INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_definitions(cartex INTERFACE
# CARTEX_RUNTIME=${CARTEX_RUNTIME}
  CARTEX_RUNTIME_$<UPPER_CASE:${CARTEX_RUNTIME}>
  CARTEX_FLOAT=${CARTEX_FLOAT}
)
set_target_properties(
  cartex
  PROPERTIES
    INTERFACE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src)

