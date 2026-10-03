#include <cmath>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

#include "lumos/math/lin_alg/fixed_size_vector/fixed_size_vector.h"
#include "lumos/math/lin_alg/fixed_size_vector/fixed_size_vector_io.h"

namespace lumos
{

  // Test fixture for FixedSizeVector tests
  class FixedSizeVectorTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Setup common test data
      vec3_a = FixedSizeVector<double, 3>{1.0, 2.0, 3.0};
      vec3_b = FixedSizeVector<double, 3>{4.0, 5.0, 6.0};
      vec3_zero = FixedSizeVector<double, 3>{0.0, 0.0, 0.0};

      vec2_a = FixedSizeVector<float, 2>{1.0f, 2.0f};
      vec2_b = FixedSizeVector<float, 2>{3.0f, 4.0f};

      vec4_int = FixedSizeVector<int, 4>{1, 2, 3, 4};
      vec4_ones = FixedSizeVector<int, 4>{1, 1, 1, 1};
    }

    FixedSizeVector<double, 3> vec3_a, vec3_b, vec3_zero;
    FixedSizeVector<float, 2> vec2_a, vec2_b;
    FixedSizeVector<int, 4> vec4_int, vec4_ones;
  };

  // CONSTRUCTOR TESTS

  TEST_F(FixedSizeVectorTest, DefaultConstructor)
  {
    FixedSizeVector<double, 3> vec;
    // Default constructor should leave values uninitialized
    // We can't test the actual values as they are undefined
    // But we can test that the vector exists and can be used
    vec[0] = 1.0;
    vec[1] = 2.0;
    vec[2] = 3.0;

    EXPECT_DOUBLE_EQ(vec[0], 1.0);
    EXPECT_DOUBLE_EQ(vec[1], 2.0);
    EXPECT_DOUBLE_EQ(vec[2], 3.0);
  }

  TEST_F(FixedSizeVectorTest, InitializerListConstructor)
  {
    FixedSizeVector<double, 3> vec{1.0, 2.0, 3.0};

    EXPECT_DOUBLE_EQ(vec[0], 1.0);
    EXPECT_DOUBLE_EQ(vec[1], 2.0);
    EXPECT_DOUBLE_EQ(vec[2], 3.0);
  }

  TEST_F(FixedSizeVectorTest, InitializerListConstructorPartial)
  {
    FixedSizeVector<double, 3> vec{1.0, 2.0};

    EXPECT_DOUBLE_EQ(vec[0], 1.0);
    EXPECT_DOUBLE_EQ(vec[1], 2.0);
    // vec[2] is uninitialized, so we won't test it
  }

  TEST_F(FixedSizeVectorTest, InitializerListConstructorTooMany)
  {
    // Should only take first N elements
    FixedSizeVector<double, 2> vec{1.0, 2.0, 3.0, 4.0, 5.0};

    EXPECT_DOUBLE_EQ(vec[0], 1.0);
    EXPECT_DOUBLE_EQ(vec[1], 2.0);
    // Elements 3.0, 4.0, 5.0 should be ignored
  }

  TEST_F(FixedSizeVectorTest, InitializerListConstructorEmpty)
  {
    FixedSizeVector<double, 3> vec{};
    // All elements should be uninitialized
    // We can't test the actual values, but we can test assignment
    vec[0] = 0.0;
    vec[1] = 0.0;
    vec[2] = 0.0;

    EXPECT_DOUBLE_EQ(vec[0], 0.0);
    EXPECT_DOUBLE_EQ(vec[1], 0.0);
    EXPECT_DOUBLE_EQ(vec[2], 0.0);
  }

  // OPERATOR[] TESTS

  TEST_F(FixedSizeVectorTest, SubscriptOperator)
  {
    EXPECT_DOUBLE_EQ(vec3_a[0], 1.0);
    EXPECT_DOUBLE_EQ(vec3_a[1], 2.0);
    EXPECT_DOUBLE_EQ(vec3_a[2], 3.0);
  }

  TEST_F(FixedSizeVectorTest, SubscriptOperatorConst)
  {
    const FixedSizeVector<double, 3> vec{1.0, 2.0, 3.0};

    EXPECT_DOUBLE_EQ(vec[0], 1.0);
    EXPECT_DOUBLE_EQ(vec[1], 2.0);
    EXPECT_DOUBLE_EQ(vec[2], 3.0);
  }

  TEST_F(FixedSizeVectorTest, SubscriptOperatorModification)
  {
    FixedSizeVector<double, 3> vec{1.0, 2.0, 3.0};

    vec[0] = 10.0;
    vec[1] = 20.0;
    vec[2] = 30.0;

    EXPECT_DOUBLE_EQ(vec[0], 10.0);
    EXPECT_DOUBLE_EQ(vec[1], 20.0);
    EXPECT_DOUBLE_EQ(vec[2], 30.0);
  }

  // ARITHMETIC OPERATOR TESTS

  TEST_F(FixedSizeVectorTest, AdditionOperator)
  {
    FixedSizeVector<double, 3> result = vec3_a + vec3_b;

    EXPECT_DOUBLE_EQ(result[0], 5.0); // 1.0 + 4.0
    EXPECT_DOUBLE_EQ(result[1], 7.0); // 2.0 + 5.0
    EXPECT_DOUBLE_EQ(result[2], 9.0); // 3.0 + 6.0
  }

  TEST_F(FixedSizeVectorTest, AdditionWithZero)
  {
    FixedSizeVector<double, 3> result = vec3_a + vec3_zero;

    EXPECT_DOUBLE_EQ(result[0], 1.0);
    EXPECT_DOUBLE_EQ(result[1], 2.0);
    EXPECT_DOUBLE_EQ(result[2], 3.0);
  }

  TEST_F(FixedSizeVectorTest, SubtractionOperator)
  {
    FixedSizeVector<double, 3> result = vec3_b - vec3_a;

    EXPECT_DOUBLE_EQ(result[0], 3.0); // 4.0 - 1.0
    EXPECT_DOUBLE_EQ(result[1], 3.0); // 5.0 - 2.0
    EXPECT_DOUBLE_EQ(result[2], 3.0); // 6.0 - 3.0
  }

  TEST_F(FixedSizeVectorTest, SubtractionWithZero)
  {
    FixedSizeVector<double, 3> result = vec3_a - vec3_zero;

    EXPECT_DOUBLE_EQ(result[0], 1.0);
    EXPECT_DOUBLE_EQ(result[1], 2.0);
    EXPECT_DOUBLE_EQ(result[2], 3.0);
  }

  TEST_F(FixedSizeVectorTest, SubtractionFromZero)
  {
    FixedSizeVector<double, 3> result = vec3_zero - vec3_a;

    EXPECT_DOUBLE_EQ(result[0], -1.0);
    EXPECT_DOUBLE_EQ(result[1], -2.0);
    EXPECT_DOUBLE_EQ(result[2], -3.0);
  }

  TEST_F(FixedSizeVectorTest, ScalarMultiplication)
  {
    FixedSizeVector<double, 3> result = vec3_a * 2.0;

    EXPECT_DOUBLE_EQ(result[0], 2.0); // 1.0 * 2.0
    EXPECT_DOUBLE_EQ(result[1], 4.0); // 2.0 * 2.0
    EXPECT_DOUBLE_EQ(result[2], 6.0); // 3.0 * 2.0
  }

  TEST_F(FixedSizeVectorTest, ScalarMultiplicationByZero)
  {
    FixedSizeVector<double, 3> result = vec3_a * 0.0;

    EXPECT_DOUBLE_EQ(result[0], 0.0);
    EXPECT_DOUBLE_EQ(result[1], 0.0);
    EXPECT_DOUBLE_EQ(result[2], 0.0);
  }

  TEST_F(FixedSizeVectorTest, ScalarMultiplicationByNegative)
  {
    FixedSizeVector<double, 3> result = vec3_a * (-1.0);

    EXPECT_DOUBLE_EQ(result[0], -1.0);
    EXPECT_DOUBLE_EQ(result[1], -2.0);
    EXPECT_DOUBLE_EQ(result[2], -3.0);
  }

  // DOT PRODUCT TESTS

  TEST_F(FixedSizeVectorTest, DotProduct)
  {
    double result = vec3_a.dot(vec3_b);

    // (1.0*4.0) + (2.0*5.0) + (3.0*6.0) = 4 + 10 + 18 = 32
    EXPECT_DOUBLE_EQ(result, 32.0);
  }

  TEST_F(FixedSizeVectorTest, DotProductWithZero)
  {
    double result = vec3_a.dot(vec3_zero);

    EXPECT_DOUBLE_EQ(result, 0.0);
  }

  TEST_F(FixedSizeVectorTest, DotProductWithItself)
  {
    double result = vec3_a.dot(vec3_a);

    // 1.0*1.0 + 2.0*2.0 + 3.0*3.0 = 1 + 4 + 9 = 14
    EXPECT_DOUBLE_EQ(result, 14.0);
  }

  TEST_F(FixedSizeVectorTest, DotProductCommutative)
  {
    double result1 = vec3_a.dot(vec3_b);
    double result2 = vec3_b.dot(vec3_a);

    EXPECT_DOUBLE_EQ(result1, result2);
  }

  TEST_F(FixedSizeVectorTest, DotProductFloat)
  {
    float result = vec2_a.dot(vec2_b);

    // (1.0*3.0) + (2.0*4.0) = 3 + 8 = 11
    EXPECT_FLOAT_EQ(result, 11.0f);
  }

  TEST_F(FixedSizeVectorTest, DotProductInteger)
  {
    int result = vec4_int.dot(vec4_ones);

    // (1*1) + (2*1) + (3*1) + (4*1) = 1 + 2 + 3 + 4 = 10
    EXPECT_EQ(result, 10);
  }

  // NORM TESTS

  TEST_F(FixedSizeVectorTest, NormBasic)
  {
    FixedSizeVector<double, 3> vec{3.0, 4.0, 0.0};
    double result = vec.norm();

    // sqrt(3^2 + 4^2 + 0^2) = sqrt(9 + 16 + 0) = sqrt(25) = 5
    EXPECT_DOUBLE_EQ(result, 5.0);
  }

  TEST_F(FixedSizeVectorTest, NormZeroVector)
  {
    double result = vec3_zero.norm();

    EXPECT_DOUBLE_EQ(result, 0.0);
  }

  TEST_F(FixedSizeVectorTest, NormUnitVector)
  {
    FixedSizeVector<double, 3> unit{1.0, 0.0, 0.0};
    double result = unit.norm();

    EXPECT_DOUBLE_EQ(result, 1.0);
  }

  TEST_F(FixedSizeVectorTest, NormVec3A)
  {
    double result = vec3_a.norm();

    // sqrt(1^2 + 2^2 + 3^2) = sqrt(1 + 4 + 9) = sqrt(14)
    EXPECT_DOUBLE_EQ(result, std::sqrt(14.0));
  }

  TEST_F(FixedSizeVectorTest, Norm2D)
  {
    float result = vec2_a.norm();

    // sqrt(1^2 + 2^2) = sqrt(1 + 4) = sqrt(5)
    EXPECT_FLOAT_EQ(result, std::sqrt(5.0f));
  }

  TEST_F(FixedSizeVectorTest, NormInteger)
  {
    // Create a 3-4-5 triangle with integers
    FixedSizeVector<int, 2> vec{3, 4};
    int result = vec.norm();

    // sqrt(3^2 + 4^2) = sqrt(9 + 16) = sqrt(25) = 5
    EXPECT_EQ(result, 5);
  }

  // STREAM OPERATOR TESTS

  TEST_F(FixedSizeVectorTest, StreamOperatorOutput)
  {
    std::ostringstream oss;
    oss << vec3_a;

    EXPECT_EQ(oss.str(), "[1, 2, 3]");
  }

  TEST_F(FixedSizeVectorTest, StreamOperatorZero)
  {
    std::ostringstream oss;
    oss << vec3_zero;

    EXPECT_EQ(oss.str(), "[0, 0, 0]");
  }

  TEST_F(FixedSizeVectorTest, StreamOperator2D)
  {
    std::ostringstream oss;
    oss << vec2_a;

    EXPECT_EQ(oss.str(), "[1, 2]");
  }

  TEST_F(FixedSizeVectorTest, StreamOperator4D)
  {
    std::ostringstream oss;
    oss << vec4_int;

    EXPECT_EQ(oss.str(), "[1, 2, 3, 4]");
  }

  TEST_F(FixedSizeVectorTest, StreamOperatorSingleElement)
  {
    FixedSizeVector<double, 1> vec{42.0};
    std::ostringstream oss;
    oss << vec;

    EXPECT_EQ(oss.str(), "[42]");
  }

  // TEMPLATE SPECIALIZATION TESTS

  TEST_F(FixedSizeVectorTest, DifferentTypes)
  {
    FixedSizeVector<int, 3> int_vec{1, 2, 3};
    FixedSizeVector<float, 3> float_vec{1.5f, 2.5f, 3.5f};
    FixedSizeVector<double, 3> double_vec{1.1, 2.2, 3.3};

    // Test that different types work correctly
    EXPECT_EQ(int_vec[0], 1);
    EXPECT_FLOAT_EQ(float_vec[0], 1.5f);
    EXPECT_DOUBLE_EQ(double_vec[0], 1.1);
  }

  TEST_F(FixedSizeVectorTest, DifferentSizes)
  {
    FixedSizeVector<double, 1> vec1{1.0};
    FixedSizeVector<double, 2> vec2{1.0, 2.0};
    FixedSizeVector<double, 5> vec5{1.0, 2.0, 3.0, 4.0, 5.0};

    EXPECT_DOUBLE_EQ(vec1[0], 1.0);
    EXPECT_DOUBLE_EQ(vec2[1], 2.0);
    EXPECT_DOUBLE_EQ(vec5[4], 5.0);
  }

  // BASIC FUNCTIONALITY TESTS

  TEST_F(FixedSizeVectorTest, BasicConstructor)
  {
    FixedSizeVector<int, 3> vec{1, 2, 3};

    // Test that basic initialization works
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
  }

  TEST_F(FixedSizeVectorTest, BasicOperations)
  {
    FixedSizeVector<int, 3> vec1{1, 2, 3};
    FixedSizeVector<int, 3> vec2{4, 5, 6};

    auto sum = vec1 + vec2;
    auto diff = vec2 - vec1;
    auto scaled = vec1 * 2;
    auto dot_result = vec1.dot(vec2);

    EXPECT_EQ(sum[0], 5);
    EXPECT_EQ(diff[0], 3);
    EXPECT_EQ(scaled[0], 2);
    EXPECT_EQ(dot_result, 32); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
  }

  // EDGE CASE TESTS

  TEST_F(FixedSizeVectorTest, LargeSize)
  {
    FixedSizeVector<double, 100> large_vec{};

    // Initialize first few elements
    large_vec[0] = 1.0;
    large_vec[50] = 2.0;
    large_vec[99] = 3.0;

    EXPECT_DOUBLE_EQ(large_vec[0], 1.0);
    EXPECT_DOUBLE_EQ(large_vec[50], 2.0);
    EXPECT_DOUBLE_EQ(large_vec[99], 3.0);
  }

  TEST_F(FixedSizeVectorTest, ChainedOperations)
  {
    FixedSizeVector<double, 3> result = (vec3_a + vec3_b) * 2.0 - vec3_a;

    // vec3_a + vec3_b = [5, 7, 9]
    // * 2.0 = [10, 14, 18]
    // - vec3_a = [10-1, 14-2, 18-3] = [9, 12, 15]

    EXPECT_DOUBLE_EQ(result[0], 9.0);
    EXPECT_DOUBLE_EQ(result[1], 12.0);
    EXPECT_DOUBLE_EQ(result[2], 15.0);
  }

  TEST_F(FixedSizeVectorTest, ScalarMultiplicationPrecision)
  {
    FixedSizeVector<double, 2> vec{0.1, 0.2};
    FixedSizeVector<double, 2> result = vec * 10.0;

    EXPECT_NEAR(result[0], 1.0, 1e-10);
    EXPECT_NEAR(result[1], 2.0, 1e-10);
  }

  TEST_F(FixedSizeVectorTest, DotProductPrecision)
  {
    FixedSizeVector<double, 2> vec1{0.1, 0.2};
    FixedSizeVector<double, 2> vec2{0.3, 0.4};

    double result = vec1.dot(vec2);
    double expected = 0.1 * 0.3 + 0.2 * 0.4; // 0.03 + 0.08 = 0.11

    EXPECT_NEAR(result, expected, 1e-10);
  }

  // MATHEMATICAL PROPERTIES TESTS

  TEST_F(FixedSizeVectorTest, DotProductDistributive)
  {
    // Test that a.dot(b + c) == a.dot(b) + a.dot(c)
    FixedSizeVector<double, 3> c{7.0, 8.0, 9.0};

    double left_side = vec3_a.dot(vec3_b + c);
    double right_side = vec3_a.dot(vec3_b) + vec3_a.dot(c);

    EXPECT_NEAR(left_side, right_side, 1e-10);
  }

  TEST_F(FixedSizeVectorTest, ScalarMultiplicationDistributive)
  {
    // Test that (a + b) * c == a * c + b * c
    double scalar = 3.0;

    FixedSizeVector<double, 3> left_side = (vec3_a + vec3_b) * scalar;
    FixedSizeVector<double, 3> right_side = vec3_a * scalar + vec3_b * scalar;

    EXPECT_NEAR(left_side[0], right_side[0], 1e-10);
    EXPECT_NEAR(left_side[1], right_side[1], 1e-10);
    EXPECT_NEAR(left_side[2], right_side[2], 1e-10);
  }

  TEST_F(FixedSizeVectorTest, AdditionCommutative)
  {
    // Test that a + b == b + a
    FixedSizeVector<double, 3> result1 = vec3_a + vec3_b;
    FixedSizeVector<double, 3> result2 = vec3_b + vec3_a;

    EXPECT_DOUBLE_EQ(result1[0], result2[0]);
    EXPECT_DOUBLE_EQ(result1[1], result2[1]);
    EXPECT_DOUBLE_EQ(result1[2], result2[2]);
  }

  TEST_F(FixedSizeVectorTest, AdditionAssociative)
  {
    // Test that (a + b) + c == a + (b + c)
    FixedSizeVector<double, 3> c{7.0, 8.0, 9.0};

    FixedSizeVector<double, 3> result1 = (vec3_a + vec3_b) + c;
    FixedSizeVector<double, 3> result2 = vec3_a + (vec3_b + c);

    EXPECT_NEAR(result1[0], result2[0], 1e-10);
    EXPECT_NEAR(result1[1], result2[1], 1e-10);
    EXPECT_NEAR(result1[2], result2[2], 1e-10);
  }

  TEST_F(FixedSizeVectorTest, NormSquaredEqualsDotProduct)
  {
    // Test that ||v||^2 == v.dot(v)
    double norm_squared = vec3_a.norm() * vec3_a.norm();
    double dot_product = vec3_a.dot(vec3_a);

    EXPECT_NEAR(norm_squared, dot_product, 1e-10);
  }

  TEST_F(FixedSizeVectorTest, CauchySchwarzInequality)
  {
    // Test that |a.dot(b)| <= ||a|| * ||b||
    double dot_product = std::abs(vec3_a.dot(vec3_b));
    double norm_product = vec3_a.norm() * vec3_b.norm();

    EXPECT_LE(dot_product,
              norm_product + 1e-10); // Add small epsilon for floating point
  }

  // PERFORMANCE/STRESS TESTS

  TEST_F(FixedSizeVectorTest, ManyOperations)
  {
    FixedSizeVector<double, 3> result = vec3_a;

    // Perform many operations to test stability
    for (int i = 0; i < 1000; ++i)
    {
      result = result + vec3_zero;
      result = result - vec3_zero;
      result = result * 1.0;
    }

    // Should be unchanged
    EXPECT_NEAR(result[0], vec3_a[0], 1e-10);
    EXPECT_NEAR(result[1], vec3_a[1], 1e-10);
    EXPECT_NEAR(result[2], vec3_a[2], 1e-10);
  }

  // INTEGRATION TESTS

  TEST_F(FixedSizeVectorTest, TypeAliasesIntegration)
  {
    // Test that the class works with different template parameters
    FixedSizeVector<float, 2> vec2f{1.0f, 2.0f};
    FixedSizeVector<double, 3> vec3d{1.0, 2.0, 3.0};
    FixedSizeVector<int, 4> vec4i{1, 2, 3, 4};

    // Basic operations should work
    auto sum2f = vec2f + vec2f;
    auto sum3d = vec3d + vec3d;
    auto sum4i = vec4i + vec4i;

    EXPECT_FLOAT_EQ(sum2f[0], 2.0f);
    EXPECT_DOUBLE_EQ(sum3d[0], 2.0);
    EXPECT_EQ(sum4i[0], 2);
  }

} // namespace lumos
