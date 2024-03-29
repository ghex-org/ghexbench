#cmake_minimum_required(VERSION 3.14.5)

file(STRINGS "version.txt" _cartex_version)
project(CartEx VERSION ${_cartex_version} LANGUAGES CXX)
unset(_cartex_version)

#set(CMAKE_CXX_STANDARD 14)

message(${CMAKE_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# User options
set(CARTEX_FLOAT "float" CACHE STRING "Floating-point type")
set_property(CACHE CARTEX_FLOAT PROPERTY STRINGS "float" "double")
set(CARTEX_RUNTIME "mpi_comm" CACHE STRING "Runtime")
set_property(CACHE CARTEX_RUNTIME PROPERTY STRINGS "mpi_comm" "gcl_comm" "ghex_comm" "oomph_comm"
    "tausch_comm")

if (GHEXBENCH_ENABLE_DEVICE)
    if (GHEXBENCH_DEVICE_TYPE STREQUAL "NVIDIA")
        set(CARTEX_ON_GPU "ON")
    else()
        set(CARTEX_ON_GPU "OFF")
    endif()
else()
    set(CARTEX_ON_GPU "OFF")
endif()

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

#set(c_cxx_lang "$<COMPILE_LANGUAGE:C,CXX>")
#set(c_cxx_lang_clang "$<COMPILE_LANG_AND_ID:CXX,Clang>")
set(cuda_lang "$<COMPILE_LANGUAGE:CUDA>")

function(cartex_target_compile_options target)
    ghexbench_target_compile_options(${target})
    target_compile_options(${target} PRIVATE
    # flags for CXX builds
    #$<${c_cxx_lang}:$<BUILD_INTERFACE:-Wall -Wextra -Wpedantic -Wno-unknown-pragmas>>
    #$<${c_cxx_lang_clang}:$<BUILD_INTERFACE:-Wno-c++17-extensions -Wno-unused-lambda-capture>>
    # flags for CUDA builds
    $<${cuda_lang}:$<BUILD_INTERFACE:-Xcompiler=-Wall -Wextra -Wno-unknown-pragmas --default-stream per-thread>>)
endfunction()

function(compile_as_cuda)
  if(_cartex_cuda_enabled)
    set_source_files_properties(${ARGN} PROPERTIES LANGUAGE CUDA)
  endif()
endfunction()

add_library(cartex INTERFACE)
add_library(CartEx::cartex ALIAS cartex)
message(${PROJECT_SOURCE_DIR}/include/)
message(${CMAKE_CURRENT_LIST_DIR}/include)
message(${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories(cartex INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(cartex INTERFACE ghexbench)
target_compile_definitions(cartex INTERFACE
# CARTEX_RUNTIME=${CARTEX_RUNTIME}
# CARTEX_RUNTIME_$<UPPER_CASE:${CARTEX_RUNTIME}>
  CARTEX_FLOAT=${CARTEX_FLOAT}
)
#set_target_properties(
#  cartex
#  PROPERTIES
#    INTERFACE_POSITION_INDEPENDENT_CODE ON)

#message("config location")
#message(${CMAKE_CURRENT_BINARY_DIR
#message(${CMAKE_BINARY_DIR}/include/cmake_config.inc)
#message(${CMAKE_CURRENT_BINARY_DIR}/../../include/cmake_config.inc)
#message(${CMAKE_PROJECT_BINARY_DIR}/include/cmake_config.inc)
#configure_file(${PROJECT_SOURCE_DIR}/cmake/cmake_config.inc.in 
#    #${CMAKE_BINARY_DIR}/include/cmake_config.inc)
#    ${CMAKE_CURRENT_BINARY_DIR}/../../include/cmake_config.inc)
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/cmake_config.inc.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/cmake_config.inc)
add_subdirectory(src)

