# Test executable for the default (embedded) assert in math/misc/assert.h
add_executable(assert_test assert_test.cpp)

# The rest of the build uses the assert from lumos/logging.h. This test is for
# the default one, so undo the definition for this target
target_compile_options(assert_test PRIVATE -ULUMOS_ASSERT_USE_LOGGING)

# Link with Google Test libraries
target_link_libraries(assert_test ${GTEST_LIB_FILES})

# Include directories for the test
target_include_directories(assert_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
)

# Add the test to CTest
add_test(NAME AssertTest COMMAND assert_test)
