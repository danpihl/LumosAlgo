#ifndef LUMOS_MATH_TRANSFORMATIONS_SO3_H_
#define LUMOS_MATH_TRANSFORMATIONS_SO3_H_

#include <cmath>

#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/math/transformations/quaternion.h"

namespace lumos
{

  namespace internal
  {
    // Below this angle the closed form expressions of the coefficients lose
    // precision to cancellation, and their Taylor expansions are used instead
    template <typename T>
    constexpr T so3SmallAngle()
    {
      return static_cast<T>(0.2);
    }

    // Returns I + a * K + b * K^2, where K = hat(phi)
    template <typename T>
    FixedSizeMatrix<T, 3, 3> so3Polynomial(const Vec3<T> &phi, const T a,
                                           const T b)
    {
      // K^2 = phi * phi^T - (phi^T * phi) * I
      const T d = static_cast<T>(1) - b * phi.squaredNorm();

      FixedSizeMatrix<T, 3, 3> m;

      m(0, 0) = d + b * phi.x * phi.x;
      m(0, 1) = b * phi.x * phi.y - a * phi.z;
      m(0, 2) = b * phi.x * phi.z + a * phi.y;
      m(1, 0) = b * phi.x * phi.y + a * phi.z;
      m(1, 1) = d + b * phi.y * phi.y;
      m(1, 2) = b * phi.y * phi.z - a * phi.x;
      m(2, 0) = b * phi.x * phi.z - a * phi.y;
      m(2, 1) = b * phi.y * phi.z + a * phi.x;
      m(2, 2) = d + b * phi.z * phi.z;

      return m;
    }

    // sin(x) / x
    template <typename T>
    T sinc(const T x)
    {
      if (std::abs(x) < so3SmallAngle<T>())
      {
        const T x2 = x * x;
        return static_cast<T>(1) -
               x2 / static_cast<T>(6) *
                   (static_cast<T>(1) -
                    x2 / static_cast<T>(20) *
                        (static_cast<T>(1) - x2 / static_cast<T>(42)));
      }
      return std::sin(x) / x;
    }

    // (1 - cos(x)) / x^2
    template <typename T>
    T oneMinusCosOverSquare(const T x)
    {
      const T s = sinc(x / static_cast<T>(2));
      return static_cast<T>(0.5) * s * s;
    }

    // (x - sin(x)) / x^3
    template <typename T>
    T xMinusSinOverCube(const T x)
    {
      const T x2 = x * x;
      if (std::abs(x) < so3SmallAngle<T>())
      {
        return static_cast<T>(1) / static_cast<T>(6) *
               (static_cast<T>(1) -
                x2 / static_cast<T>(20) *
                    (static_cast<T>(1) -
                     x2 / static_cast<T>(42) *
                         (static_cast<T>(1) - x2 / static_cast<T>(72))));
      }
      return (x - std::sin(x)) / (x2 * x);
    }

    // (1 - (x / 2) / tan(x / 2)) / x^2
    template <typename T>
    T leftJacobianInverseCoefficient(const T x)
    {
      const T x2 = x * x;
      if (std::abs(x) < so3SmallAngle<T>())
      {
        return static_cast<T>(1) / static_cast<T>(12) +
               x2 * (static_cast<T>(1) / static_cast<T>(720) +
                     x2 * (static_cast<T>(1) / static_cast<T>(30240) +
                           x2 / static_cast<T>(1209600)));
      }
      const T half_x = x / static_cast<T>(2);
      return (static_cast<T>(1) - half_x / std::tan(half_x)) / x2;
    }

  } // namespace internal

  // Maps a vector to its skew symmetric (cross product) matrix, such that
  // hat(a) * b = a x b
  template <typename T>
  FixedSizeMatrix<T, 3, 3> hat(const Vec3<T> &v)
  {
    return v.toCrossProductMatrix();
  }

  // Inverse of hat. Uses the skew symmetric part of m
  template <typename T>
  Vec3<T> vee(const FixedSizeMatrix<T, 3, 3> &m)
  {
    const T half = static_cast<T>(0.5);
    return Vec3<T>(half * (m(2, 1) - m(1, 2)), half * (m(0, 2) - m(2, 0)),
                   half * (m(1, 0) - m(0, 1)));
  }

  // Exponential map (Rodrigues' formula): rotation vector (axis * angle) to
  // rotation matrix
  template <typename T>
  FixedSizeMatrix<T, 3, 3> expSO3(const Vec3<T> &phi)
  {
    const T angle = phi.norm();
    return internal::so3Polynomial(phi, internal::sinc(angle),
                                   internal::oneMinusCosOverSquare(angle));
  }

  // Logarithmic map: rotation matrix to rotation vector (axis * angle), with
  // the angle in [0, pi]
  template <typename T>
  Vec3<T> logSO3(const FixedSizeMatrix<T, 3, 3> &r)
  {
    // Going through the quaternion is well conditioned for all angles,
    // including those close to pi
    return Quaternion<T>::fromRotationMatrix(r).log();
  }

  // Left Jacobian of SO(3). Also the matrix V that maps the linear part of an
  // SE(3) twist to the translation
  template <typename T>
  FixedSizeMatrix<T, 3, 3> leftJacobianSO3(const Vec3<T> &phi)
  {
    const T angle = phi.norm();
    return internal::so3Polynomial(phi, internal::oneMinusCosOverSquare(angle),
                                   internal::xMinusSinOverCube(angle));
  }

  // Inverse of the left Jacobian of SO(3). Valid for angles smaller than 2*pi
  template <typename T>
  FixedSizeMatrix<T, 3, 3> leftJacobianInverseSO3(const Vec3<T> &phi)
  {
    const T angle = phi.norm();
    return internal::so3Polynomial(
        phi, static_cast<T>(-0.5),
        internal::leftJacobianInverseCoefficient(angle));
  }

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_SO3_H_
