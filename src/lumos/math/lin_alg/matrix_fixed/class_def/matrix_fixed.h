#ifndef LUMOS_MATH_LIN_ALG_MATRIX_FIXED_CLASS_DEF_MATRIX_FIXED_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_FIXED_CLASS_DEF_MATRIX_FIXED_H_

#include <assert.h>

#include <optional>
#include <complex>

#include "lumos/math/misc/assert.h"
#include "lumos/math/misc/forward_decl.h"

namespace lumos
{

  template <typename T, uint16_t R, uint16_t C>
  struct LUMatrices
  {
    static constexpr uint16_t K = (R < C) ? R : C;

    FixedSizeMatrix<T, R, K> l_matrix;
    FixedSizeMatrix<T, K, C> u_matrix;
    FixedSizeVector<uint16_t, R> row_permutation; // maps row i in PA = LU
  };

  template <typename T, uint16_t R, uint16_t C>
  struct SVDMatrices
  {
    FixedSizeMatrix<T, R, R> u_matrix;
    FixedSizeMatrix<T, R, C> sigma_matrix;
    FixedSizeMatrix<T, C, C> v_matrix;
  };

  template <typename T, uint16_t R, uint16_t C>
  struct QRResult
  {
    FixedSizeMatrix<T, R, C> q; // Orthogonal (R×C for economy form)
    FixedSizeMatrix<T, C, C> r; // Upper triangular
  };

  template <typename T, uint16_t R, uint16_t C>
  struct EigenDecomposition
  {
    static_assert(R == C, "Eigen decomposition requires square matrix");
    FixedSizeMatrix<std::complex<T>, R, R> eigenvectors;
    FixedSizeVector<std::complex<T>, R> eigenvalues;
  };

  template <typename T, uint16_t R, uint16_t C>
  class FixedSizeMatrix
  {
    // protected:
  public:
    T data_[R * C];

    FixedSizeMatrix();
    template <typename Y>
    FixedSizeMatrix(const FixedSizeMatrix<Y, R, C> &m);

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

    Matrix<T> toMatrix() const
    {
      Matrix<T> matrix(R, C);
      for (size_t r = 0; r < R; ++r)
      {
        for (size_t c = 0; c < C; ++c)
        {
          matrix(r, c) = data_[r * C + c];
        }
      }
      return matrix;
    }

    FixedSizeMatrix<T, C, R> transposed() const;

    std::optional<FixedSizeMatrix<T, R, C>> inverse() const;
    std::optional<LUMatrices<T, R, C>> luDecomposition() const;
    std::optional<SVDMatrices<T, R, C>> svd() const;
    std::optional<QRResult<T, R, C>> qrDecomposition() const;

    EigenDecomposition<T, R, C> eigen() const;
    FixedSizeMatrix<T, R, C> cholesky() const;

    T frobeniusNorm() const;
    T oneNorm() const;
    T infNorm() const;
    T pNorm(T p) const;

    // --- Condition number functions ---
    std::optional<T> condition_number_frobenius() const;
    std::optional<T> condition_number_l1() const;
    std::optional<T> condition_number_inf() const;
  };

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_FIXED_CLASS_DEF_MATRIX_FIXED_H_
