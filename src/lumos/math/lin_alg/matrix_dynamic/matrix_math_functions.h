
#ifndef LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_MATRIX_MATH_FUNCTIONS_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_MATRIX_MATH_FUNCTIONS_H_

#include <cmath>
#include <cstdarg>
#include <utility>

#include "lumos/math/misc/assert.h"
#include "lumos/math/lin_alg/matrix_dynamic/matrix_dynamic.h"

namespace lumos
{
  template <typename T>
  Vector<T> linspaceFromBoundariesAndCount(const T x0, const T x1,
                                           const size_t num_values);

  template <typename T>
  std::pair<Matrix<T>, Matrix<T>> meshGrid(const T x0, const T x1, const T y0,
                                           const T y1, const size_t xn,
                                           const size_t yn)
  {
    const Vector<T> x_vec = linspaceFromBoundariesAndCount(x0, x1, xn);
    const Vector<T> y_vec = linspaceFromBoundariesAndCount(y0, y1, yn);

    Matrix<T> x_mat(yn, xn), y_mat(yn, xn);
    for (size_t r = 0; r < yn; r++)
    {
      for (size_t c = 0; c < xn; c++)
      {
        x_mat(r, c) = x_vec(c);
        y_mat(r, c) = y_vec(r);
      }
    }

    return std::pair<Matrix<T>, Matrix<T>>(std::move(x_mat), std::move(y_mat));
  }

  template <typename T>
  std::pair<Matrix<T>, Matrix<T>> meshGrid(const Vector<T> &x_vec,
                                           const Vector<T> &y_vec)
  {
    Matrix<T> x_mat(y_vec.size(), x_vec.size()),
        y_mat(y_vec.size(), x_vec.size());

    for (size_t r = 0; r < y_vec.size(); r++)
    {
      for (size_t c = 0; c < x_vec.size(); c++)
      {
        x_mat(r, c) = x_vec(c);
        y_mat(r, c) = y_vec(r);
      }
    }

    return std::pair<Matrix<T>, Matrix<T>>(x_mat, y_mat);
  }

  template <typename T>
  Matrix<T> log10(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::log10(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> pow(const Matrix<T> &m_in, const T e)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::pow(m_in(r, c), e);
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> log(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::log(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> exp(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::exp(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> cos(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::cos(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> sin(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::sin(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> cosh(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::cosh(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> sinh(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::sinh(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> sqrt(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::sqrt(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> elementWiseMultiply(const Matrix<T> &m0, const Matrix<T> &m1)
  {
    ASSERT((m0.numRows() > 0) && (m0.numCols() > 0));
    ASSERT((m0.numRows() == m1.numRows()) && (m0.numCols() == m1.numCols()));
    Matrix<T> m(m0.numRows(), m0.numCols());

    for (size_t r = 0; r < m0.numRows(); r++)
    {
      for (size_t c = 0; c < m0.numCols(); c++)
      {
        m(r, c) = m0(r, c) * m1(r, c);
      }
    }

    return m;
  }

  template <typename T>
  T max(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    T max_val = m_in(0, 0);

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        max_val = std::max(max_val, m_in(r, c));
      }
    }

    return max_val;
  }

  template <typename T>
  T min(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    T min_val = m_in(0, 0);

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        min_val = std::min(min_val, m_in(r, c));
      }
    }

    return min_val;
  }

  template <typename T>
  Matrix<T> abs(const Matrix<T> &m_in)
  {
    ASSERT((m_in.numRows() > 0) && (m_in.numCols() > 0));
    Matrix<T> m(m_in.numRows(), m_in.numCols());

    for (size_t r = 0; r < m_in.numRows(); r++)
    {
      for (size_t c = 0; c < m_in.numCols(); c++)
      {
        m(r, c) = std::fabs(m_in(r, c));
      }
    }

    return m;
  }

  template <typename T>
  Matrix<T> rotationMatrixX(const T angle)
  {
    const T ca = std::cos(angle);
    const T sa = std::sin(angle);
    Matrix<T> rotation_matrix(3, 3);

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
  Matrix<T> rotationMatrixY(const T angle)
  {
    const T ca = std::cos(angle);
    const T sa = std::sin(angle);
    Matrix<T> rotation_matrix(3, 3);

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
  Matrix<T> rotationMatrixZ(const T angle)
  {
    const T ca = std::cos(angle);
    const T sa = std::sin(angle);
    Matrix<T> rotation_matrix(3, 3);

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
  Matrix<T> rotationMatrix2D(const T angle)
  {
    const T ca = std::cos(angle);
    const T sa = std::sin(angle);
    Matrix<T> rotation_matrix(2, 2);

    rotation_matrix(0, 0) = ca;
    rotation_matrix(0, 1) = -sa;

    rotation_matrix(1, 0) = sa;
    rotation_matrix(1, 1) = ca;

    return rotation_matrix;
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_MATRIX_MATH_FUNCTIONS_H_
