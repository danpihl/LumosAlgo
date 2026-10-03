#ifndef LUMOS_MATH_IMAGE_IMAGE_GRAY_H_
#define LUMOS_MATH_IMAGE_IMAGE_GRAY_H_

#include <assert.h>
#include <stdint.h>

#include <cstring>
#include <iostream>

#include "lumos/math/misc/assert.h"

namespace lumos
{
  template <typename T>
  class ImageGrayView
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    ImageGrayView() : data_{nullptr}, num_rows_{0U}, num_cols_{0U} {}

    ImageGrayView(T *const data_ptr_in, const size_t num_rows,
                  const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols} {}

    T *data() const { return data_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }

    size_t numBytes() const { return num_rows_ * num_cols_ * sizeof(T); }

    size_t numElements() const { return num_rows_ * num_cols_; }

    T &operator()(const size_t r, const size_t c)
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");

      return data_[r * num_cols_ + c];
    }

    const T &operator()(const size_t r, const size_t c) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");

      return data_[r * num_cols_ + c];
    }
  };

  template <typename T>
  class ImageGrayConstView
  {
  private:
    const T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    ImageGrayConstView() : data_{nullptr}, num_rows_{0U}, num_cols_{0U} {}

    ImageGrayConstView(const T *const data_ptr_in, const size_t num_rows,
                       const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols} {}

    const T *data() const { return data_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }

    size_t numBytes() const { return num_rows_ * num_cols_ * sizeof(T); }

    size_t numElements() const { return num_rows_ * num_cols_; }

    const T &operator()(const size_t r, const size_t c) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");

      return data_[r * num_cols_ + c];
    }
  };

  template <typename T>
  class ImageGray
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;

  public:
    ImageGray();
    ImageGray(const size_t num_rows, const size_t num_cols);
    ~ImageGray();

    T &operator()(const size_t r, const size_t c);
    const T &operator()(const size_t r, const size_t c) const;

    size_t numRows() const;
    size_t numCols() const;
    size_t numBytes() const;
    void fillBufferWithData(uint8_t *const buffer) const;

    void resize(const size_t num_rows_, const size_t num_cols_);

    size_t numElements() const;
    T *data() const;

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }
  };

  template <typename T>
  ImageGray<T>::~ImageGray()
  {
    if (num_rows_ > 0U)
    {
      delete[] data_;
    }
  }

  template <typename T>
  ImageGray<T>::ImageGray() : data_{nullptr}, num_rows_{0}, num_cols_{0} {}

  template <typename T>
  ImageGray<T>::ImageGray(const size_t num_rows, const size_t num_cols)
  {
    ASSERT(num_rows > 0U) << "Cannot initialize with number of rows to 0!";
    ASSERT(num_cols > 0U) << "Cannot initialize with number of columns to 0!";
    data_ = new T[num_rows * num_cols];
    num_rows_ = num_rows;
    num_cols_ = num_cols;
  }

  template <typename T>
  void ImageGray<T>::fillBufferWithData(uint8_t *const buffer) const
  {
    const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
    const size_t num_bytes = num_rows_ * num_cols_ * sizeof(T);
    std::memcpy(buffer, internal_ptr, num_bytes);
  }

  template <typename T>
  size_t ImageGray<T>::numBytes() const
  {
    return num_rows_ * num_cols_ * sizeof(T);
  }

  template <typename T>
  void ImageGray<T>::resize(const size_t num_rows, const size_t num_cols)
  {
    ASSERT(num_rows > 0U) << "Cannot initialize with number of rows to 0!";
    ASSERT(num_cols > 0U) << "Cannot initialize with number of columns to 0!";

    if (num_rows_ > 0U)
    {
      delete[] data_;
    }

    data_ = new T[num_rows * num_cols];
    num_rows_ = num_rows;
    num_cols_ = num_cols;
  }

  template <typename T>
  size_t ImageGray<T>::numRows() const { return num_rows_; }

  template <typename T>
  size_t ImageGray<T>::numCols() const { return num_cols_; }

  template <typename T>
  size_t ImageGray<T>::numElements() const
  {
    return num_rows_ * num_cols_;
  }

  template <typename T>
  T *ImageGray<T>::data() const { return data_; }

  template <typename T>
  T &ImageGray<T>::operator()(const size_t r, const size_t c)
  {
    assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
    assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");

    return data_[r * num_cols_ + c];
  }

  template <typename T>
  const T &ImageGray<T>::operator()(const size_t r, const size_t c) const
  {
    assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
    assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");

    return data_[r * num_cols_ + c];
  }

} // namespace lumos

#endif // LUMOS_MATH_IMAGE_IMAGE_GRAY_H_
