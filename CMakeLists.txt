cmake_minimum_required(VERSION 3.13)

set(PROJECT_VERSION "0.1.1")
project(wgpu_native VERSION ${PROJECT_VERSION})

set(CMAKE_C_STANDARD "11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic")

list( APPEND CMAKE_INCLUDE_PATH ${CMAKE_PREFIX_PATH}/include )
list( APPEND CMAKE_LIBRARY_PATH ${CMAKE_PREFIX_PATH}/lib )
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

set(CMAKE_SKIP_BUILD_RPATH OFF)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

set(CMAKE_INSTALL_RPATH ${CMAKE_LIBRARY_PATH})
set(CMAKE_BUILD_RPATH ${CMAKE_LIBRARY_PATH})

find_package(Dawn REQUIRED)
message(STATUS "dawn libwebgpu_dawn library found")

add_executable(adapter_info src/adapter_info.c)
target_link_libraries(adapter_info dawn::webgpu_dawn)
