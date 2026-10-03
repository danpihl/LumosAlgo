# Test executable for transformations module
add_executable(transformations_test transformations_test.cpp)

# Link with Google Test libraries
target_link_libraries(transformations_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(transformations_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME TransformationsTest COMMAND transformations_test)
