#ifndef LUMOS_MATH_TRANSFORMATIONS_TRANSFORMATIONS_IO_H_
#define LUMOS_MATH_TRANSFORMATIONS_TRANSFORMATIONS_IO_H_

// Stream output (operator<<) for Quaternion, SE2 and SE3. This is kept apart from the types
// themselves so that they do not depend on <iostream>, which is costly on
// embedded targets. Include this header where printing is wanted

#include <ostream>

#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed_io.h"
#include "lumos/math/lin_alg/vector_low_dim/vec_io.h"
#include "lumos/math/transformations/quaternion.h"
#include "lumos/math/transformations/se2.h"
#include "lumos/math/transformations/se3.h"

namespace lumos
{
  template <typename T>
  std::ostream &operator<<(std::ostream &os, const Quaternion<T> &q)
  {
    os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
    return os;
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const SE2<T> &pose)
  {
    os << "theta: " << pose.theta << ", translation: " << pose.translation;
    return os;
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const SE3<T> &pose)
  {
    os << "rotation:\n"
       << pose.rotation << "translation: " << pose.translation;
    return os;
  }

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_TRANSFORMATIONS_IO_H_
