#ifndef LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_MATRIX_DYNAMIC_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_MATRIX_DYNAMIC_H_

#include <cmath>
#include <cstring>

#include <iostream>
#include <string>
#include "lumos/math/misc/assert.h"
#include "lumos/math/lin_alg/matrix_dynamic/class_def/matrix_dynamic.h"
#include "lumos/math/misc/math_macros.h"

namespace lumos
{
  template <typename T>
  Matrix<T> &Matrix<T>::operator=(const Matrix<T> &m)
  {
    ASSERT((m.numRows() > 0U) && (m.numCols() > 0U))
        << "Input matrix not allocated before assignment!";
    if (this != &m)
    {
      if ((num_rows_ > 0U) && (num_cols_ > 0U))
      {
        if ((m.numRows() != num_rows_) || (m.numCols() != num_cols_))
        {
          delete[] data_;
          DATA_ALLOCATION(data_, m.numRows() * m.numCols(), T, "Matrix");
        }
      }
      else
      {
        DATA_ALLOCATION(data_, m.numRows() * m.numCols(), T, "Matrix");
      }

      num_rows_ = m.numRows();
      num_cols_ = m.numCols();

      for (size_t r = 0; r < m.numRows(); r++)
      {
        for (size_t c = 0; c < m.numCols(); c++)
        {
          data_[r * num_cols_ + c] = m(r, c);
        }
      }
    }
    return *this;
  }

  template <typename T>
  Matrix<T>::Matrix(Matrix<T> &&m)
  {
    ASSERT((m.numRows() > 0U) && (m.numCols() > 0U))
        << "Input matrix not allocated!";
    data_ = m.data_;
    num_rows_ = m.num_rows_;
    num_cols_ = m.num_cols_;

    m.data_ = nullptr;
    m.num_rows_ = 0;
    m.num_cols_ = 0;
  }

  template <typename T>
  template <typename Y>
  Matrix<T>::Matrix(const Matrix<Y> &m)
  {
    ASSERT((m.numRows() > 0U) && (m.numCols() > 0U))
        << "Input matrix not allocated!";
    num_rows_ = m.numRows();
    num_cols_ = m.numCols();

    DATA_ALLOCATION(data_, m.numRows() * m.numCols(), T, "Matrix");
    for (size_t r = 0; r < m.numRows(); r++)
    {
      for (size_t c = 0; c < m.numCols(); c++)
      {
        data_[r * m.numCols() + c] = m(r, c);
      }
    }
  }

  template <typename T>
  void Matrix<T>::fillBufferWithData(uint8_t *const buffer) const
  {
    const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
    const size_t num_bytes = num_rows_ * num_cols_ * sizeof(T);
    std::memcpy(buffer, internal_ptr, num_bytes);
  }

  template <typename T>
  Matrix<T> &Matrix<T>::operator=(Matrix<T> &&m)
  {
    if (this != &m)
    {
      ASSERT((m.numRows() > 0U) && (m.numCols() > 0U))
          << "Input matrix not allocated before assignment!";

      if ((num_rows_ > 0U) && (num_cols_ > 0U))
      {
        delete[] data_;
      }

      num_rows_ = m.numRows();
      num_cols_ = m.numCols();

      data_ = m.data();

      m.data_ = nullptr;
      m.num_rows_ = 0U;
      m.num_cols_ = 0U;
    }

    return *this;
  }

  template <typename T>
  Matrix<T>::Matrix() : data_(nullptr), num_rows_(0), num_cols_(0) {}

  template <typename T>
  Matrix<T>::Matrix(const size_t num_rows, const size_t num_cols)
  {
    ASSERT(num_rows > 0U) << "Cannot set number of rows to 0!";
    ASSERT(num_cols > 0U) << "Cannot set number of columns to 0!";

    num_rows_ = num_rows;
    num_cols_ = num_cols;

    DATA_ALLOCATION(data_, num_rows_ * num_cols_, T, "Matrix");
  }

  template <typename T>
  Matrix<T>::Matrix(const Matrix<T> &m)
  {
    ASSERT((m.numRows() > 0U) && (m.numCols() > 0U))
        << "Input matrix not allocated before assignment!";

    num_rows_ = m.numRows();
    num_cols_ = m.numCols();

    DATA_ALLOCATION(data_, m.numRows() * m.numCols(), T, "Matrix");
    for (size_t r = 0; r < m.numRows(); r++)
    {
      for (size_t c = 0; c < m.numCols(); c++)
      {
        data_[r * m.numCols() + c] = m(r, c);
      }
    }
  }

  template <typename T>
  Matrix<T>::~Matrix()
  {
    if ((num_rows_ > 0U) && (num_cols_ > 0U))
    {
      delete[] data_;
    }
  }

  template <typename T>
  size_t Matrix<T>::size() const
  {
    return num_rows_ * num_cols_;
  }

  template <typename T>
  void Matrix<T>::resize(const size_t num_rows, const size_t num_cols)
  {
    ASSERT(num_rows > 0U) << "Cannot set number of rows to 0!";
    ASSERT(num_cols > 0U) << "Cannot set number of columns to 0!";
    if ((num_rows_ > 0U) && (num_cols_ > 0U))
    {
      delete[] data_;
    }

    num_rows_ = num_rows;
    num_cols_ = num_cols;

    DATA_ALLOCATION(data_, num_rows_ * num_cols_, T, "Matrix");
  }

  template <typename T>
  T *Matrix<T>::data() const { return data_; }

  template <typename T>
  size_t Matrix<T>::lastRowIdx() const
  {
    ASSERT(num_rows_ != 0);
    return num_rows_ - 1;
  }

  template <typename T>
  size_t Matrix<T>::lastColIdx() const
  {
    ASSERT(num_cols_ != 0);
    return num_cols_ - 1;
  }

  template <typename T>
  size_t Matrix<T>::numRows() const { return num_rows_; }

  template <typename T>
  size_t Matrix<T>::numCols() const { return num_cols_; }

  template <typename T>
  size_t Matrix<T>::numElements() const
  {
    // Returns totalt number of elements in matrix
    return num_rows_ * num_cols_;
  }

  template <typename T>
  size_t Matrix<T>::numBytes() const
  {
    return num_rows_ * num_cols_ * sizeof(T);
  }

  template <typename T>
  void Matrix<T>::fill(T val)
  {
    ASSERT((num_rows_ > 0U) && (num_cols_ > 0U))
        << "Tried to fill unallocated matrix!";
    for (size_t k = 0; k < (num_rows_ * num_cols_); k++)
    {
      data_[k] = val;
    }
  }

  template <typename T>
  T &Matrix<T>::operator()(const size_t r, const size_t c)
  {
    assert((r < num_rows_) && "Row index is larger than num_rows_-1!");
    assert((c < num_cols_) && "Column index is larger than num_cols_-1!");

    return data_[r * num_cols_ + c];
  }

  template <typename T>
  const T &Matrix<T>::operator()(const size_t r, const size_t c) const
  {
    assert((r < num_rows_) && "Row index is larger than num_rows_-1!");
    assert((c < num_cols_) && "Column index is larger than num_cols_-1!");

    return data_[r * num_cols_ + c];
  }

  template <typename T>
  Matrix<T> operator*(const Matrix<T> &m0, const Matrix<T> &m1)
  {
    ASSERT(m0.numCols() == m1.numRows());
    Matrix<T> res(m0.numRows(), m1.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        T p = 0.0f;
        for (size_t i = 0; i < m0.numCols(); i++)
        {
          p = p + m0(r, i) * m1(i, c);
        }
        res(r, c) = p;
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator+(const Matrix<T> &m0, const Matrix<T> &m1)
  {
    ASSERT(m0.numCols() == m1.numCols());
    ASSERT(m0.numRows() == m1.numRows());
    Matrix<T> res(m0.numRows(), m1.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = m0(r, c) + m1(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator-(const Matrix<T> &m0, const Matrix<T> &m1)
  {
    ASSERT(m0.numCols() == m1.numCols());
    ASSERT(m0.numRows() == m1.numRows());
    Matrix<T> res(m0.numRows(), m1.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = m0(r, c) - m1(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator*(const Matrix<T> &m, const T f)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = f * m(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator*(const T f, const Matrix<T> &m)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = f * m(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator^(const Matrix<T> &m0, const Matrix<T> &m1)
  {
    ASSERT(m0.numRows() == m1.numRows());
    ASSERT(m0.numCols() == m1.numCols());

    Matrix<T> res(m0.numRows(), m0.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = m0(r, c) * m1(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator/(const Matrix<T> &m0, const Matrix<T> &m1)
  {
    ASSERT(m0.numRows() == m1.numRows());
    ASSERT(m0.numCols() == m1.numCols());

    Matrix<T> res(m0.numRows(), m0.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = m0(r, c) / m1(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator/(const Matrix<T> &m, const T f)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = m(r, c) / f;
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator/(const T f, const Matrix<T> &m)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = f / m(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator-(const Matrix<T> &m, const T f)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = m(r, c) - f;
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator-(const T f, const Matrix<T> &m)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = f - m(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator+(const Matrix<T> &m, const T f)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = m(r, c) + f;
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator+(const T f, const Matrix<T> &m)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = m(r, c) + f;
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> operator-(const Matrix<T> &m)
  {
    Matrix<T> res(m.numRows(), m.numCols());

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = -m(r, c);
      }
    }
    return res;
  }

  template <typename T>
  Vector<T> operator*(const Matrix<T> &m, const Vector<T> &v)
  {
    ASSERT(m.numCols() == v.size());
    Vector<T> res(m.numRows());

    for (size_t r = 0; r < m.numRows(); r++)
    {
      T p = 0.0f;
      for (size_t c = 0; c < m.numCols(); c++)
      {
        p = p + m(r, c) * v(c);
      }
      res(r) = p;
    }
    return res;
  }

  template <typename T>
  Vector<T> operator*(const Vector<T> &v, const Matrix<T> &m)
  {
    ASSERT(m.numRows() == v.size());
    Vector<T> res(m.numCols());

    for (size_t c = 0; c < m.numCols(); c++)
    {
      T p = 0.0f;
      for (size_t r = 0; r < m.numRows(); r++)
      {
        p = p + m(r, c) * v(r);
      }
      res(c) = p;
    }
    return res;
  }

  template <typename T>
  Matrix<T> Matrix<T>::getTranspose() const
  {
    Matrix<T> res(num_cols_, num_rows_);

    for (size_t r = 0; r < res.numRows(); r++)
    {
      for (size_t c = 0; c < res.numCols(); c++)
      {
        res(r, c) = data_[c * num_cols_ + r];
      }
    }
    return res;
  }

  template <typename T>
  Matrix<T> unitMatrix(const size_t rows, const size_t cols)
  {
    Matrix<T> unit_matrix(rows, cols);
    for (size_t r = 0; r < rows; r++)
    {
      for (size_t c = 0; c < cols; c++)
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

  template <typename T>
  Matrix<T> zerosMatrix(const size_t rows, const size_t cols)
  {
    Matrix<T> zero_matrix(rows, cols);
    for (size_t r = 0; r < rows; r++)
    {
      for (size_t c = 0; c < cols; c++)
      {
        zero_matrix(r, c) = 0.0;
      }
    }
    return zero_matrix;
  }

  template <typename T>
  Matrix<T> onesMatrix(const size_t rows, const size_t cols)
  {
    Matrix<T> ones_matrix(rows, cols);
    for (size_t r = 0; r < rows; r++)
    {
      for (size_t c = 0; c < cols; c++)
      {
        ones_matrix(r, c) = 1.0;
      }
    }
    return ones_matrix;
  }

  template <typename T>
  Matrix<T> filledMatrix(const size_t rows, const size_t cols, T val)
  {
    Matrix<T> filled_matrix(rows, cols);
    for (size_t r = 0; r < rows; r++)
    {
      for (size_t c = 0; c < cols; c++)
      {
        filled_matrix(r, c) = val;
      }
    }
    return filled_matrix;
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const Matrix<T> &m)
  {
    std::string s = "";

    for (size_t r = 0; r < m.numRows(); r++)
    {
      s = s + "[ ";
      for (size_t c = 0; c < m.numCols(); c++)
      {
        s = s + std::to_string(m(r, c));
        if (c != m.numCols() - 1)
        {
          s = s + ", ";
        }
      }
      s = s + " ]\n";
    }

    os << s;

    return os;
  }

  template <typename T>
  T Matrix<T>::max() const
  {
    ASSERT_MAT_VALID_INTERNAL();

    T max_val = data_[0];

    for (size_t r = 0; r < num_rows_; r++)
    {
      const size_t r_times_num_cols = r * num_cols_;

      for (size_t c = 0; c < num_cols_; c++)
      {
        max_val = std::max(max_val, data_[r_times_num_cols + c]);
      }
    }

    return max_val;
  }

  template <typename T>
  T Matrix<T>::min() const
  {
    ASSERT_MAT_VALID_INTERNAL();

    T min_val = data_[0];

    for (size_t r = 0; r < num_rows_; r++)
    {
      const size_t r_times_num_cols = r * num_cols_;

      for (size_t c = 0; c < num_cols_; c++)
      {
        min_val = std::min(min_val, data_[r_times_num_cols + c]);
      }
    }

    return min_val;
  }

  template <typename T>
  T Matrix<T>::sum() const
  {
    T s = 0.0;
    for (size_t r = 0; r < num_rows_; r++)
    {
      const size_t r_times_num_cols = r * num_cols_;

      for (size_t c = 0; c < num_cols_; c++)
      {
        s = s + data_[r_times_num_cols + c];
      }
    }
    return s;
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_MATRIX_DYNAMIC_H_
