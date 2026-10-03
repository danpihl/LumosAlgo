#ifndef LUMOS_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_IO_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_IO_H_

// Stream output (operator<<) for FixedSizeMatrix. This is kept apart from the types
// themselves so that they do not depend on <iostream>, which is costly on
// embedded targets. Include this header where printing is wanted

#include <ostream>
#include <string>

#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"

namespace lumos
{
  template <typename T, uint16_t R, uint16_t C>
  std::ostream &operator<<(std::ostream &os, const FixedSizeMatrix<T, R, C> &m)
  {
    std::string s = "";

    for (size_t r = 0; r < R; r++)
    {
      s = s + "[ ";
      for (size_t c = 0; c < C; c++)
      {
        s = s + std::to_string(m(r, c));
        if (c != C - 1)
        {
          s = s + ", ";
        }
      }
      s = s + " ]\n";
    }

    os << s;

    return os;
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_IO_H_
