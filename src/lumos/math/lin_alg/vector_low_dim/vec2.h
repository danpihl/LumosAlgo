#ifndef LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC2_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC2_H_

#include <cmath>

#include "lumos/math/lin_alg/vector_low_dim/class_def/vec2.h"

namespace lumos
{
  template <typename T>
  Vec2<T>::Vec2(const T x_, const T y_)
  {
    x = x_;
    y = y_;
  }

  template <typename T>
  Vec2<T>::Vec2() {}

  template <typename T>
  template <typename Y>
  Vec2<T>::Vec2(const Vec2<Y> &v)
  {
    x = v.x;
    y = v.y;
  }

  template <typename T>
  T Vec2<T>::angle() const { return std::atan2(y, x); }

  template <typename T>
  Vec2<T> Vec2<T>::normalized() const
  {
    return Vec2<T>(x, y) / std::sqrt(x * x + y * y);
  }

  template <typename T>
  Vec2<T> Vec2<T>::vectorBetweenPoints(const Point2<T> &end_point) const
  {
    // "This" is start point
    return end_point - *this;
  }

  template <typename T>
  Vec2<T>
  Vec2<T>::normalizedVectorBetweenPoints(const Point2<T> &end_point) const
  {
    // "This" is start point
    return (end_point - *this).normalized();
  }

  template <typename T>
  T Vec2<T>::squaredNorm() const { return x * x + y * y; }

  template <typename T>
  T Vec2<T>::norm() const
  {
    return std::sqrt(x * x + y * y);
  }

  template <typename T>
  Vec2<T> Vec2<T>::elementWiseMultiply(const Vec2<T> &factor_vector) const
  {
    return Vec2<T>(x * factor_vector.x, y * factor_vector.y);
  }

  template <typename T>
  Vec2<T> Vec2<T>::elementWiseDivide(const Vec2<T> &denominator_vector) const
  {
    return Vec2<T>(x / denominator_vector.x, y / denominator_vector.y);
  }

  template <typename T>
  T Vec2<T>::angleBetweenVectors(const Vec2<T> &v) const
  {
    T dot_product = (*this) * v;
    return std::acos(dot_product / ((this->norm()) * (v.norm())));
  }

  // Non class functions

  template <typename T>
  bool operator==(const Vec2<T> &v0, const Vec2<T> &v1)
  {
    return (v0.x == v1.x) && (v0.y == v1.y);
  }

  template <typename T>
  bool operator!=(const Vec2<T> &v0, const Vec2<T> &v1)
  {
    return !(v0 == v1);
  }

  template <typename T>
  Vec2<T> operator*(const T f, const Vec2<T> &v)
  {
    return Vec2<T>(f * v.x, f * v.y);
  }

  template <typename T>
  Vec2<T> operator*(const Vec2<T> &v, const T f)
  {
    return Vec2<T>(f * v.x, f * v.y);
  }

  template <typename T>
  T operator*(const Vec2<T> &v0, const Vec2<T> &v1)
  {
    return v0.x * v1.x + v0.y * v1.y;
  }

  template <typename T>
  Vec2<T> operator/(const Vec2<T> &v, const T f)
  {
    return Vec2<T>(v.x / f, v.y / f);
  }

  template <typename T>
  Vec2<T> operator+(const Vec2<T> &v0, const Vec2<T> &v1)
  {
    return Vec2<T>(v0.x + v1.x, v0.y + v1.y);
  }

  template <typename T>
  Vec2<T> operator-(const Vec2<T> &v0, const Vec2<T> &v1)
  {
    return Vec2<T>(v0.x - v1.x, v0.y - v1.y);
  }

  template <typename T>
  Vec2<T> operator*(const Matrix<T> &m, const Vec2<T> &v)
  {
    ASSERT(m.numRows() == 2) << "Matrix dimension mismatch!";
    ASSERT(m.numCols() == 2) << "Matrix dimension mismatch!";
    Vec2<T> res;
    res.x = m(0, 0) * v.x + m(0, 1) * v.y;
    res.y = m(1, 0) * v.x + m(1, 1) * v.y;
    return res;
  }

  template <typename T>
  Vec2<T> operator*(const Vec2<T> &v, const Matrix<T> &m)
  {
    ASSERT(m.numRows() == 2) << "Matrix dimension mismatch!";
    ASSERT(m.numCols() == 2) << "Matrix dimension mismatch!";
    Vec2<T> res;
    res.x = v.x * m(0, 0) + v.y * m(1, 0);
    res.y = v.x * m(0, 1) + v.y * m(1, 1);
    return res;
  }

  template <typename T>
  Vec2<T> operator+(const Vec2<T> &v, const T f)
  {
    return Vec2<T>(v.x + f, v.y + f);
  }

  template <typename T>
  Vec2<T> operator+(const T f, const Vec2<T> &v)
  {
    return Vec2<T>(v.x + f, v.y + f);
  }

  template <typename T>
  Vec2<T> operator-(const Vec2<T> &v, const T f)
  {
    return Vec2<T>(v.x - f, v.y - f);
  }

  template <typename T>
  Vec2<T> operator-(const T f, const Vec2<T> &v)
  {
    return Vec2<T>(f - v.x, f - v.y);
  }

  template <typename T>
  Vec2<T> operator/(const T f, const Vec2<T> &v)
  {
    return Vec2<T>(f / v.x, f / v.y);
  }

  template <typename T>
  Vec2<T> operator-(const Vec2<T> &v)
  {
    return Vec2<T>(-v.x, -v.y);
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC2_H_
