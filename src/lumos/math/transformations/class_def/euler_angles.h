#ifndef LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_EULER_ANGLES_H_
#define LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_EULER_ANGLES_H_

#include "lumos/math/lin_alg/matrix_fixed/class_def/matrix_fixed.h"
#include "lumos/math/misc/forward_decl.h"

namespace lumos
{

  // The order in which the rotations about the fixed x (roll), y (pitch) and z
  // (yaw) axes are applied. XYZ is roll first, then pitch, then yaw, which
  // gives the rotation matrix R = Rz(yaw) * Ry(pitch) * Rx(roll)
  enum class RotationOrder
  {
    XYZ,
    XZY,
    YXZ,
    YZX,
    ZXY,
    ZYX
  };

  template <typename T>
  class EulerAngles
  {
  public:
    T roll;
    T pitch;
    T yaw;

    EulerAngles() : roll(T(0)), pitch(T(0)), yaw(T(0)) {}
    EulerAngles(T r, T p, T y) : roll(r), pitch(p), yaw(y) {}
    template <typename Y>
    EulerAngles(const EulerAngles<Y> &e)
        : roll(e.roll), pitch(e.pitch), yaw(e.yaw)
    {
    }

    // The functions without a rotation order use RotationOrder::XYZ.
    // When converting to Euler angles the middle angle is in [-pi/2, pi/2]. At
    // gimbal lock (middle angle +-pi/2) the angle of the first applied rotation
    // is set to zero
    static EulerAngles fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m);
    static EulerAngles fromRotationMatrix(const FixedSizeMatrix<T, 3, 3> &m,
                                          RotationOrder order);
    static EulerAngles fromQuaternion(const Quaternion<T> &q);
    static EulerAngles fromQuaternion(const Quaternion<T> &q,
                                      RotationOrder order);

    FixedSizeMatrix<T, 3, 3> toRotationMatrix() const;

    FixedSizeMatrix<T, 3, 3> rollMatrix() const;

    FixedSizeMatrix<T, 3, 3> pitchMatrix() const;

    FixedSizeMatrix<T, 3, 3> yawMatrix() const;

    FixedSizeMatrix<T, 3, 3> toRotationMatrix(RotationOrder order) const;

    Quaternion<T> toQuaternion() const;
    Quaternion<T> toQuaternion(RotationOrder order) const;
  };
} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_EULER_ANGLES_H_
