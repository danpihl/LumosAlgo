#ifndef LUMOS_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_H_

#include <cmath>
#include <cstring>
#include <limits>

#include "lumos/math/misc/assert.h"
#include "lumos/math/lin_alg/matrix_fixed/class_def/matrix_fixed.h"
#include "lumos/math/misc/math_macros.h"

namespace lumos
{
  template <typename T, uint16_t R, uint16_t C>
  template <typename Y>
  FixedSizeMatrix<T, R, C>::FixedSizeMatrix(const FixedSizeMatrix<Y, R, C> &m)
  {
    for (size_t r = 0; r < R; r++)
    {
      for (size_t c = 0; c < C; c++)
      {
        data_[r * C + c] = m(r, c);
      }
    }
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C>::FixedSizeMatrix()
  {
    static_assert((R * C) < 10001, "Too many elements!");
  }

  template <typename T, uint16_t R, uint16_t C>
  void FixedSizeMatrix<T, R, C>::fillBufferWithData(uint8_t *const buffer) const
  {
    const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
    const size_t num_bytes = R * C * sizeof(T);
    std::memcpy(buffer, internal_ptr, num_bytes);
  }

  template <typename T, uint16_t R, uint16_t C>
  size_t FixedSizeMatrix<T, R, C>::size() const
  {
    return R * C;
  }

  template <typename T, uint16_t R, uint16_t C>
  T *FixedSizeMatrix<T, R, C>::data() const
  {
    return data_;
  }

  template <typename T, uint16_t R, uint16_t C>
  size_t FixedSizeMatrix<T, R, C>::numRows() const
  {
    return R;
  }

  template <typename T, uint16_t R, uint16_t C>
  size_t FixedSizeMatrix<T, R, C>::numCols() const
  {
    return C;
  }

  template <typename T, uint16_t R, uint16_t C>
  size_t FixedSizeMatrix<T, R, C>::numElements() const
  {
    // Returns totalt number of elements in matrix
    return R * C;
  }

  template <typename T, uint16_t R, uint16_t C>
  size_t FixedSizeMatrix<T, R, C>::numBytes() const
  {
    return R * C * sizeof(T);
  }

  template <typename T, uint16_t R, uint16_t C>
  void FixedSizeMatrix<T, R, C>::fill(T val)
  {
    for (size_t k = 0; k < (R * C); k++)
    {
      data_[k] = val;
    }
  }

  template <typename T, uint16_t R, uint16_t C>
  T &FixedSizeMatrix<T, R, C>::operator()(const size_t r, const size_t c)
  {
    assert((r < R) && "Row index is larger than R - 1!");
    assert((c < C) && "Column index is larger than C - 1!");

    return data_[r * C + c];
  }

  template <typename T, uint16_t R, uint16_t C>
  const T &FixedSizeMatrix<T, R, C>::operator()(const size_t r,
                                                const size_t c) const
  {
    assert((r < R) && "Row index is larger than R - 1!");
    assert((c < C) && "Column index is larger than C - 1!");

    return data_[r * C + c];
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, C, R> FixedSizeMatrix<T, R, C>::transposed() const
  {
    FixedSizeMatrix<T, C, R> m_out;

    for (size_t r = 0; r < R; r++)
    {
      for (size_t c = 0; c < C; c++)
      {
        m_out(c, r) = (*this)(r, c);
      }
    }

    return m_out;
  }

  template <typename T, uint16_t R0, uint16_t C0, uint16_t R1, uint16_t C1>
  FixedSizeMatrix<T, R0, C1> operator*(const FixedSizeMatrix<T, R0, C0> &m0,
                                       const FixedSizeMatrix<T, R1, C1> &m1)
  {
    static_assert(C0 == R1);
    FixedSizeMatrix<T, R0, C1> res;

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        T p = 0.0;
        for (size_t i = 0; i < m0.numCols(); i++)
        {
          p = p + m0(r, i) * m1(i, c);
        }
        res(r, c) = p;
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> unitMatrix();

  template <typename T, uint16_t R, uint16_t C>
  std::optional<FixedSizeMatrix<T, R, C>> FixedSizeMatrix<T, R, C>::inverse() const
  {
    static_assert(R == C, "Matrix must be square to invert.");
    FixedSizeMatrix<T, R, C> a(*this);
    FixedSizeMatrix<T, R, C> inv = unitMatrix<T, R, C>();

    for (size_t i = 0; i < R; ++i)
    {
      // Find pivot
      const T pivot = a(i, i);
      if (std::abs(pivot) < 1e-12)
      {
        return std::nullopt;
      }
      // Normalize row
      for (size_t j = 0; j < C; ++j)
      {
        a(i, j) /= pivot;
        inv(i, j) /= pivot;
      }
      // Eliminate other rows
      for (size_t k = 0; k < R; ++k)
      {
        if (k == i)
          continue;
        T factor = a(k, i);
        for (size_t j = 0; j < C; ++j)
        {
          a(k, j) -= factor * a(i, j);
          inv(k, j) -= factor * inv(i, j);
        }
      }
    }
    return inv;
  }

  template <typename T, uint16_t R, uint16_t C>
  std::optional<LUMatrices<T, R, C>> FixedSizeMatrix<T, R, C>::luDecomposition() const
  {
    LUMatrices<T, R, C> result;
    constexpr uint16_t K = LUMatrices<T, R, C>::K;
    auto &L = result.l_matrix;
    auto &U = result.u_matrix;
    auto &P = result.row_permutation;

    // Copy of A (mutable)
    FixedSizeMatrix<T, R, C> A = *this;

    // Initialize permutation vector
    for (uint16_t i = 0; i < R; ++i)
    {
      P[i] = i;
    }

    for (uint16_t i = 0; i < K; ++i)
    {
      // Partial pivoting: find max in column i
      uint16_t pivot_row = i;
      T max_val = std::abs(A(P[i], i));
      for (uint16_t j = i + 1; j < R; ++j)
      {
        T val = std::abs(A(P[j], i));
        if (val > max_val)
        {
          max_val = val;
          pivot_row = j;
        }
      }

      // Failure if pivot is too small
      if (max_val < T(1e-12))
      {
        return std::nullopt; // Matrix is singular or nearly singular
      }

      // Swap rows in permutation vector
      if (pivot_row != i)
      {
        std::swap(P[i], P[pivot_row]);
      }

      // Compute U(i, j)
      for (uint16_t j = i; j < C; ++j)
      {
        T sum = 0;
        for (uint16_t k = 0; k < i; ++k)
        {
          sum += L(i, k) * U(k, j);
        }

        U(i, j) = A(P[i], j) - sum;
      }

      // Compute L(j, i)
      for (uint16_t j = i + 1; j < R; ++j)
      {
        T sum = 0;
        for (uint16_t k = 0; k < i; ++k)
        {
          sum += L(j, k) * U(k, i);
        }

        T u_ii = U(i, i);
        if (std::abs(u_ii) < T(1e-12))
        {
          return std::nullopt; // division by zero risk
        }

        L(j, i) = (A(P[j], i) - sum) / u_ii;
      }
    }

    // Set unit diagonal in L
    for (uint16_t i = 0; i < K; ++i)
    {
      L(i, i) = 1;
    }

    return result;
  }

  template <typename T, uint16_t R, uint16_t C>
  std::optional<SVDMatrices<T, R, C>> FixedSizeMatrix<T, R, C>::svd() const
  {
    static_assert(R >= C, "SVD requires R >= C.");

    constexpr uint16_t M = R;
    constexpr uint16_t N = C;

    constexpr uint16_t max_iterations{100U};
    constexpr T tol{1e-9};

    // Working copy
    FixedSizeMatrix<T, M, N> A = *this;

    // Initialize U and V
    FixedSizeMatrix<T, M, M> U;
    FixedSizeMatrix<T, N, N> V;

    // U = Identity
    for (uint16_t i = 0; i < M; i++)
    {
      for (uint16_t j = 0; j < M; j++)
      {
        U(i, j) = (i == j) ? T(1) : T(0);
      }
    }

    // V = Identity
    for (uint16_t i = 0; i < N; i++)
    {
      for (uint16_t j = 0; j < N; j++)
      {
        V(i, j) = (i == j) ? T(1) : T(0);
      }
    }

    // Jacobi rotations
    for (uint16_t iter = 0; iter < max_iterations; ++iter)
    {
      bool converged = true;
      for (uint16_t p = 0; p < N - 1; ++p)
      {
        for (uint16_t q = p + 1; q < N; ++q)
        {
          T alpha = T(0), beta = T(0), gamma = T(0);
          for (uint16_t i = 0; i < M; ++i)
          {
            alpha += A(i, p) * A(i, p);
            beta += A(i, q) * A(i, q);
            gamma += A(i, p) * A(i, q);
          }

          if (std::abs(gamma) <= tol * std::sqrt(alpha * beta))
            continue;

          converged = false;

          T zeta = (beta - alpha) / (2 * gamma);
          T t = ((zeta >= 0) ? 1 : -1) /
                (std::abs(zeta) + std::sqrt(1 + zeta * zeta));
          T c = 1 / std::sqrt(1 + t * t);
          T s = c * t;

          for (uint16_t i = 0; i < M; ++i)
          {
            T aip = A(i, p);
            T aiq = A(i, q);
            A(i, p) = c * aip - s * aiq;
            A(i, q) = s * aip + c * aiq;
          }

          for (uint16_t i = 0; i < N; ++i)
          {
            T vip = V(i, p);
            T viq = V(i, q);
            V(i, p) = c * vip - s * viq;
            V(i, q) = s * vip + c * viq;
          }
        }
      }
      if (converged)
        break;

      if (iter == max_iterations - 1)
        return std::nullopt; // failed to converge
    }

    // Compute singular values and U
    FixedSizeMatrix<T, M, N> S_diag;
    S_diag.fill(T(0));
    for (uint16_t j = 0; j < N; ++j)
    {
      T norm = T(0);
      for (uint16_t i = 0; i < M; ++i)
        norm += A(i, j) * A(i, j);
      norm = std::sqrt(norm);

      for (uint16_t i = 0; i < M; ++i)
        U(i, j) = (norm > tol) ? A(i, j) / norm : 0;

      if (j < M && j < N)
        S_diag(j, j) = norm;
    }

    // Fill result
    SVDMatrices<T, R, C> result;
    // The matrix is u_matrix * sigma_matrix * v_matrix.transposed()
    result.u_matrix = U;
    result.sigma_matrix = S_diag;
    result.v_matrix = V;

    return result;
  }

  template <typename T, uint16_t R, uint16_t C>
  std::optional<QRResult<T, R, C>> FixedSizeMatrix<T, R, C>::qrDecomposition() const
  {
    static_assert(R >= C, "QR decomposition requires R >= C.");

    QRResult<T, R, C> result;
    FixedSizeMatrix<T, R, C> A_work = *this; // Working copy

    // Store Householder reflection vectors
    FixedSizeMatrix<T, R, C> householder_vectors{};
    FixedSizeMatrix<T, C, 1> householder_betas{}; // Store beta = 2/||v||^2

    // Forward elimination using Householder reflections
    for (uint16_t k = 0; k < C; ++k)
    {
      // Compute 2-norm of column k from row k downwards
      T norm_x = T(0);
      for (uint16_t i = k; i < R; ++i)
      {
        norm_x += A_work(i, k) * A_work(i, k);
      }
      norm_x = std::sqrt(norm_x);

      if (norm_x == T(0))
      {
        return std::nullopt; // Degenerate case
      }

      // Choose sign to avoid catastrophic cancellation
      T sign = (A_work(k, k) >= 0) ? T(1) : T(-1);

      // Form Householder vector v
      for (uint16_t i = 0; i < k; ++i)
      {
        householder_vectors(i, k) = T(0);
      }
      householder_vectors(k, k) = A_work(k, k) + sign * norm_x;
      for (uint16_t i = k + 1; i < R; ++i)
      {
        householder_vectors(i, k) = A_work(i, k);
      }

      // Compute norm squared of v and beta = 2/||v||^2
      T norm_v_sq = T(0);
      for (uint16_t i = k; i < R; ++i)
      {
        norm_v_sq += householder_vectors(i, k) * householder_vectors(i, k);
      }

      if (norm_v_sq == T(0))
      {
        householder_betas(k, 0) = T(0);
        continue; // Skip if v is zero
      }

      householder_betas(k, 0) = T(2) / norm_v_sq;

      // Apply Householder reflection to A_work: H*A where H = I - beta*v*v^T
      for (uint16_t j = k; j < C; ++j)
      {
        T dot = T(0);
        for (uint16_t i = k; i < R; ++i)
        {
          dot += householder_vectors(i, k) * A_work(i, j);
        }
        T factor = householder_betas(k, 0) * dot;

        for (uint16_t i = k; i < R; ++i)
        {
          A_work(i, j) -= factor * householder_vectors(i, k);
        }
      }
    }

    // Construct Q by applying Householder transformations to identity matrix
    // Start with identity
    FixedSizeMatrix<T, R, C> Q{};
    for (uint16_t i = 0; i < R; ++i)
    {
      for (uint16_t j = 0; j < C; ++j)
      {
        Q(i, j) = (i == j) ? T(1) : T(0);
      }
    }

    // Apply Householder transformations in reverse order to build Q
    for (int k = C - 1; k >= 0; --k)
    {
      if (householder_betas(k, 0) == T(0))
        continue;

      // Apply H_k to Q: Q := H_k * Q where H_k = I - beta_k * v_k * v_k^T
      for (uint16_t j = 0; j < C; ++j)
      {
        T dot = T(0);
        for (uint16_t i = k; i < R; ++i)
        {
          dot += householder_vectors(i, k) * Q(i, j);
        }
        T factor = householder_betas(k, 0) * dot;

        for (uint16_t i = k; i < R; ++i)
        {
          Q(i, j) -= factor * householder_vectors(i, k);
        }
      }
    }

    // Extract R from upper triangular part of A_work
    FixedSizeMatrix<T, C, C> R_matrix{};
    for (uint16_t i = 0; i < C; ++i)
    {
      for (uint16_t j = 0; j < C; ++j)
      {
        R_matrix(i, j) = (j >= i) ? A_work(i, j) : T(0);
      }
    }

    // Ensure R has positive diagonal elements by flipping signs if needed
    for (uint16_t k = 0; k < C; ++k)
    {
      if (R_matrix(k, k) < T(0))
      {
        // Flip signs in R row k
        for (uint16_t j = k; j < C; ++j)
        {
          R_matrix(k, j) = -R_matrix(k, j);
        }
        // Flip signs in Q column k
        for (uint16_t i = 0; i < R; ++i)
        {
          Q(i, k) = -Q(i, k);
        }
      }
    }

    result.q = Q;
    result.r = R_matrix;
    return result;
  }

  // Frobenius norm
  template <typename T, uint16_t R, uint16_t C>
  T FixedSizeMatrix<T, R, C>::frobeniusNorm() const
  {
    T sum_sq = 0;
    for (uint16_t i = 0; i < R * C; ++i)
    {
      sum_sq += data_[i] * data_[i];
    }
    return std::sqrt(sum_sq);
  }

  // 1-norm (max column sum)
  template <typename T, uint16_t R, uint16_t C>
  T FixedSizeMatrix<T, R, C>::oneNorm() const
  {
    T max_sum = 0;
    for (uint16_t col = 0; col < C; ++col)
    {
      T col_sum = 0;
      for (uint16_t row = 0; row < R; ++row)
      {
        col_sum += std::abs((*this)(row, col));
      }
      max_sum = std::max(max_sum, col_sum);
    }
    return max_sum;
  }

  // Infinity norm (max row sum)
  template <typename T, uint16_t R, uint16_t C>
  T FixedSizeMatrix<T, R, C>::infNorm() const
  {
    T max_sum = 0;
    for (uint16_t row = 0; row < R; ++row)
    {
      T row_sum = 0;
      for (uint16_t col = 0; col < C; ++col)
      {
        row_sum += std::abs((*this)(row, col));
      }
      max_sum = std::max(max_sum, row_sum);
    }
    return max_sum;
  }

  // General p-norm (vectorized over all elements)
  template <typename T, uint16_t R, uint16_t C>
  T FixedSizeMatrix<T, R, C>::pNorm(const T p) const
  {
    if (p <= 0)
    {
      return std::numeric_limits<T>::quiet_NaN();
    }
    T sum = 0;
    for (uint16_t i = 0; i < R * C; ++i)
    {
      sum += std::pow(std::abs(data_[i]), p);
    }
    return std::pow(sum, T(1) / p);
  }

  template <typename T, uint16_t R, uint16_t C>
  EigenDecomposition<T, R, C> FixedSizeMatrix<T, R, C>::eigen() const
  {
    static_assert(R == C, "Matrix must be square for eigen decomposition.");
    // Dummy implementation: returns a default-constructed EigenDecomposition
    return EigenDecomposition<T, R, C>{};
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> FixedSizeMatrix<T, R, C>::cholesky() const
  {
    static_assert(R == C, "Matrix must be square for Cholesky decomposition.");
    // Dummy implementation: returns a unit matrix of the same size
    return unitMatrix<T, R, C>();
  }

  /*template <typename T, uint16_t R, uint16_t C>
  std::optional<T> FixedSizeMatrix<T, R, C>::frobeniusConditionNumber() const
  {
    auto inv_opt = inverse();
    if (!inv_opt.has_value())
      return std::nullopt;
    return frobeniusNorm() * inv_opt->frobeniusNorm();
  }

  template <typename T, uint16_t R, uint16_t C>
  std::optional<T> FixedSizeMatrix<T, R, C>::l1NormConditionNumber() const
  {
    auto inv_opt = inverse();
    if (!inv_opt.has_value())
      return std::nullopt;
    return oneNorm() * inv_opt->oneNorm();
  }

  template <typename T, uint16_t R, uint16_t C>
  std::optional<T> FixedSizeMatrix<T, R, C>::infConditionNumber() const
  {
    auto inv_opt = inverse();
    if (!inv_opt.has_value())
      return std::nullopt;
    return norm_inf() * inv_opt->norm_inf();
  }*/

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator+(const FixedSizeMatrix<T, R, C> &m0,
                                     const FixedSizeMatrix<T, R, C> &m1) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = m0(r, c) + m1(r, c);
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator-(const FixedSizeMatrix<T, R, C> &m0,
                                     const FixedSizeMatrix<T, R, C> &m1) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = m0(r, c) - m1(r, c);
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator*(const FixedSizeMatrix<T, R, C> &m,
                                     const T f) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = m(r, c) * f;
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator*(const T f,
                                     const FixedSizeMatrix<T, R, C> &m) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = f * m(r, c);
      }
    }
    return res;
  }

  // Element wise multiplication
  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator^(const FixedSizeMatrix<T, R, C> &m0,
                                     const FixedSizeMatrix<T, R, C> &m1) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = m0(r, c) * m1(r, c);
      }
    }
    return res;
  }

  // Element wise division
  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator/(const FixedSizeMatrix<T, R, C> &m0,
                                     const FixedSizeMatrix<T, R, C> &m1) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = m0(r, c) / m1(r, c);
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator/(const FixedSizeMatrix<T, R, C> &m,
                                     const T f) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = m(r, c) / f;
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator/(const T f,
                                     const FixedSizeMatrix<T, R, C> &m) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = f / m(r, c);
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator-(const FixedSizeMatrix<T, R, C> &m,
                                     const T f) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = m(r, c) - f;
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator-(const T f,
                                     const FixedSizeMatrix<T, R, C> &m) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = f - m(r, c);
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator+(const FixedSizeMatrix<T, R, C> &m,
                                     const T f) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = m(r, c) + f;
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator+(const T f,
                                     const FixedSizeMatrix<T, R, C> &m) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = f + m(r, c);
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> operator-(const FixedSizeMatrix<T, R, C> &m) {
    FixedSizeMatrix<T, R, C> res;

    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        res(r, c) = -m(r, c);
      }
    }
    return res;
  }

  template <typename T, uint16_t R, uint16_t C>
  bool operator==(const FixedSizeMatrix<T, R, C> &m0,
                  const FixedSizeMatrix<T, R, C> &m1) {
    for (size_t r = 0; r < R; r++) {
      for (size_t c = 0; c < C; c++) {
        if (m0(r, c) != m1(r, c)) {
          return false;
        }
      }
    }
    return true;
  }

  template <typename T, uint16_t R, uint16_t C>
  bool operator!=(const FixedSizeMatrix<T, R, C> &m0,
                  const FixedSizeMatrix<T, R, C> &m1) {
    return !(m0 == m1);
  }

  /*template <typename T, uint16_t R, uint16_t C> FixedSizeMatrix<T, R, C>
  FixedSizeMatrix<T, R, C>::getTranspose() const
  {
      FixedSizeMatrix<T, R, C> res(num_cols_, num_rows_);

      for (size_t r = 0; r < res.numRows(); r++)
      {
          for (size_t c = 0; c < res.numCols(); c++)
          {
              res(r, c) = data_[c * num_cols_ + r];
          }
      }
      return res;
  }*/

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> unitMatrix()
  {
    FixedSizeMatrix<T, R, C> unit_matrix;
    for (size_t r = 0; r < R; r++)
    {
      for (size_t c = 0; c < C; c++)
      {
        if (r == c)
        {
          unit_matrix(r, c) = 1.0;
        }
        else
        {
          unit_matrix(r, c) = 0.0;
        }
      }
    }
    return unit_matrix;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> zerosMatrix()
  {
    FixedSizeMatrix<T, R, C> zero_matrix;
    for (size_t r = 0; r < R; r++)
    {
      for (size_t c = 0; c < C; c++)
      {
        zero_matrix(r, c) = 0.0;
      }
    }
    return zero_matrix;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> onesMatrix()
  {
    FixedSizeMatrix<T, R, C> ones_matrix;
    for (size_t r = 0; r < R; r++)
    {
      for (size_t c = 0; c < C; c++)
      {
        ones_matrix(r, c) = 1.0;
      }
    }
    return ones_matrix;
  }

  template <typename T, uint16_t R, uint16_t C>
  T FixedSizeMatrix<T, R, C>::max() const
  {
    T max_val = data_[0];

    for (size_t r = 0; r < R; r++)
    {
      const size_t r_times_c = r * C;

      for (size_t c = 0; c < C; c++)
      {
        max_val = std::max(max_val, data_[r_times_c + c]);
      }
    }

    return max_val;
  }

  template <typename T, uint16_t R, uint16_t C>
  T FixedSizeMatrix<T, R, C>::min() const
  {
    T min_val = data_[0];

    for (size_t r = 0; r < R; r++)
    {
      const size_t r_times_c = r * C;

      for (size_t c = 0; c < C; c++)
      {
        min_val = std::min(min_val, data_[r_times_c + c]);
      }
    }

    return min_val;
  }

  template <typename T, uint16_t R, uint16_t C>
  T FixedSizeMatrix<T, R, C>::sum() const
  {
    T s = 0.0;
    for (size_t r = 0; r < R; r++)
    {
      const size_t r_times_c = r * C;
      for (size_t c = 0; c < C; c++)
      {
        s = s + data_[r_times_c + c];
      }
    }
    return s;
  }

  template <typename T>
  FixedSizeMatrix<T, 3, 3> fixedRotationMatrixX(const T angle)
  {
    const T ca = std::cos(angle);
    const T sa = std::sin(angle);
    FixedSizeMatrix<T, 3, 3> rotation_matrix;

    rotation_matrix(0, 0) = 1.0;
    rotation_matrix(0, 1) = 0.0;
    rotation_matrix(0, 2) = 0.0;

    rotation_matrix(1, 0) = 0.0;
    rotation_matrix(1, 1) = ca;
    rotation_matrix(1, 2) = -sa;

    rotation_matrix(2, 0) = 0.0;
    rotation_matrix(2, 1) = sa;
    rotation_matrix(2, 2) = ca;

    return rotation_matrix;
  }

  template <typename T>
  FixedSizeMatrix<T, 3, 3> fixedRotationMatrixY(const T angle)
  {
    const T ca = std::cos(angle);
    const T sa = std::sin(angle);
    FixedSizeMatrix<T, 3, 3> rotation_matrix;

    rotation_matrix(0, 0) = ca;
    rotation_matrix(0, 1) = 0.0;
    rotation_matrix(0, 2) = sa;

    rotation_matrix(1, 0) = 0.0;
    rotation_matrix(1, 1) = 1.0;
    rotation_matrix(1, 2) = 0.0;

    rotation_matrix(2, 0) = -sa;
    rotation_matrix(2, 1) = 0.0;
    rotation_matrix(2, 2) = ca;

    return rotation_matrix;
  }

  template <typename T>
  FixedSizeMatrix<T, 3, 3> fixedRotationMatrixZ(const T angle)
  {
    const T ca = std::cos(angle);
    const T sa = std::sin(angle);
    FixedSizeMatrix<T, 3, 3> rotation_matrix;

    rotation_matrix(0, 0) = ca;
    rotation_matrix(0, 1) = -sa;
    rotation_matrix(0, 2) = 0.0;

    rotation_matrix(1, 0) = sa;
    rotation_matrix(1, 1) = ca;
    rotation_matrix(1, 2) = 0.0;

    rotation_matrix(2, 0) = 0.0;
    rotation_matrix(2, 1) = 0.0;
    rotation_matrix(2, 2) = 1.0;

    return rotation_matrix;
  }

  template <typename T>
  FixedSizeMatrix<T, 2, 2> fixedRotationMatrix2D(const T angle)
  {
    const T ca = std::cos(angle);
    const T sa = std::sin(angle);
    FixedSizeMatrix<T, 2, 2> rotation_matrix;

    rotation_matrix(0, 0) = ca;
    rotation_matrix(0, 1) = -sa;

    rotation_matrix(1, 0) = sa;
    rotation_matrix(1, 1) = ca;

    return rotation_matrix;
  }

  template <typename T, uint16_t R, uint16_t C>
  FixedSizeMatrix<T, R, C> unitFixedSizeMatrix()
  {
    FixedSizeMatrix<T, R, C> unit_mat;

    for (size_t r = 0; r < R; r++)
    {
      for (size_t c = 0; c < C; c++)
      {
        if (r == c)
        {
          unit_mat(r, c) = 1.0;
        }
        else
        {
          unit_mat(r, c) = 0.0;
        }
      }
    }

    return unit_mat;
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_FIXED_MATRIX_FIXED_H_
