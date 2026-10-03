#ifndef LUMOS_MATH_TRANSFORMATIONS_QUATERNION_H_
#define LUMOS_MATH_TRANSFORMATIONS_QUATERNION_H_

#include "math/transformations/class_def/quaternion.h"

#include <cmath>
#include <limits>

#include "math/lin_alg/vector_low_dim/vec3.h"

namespace lumos {
// Default constructor
template <typename T>
constexpr Quaternion<T>::Quaternion() : w(1), x(0), y(0), z(0) {}

// Parameterized constructor
template <typename T>
constexpr Quaternion<T>::Quaternion(T w_, T x_, T y_, T z_)
    : w(w_), x(x_), y(y_), z(z_) {}

// Conversion from a quaternion with another scalar type
template <typename T>
template <typename Y>
constexpr Quaternion<T>::Quaternion(const Quaternion<Y> &q)
    : w(q.w), x(q.x), y(q.y), z(q.z) {}

// Addition
template <typename T>
constexpr Quaternion<T>
Quaternion<T>::operator+(const Quaternion &other) const {
  return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
}

// Subtraction
template <typename T>
constexpr Quaternion<T>
Quaternion<T>::operator-(const Quaternion &other) const {
  return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
}

// Multiplication
template <typename T>
constexpr Quaternion<T>
    Quaternion<T>::operator*(const Quaternion &other) const {
  return Quaternion(
      w * other.w - x * other.x - y * other.y - z * other.z,
      w * other.x + x * other.w + y * other.z - z * other.y,
      w * other.y - x * other.z + y * other.w + z * other.x,
      w * other.z + x * other.y - y * other.x + z * other.w);
}

// Scalar multiplication
template <typename T>
constexpr Quaternion<T> Quaternion<T>::operator*(const T scalar) const {
  return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
}

// Conjugate
template <typename T> constexpr Quaternion<T> Quaternion<T>::conjugate() const {
  return Quaternion(w, -x, -y, -z);
}

// Normalize
template <typename T> constexpr void Quaternion<T>::normalize() {
  T norm = std::sqrt(w * w + x * x + y * y + z * z);
  if (norm > 0) {
    T inv_norm = static_cast<T>(1.0) / norm;
    w *= inv_norm;
    x *= inv_norm;
    y *= inv_norm;
    z *= inv_norm;
  }
}

template <typename T> Quaternion<T> Quaternion<T>::normalized() const {
  Quaternion<T> q = *this;
  q.normalize();
  return q;
}

// Inverse
template <typename T> constexpr Quaternion<T> Quaternion<T>::inverse() const {
  const T inv_squared_norm = static_cast<T>(1) / squaredNorm();
  return Quaternion(w * inv_squared_norm, -x * inv_squared_norm,
                    -y * inv_squared_norm, -z * inv_squared_norm);
}

template <typename T>
constexpr T Quaternion<T>::dot(const Quaternion &other) const {
  return w * other.w + x * other.x + y * other.y + z * other.z;
}

template <typename T> constexpr T Quaternion<T>::squaredNorm() const {
  return w * w + x * x + y * y + z * z;
}

template <typename T> T Quaternion<T>::norm() const {
  return std::sqrt(squaredNorm());
}

template <typename T>
constexpr FixedSizeMatrix<T, 3, 3> Quaternion<T>::toRotationMatrix() const {
  const T two = static_cast<T>(2);
  const T one = static_cast<T>(1);

  const T xx = x * x;
  const T yy = y * y;
  const T zz = z * z;
  const T xy = x * y;
  const T xz = x * z;
  const T yz = y * z;
  const T wx = w * x;
  const T wy = w * y;
  const T wz = w * z;

  FixedSizeMatrix<T, 3, 3> r_mat;

  r_mat(0, 0) = one - two * (yy + zz);
  r_mat(0, 1) = two * (xy - wz);
  r_mat(0, 2) = two * (xz + wy);
  r_mat(1, 0) = two * (xy + wz);
  r_mat(1, 1) = one - two * (xx + zz);
  r_mat(1, 2) = two * (yz - wx);
  r_mat(2, 0) = two * (xz - wy);
  r_mat(2, 1) = two * (yz + wx);
  r_mat(2, 2) = one - two * (xx + yy);

  return r_mat;
}

template <typename T> Vec3<T> Quaternion<T>::rotate(const Vec3<T> &v) const {
  // v' = v + 2 * w * (u x v) + 2 * u x (u x v), with u = (x, y, z)
  const Vec3<T> u(x, y, z);
  const Vec3<T> uv = u.crossProduct(v);
  const Vec3<T> uuv = u.crossProduct(uv);
  return v + static_cast<T>(2) * (w * uv + uuv);
}

template <typename T>
Quaternion<T> Quaternion<T>::fromAxisAngle(const Vec3<T> &axis, T angle) {
  const T axis_norm = axis.norm();
  if (axis_norm <= std::numeric_limits<T>::min()) {
    return Quaternion<T>();
  }
  const T half_angle = angle / static_cast<T>(2);
  const T s = std::sin(half_angle) / axis_norm;
  return Quaternion<T>(std::cos(half_angle), axis.x * s, axis.y * s,
                       axis.z * s);
}

template <typename T>
Quaternion<T>
Quaternion<T>::fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m) {
  // Shepperd's method: pick the largest of w, x, y, z as pivot to avoid
  // dividing by a small number
  const T one = static_cast<T>(1);
  const T quarter = static_cast<T>(0.25);
  const T trace = m(0, 0) + m(1, 1) + m(2, 2);

  Quaternion<T> q;
  if (trace > static_cast<T>(0)) {
    const T s = static_cast<T>(2) * std::sqrt(trace + one);
    q.w = quarter * s;
    q.x = (m(2, 1) - m(1, 2)) / s;
    q.y = (m(0, 2) - m(2, 0)) / s;
    q.z = (m(1, 0) - m(0, 1)) / s;
  } else if ((m(0, 0) > m(1, 1)) && (m(0, 0) > m(2, 2))) {
    const T s = static_cast<T>(2) * std::sqrt(one + m(0, 0) - m(1, 1) - m(2, 2));
    q.w = (m(2, 1) - m(1, 2)) / s;
    q.x = quarter * s;
    q.y = (m(0, 1) + m(1, 0)) / s;
    q.z = (m(0, 2) + m(2, 0)) / s;
  } else if (m(1, 1) > m(2, 2)) {
    const T s = static_cast<T>(2) * std::sqrt(one + m(1, 1) - m(0, 0) - m(2, 2));
    q.w = (m(0, 2) - m(2, 0)) / s;
    q.x = (m(0, 1) + m(1, 0)) / s;
    q.y = quarter * s;
    q.z = (m(1, 2) + m(2, 1)) / s;
  } else {
    const T s = static_cast<T>(2) * std::sqrt(one + m(2, 2) - m(0, 0) - m(1, 1));
    q.w = (m(1, 0) - m(0, 1)) / s;
    q.x = (m(0, 2) + m(2, 0)) / s;
    q.y = (m(1, 2) + m(2, 1)) / s;
    q.z = quarter * s;
  }
  q.normalize();
  return q;
}

template <typename T>
Quaternion<T> Quaternion<T>::exp(const Vec3<T> &rotation_vector) {
  const T angle = rotation_vector.norm();
  const T half_angle = angle / static_cast<T>(2);
  // sin(angle / 2) / angle, which tends to 1/2 as angle tends to 0
  const T s = (angle < std::numeric_limits<T>::epsilon())
                  ? static_cast<T>(0.5)
                  : std::sin(half_angle) / angle;
  return Quaternion<T>(std::cos(half_angle), rotation_vector.x * s,
                       rotation_vector.y * s, rotation_vector.z * s);
}

template <typename T> Vec3<T> Quaternion<T>::log() const {
  // q and -q are the same rotation, use the one with w >= 0 so that the
  // angle ends up in [0, pi]
  const T sign = (w < static_cast<T>(0)) ? static_cast<T>(-1) : static_cast<T>(1);
  const T w_abs = sign * w;
  const Vec3<T> u(sign * x, sign * y, sign * z);
  const T u_norm = u.norm();

  // angle / sin(angle / 2), which tends to 2 as angle tends to 0
  const T s = (u_norm < std::numeric_limits<T>::epsilon())
                  ? static_cast<T>(2) / w_abs
                  : static_cast<T>(2) * std::atan2(u_norm, w_abs) / u_norm;
  return s * u;
}

// Non class functions

// Spherical linear interpolation along the shortest arc between q0 (t = 0)
// and q1 (t = 1). Both must have unit norm
template <typename T>
Quaternion<T> slerp(const Quaternion<T> &q0, const Quaternion<T> &q1,
                    const T t) {
  const T one = static_cast<T>(1);
  T d = q0.dot(q1);
  Quaternion<T> q1_near = q1;
  if (d < static_cast<T>(0)) {
    d = -d;
    q1_near = q1 * static_cast<T>(-1);
  }

  T s0 = one - t;
  T s1 = t;
  // When the quaternions are nearly parallel sin(angle) is close to 0, so
  // fall back to normalized linear interpolation
  if (d < static_cast<T>(0.9995)) {
    const T angle = std::acos(d);
    const T inv_sin_angle = one / std::sin(angle);
    s0 = std::sin((one - t) * angle) * inv_sin_angle;
    s1 = std::sin(t * angle) * inv_sin_angle;
  }

  return (q0 * s0 + q1_near * s1).normalized();
}

using Quaterniond = Quaternion<double>;
using Quaternionf = Quaternion<float>;

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_QUATERNION_H_
