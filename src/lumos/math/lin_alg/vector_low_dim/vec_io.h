#ifndef LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC_IO_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC_IO_H_

// Stream output (operator<<) for Vec2, Vec3 and Vec4. This is kept apart from the types
// themselves so that they do not depend on <iostream>, which is costly on
// embedded targets. Include this header where printing is wanted

#include <ostream>
#include <string>

#include "lumos/math/lin_alg/vector_low_dim/vec2.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/math/lin_alg/vector_low_dim/vec4.h"

namespace lumos
{
  template <typename T>
  std::ostream &operator<<(std::ostream &os, const Vec2<T> &v)
  {
    std::string s =
        "[ " + std::to_string(v.x) + ", " + std::to_string(v.y) + " ]";
    os << s;
    return os;
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const Vec3<T> &v)
  {
    std::string s = "[ " + std::to_string(v.x) + ", " + std::to_string(v.y) +
                    ", " + std::to_string(v.z) + " ]";
    os << s;
    return os;
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const Vec4<T> &v)
  {
    std::string s = "[ " + std::to_string(v.x) + ", " + std::to_string(v.y) +
                    ", " + std::to_string(v.z) + ", " + std::to_string(v.w) +
                    " ]";
    os << s;
    return os;
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_LOW_DIM_VEC_IO_H_
