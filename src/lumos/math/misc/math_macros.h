#ifndef LUMOS_MATH_MISC_MATH_MACROS_H_
#define LUMOS_MATH_MISC_MATH_MACROS_H_

#include "lumos/math/misc/assert.h"

namespace lumos
{
#define DATA_ALLOCATION(data, data_size, data_type, alloc_type)    \
  try                                                              \
  {                                                                \
    data = new data_type[data_size];                               \
  }                                                                \
  catch (std::bad_alloc & ba)                                      \
  {                                                                \
    std::cerr << alloc_type << " allocation failed: " << ba.what() \
              << std::endl;                                        \
    exit(-1);                                                      \
  }

#define ASSERT_MAT_VALID_INTERNAL()                \
  ASSERT(num_rows_ > 0) << "Number of rows is 0!"; \
  ASSERT(num_cols_ > 0) << "Number of cols is 0!"

#define ASSERT_VEC_VALID_INTERNAL() \
  ASSERT(size_ > 0) << "Number of elements is 0!"

} // namespace lumos

#endif // LUMOS_MATH_MISC_MATH_MACROS_H_
