#ifndef LUMOS_MATH_TRANSFORMATIONS_AXIS_ANGLE_H_
#define LUMOS_MATH_TRANSFORMATIONS_AXIS_ANGLE_H_

#include <cmath>
#include <limits>

#include "math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "math/lin_alg/vector_low_dim/vec3.h"
#include "math/transformations/class_def/axis_angle.h"
#include "math/transformations/quaternion.h"
#include "math/transformations/so3.h"

namespace lumos {
// TODO: Axis needs to be normalized
template <typename T>
AxisAngle<T>::AxisAngle(const T phi_, const T x_, const T y_, const T z_) {
  phi = phi_;
  x = x_;
  y = y_;
  z = z_;
}

template <typename T>
AxisAngle<T>::AxisAngle(const T x_, const T y_, const T z_) {
  phi = std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
  if (phi == 0.0) {
    x = 0.0;
    y = 0.0;
    z = 0.0;
  } else {
    x = x_ / phi;
    y = y_ / phi;
    z = z_ / phi;
  }
}

template <typename T> AxisAngle<T>::AxisAngle(const Vec3<T> &v) {
  phi = v.norm();
  if (phi == 0.0) {
    x = 0.0;
    y = 0.0;
    z = 0.0;
  } else {
    x = v.x / phi;
    y = v.y / phi;
    z = v.z / phi;
  }
}

template <typename T>
AxisAngle<T>::AxisAngle() : phi(0), x(1), y(0), z(0) {}

// Conversion from an axis angle with another scalar type
template <typename T>
template <typename Y>
AxisAngle<T>::AxisAngle(const AxisAngle<Y> &a)
    : phi(a.phi), x(a.x), y(a.y), z(a.z) {}

template <typename T> AxisAngle<T> AxisAngle<T>::normalized() const {
  AxisAngle<T> normalized_axis_angle;
  T d = std::sqrt(x * x + y * y + z * z);
  if (d == 0) {
    normalized_axis_angle.x = 0.0;
    normalized_axis_angle.y = 0.0;
    normalized_axis_angle.z = 0.0;
  } else {
    normalized_axis_angle.x = x / d;
    normalized_axis_angle.y = y / d;
    normalized_axis_angle.z = z / d;
  }
  normalized_axis_angle.phi = phi;
  return normalized_axis_angle;
}

template <typename T>
FixedSizeMatrix<T, 3, 3> AxisAngle<T>::toRotationMatrix() const {
  return expSO3(toRotationVector());
}

template <typename T> Quaternion<T> AxisAngle<T>::toQuaternion() const {
  return Quaternion<T>::fromAxisAngle(Vec3<T>(x, y, z), phi);
}

template <typename T> Vec3<T> AxisAngle<T>::toRotationVector() const {
  const AxisAngle<T> normalized_axis_angle = normalized();
  return Vec3<T>(phi * normalized_axis_angle.x, phi * normalized_axis_angle.y,
                 phi * normalized_axis_angle.z);
}

// The angle is in [0, pi]. For a zero rotation the axis is the x axis
template <typename T>
AxisAngle<T> AxisAngle<T>::fromQuaternion(const Quaternion<T> &q) {
  const Vec3<T> rotation_vector = q.log();
  const T angle = rotation_vector.norm();
  if (angle < std::numeric_limits<T>::epsilon()) {
    return AxisAngle<T>(angle, 1, 0, 0);
  }
  return AxisAngle<T>(angle, rotation_vector.x / angle,
                      rotation_vector.y / angle, rotation_vector.z / angle);
}

template <typename T>
AxisAngle<T>
AxisAngle<T>::fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m) {
  return fromQuaternion(Quaternion<T>::fromRotationMatrix(m));
}

// Non class methods

template <typename T>
AxisAngle<T> rotationMatrixToAxisAngle(const FixedSizeMatrix<T, 3, 3> &m) {
  return AxisAngle<T>::fromRotationMatrix(m);
}

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_AXIS_ANGLE_H_
