#ifndef LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_
#define LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_

#include "math/lin_alg/matrix_fixed/class_def/matrix_fixed.h"
#include "math/misc/forward_decl.h"

namespace lumos {
template <typename T> struct AxisAngle {
  T phi;
  T x;
  T y;
  T z;

  AxisAngle(const T phi_, const T x_, const T y_, const T z_);
  // From a rotation vector (axis * angle)
  AxisAngle(const T x_, const T y_, const T z_);
  AxisAngle(const Vec3<T> &v);
  // Zero rotation about the x axis
  AxisAngle();
  template <typename Y> AxisAngle(const AxisAngle<Y> &a);

  static AxisAngle fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m);
  static AxisAngle fromQuaternion(const Quaternion<T> &q);

  AxisAngle<T> normalized() const;

  FixedSizeMatrix<T, 3, 3> toRotationMatrix() const;
  Quaternion<T> toQuaternion() const;
  // Rotation vector (axis * angle)
  Vec3<T> toRotationVector() const;
};

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_AXIS_ANGLE_H_
