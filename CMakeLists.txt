cmake_minimum_required(VERSION 3.24)

set(PROJECT_VERSION "0.1.1")
project(wgpu_native VERSION ${PROJECT_VERSION})

set(CMAKE_C_STANDARD "11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic")

if (WIN32)
    set(LIB_EXTENSION "dll")
    set(LIB_PREFIX "")
    set(OS_NAME "windows")
elseif (APPLE)
    set(LIB_EXTENSION "dylib")
    set(LIB_PREFIX "lib")
    set(OS_NAME "darwin")
else ()
    set(LIB_EXTENSION "so")
    set(LIB_PREFIX "lib")
    set(OS_NAME "linux")
endif ()
set(DAWN_SHARED_LIB ${CMAKE_CURRENT_LIST_DIR}/extern/dawn/lib/${LIB_PREFIX}webgpu_dawn.${LIB_EXTENSION})

include_directories(${CMAKE_CURRENT_LIST_DIR}/extern/dawn/include)
link_directories(${CMAKE_CURRENT_LIST_DIR}/extern/dawn/lib)

set(CMAKE_SKIP_BUILD_RPATH OFF)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

set(CMAKE_INSTALL_RPATH ${CMAKE_CURRENT_LIST_DIR}/extern/dawn/lib)
set(CMAKE_BUILD_RPATH ${CMAKE_CURRENT_LIST_DIR}/extern/dawn/lib)

include (GNUInstallDirs)
set(DAWN_INSTALL_CMAKEDIR
    ${CMAKE_CURRENT_LIST_DIR}/extern/dawn/lib/cmake/Dawn
    CACHE STRING “Path to dawn::webgpu_dawn cmake files”
)
install(FILES
    DawnConfig.cmake
    ${CMAKE_CURRENT_LIST_DIR}/extern/dawn/lib/cmake/DawnConfig.cmake
    DESTINATION ${CMAKE_CURRENT_LIST_DIR}/extern/dawn/lib
)
find_package(Dawn REQUIRED)
message(STATUS "dawn libwebgpu_dawn library found")

add_executable(adapter_info src/adapter_info.c)
target_link_libraries(adapter_info ${DAWN_SHARED_LIB})
