#ifndef LUMOS_MATH_TRANSFORMATIONS_SE2_H_
#define LUMOS_MATH_TRANSFORMATIONS_SE2_H_

#include <cmath>

#include "lumos/math/transformations/angles.h"
#include "lumos/math/transformations/class_def/se2.h"
#include "lumos/math/transformations/so3.h"

namespace lumos
{

  template <typename T>
  Twist2<T>::Twist2() : linear(0, 0), angular(0)
  {
  }

  template <typename T>
  Twist2<T>::Twist2(const Vec2<T> &linear_, const T angular_)
      : linear(linear_), angular(angular_)
  {
  }

  // Conversion from a twist with another scalar type
  template <typename T>
  template <typename Y>
  Twist2<T>::Twist2(const Twist2<Y> &twist)
      : linear(twist.linear), angular(twist.angular)
  {
  }

  template <typename T>
  SE2<T>::SE2() : theta(0), translation(0, 0)
  {
  }

  template <typename T>
  SE2<T>::SE2(const T theta_, const Vec2<T> &translation_)
      : theta(wrapToPi(theta_)), translation(translation_)
  {
  }

  template <typename T>
  SE2<T>::SE2(const T theta_, const T x, const T y)
      : theta(wrapToPi(theta_)), translation(x, y)
  {
  }

  template <typename T>
  SE2<T>::SE2(const FixedSizeMatrix<T, 3, 3> &homogeneous_matrix)
      : theta(wrapToPi(
            std::atan2(homogeneous_matrix(1, 0), homogeneous_matrix(0, 0)))),
        translation(homogeneous_matrix(0, 2), homogeneous_matrix(1, 2))
  {
  }

  // Conversion from a pose with another scalar type
  template <typename T>
  template <typename Y>
  SE2<T>::SE2(const SE2<Y> &pose)
      : theta(pose.theta), translation(pose.translation)
  {
  }

  template <typename T>
  SE2<T> SE2<T>::exp(const Twist2<T> &twist)
  {
    // translation = V * linear, with
    // V = [a, -b; b, a], a = sin(w) / w, b = (1 - cos(w)) / w
    const T w = twist.angular;
    const T a = internal::sinc(w);
    const T b = w * internal::oneMinusCosOverSquare(w);

    return SE2<T>(w, a * twist.linear.x - b * twist.linear.y,
                  b * twist.linear.x + a * twist.linear.y);
  }

  template <typename T>
  Twist2<T> SE2<T>::log() const
  {
    // linear = V^-1 * translation, with V as in exp
    const T a = internal::sinc(theta);
    const T b = theta * internal::oneMinusCosOverSquare(theta);
    const T inv_det = static_cast<T>(1) / (a * a + b * b);

    return Twist2<T>(
        Vec2<T>(inv_det * (a * translation.x + b * translation.y),
                inv_det * (-b * translation.x + a * translation.y)),
        theta);
  }

  template <typename T>
  SE2<T> SE2<T>::operator*(const SE2<T> &other) const
  {
    return SE2<T>(theta + other.theta,
                  rotateVector(other.translation) + translation);
  }

  template <typename T>
  Vec2<T> SE2<T>::operator*(const Vec2<T> &p) const
  {
    return rotateVector(p) + translation;
  }

  template <typename T>
  Vec2<T> SE2<T>::transformPoint(const Vec2<T> &p) const
  {
    return rotateVector(p) + translation;
  }

  template <typename T>
  Vec2<T> SE2<T>::rotateVector(const Vec2<T> &v) const
  {
    const T ct = std::cos(theta);
    const T st = std::sin(theta);
    return Vec2<T>(ct * v.x - st * v.y, st * v.x + ct * v.y);
  }

  template <typename T>
  SE2<T> SE2<T>::inverse() const
  {
    const T ct = std::cos(theta);
    const T st = std::sin(theta);
    // -R^T * translation
    return SE2<T>(-theta, -(ct * translation.x + st * translation.y),
                  st * translation.x - ct * translation.y);
  }

  template <typename T>
  FixedSizeMatrix<T, 2, 2> SE2<T>::rotationMatrix() const
  {
    return fixedRotationMatrix2D(theta);
  }

  template <typename T>
  FixedSizeMatrix<T, 3, 3> SE2<T>::toHomogeneousMatrix() const
  {
    const T ct = std::cos(theta);
    const T st = std::sin(theta);

    FixedSizeMatrix<T, 3, 3> m;

    m(0, 0) = ct;
    m(0, 1) = -st;
    m(0, 2) = translation.x;
    m(1, 0) = st;
    m(1, 1) = ct;
    m(1, 2) = translation.y;
    m(2, 0) = 0;
    m(2, 1) = 0;
    m(2, 2) = 1;

    return m;
  }

  // Non class functions

  // Interpolates along the geodesic between pose0 (t = 0) and pose1 (t = 1)
  template <typename T>
  SE2<T> interpolate(const SE2<T> &pose0, const SE2<T> &pose1, const T t)
  {
    const Twist2<T> delta = (pose0.inverse() * pose1).log();
    return pose0 * SE2<T>::exp(Twist2<T>(t * delta.linear, t * delta.angular));
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const SE2<T> &pose)
  {
    os << "theta: " << pose.theta << ", translation: " << pose.translation;
    return os;
  }

  using SE2d = SE2<double>;
  using SE2f = SE2<float>;
  using Twist2d = Twist2<double>;
  using Twist2f = Twist2<float>;

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_SE2_H_
