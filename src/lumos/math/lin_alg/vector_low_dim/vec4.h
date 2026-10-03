#ifndef LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC4_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC4_H_

#include <cmath>

#include "lumos/math/lin_alg/vector_low_dim/class_def/vec4.h"

namespace lumos
{
  template <typename T>
  Vec4<T>::Vec4(const T x_, const T y_, const T z_, const T w_)
  {
    x = x_;
    y = y_;
    z = z_;
    w = w_;
  }

  template <typename T>
  Vec4<T>::Vec4() {}

  template <typename T>
  template <typename Y>
  Vec4<T>::Vec4(const Vec4<Y> &v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
  }

  template <typename T>
  Vec4<T> Vec4<T>::normalized() const
  {
    return Vec4<T>(x, y, z, w) / std::sqrt(x * x + y * y + z * z + w * w);
  }

  template <typename T>
  T Vec4<T>::squaredNorm() const
  {
    return x * x + y * y + z * z + w * w;
  }

  template <typename T>
  T Vec4<T>::norm() const
  {
    return std::sqrt(x * x + y * y + z * z + w * w);
  }

  template <typename T>
  Vec4<T> Vec4<T>::elementWiseMultiply(const Vec4<T> &factor_vector) const
  {
    return Vec4<T>(x * factor_vector.x, y * factor_vector.y, z * factor_vector.z,
                   w * factor_vector.w);
  }

  template <typename T>
  Vec4<T> Vec4<T>::elementWiseDivide(const Vec4<T> &denominator_vector) const
  {
    return Vec4<T>(x / denominator_vector.x, y / denominator_vector.y,
                   z / denominator_vector.z, w / denominator_vector.w);
  }

  template <typename T>
  Vec4<T> Vec4<T>::vectorBetweenPoints(const Point4<T> &end_point) const
  {
    // "This" is start point
    return end_point - *this;
  }

  template <typename T>
  Vec4<T>
  Vec4<T>::normalizedVectorBetweenPoints(const Point4<T> &end_point) const
  {
    // "This" is start point
    return (end_point - *this).normalized();
  }

  template <typename T>
  T Vec4<T>::angleBetweenVectors(const Vec4<T> &v) const
  {
    T dot_product = (*this) * v;
    return std::acos(dot_product / ((this->norm()) * (v.norm())));
  }

  // Non class functions

  template <typename T>
  bool operator==(const Vec4<T> &v0, const Vec4<T> &v1)
  {
    return (v0.x == v1.x) && (v0.y == v1.y) && (v0.z == v1.z) && (v0.w == v1.w);
  }

  template <typename T>
  bool operator!=(const Vec4<T> &v0, const Vec4<T> &v1)
  {
    return !(v0 == v1);
  }

  template <typename T>
  Vec4<T> operator*(const T f, const Vec4<T> &v)
  {
    return Vec4<T>(f * v.x, f * v.y, f * v.z, f * v.w);
  }

  template <typename T>
  Vec4<T> operator*(const Vec4<T> &v, const T f)
  {
    return Vec4<T>(f * v.x, f * v.y, f * v.z, f * v.w);
  }

  template <typename T>
  T operator*(const Vec4<T> &v0, const Vec4<T> &v1)
  {
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
  }

  template <typename T>
  Vec4<T> operator/(const Vec4<T> &v, const T f)
  {
    return Vec4<T>(v.x / f, v.y / f, v.z / f, v.w / f);
  }

  template <typename T>
  Vec4<T> operator+(const Vec4<T> &v0, const Vec4<T> &v1)
  {
    return Vec4<T>(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w);
  }

  template <typename T>
  Vec4<T> operator-(const Vec4<T> &v0, const Vec4<T> &v1)
  {
    return Vec4<T>(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w);
  }

  template <typename T>
  Vec4<T> operator*(const Matrix<T> &m, const Vec4<T> &v)
  {
    ASSERT(m.numRows() == 4) << "Matrix dimension mismatch!";
    ASSERT(m.numCols() == 4) << "Matrix dimension mismatch!";
    Vec4<T> res;
    res.x = m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z + m(0, 3) * v.w;
    res.y = m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z + m(1, 3) * v.w;
    res.z = m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z + m(2, 3) * v.w;
    res.w = m(3, 0) * v.x + m(3, 1) * v.y + m(3, 2) * v.z + m(3, 3) * v.w;
    return res;
  }

  template <typename T>
  Vec4<T> operator*(const Vec4<T> &v, const Matrix<T> &m)
  {
    ASSERT(m.numRows() == 4) << "Matrix dimension mismatch!";
    ASSERT(m.numCols() == 4) << "Matrix dimension mismatch!";
    Vec4<T> res;
    res.x = m(0, 0) * v.x + m(1, 0) * v.y + m(2, 0) * v.z + m(3, 0) * v.w;
    res.y = m(0, 1) * v.x + m(1, 1) * v.y + m(2, 1) * v.z + m(3, 1) * v.w;
    res.z = m(0, 2) * v.x + m(1, 2) * v.y + m(2, 2) * v.z + m(3, 2) * v.w;
    res.w = m(0, 3) * v.x + m(1, 3) * v.y + m(2, 3) * v.z + m(3, 3) * v.w;
    return res;
  }

  template <typename T>
  Vec4<T> operator+(const Vec4<T> &v, const T f)
  {
    return Vec4<T>(v.x + f, v.y + f, v.z + f, v.w + f);
  }

  template <typename T>
  Vec4<T> operator+(const T f, const Vec4<T> &v)
  {
    return Vec4<T>(v.x + f, v.y + f, v.z + f, v.w + f);
  }

  template <typename T>
  Vec4<T> operator-(const Vec4<T> &v, const T f)
  {
    return Vec4<T>(v.x - f, v.y - f, v.z - f, v.w - f);
  }

  template <typename T>
  Vec4<T> operator-(const T f, const Vec4<T> &v)
  {
    return Vec4<T>(f - v.x, f - v.y, f - v.z, f - v.w);
  }

  template <typename T>
  Vec4<T> operator/(const T f, const Vec4<T> &v)
  {
    return Vec4<T>(f / v.x, f / v.y, f / v.z, f / v.w);
  }

  template <typename T>
  Vec4<T> operator-(const Vec4<T> &v)
  {
    return Vec4<T>(-v.x, -v.y, -v.z, -v.w);
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC4_H_
