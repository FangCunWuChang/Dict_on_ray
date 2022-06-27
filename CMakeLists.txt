cmake_minimum_required (VERSION 3.15)

project (Dict_on_ray VERSION 1.0.0 LANGUAGES CXX C)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release"  CACHE STRING "Choose the type of build." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

set (CMAKE_CXX_STANDARD 20)#使用C++20
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_C_STANDARD 11)#使用C11
set (CMAKE_C_EXTENSIONS OFF)
set (CMAKE_C_STANDARD_REQUIRED ON)

set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")#MSVC下使用/MD和/MDd

#配置目录
set (OUT_DIR "${CMAKE_SOURCE_DIR}/out")

#子项目目录
set (LOGC_DIR "${CMAKE_SOURCE_DIR}/log.c/src")
set (JUCE_DIR "${CMAKE_SOURCE_DIR}/JUCE/modules")

#编译目标
add_subdirectory (server)
add_subdirectory (client)
add_subdirectory (dictc)
