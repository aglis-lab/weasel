# https://llvm.org/docs/CMake.html#options-and-variables
# INITIALIZE
cmake_minimum_required(VERSION 3.15)
project(weasel VERSION 0.0.1)

# Set LLVM Version
set(LLVM_VERSION 15)

# CMAKE LINKER
set(CMAKE_LINKER ld64.lld)

# LLVM
set(LLVM_DIR /opt/homebrew/opt/llvm@${LLVM_VERSION}/lib/cmake/llvm/)

# Find THREADS
find_package(Threads REQUIRED)

# Find FMT
find_package(fmt REQUIRED)

# Find Google Loging
find_package (glog REQUIRED)

# Find LLVM PACKAGE
find_package(LLVM REQUIRED CONFIG)

# SPIRV
set(SPIRV_DIR ${CMAKE_SOURCE_DIR}/spirv)
set(SPIRV_INCLUDE_DIR ${SPIRV_DIR}/include)
set(SPIRV_LIB_DIR ${SPIRV_DIR}/build/lib/SPIRV)

# SPIRV-CROSS
set(SPIRV_CROSS_DIR ${CMAKE_SOURCE_DIR}/spirv-cross)
set(SPIRV_CROSS_INCLUDE_DIR ${SPIRV_CROSS_DIR})
set(SPIRV_CROSS_LIB_DIR ${SPIRV_CROSS_DIR}/build)

# WEASEL PARALLEL
set(WEASEL_PARALLEL_LIB_DIR ${CMAKE_SOURCE_DIR}/build/libparallel)

# WEASEL CMAKE
set(WEASEL_DIR ${CMAKE_SOURCE_DIR}/cmake)

# CXX
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SET WEASEL LIBS
set(WEASEL_LIBS WeaselLib)

list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})

include(HandleLLVMOptions)
include(AddLLVM)

# CHECL LLVM PACKAGES
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS ${LLVM_INCLUDE_DIRS})
message(STATUS ${LLVM_DEFINITIONS})

# C++ DEFINITIONS
add_definitions("-Wno-reorder-ctor")
add_definitions("-std=c++17")

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # additional warnings
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Sanitize Address
# add_compile_options(-fsanitize=address)

# LLVM DEFINITIONS
add_definitions(${LLVM_DEFINITIONS})

# INCLUDE LLVM LIBRARY
include_directories(${LLVM_INCLUDE_DIRS})

# WEASEL DIRECTORIES
add_subdirectory(lib)
add_subdirectory(libparallel)
add_subdirectory(tools)
add_subdirectory(test)
