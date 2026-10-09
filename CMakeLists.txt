cmake_minimum_required(VERSION 3.24)

project(mip
    VERSION 0.1.0
    DESCRIPTION "Matt's Image Processing Library"
    LANGUAGES CXX
)

# --------------------------------------------------
# Build options
# --------------------------------------------------

option(MIP_BUILD_EXAMPLES
    "Build mip example programs"
    ON
)

option(MIP_BUILD_TESTS
    "Build mip unit tests"
    ON
)

option(MIP_ENABLE_CUDA
    "Build CUDA implementations"
    OFF
)

# --------------------------------------------------
# C++ standard
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# Default build type
# --------------------------------------------------

# Set a default for single-configuration generators.
# Multi-configuration generators (e.g., Visual Studio) select
# their build type when building instead.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)"
        FORCE
    )
endif()

# --------------------------------------------------
# Core library
# --------------------------------------------------

add_library(mip
    src/core/image.cpp
    include/mip/image.hpp
    include/mip/image.tpp
)

add_library(mip::mip ALIAS mip)

target_include_directories(mip
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(mip PUBLIC cxx_std_17)

# --------------------------------------------------
# Compiler warnings
# --------------------------------------------------

if(MSVC)
    target_compile_options(mip PRIVATE /W4)
else()
    target_compile_options(mip PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
    )
endif()

# --------------------------------------------------
# Optional CUDA support
# --------------------------------------------------

if(MIP_ENABLE_CUDA)
    message(FATAL_ERROR
        "CUDA support has not been implemented yet. "
        "Set MIP_ENABLE_CUDA=OFF for the current build."
    )
endif()

# --------------------------------------------------
# Examples
# --------------------------------------------------

if(MIP_BUILD_EXAMPLES)
    add_executable(mip_image_example
        examples/image_example.cpp
    )

    target_link_libraries(mip_image_example
        PRIVATE mip::mip
    )
endif()

# --------------------------------------------------
# Tests
# --------------------------------------------------

# if(MIP_BUILD_TESTS)
#     enable_testing()
#     add_subdirectory(tests)
# endif()