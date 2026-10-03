#ifndef LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_CLASS_DEF_VEC3_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_CLASS_DEF_VEC3_H_

#include "math/lin_alg/matrix_fixed/class_def/matrix_fixed.h"
#include "math/misc/forward_decl.h"

namespace lumos {
template <typename T> struct Vec3 {
  T x;
  T y;
  T z;

  Vec3(const T x_, const T y_, const T z_);
  template <typename Y> Vec3(const Vec3<Y> &v);
  Vec3();

  Vec3<T> normalized() const;
  Vec3<T> vectorBetweenPoints(const Point3<T> &end_point) const;
  Vec3<T> normalizedVectorBetweenPoints(const Point3<T> &end_point) const;
  T squaredNorm() const;
  T norm() const;
  Vec3<T> elementWiseMultiply(const Vec3<T> &factor_vector) const;
  Vec3<T> elementWiseDivide(const Vec3<T> &numerator_vector) const;
  Vec3<T> crossProduct(const Vec3<T> &right_vector) const;
  FixedSizeMatrix<T, 3, 3> toCrossProductMatrix() const;
  T angleBetweenVectors(const Vec3<T> &v) const;
};

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_CLASS_DEF_VEC3_H_
