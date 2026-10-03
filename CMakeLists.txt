# Minimum CMake version required
cmake_minimum_required(VERSION 3.14)

# Project name and languages
project(PhysicsSim C CXX)

# Set the C++ standard (adjust according to your needs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optionally enable debugging support
set(CMAKE_BUILD_TYPE Debug)

# Optionally define a version
set(PROJECT_VERSION "1.0.0")

# OpenCV
# set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/third_party/opencv/build/" CACHE PATH "Path to local OpenCVConfig.cmake")
# find_package(OpenCV REQUIRED)
# message(STATUS "OpenCV version: ${OpenCV_VERSION}")
# message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")

# Googletest
link_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/lib)
include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

set(GTEST_LIB_FILES libgtest.a
                    libgtest_main.a
                    pthread)

# Nlohmann
include_directories(${CMAKE_SOURCE_DIR}/external/nlohmann/include)

add_compile_options(-Wall -Wextra -pedantic)

# The math headers default to a small assert with no OS or stream dependencies,
# so that they build for embedded targets without any defines. On the desktop
# use the assert from lumos/logging.h instead, which prints the message
add_compile_definitions(LUMOS_ASSERT_USE_LOGGING)

# STB
set(STB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/stb)

# Include
include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${STB_SOURCE_DIR})

# Enable testing
enable_testing()

add_subdirectory(applications/simple)
add_subdirectory(applications/image)
add_subdirectory(src/lumos/math/curves/test)
add_subdirectory(src/lumos/math/lin_alg/fixed_size_vector/test)
add_subdirectory(src/lumos/math/lin_alg/matrix_fixed/test)
add_subdirectory(src/lumos/math/lin_alg/matrix_dynamic/test)
add_subdirectory(src/lumos/math/filters/test)
add_subdirectory(src/lumos/math/geometry/test)
add_subdirectory(src/lumos/math/transformations/test)
add_subdirectory(src/lumos/math/estimation/test)
add_subdirectory(src/lumos/math/misc/test)
add_subdirectory(src/lumos/math/fft/test)
add_subdirectory(src/lumos/test_reader)
add_subdirectory(src/lumos/binary_io/test)
add_subdirectory(src/lumos/json/test)
add_subdirectory(src/lumos/argparse/test)
add_subdirectory(src/lumos/plotting/test)
