#ifndef LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_QUATERNION_H_
#define LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_QUATERNION_H_

#include <cmath>

#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/misc/forward_decl.h"

namespace lumos
{
  template <typename T>
  class Quaternion
  {
  public:
    T w;
    T x;
    T y;
    T z;

    // Constructors
    constexpr Quaternion();
    constexpr Quaternion(T w_, T x_, T y_, T z_);
    template <typename Y>
    constexpr Quaternion(const Quaternion<Y> &q);

    // Axis does not need to be normalized
    static Quaternion fromAxisAngle(const Vec3<T> &axis, T angle);
    static Quaternion fromAxisAngle(const AxisAngle<T> &axis_angle);
    // Uses RotationOrder::XYZ, see EulerAngles
    static Quaternion fromEulerAngles(const EulerAngles<T> &euler);
    static Quaternion fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m);
    // Exponential map: rotation vector (axis * angle) to unit quaternion
    static Quaternion exp(const Vec3<T> &rotation_vector);

    // Quaternion operations
    constexpr Quaternion operator+(const Quaternion &other) const;
    constexpr Quaternion operator-(const Quaternion &other) const;
    constexpr Quaternion operator*(const Quaternion &other) const;
    constexpr Quaternion operator*(const T scalar) const;
    constexpr Quaternion conjugate() const;
    constexpr Quaternion inverse() const;
    constexpr T dot(const Quaternion &other) const;
    constexpr T squaredNorm() const;
    T norm() const;
    // The functions below assume that the quaternion has unit norm
    constexpr FixedSizeMatrix<T, 3, 3> toRotationMatrix() const;
    Vec3<T> rotate(const Vec3<T> &v) const;
    // Logarithmic map: unit quaternion to rotation vector (axis * angle), with
    // the angle in [0, pi]
    Vec3<T> log() const;

    // Normalization
    constexpr void normalize();
    Quaternion normalized() const;
  };

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_QUATERNION_H_
