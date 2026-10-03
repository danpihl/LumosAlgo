#ifndef LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_SE3_H_
#define LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_SE3_H_

#include "math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "math/lin_alg/vector_low_dim/vec3.h"
#include "math/misc/forward_decl.h"
#include "math/transformations/class_def/quaternion.h"

namespace lumos {

// Element of the Lie algebra se(3)
template <typename T> struct Twist3 {
  Vec3<T> linear;
  Vec3<T> angular;

  Twist3();
  Twist3(const Vec3<T> &linear_, const Vec3<T> &angular_);
  template <typename Y> Twist3(const Twist3<Y> &twist);
};

// Rigid transformation in 3D: p_a = rotation * p_b + translation
template <typename T> class SE3 {
public:
  FixedSizeMatrix<T, 3, 3> rotation;
  Vec3<T> translation;

  // Identity transformation
  SE3();
  SE3(const FixedSizeMatrix<T, 3, 3> &rotation_, const Vec3<T> &translation_);
  SE3(const Quaternion<T> &q, const Vec3<T> &translation_);
  explicit SE3(const FixedSizeMatrix<T, 4, 4> &homogeneous_matrix);
  template <typename Y> SE3(const SE3<Y> &pose);

  // Exponential map
  static SE3 exp(const Twist3<T> &twist);
  // Logarithmic map
  Twist3<T> log() const;

  // Composition: (a * b) * p == a * (b * p)
  SE3 operator*(const SE3 &other) const;
  // Transforms a point (applies rotation and translation)
  Vec3<T> operator*(const Vec3<T> &p) const;
  Vec3<T> transformPoint(const Vec3<T> &p) const;
  // Transforms a direction (applies rotation only)
  Vec3<T> rotateVector(const Vec3<T> &v) const;

  SE3 inverse() const;

  FixedSizeMatrix<T, 4, 4> toHomogeneousMatrix() const;
  Quaternion<T> toQuaternion() const;
};

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_SE3_H_
