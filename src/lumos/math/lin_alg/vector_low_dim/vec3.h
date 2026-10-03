#ifndef LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC3_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC3_H_

#include <cmath>

#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/lin_alg/vector_low_dim/class_def/vec3.h"

namespace lumos
{
  template <typename T>
  Vec3<T>::Vec3(const T x_, const T y_, const T z_) : x{x_}, y{y_}, z{z_} {}

  template <typename T>
  Vec3<T>::Vec3() {}

  template <typename T>
  template <typename Y>
  Vec3<T>::Vec3(const Vec3<Y> &v) : x{static_cast<T>(v.x)},
                                    y{static_cast<T>(v.y)},
                                    z{static_cast<T>(v.z)}
  {
  }

  template <typename T>
  Vec3<T> Vec3<T>::normalized() const
  {
    return Vec3<T>(x, y, z) / static_cast<T>(std::sqrt(x * x + y * y + z * z));
  }

  template <typename T>
  Vec3<T> Vec3<T>::vectorBetweenPoints(const Point3<T> &end_point) const
  {
    // "This" is start point
    return end_point - *this;
  }

  template <typename T>
  Vec3<T>
  Vec3<T>::normalizedVectorBetweenPoints(const Point3<T> &end_point) const
  {
    // "This" is start point
    return (end_point - *this).normalized();
  }

  template <typename T>
  T Vec3<T>::squaredNorm() const
  {
    return x * x + y * y + z * z;
  }

  template <typename T>
  T Vec3<T>::norm() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }

  template <typename T>
  Vec3<T> Vec3<T>::elementWiseMultiply(const Vec3<T> &factor_vector) const
  {
    return Vec3<T>(x * factor_vector.x, y * factor_vector.y, z * factor_vector.z);
  }

  template <typename T>
  Vec3<T> Vec3<T>::elementWiseDivide(const Vec3<T> &denominator_vector) const
  {
    return Vec3<T>(x / denominator_vector.x, y / denominator_vector.y,
                   z / denominator_vector.z);
  }

  template <typename T>
  Vec3<T> Vec3<T>::crossProduct(const Vec3<T> &right_vector) const
  {
    return Vec3<T>(y * right_vector.z - z * right_vector.y,
                   z * right_vector.x - x * right_vector.z,
                   x * right_vector.y - y * right_vector.x);
  }

  // Skew symmetric matrix K such that K * b = (*this) x b
  template <typename T>
  FixedSizeMatrix<T, 3, 3> Vec3<T>::toCrossProductMatrix() const
  {
    FixedSizeMatrix<T, 3, 3> m;
    m(0, 0) = 0;
    m(0, 1) = -z;
    m(0, 2) = y;

    m(1, 0) = z;
    m(1, 1) = 0;
    m(1, 2) = -x;

    m(2, 0) = -y;
    m(2, 1) = x;
    m(2, 2) = 0;
    return m;
  }

  template <typename T>
  T Vec3<T>::angleBetweenVectors(const Vec3<T> &v) const
  {
    T dot_product = (*this) * v;
    return std::acos(dot_product / ((this->norm()) * (v.norm())));
  }

  // Non class functions

  template <typename T>
  bool operator==(const Vec3<T> &v0, const Vec3<T> &v1)
  {
    return (v0.x == v1.x) && (v0.y == v1.y) && (v0.z == v1.z);
  }

  template <typename T>
  bool operator!=(const Vec3<T> &v0, const Vec3<T> &v1)
  {
    return !(v0 == v1);
  }

  template <typename T>
  Vec3<T> operator*(const T f, const Vec3<T> &v)
  {
    return Vec3<T>(f * v.x, f * v.y, f * v.z);
  }

  template <typename T>
  Vec3<T> operator*(const Vec3<T> &v, const T f)
  {
    return Vec3<T>(f * v.x, f * v.y, f * v.z);
  }

  template <typename T>
  T operator*(const Vec3<T> &v0, const Vec3<T> &v1)
  {
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
  }

  template <typename T>
  Vec3<T> operator/(const Vec3<T> &v, const T f)
  {
    return Vec3<T>(v.x / f, v.y / f, v.z / f);
  }

  template <typename T>
  Vec3<T> operator/(const Vec3<T> &v0, const Vec3<T> &v1)
  {
    return Vec3<T>(v0.x / v1.x, v0.y / v1.y, v0.z / v1.z);
  }

  template <typename T>
  Vec3<T> operator+(const Vec3<T> &v0, const Vec3<T> &v1)
  {
    return Vec3<T>(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
  }

  template <typename T>
  Vec3<T> operator-(const Vec3<T> &v0, const Vec3<T> &v1)
  {
    return Vec3<T>(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
  }

  template <typename T>
  Vec3<T> operator*(const Matrix<T> &m, const Vec3<T> &v)
  {
    ASSERT(m.numRows() == 3) << "Matrix dimension mismatch!";
    ASSERT(m.numCols() == 3) << "Matrix dimension mismatch!";
    Vec3<T> res;
    res.x = m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z;
    res.y = m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z;
    res.z = m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z;
    return res;
  }

  template <typename T>
  Vec3<T> operator*(const FixedSizeMatrix<T, 3, 3> &m, const Vec3<T> &v)
  {
    Vec3<T> res;
    res.x = m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z;
    res.y = m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z;
    res.z = m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z;
    return res;
  }

  template <typename T>
  Vec3<T> operator*(const Vec3<T> &v, const Matrix<T> &m)
  {
    ASSERT(m.numRows() == 3) << "Matrix dimension mismatch!";
    ASSERT(m.numCols() == 3) << "Matrix dimension mismatch!";
    Vec3<T> res;
    res.x = v.x * m(0, 0) + v.y * m(1, 0) + v.z * m(2, 0);
    res.y = v.x * m(0, 1) + v.y * m(1, 1) + v.z * m(2, 1);
    res.z = v.x * m(0, 2) + v.y * m(1, 2) + v.z * m(2, 2);
    return res;
  }

  template <typename T>
  Vec3<T> operator+(const Vec3<T> &v, const T f)
  {
    return Vec3<T>(v.x + f, v.y + f, v.z + f);
  }

  template <typename T>
  Vec3<T> operator+(const T f, const Vec3<T> &v)
  {
    return Vec3<T>(v.x + f, v.y + f, v.z + f);
  }

  template <typename T>
  Vec3<T> operator-(const Vec3<T> &v, const T f)
  {
    return Vec3<T>(v.x - f, v.y - f, v.z - f);
  }

  template <typename T>
  Vec3<T> operator-(const T f, const Vec3<T> &v)
  {
    return Vec3<T>(f - v.x, f - v.y, f - v.z);
  }

  template <typename T>
  Vec3<T> operator/(const T f, const Vec3<T> &v)
  {
    return Vec3<T>(f / v.x, f / v.y, f / v.z);
  }

  template <typename T>
  Vec3<T> operator-(const Vec3<T> &v)
  {
    return Vec3<T>(-v.x, -v.y, -v.z);
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const Vec3<T> &v)
  {
    std::string s = "[ " + std::to_string(v.x) + ", " + std::to_string(v.y) +
                    ", " + std::to_string(v.z) + " ]";
    os << s;
    return os;
  }

  template <typename T>
  T angleBetweenVectors(const Vec3<T> v0, const Vec3<T> v1)
  {
    return std::acos((v0 * v1) / (v0.norm() * v1.norm()));
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC3_H_
