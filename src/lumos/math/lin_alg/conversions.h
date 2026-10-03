#ifndef LUMOS_MATH_LIN_ALG_CONVERSIONS_H_
#define LUMOS_MATH_LIN_ALG_CONVERSIONS_H_

#include "lumos/math/lin_alg/matrix_dynamic/matrix_dynamic.h"
#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/lin_alg/vector_dynamic/vector_dynamic.h"
#include "lumos/math/lin_alg/vector_low_dim/vec2.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/math/lin_alg/vector_low_dim/vec4.h"
#include "lumos/math/misc/math_macros.h"

// Conversions between the fixed size types (FixedSizeMatrix, Vec2, Vec3, Vec4)
// and the dynamically sized types (Matrix, Vector)

namespace lumos
{

  template <typename T, uint16_t R, uint16_t C>
  Matrix<T> toDynamicMatrix(const FixedSizeMatrix<T, R, C> &m)
  {
    Matrix<T> res(R, C);
    for (size_t r = 0; r < R; r++)
    {
      for (size_t c = 0; c < C; c++)
      {
        res(r, c) = m(r, c);
      }
    }
    return res;
  }

  // The size is given explicitly, e.g. toFixedSizeMatrix<3, 3>(m), and must
  // match the size of m
  template <uint16_t R, uint16_t C, typename T>
  FixedSizeMatrix<T, R, C> toFixedSizeMatrix(const Matrix<T> &m)
  {
    ASSERT(m.numRows() == R) << "Matrix dimension mismatch!";
    ASSERT(m.numCols() == C) << "Matrix dimension mismatch!";
    FixedSizeMatrix<T, R, C> res;
    for (size_t r = 0; r < R; r++)
    {
      for (size_t c = 0; c < C; c++)
      {
        res(r, c) = m(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Vector<T> toDynamicVector(const Vec2<T> &v)
  {
    Vector<T> res(2);
    res(0) = v.x;
    res(1) = v.y;
    return res;
  }

  template <typename T>
  Vector<T> toDynamicVector(const Vec3<T> &v)
  {
    Vector<T> res(3);
    res(0) = v.x;
    res(1) = v.y;
    res(2) = v.z;
    return res;
  }

  template <typename T>
  Vector<T> toDynamicVector(const Vec4<T> &v)
  {
    Vector<T> res(4);
    res(0) = v.x;
    res(1) = v.y;
    res(2) = v.z;
    res(3) = v.w;
    return res;
  }

  template <typename T>
  Vec2<T> toVec2(const Vector<T> &v)
  {
    ASSERT(v.size() == 2) << "Vector dimension mismatch!";
    return Vec2<T>(v(0), v(1));
  }

  template <typename T>
  Vec3<T> toVec3(const Vector<T> &v)
  {
    ASSERT(v.size() == 3) << "Vector dimension mismatch!";
    return Vec3<T>(v(0), v(1), v(2));
  }

  template <typename T>
  Vec4<T> toVec4(const Vector<T> &v)
  {
    ASSERT(v.size() == 4) << "Vector dimension mismatch!";
    return Vec4<T>(v(0), v(1), v(2), v(3));
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_CONVERSIONS_H_
