# Test executable for matrix_fixed module
add_executable(matrix_fixed_test matrix_fixed_test.cpp qr_decomposition_test.cpp matrix_fixed_operators_test.cpp)

# Link with Google Test libraries
target_link_libraries(matrix_fixed_test ${GTEST_LIB_FILES})

# Include directories for the tests
target_include_directories(matrix_fixed_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the tests to CTest
add_test(NAME MatrixFixedTest COMMAND matrix_fixed_test)
