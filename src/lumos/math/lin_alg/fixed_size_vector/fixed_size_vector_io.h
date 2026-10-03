#ifndef LUMOS_MATH_LIN_ALG_FIXED_SIZE_VECTOR_FIXED_SIZE_VECTOR_IO_H_
#define LUMOS_MATH_LIN_ALG_FIXED_SIZE_VECTOR_FIXED_SIZE_VECTOR_IO_H_

// Stream output (operator<<) for FixedSizeVector. This is kept apart from the types
// themselves so that they do not depend on <iostream>, which is costly on
// embedded targets. Include this header where printing is wanted

#include <ostream>

#include "lumos/math/lin_alg/fixed_size_vector/fixed_size_vector.h"

namespace lumos
{
  template <typename T, uint16_t N>
  std::ostream &operator<<(std::ostream &os, const FixedSizeVector<T, N> &v)
  {
    os << "[";
    for (uint16_t i = 0; i < N; ++i)
    {
      os << v[i];
      if (i < N - 1)
        os << ", ";
    }
    os << "]";
    return os;
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_FIXED_SIZE_VECTOR_FIXED_SIZE_VECTOR_IO_H_
