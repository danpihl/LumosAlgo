#include <cmath>
#include <gtest/gtest.h>

#include "math/math.h"

namespace lumos {

class FixedSizeMatrixTest : public ::testing::Test {
protected:
  void SetUp() override {
    // m0 = [1, 2, 3; 4, 5, 6], m1 = [6, 5, 4; 3, 2, 1]
    double value = 1.0;
    for (size_t r = 0; r < 2; r++) {
      for (size_t c = 0; c < 3; c++) {
        m0(r, c) = value;
        m1(r, c) = 7.0 - value;
        value += 1.0;
      }
    }
  }

  FixedSizeMatrix<double, 2, 3> m0, m1;
};

template <uint16_t R, uint16_t C>
void expectAllEqual(const FixedSizeMatrix<double, R, C> &m,
                    const std::vector<double> &expected) {
  ASSERT_EQ(expected.size(), static_cast<size_t>(R * C));
  for (size_t r = 0; r < R; r++) {
    for (size_t c = 0; c < C; c++) {
      EXPECT_DOUBLE_EQ(m(r, c), expected[r * C + c])
          << "at (" << r << ", " << c << ")";
    }
  }
}

TEST_F(FixedSizeMatrixTest, MatrixAdditionAndSubtraction) {
  expectAllEqual(m0 + m1, {7.0, 7.0, 7.0, 7.0, 7.0, 7.0});
  expectAllEqual(m0 - m1, {-5.0, -3.0, -1.0, 1.0, 3.0, 5.0});
}

TEST_F(FixedSizeMatrixTest, ScalarMultiplicationAndDivision) {
  expectAllEqual(m0 * 2.0, {2.0, 4.0, 6.0, 8.0, 10.0, 12.0});
  expectAllEqual(2.0 * m0, {2.0, 4.0, 6.0, 8.0, 10.0, 12.0});
  expectAllEqual(m0 / 2.0, {0.5, 1.0, 1.5, 2.0, 2.5, 3.0});
  expectAllEqual(60.0 / m0, {60.0, 30.0, 20.0, 15.0, 12.0, 10.0});
}

TEST_F(FixedSizeMatrixTest, ScalarAdditionAndSubtraction) {
  expectAllEqual(m0 + 1.0, {2.0, 3.0, 4.0, 5.0, 6.0, 7.0});
  expectAllEqual(1.0 + m0, {2.0, 3.0, 4.0, 5.0, 6.0, 7.0});
  expectAllEqual(m0 - 1.0, {0.0, 1.0, 2.0, 3.0, 4.0, 5.0});
  expectAllEqual(1.0 - m0, {0.0, -1.0, -2.0, -3.0, -4.0, -5.0});
}

TEST_F(FixedSizeMatrixTest, ElementWiseOperators) {
  expectAllEqual(m0 ^ m1, {6.0, 10.0, 12.0, 12.0, 10.0, 6.0});
  expectAllEqual(m0 / m1, {1.0 / 6.0, 2.0 / 5.0, 3.0 / 4.0, 4.0 / 3.0,
                           5.0 / 2.0, 6.0});
}

TEST_F(FixedSizeMatrixTest, TransposedSwapsDimensions) {
  const FixedSizeMatrix<double, 3, 2> mt = m0.transposed();
  expectAllEqual(mt, {1.0, 4.0, 2.0, 5.0, 3.0, 6.0});
  EXPECT_TRUE(mt.transposed() == m0);
}

TEST_F(FixedSizeMatrixTest, Negation) {
  expectAllEqual(-m0, {-1.0, -2.0, -3.0, -4.0, -5.0, -6.0});
}

TEST_F(FixedSizeMatrixTest, Equality) {
  FixedSizeMatrix<double, 2, 3> m0_copy = m0;
  EXPECT_TRUE(m0 == m0_copy);
  EXPECT_FALSE(m0 != m0_copy);
  EXPECT_FALSE(m0 == m1);
  EXPECT_TRUE(m0 != m1);

  m0_copy(1, 2) += 1.0;
  EXPECT_FALSE(m0 == m0_copy);
}

TEST_F(FixedSizeMatrixTest, OperatorsCombineWithMatrixProduct) {
  // (A + B) * C == A * C + B * C
  FixedSizeMatrix<double, 3, 2> m2 = m0.transposed();
  EXPECT_TRUE(((m0 + m1) * m2) == (m0 * m2 + m1 * m2));
  // Identity: I + 2 * (R - I) for a 3x3 matrix
  const Matrix3x3<double> identity = unitFixedSizeMatrix<double, 3, 3>();
  const Matrix3x3<double> r = fixedRotationMatrixZ(0.3);
  const Matrix3x3<double> combined = identity + 2.0 * (r - identity);
  EXPECT_DOUBLE_EQ(combined(0, 0), 2.0 * std::cos(0.3) - 1.0);
  EXPECT_DOUBLE_EQ(combined(1, 0), 2.0 * std::sin(0.3));
  EXPECT_DOUBLE_EQ(combined(2, 2), 1.0);
}

TEST_F(FixedSizeMatrixTest, SameOperatorsAsDynamicMatrix) {
  // Every expression gives the same result for the fixed and dynamic types
  const Matrix<double> d0 = toDynamicMatrix(m0);
  const Matrix<double> d1 = toDynamicMatrix(m1);

  const auto expectSame = [](const FixedSizeMatrix<double, 2, 3> &fixed,
                             const Matrix<double> &dynamic) {
    ASSERT_EQ(dynamic.numRows(), 2u);
    ASSERT_EQ(dynamic.numCols(), 3u);
    for (size_t r = 0; r < 2; r++) {
      for (size_t c = 0; c < 3; c++) {
        EXPECT_DOUBLE_EQ(fixed(r, c), dynamic(r, c));
      }
    }
  };

  expectSame(m0 + m1, d0 + d1);
  expectSame(m0 - m1, d0 - d1);
  expectSame(m0 * 3.0, d0 * 3.0);
  expectSame(3.0 * m0, 3.0 * d0);
  expectSame(m0 ^ m1, d0 ^ d1);
  expectSame(m0 / m1, d0 / d1);
  expectSame(m0 / 3.0, d0 / 3.0);
  expectSame(3.0 / m0, 3.0 / d0);
  expectSame(m0 - 3.0, d0 - 3.0);
  expectSame(3.0 - m0, 3.0 - d0);
  expectSame(m0 + 3.0, d0 + 3.0);
  expectSame(3.0 + m0, 3.0 + d0);
  expectSame(-m0, -d0);
}

TEST_F(FixedSizeMatrixTest, FloatAndIntInstantiation) {
  FixedSizeMatrix<float, 2, 2> mf;
  mf.fill(1.5f);
  const FixedSizeMatrix<float, 2, 2> rf = mf * 2.0f + mf;
  EXPECT_FLOAT_EQ(rf(1, 1), 4.5f);

  FixedSizeMatrix<int, 2, 2> mi;
  mi.fill(3);
  const FixedSizeMatrix<int, 2, 2> ri = (mi + mi) * 2 - 1;
  EXPECT_EQ(ri(0, 1), 11);

  // Conversion between scalar types
  const FixedSizeMatrix<double, 2, 2> md = mf;
  EXPECT_DOUBLE_EQ(md(0, 0), 1.5);
}

// Conversions between fixed size and dynamic types

TEST(ConversionsTest, FixedToDynamicMatrixRoundTrip) {
  FixedSizeMatrix<double, 2, 3> fixed;
  for (size_t r = 0; r < 2; r++) {
    for (size_t c = 0; c < 3; c++) {
      fixed(r, c) = 10.0 * r + c;
    }
  }

  const Matrix<double> dynamic = toDynamicMatrix(fixed);
  ASSERT_EQ(dynamic.numRows(), 2u);
  ASSERT_EQ(dynamic.numCols(), 3u);
  for (size_t r = 0; r < 2; r++) {
    for (size_t c = 0; c < 3; c++) {
      EXPECT_EQ(dynamic(r, c), fixed(r, c));
    }
  }

  const FixedSizeMatrix<double, 2, 3> back = toFixedSizeMatrix<2, 3>(dynamic);
  EXPECT_TRUE(back == fixed);
}

TEST(ConversionsTest, VecToDynamicVectorRoundTrip) {
  const Vec2<double> v2(1.0, 2.0);
  const Vec3<double> v3(1.0, 2.0, 3.0);
  const Vec4<double> v4(1.0, 2.0, 3.0, 4.0);

  const Vector<double> d2 = toDynamicVector(v2);
  const Vector<double> d3 = toDynamicVector(v3);
  const Vector<double> d4 = toDynamicVector(v4);

  ASSERT_EQ(d2.size(), 2u);
  ASSERT_EQ(d3.size(), 3u);
  ASSERT_EQ(d4.size(), 4u);
  EXPECT_EQ(d3(0), 1.0);
  EXPECT_EQ(d3(1), 2.0);
  EXPECT_EQ(d3(2), 3.0);
  EXPECT_EQ(d4(3), 4.0);

  EXPECT_TRUE(toVec2(d2) == v2);
  EXPECT_TRUE(toVec3(d3) == v3);
  EXPECT_TRUE(toVec4(d4) == v4);
}

TEST(ConversionsTest, FixedAndDynamicProductsAgree) {
  // A fixed size rotation applied to a Vec3 matches the dynamic product
  const Matrix3x3<double> r = fixedRotationMatrixX(0.4) *
                              fixedRotationMatrixZ(-1.1);
  const Vec3<double> v(0.3, -1.7, 2.2);

  const Vec3<double> fixed_result = r * v;
  const Vector<double> dynamic_result = toDynamicMatrix(r) * toDynamicVector(v);

  EXPECT_DOUBLE_EQ(fixed_result.x, dynamic_result(0));
  EXPECT_DOUBLE_EQ(fixed_result.y, dynamic_result(1));
  EXPECT_DOUBLE_EQ(fixed_result.z, dynamic_result(2));
}

TEST(ConversionsTest, CrossProductMatrixIsFixedSize) {
  const Vec3<double> a(0.3, -1.7, 2.2);
  const Vec3<double> b(-0.9, 0.4, 1.5);

  const FixedSizeMatrix<double, 3, 3> k = a.toCrossProductMatrix();
  EXPECT_TRUE(k * b == a.crossProduct(b));
  // Skew symmetric
  EXPECT_TRUE(k.transposed() == -k);
}

// The aliases in pre_defs.h name the same types as the templates

TEST(AliasTest, AliasesMatchTemplates) {
  EXPECT_TRUE((std::is_same<Vec3d, Vec3<double>>::value));
  EXPECT_TRUE((std::is_same<Vec3f, Vec3<float>>::value));
  EXPECT_TRUE((std::is_same<Matrixd, Matrix<double>>::value));
  EXPECT_TRUE((std::is_same<AxisAngled, AxisAngle<double>>::value));
  EXPECT_TRUE((std::is_same<AxisAnglef, AxisAngle<float>>::value));
  EXPECT_TRUE((std::is_same<EulerAnglesd, EulerAngles<double>>::value));
  EXPECT_TRUE((std::is_same<Quaterniond, Quaternion<double>>::value));
  EXPECT_TRUE((std::is_same<SE3f, SE3<float>>::value));
  EXPECT_TRUE((std::is_same<IIRFilterd, IIRFilter<double>>::value));
  EXPECT_TRUE(
      (std::is_same<Matrix3x3<double>, FixedSizeMatrix<double, 3, 3>>::value));

  const AxisAngled axis_angle;
  EXPECT_EQ(axis_angle.phi, 0.0);
}

} // namespace lumos
