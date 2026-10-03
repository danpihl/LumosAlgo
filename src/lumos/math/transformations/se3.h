#ifndef LUMOS_MATH_TRANSFORMATIONS_SE3_H_
#define LUMOS_MATH_TRANSFORMATIONS_SE3_H_

#include "lumos/math/transformations/class_def/se3.h"
#include "lumos/math/transformations/quaternion.h"
#include "lumos/math/transformations/so3.h"

namespace lumos
{

  template <typename T>
  Twist3<T>::Twist3() : linear(0, 0, 0), angular(0, 0, 0)
  {
  }

  template <typename T>
  Twist3<T>::Twist3(const Vec3<T> &linear_, const Vec3<T> &angular_)
      : linear(linear_), angular(angular_)
  {
  }

  // Conversion from a twist with another scalar type
  template <typename T>
  template <typename Y>
  Twist3<T>::Twist3(const Twist3<Y> &twist)
      : linear(twist.linear), angular(twist.angular)
  {
  }

  template <typename T>
  SE3<T>::SE3()
      : rotation(unitFixedSizeMatrix<T, 3, 3>()), translation(0, 0, 0)
  {
  }

  template <typename T>
  SE3<T>::SE3(const FixedSizeMatrix<T, 3, 3> &rotation_,
              const Vec3<T> &translation_)
      : rotation(rotation_), translation(translation_)
  {
  }

  template <typename T>
  SE3<T>::SE3(const Quaternion<T> &q, const Vec3<T> &translation_)
      : rotation(q.toRotationMatrix()), translation(translation_)
  {
  }

  template <typename T>
  SE3<T>::SE3(const FixedSizeMatrix<T, 4, 4> &homogeneous_matrix)
  {
    for (size_t r = 0; r < 3; r++)
    {
      for (size_t c = 0; c < 3; c++)
      {
        rotation(r, c) = homogeneous_matrix(r, c);
      }
    }
    translation.x = homogeneous_matrix(0, 3);
    translation.y = homogeneous_matrix(1, 3);
    translation.z = homogeneous_matrix(2, 3);
  }

  // Conversion from a pose with another scalar type
  template <typename T>
  template <typename Y>
  SE3<T>::SE3(const SE3<Y> &pose)
      : rotation(pose.rotation), translation(pose.translation)
  {
  }

  template <typename T>
  SE3<T> SE3<T>::exp(const Twist3<T> &twist)
  {
    return SE3<T>(expSO3(twist.angular),
                  leftJacobianSO3(twist.angular) * twist.linear);
  }

  template <typename T>
  Twist3<T> SE3<T>::log() const
  {
    const Vec3<T> angular = logSO3(rotation);
    return Twist3<T>(leftJacobianInverseSO3(angular) * translation, angular);
  }

  template <typename T>
  SE3<T> SE3<T>::operator*(const SE3<T> &other) const
  {
    return SE3<T>(rotation * other.rotation,
                  rotation * other.translation + translation);
  }

  template <typename T>
  Vec3<T> SE3<T>::operator*(const Vec3<T> &p) const
  {
    return rotation * p + translation;
  }

  template <typename T>
  Vec3<T> SE3<T>::transformPoint(const Vec3<T> &p) const
  {
    return rotation * p + translation;
  }

  template <typename T>
  Vec3<T> SE3<T>::rotateVector(const Vec3<T> &v) const
  {
    return rotation * v;
  }

  template <typename T>
  SE3<T> SE3<T>::inverse() const
  {
    const FixedSizeMatrix<T, 3, 3> rotation_inv = rotation.transposed();
    return SE3<T>(rotation_inv, -(rotation_inv * translation));
  }

  template <typename T>
  FixedSizeMatrix<T, 4, 4> SE3<T>::toHomogeneousMatrix() const
  {
    FixedSizeMatrix<T, 4, 4> m;
    for (size_t r = 0; r < 3; r++)
    {
      for (size_t c = 0; c < 3; c++)
      {
        m(r, c) = rotation(r, c);
      }
    }
    m(0, 3) = translation.x;
    m(1, 3) = translation.y;
    m(2, 3) = translation.z;

    m(3, 0) = 0;
    m(3, 1) = 0;
    m(3, 2) = 0;
    m(3, 3) = 1;

    return m;
  }

  template <typename T>
  Quaternion<T> SE3<T>::toQuaternion() const
  {
    return Quaternion<T>::fromRotationMatrix(rotation);
  }

  // Non class functions

  // Interpolates along the geodesic between pose0 (t = 0) and pose1 (t = 1)
  template <typename T>
  SE3<T> interpolate(const SE3<T> &pose0, const SE3<T> &pose1, const T t)
  {
    const Twist3<T> delta = (pose0.inverse() * pose1).log();
    return pose0 * SE3<T>::exp(Twist3<T>(t * delta.linear, t * delta.angular));
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const SE3<T> &pose)
  {
    os << "rotation:\n"
       << pose.rotation << "translation: " << pose.translation;
    return os;
  }

  using SE3d = SE3<double>;
  using SE3f = SE3<float>;
  using Twist3d = Twist3<double>;
  using Twist3f = Twist3<float>;

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_SE3_H_
