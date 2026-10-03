# Test executable for estimation module
add_executable(estimation_test attitude_estimator_test.cpp)

# Link with Google Test libraries
target_link_libraries(estimation_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(estimation_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME EstimationTest COMMAND estimation_test)
