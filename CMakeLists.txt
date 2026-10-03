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

# Googletest
link_directories(${CMAKE_SOURCE_DIR}/src/external/googletest/lib)
include_directories(${CMAKE_SOURCE_DIR}/src/external/googletest/googletest/include)

set(GTEST_LIB_FILES libgtest.a
                    libgtest_main.a
                    pthread)

# Nlohmann
include_directories(${CMAKE_SOURCE_DIR}/external/nlohmann/include)

add_compile_options(-Wall -Wextra -pedantic)

# Include
include_directories(${CMAKE_SOURCE_DIR}/src/)

# Enable testing
enable_testing()

add_subdirectory(src/applications/simple)
add_subdirectory(src/math/curves/test)
add_subdirectory(src/math/lin_alg/fixed_size_vector/test)
add_subdirectory(src/math/lin_alg/matrix_dynamic/test)
add_subdirectory(src/math/lin_alg/matrix_fixed/test)
add_subdirectory(src/math/filters/test)
add_subdirectory(src/math/geometry/test)
add_subdirectory(src/math/transformations/test)
