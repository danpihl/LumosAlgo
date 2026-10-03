#ifndef LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_CLASS_DEF_MATRIX_DYNAMIC_H_
#define LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_CLASS_DEF_MATRIX_DYNAMIC_H_

#include <assert.h>
#include <cstddef>
#include <vector>
#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <algorithm>

#include <cstring>
#include <new>
#include "lumos/math/misc/assert.h"
#include "lumos/math/misc/forward_decl.h"

namespace lumos
{
  template <typename T>
  class MatrixInitializer
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;
    friend class Matrix<T>;

  public:
    MatrixInitializer() = delete;
    MatrixInitializer(const MatrixInitializer<T> &v) = delete;
    MatrixInitializer(MatrixInitializer<T> &&v);
    MatrixInitializer<T> &operator=(const MatrixInitializer<T> &v) = delete;
    MatrixInitializer<T> &operator=(MatrixInitializer<T> &&v) = delete;
    explicit MatrixInitializer(
        const std::initializer_list<std::initializer_list<T>> &il)
    {
      ASSERT(il.size() > 0) << "Tried to initialize with empty vector matrix!";
      ASSERT(il.begin()[0].size() > 0)
          << "Tried to initialize with empty vector matrix!";

      for (size_t r = 0; r < il.size(); r++)
      {
        ASSERT(il.begin()[0].size() == il.begin()[r].size())
            << "All row vectors in input std vectors do not have the same size!";
      }

      num_rows_ = il.size();
      num_cols_ = il.begin()[0].size();

      DATA_ALLOCATION(data_, num_cols_ * num_rows_, T, "MatrixInitializer");

      for (size_t r = 0; r < il.size(); r++)
      {
        for (size_t c = 0; c < il.begin()[r].size(); c++)
        {
          data_[r * num_cols_ + c] = il.begin()[r].begin()[c];
        }
      }
    }

    ~MatrixInitializer() { delete[] data_; }
  };

  template <typename T>
  class MatrixView
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    MatrixView() : data_{nullptr}, num_rows_{0U}, num_cols_{0U} {}

    MatrixView(T *const data_ptr_in, const size_t num_rows, const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols} {}

    T *data() const { return data_; }

    size_t size() const { return num_rows_ * num_cols_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t numElements() const { return num_rows_ * num_cols_; }

    size_t numBytes() const { return num_rows_ * num_cols_ * sizeof(T); }

    void fillBufferWithData(uint8_t *const buffer) const
    {
      const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
      const size_t num_bytes = num_rows_ * num_cols_ * sizeof(T);

      std::memcpy(buffer, internal_ptr, num_bytes);
    }

    T &operator()(const size_t r, const size_t c)
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_-1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_-1!");

      return data_[r * num_cols_ + c];
    }

    const T &operator()(const size_t r, const size_t c) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_-1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_-1!");

      return data_[r * num_cols_ + c];
    }

    std::pair<T, T> findMinMax() const
    {
      T min_value = data_[0], max_value = data_[0];

      for (size_t r = 0; r < num_rows_; r++)
      {
        const size_t idx = r * num_cols_;
        for (size_t c = 0; c < num_cols_; c++)
        {
          const T val = data_[idx + c];
          if (val < min_value)
          {
            min_value = val;
          }
          if (val > max_value)
          {
            max_value = val;
          }
        }
      }

      return {min_value, max_value};
    }
  };

  template <typename T>
  class MatrixConstView
  {
  private:
    const T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    MatrixConstView() : data_{nullptr}, num_rows_{0U}, num_cols_{0U} {}

    MatrixConstView(const T *const data_ptr_in, const size_t num_rows,
                    const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols} {}

    const T *data() const { return data_; }

    size_t size() const { return num_rows_ * num_cols_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t numElements() const { return num_rows_ * num_cols_; }

    size_t numBytes() const { return num_rows_ * num_cols_ * sizeof(T); }

    void fillBufferWithData(uint8_t *const buffer) const
    {
      const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
      const size_t num_bytes = num_rows_ * num_cols_ * sizeof(T);

      std::memcpy(buffer, internal_ptr, num_bytes);
    }

    const T &operator()(const size_t r, const size_t c) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_-1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_-1!");

      return data_[r * num_cols_ + c];
    }

    std::pair<T, T> findMinMax() const
    {
      T min_value = data_[0], max_value = data_[0];

      for (size_t r = 0; r < num_rows_; r++)
      {
        const size_t idx = r * num_cols_;
        for (size_t c = 0; c < num_cols_; c++)
        {
          const T val = data_[idx + c];
          if (val < min_value)
          {
            min_value = val;
          }
          if (val > max_value)
          {
            max_value = val;
          }
        }
      }

      return {min_value, max_value};
    }
  };

  template <typename T>
  class Matrix
  {
  protected:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    Matrix();
    Matrix(const size_t num_rows, const size_t num_cols);
    Matrix(const Matrix<T> &m);
    template <typename Y>
    Matrix(const Matrix<Y> &m);

    Matrix(MatrixInitializer<T> &&m)
    {
      data_ = m.data_;
      num_rows_ = m.num_rows_;
      num_cols_ = m.num_cols_;

      m.data_ = nullptr;
      m.num_rows_ = 0U;
      m.num_cols_ = 0U;
    }
    Matrix(Matrix<T> &&m);
    ~Matrix();

    void fillBufferWithData(uint8_t *const buffer) const;

    T &operator()(const size_t r, const size_t c);
    const T &operator()(const size_t r, const size_t c) const;

    Matrix<T> &operator=(const Matrix<T> &m);
    Matrix<T> &operator=(Matrix<T> &&m);

    MatrixView<T> view() const { return MatrixView{data_, num_rows_, num_cols_}; }

    MatrixConstView<T> constView() const
    {
      return MatrixConstView{data_, num_rows_, num_cols_};
    }

    void resize(const size_t num_rows, const size_t num_cols);
    size_t numRows() const;
    size_t numCols() const;
    size_t size() const;
    size_t numElements() const;
    size_t numBytes() const;

    void fill(const T val);
    T *data() const;
    Matrix<T> getTranspose() const;

    size_t lastRowIdx() const;
    size_t lastColIdx() const;

    T max() const;
    T min() const;
    T sum() const;
  };

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_MATRIX_DYNAMIC_CLASS_DEF_MATRIX_DYNAMIC_H_
