#ifndef LUMOS_MATH_TRANSFORMATIONS_EULER_ANGLES_H_
#define LUMOS_MATH_TRANSFORMATIONS_EULER_ANGLES_H_

#include <cmath>
#include <limits>

#include "math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "math/transformations/class_def/euler_angles.h"
#include "math/transformations/quaternion.h"

namespace lumos {

template <typename T>
FixedSizeMatrix<T, 3, 3> EulerAngles<T>::toRotationMatrix() const {
  const T cr = std::cos(roll);
  const T sr = std::sin(roll);
  const T cp = std::cos(pitch);
  const T sp = std::sin(pitch);
  const T cy = std::cos(yaw);
  const T sy = std::sin(yaw);

  FixedSizeMatrix<T, 3, 3> r_mat;

  r_mat(0, 0) = cy * cp;
  r_mat(0, 1) = cy * sp * sr - sy * cr;
  r_mat(0, 2) = cy * sp * cr + sy * sr;
  r_mat(1, 0) = sy * cp;
  r_mat(1, 1) = sy * sp * sr + cy * cr;
  r_mat(1, 2) = sy * sp * cr - cy * sr;
  r_mat(2, 0) = -sp;
  r_mat(2, 1) = cp * sr;
  r_mat(2, 2) = cp * cr;

  return r_mat;
}

template <typename T>
FixedSizeMatrix<T, 3, 3> EulerAngles<T>::rollMatrix() const {
  const T cr = std::cos(roll);
  const T sr = std::sin(roll);

  FixedSizeMatrix<T, 3, 3> r;
  r(0, 0) = 1;
  r(0, 1) = 0;
  r(0, 2) = 0;
  r(1, 0) = 0;
  r(1, 1) = cr;
  r(1, 2) = -sr;
  r(2, 0) = 0;
  r(2, 1) = sr;
  r(2, 2) = cr;
  return r;
}

template <typename T>
FixedSizeMatrix<T, 3, 3> EulerAngles<T>::pitchMatrix() const {
  const T cp = std::cos(pitch);
  const T sp = std::sin(pitch);

  FixedSizeMatrix<T, 3, 3> r;
  r(0, 0) = cp;
  r(0, 1) = 0;
  r(0, 2) = sp;
  r(1, 0) = 0;
  r(1, 1) = 1;
  r(1, 2) = 0;
  r(2, 0) = -sp;
  r(2, 1) = 0;
  r(2, 2) = cp;
  return r;
}

template <typename T>
FixedSizeMatrix<T, 3, 3> EulerAngles<T>::yawMatrix() const {
  const T cy = std::cos(yaw);
  const T sy = std::sin(yaw);

  FixedSizeMatrix<T, 3, 3> r;
  r(0, 0) = cy;
  r(0, 1) = -sy;
  r(0, 2) = 0;
  r(1, 0) = sy;
  r(1, 1) = cy;
  r(1, 2) = 0;
  r(2, 0) = 0;
  r(2, 1) = 0;
  r(2, 2) = 1;
  return r;
}

template <typename T>
FixedSizeMatrix<T, 3, 3>
EulerAngles<T>::toRotationMatrix(RotationOrder order) const {
  switch (order) {
  case RotationOrder::XYZ:
    return yawMatrix() * pitchMatrix() * rollMatrix();
  case RotationOrder::XZY:
    return pitchMatrix() * yawMatrix() * rollMatrix();
  case RotationOrder::YXZ:
    return yawMatrix() * rollMatrix() * pitchMatrix();
  case RotationOrder::YZX:
    return rollMatrix() * yawMatrix() * pitchMatrix();
  case RotationOrder::ZXY:
    return pitchMatrix() * rollMatrix() * yawMatrix();
  case RotationOrder::ZYX:
    return rollMatrix() * pitchMatrix() * yawMatrix();
  default:
    return toRotationMatrix(); // Same as XYZ
  }
}

template <typename T>
EulerAngles<T>
EulerAngles<T>::fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m,
                                   RotationOrder order) {
  // The rotation matrix is the product R_i(a) * R_j(b) * R_k(c) of rotations
  // about the axes i, j and k, where k is the axis that is applied first
  size_t i = 2, j = 1, k = 0;
  switch (order) {
  case RotationOrder::XYZ:
    i = 2, j = 1, k = 0;
    break;
  case RotationOrder::XZY:
    i = 1, j = 2, k = 0;
    break;
  case RotationOrder::YXZ:
    i = 2, j = 0, k = 1;
    break;
  case RotationOrder::YZX:
    i = 0, j = 2, k = 1;
    break;
  case RotationOrder::ZXY:
    i = 1, j = 0, k = 2;
    break;
  case RotationOrder::ZYX:
    i = 0, j = 1, k = 2;
    break;
  }

  // +1 if (i, j, k) is a cyclic permutation of (0, 1, 2), otherwise -1
  const T sign = ((j == (i + 1) % 3) ? T(1) : T(-1));

  T sin_b = sign * m(i, k);
  sin_b = (sin_b > T(1)) ? T(1) : ((sin_b < T(-1)) ? T(-1) : sin_b);

  T angles[3];
  angles[j] = std::asin(sin_b);

  const T gimbal_lock_limit =
      T(1) - T(100) * std::numeric_limits<T>::epsilon();
  if (std::abs(sin_b) < gimbal_lock_limit) {
    angles[i] = std::atan2(-sign * m(j, k), m(k, k));
    angles[k] = std::atan2(-sign * m(i, j), m(i, i));
  } else {
    // Gimbal lock: only the sum or difference of the outer angles is
    // defined, so put all of it in the last applied rotation
    angles[i] = std::atan2(sign * m(k, j), m(j, j));
    angles[k] = T(0);
  }

  return EulerAngles<T>(angles[0], angles[1], angles[2]);
}

template <typename T>
EulerAngles<T>
EulerAngles<T>::fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m) {
  return fromRotationMatrix(m, RotationOrder::XYZ);
}

template <typename T>
EulerAngles<T> EulerAngles<T>::fromQuaternion(const Quaternion<T> &q,
                                              RotationOrder order) {
  return fromRotationMatrix(q.toRotationMatrix(), order);
}

template <typename T>
EulerAngles<T> EulerAngles<T>::fromQuaternion(const Quaternion<T> &q) {
  return fromRotationMatrix(q.toRotationMatrix(), RotationOrder::XYZ);
}

template <typename T>
Quaternion<T> EulerAngles<T>::toQuaternion(RotationOrder order) const {
  return Quaternion<T>::fromRotationMatrix(toRotationMatrix(order));
}

template <typename T> Quaternion<T> EulerAngles<T>::toQuaternion() const {
  return Quaternion<T>::fromRotationMatrix(toRotationMatrix());
}

using EulerAnglesd = EulerAngles<double>;
using EulerAnglesf = EulerAngles<float>;

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_EULER_ANGLES_H_
