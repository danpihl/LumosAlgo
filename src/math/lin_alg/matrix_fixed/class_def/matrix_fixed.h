#ifndef LUMOS_MATH_LIN_ALG_MATRIX_FIXED_CLASS_DEF_MATRIX_FIXED_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_FIXED_CLASS_DEF_MATRIX_FIXED_H_

#include <assert.h>

#include "logging.h"
#include "math/misc/forward_decl.h"

namespace lumos {
template <typename T, uint16_t R, uint16_t C> class FixedSizeMatrix {
  // protected:
public:
  T data_[R * C];

  FixedSizeMatrix();
  template <typename Y> FixedSizeMatrix(const FixedSizeMatrix<Y, R, C> &m);

  void fillBufferWithData(uint8_t *const buffer) const;

  T &operator()(const size_t r, const size_t c);
  const T &operator()(const size_t r, const size_t c) const;

  size_t numRows() const;
  size_t numCols() const;
  size_t size() const;
  size_t numElements() const;
  size_t numBytes() const;

  void fill(const T val);
  T *data() const;
  // Matrix<T, C, R> getTranspose() const;

  T max() const;
  T min() const;
  T sum() const;

  FixedSizeMatrix<T, C, R> transposed() const;
};

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_FIXED_CLASS_DEF_MATRIX_FIXED_H_
