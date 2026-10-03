#ifndef LUMOS_MATH_IMAGE_IMAGE_GRAY_ALPHA_H_
#define LUMOS_MATH_IMAGE_IMAGE_GRAY_ALPHA_H_

#include <assert.h>
#include <stdint.h>

#include <cstring>
#include <iostream>

#include "lumos/math/misc/assert.h"

namespace lumos
{
  template <typename T>
  class ImageGrayAlphaView
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;
    size_t num_element_per_channel_;

  public:
    ImageGrayAlphaView()
        : data_{nullptr}, num_rows_{0U}, num_cols_{0U}, num_element_per_channel_{
                                                            0U} {}

    ImageGrayAlphaView(T *const data_ptr_in, const size_t num_rows,
                       const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols},
          num_element_per_channel_{num_rows_ * num_cols_} {}

    T *data() const { return data_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }

    size_t numBytes() const { return 2 * num_rows_ * num_cols_ * sizeof(T); }

    T &operator()(const size_t r, const size_t c, const size_t ch)
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
      assert((ch < 2) && "Channel index is larger than 1!");

      return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
    }

    const T &operator()(const size_t r, const size_t c, const size_t ch) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
      assert((ch < 2) && "Channel index is larger than 1!");

      return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
    }
  };

  template <typename T>
  class ImageGrayAlphaConstView
  {
  private:
    const T *data_;
    size_t num_rows_;
    size_t num_cols_;
    size_t num_element_per_channel_;

  public:
    ImageGrayAlphaConstView()
        : data_{nullptr}, num_rows_{0U}, num_cols_{0U}, num_element_per_channel_{
                                                            0U} {}

    ImageGrayAlphaConstView(const T *const data_ptr_in, const size_t num_rows,
                            const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols},
          num_element_per_channel_{num_rows_ * num_cols_} {}

    const T *data() const { return data_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }

    size_t numBytes() const { return 2 * num_rows_ * num_cols_ * sizeof(T); }

    const T &operator()(const size_t r, const size_t c, const size_t ch) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
      assert((ch < 2) && "Channel index is larger than 1!");

      return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
    }
  };

  template <typename T>
  class ImageGrayAlpha
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;
    size_t num_element_per_channel_;

  public:
    ImageGrayAlpha();
    ImageGrayAlpha(const size_t num_rows, const size_t num_cols);
    ~ImageGrayAlpha();

    T &operator()(const size_t r, const size_t c, const size_t ch);
    const T &operator()(const size_t r, const size_t c, const size_t ch) const;

    size_t numRows() const;
    size_t numCols() const;
    size_t numBytes() const;
    size_t numElements() const;
    void fillBufferWithData(uint8_t *const buffer) const;

    T *data() const;

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }
  };

  template <typename T>
  ImageGrayAlpha<T>::~ImageGrayAlpha()
  {
    if (num_rows_ > 0U)
    {
      delete[] data_;
    }
  }

  template <typename T>
  ImageGrayAlpha<T>::ImageGrayAlpha()
      : data_{nullptr}, num_rows_(0U), num_cols_(0U),
        num_element_per_channel_(0U) {}

  template <typename T>
  ImageGrayAlpha<T>::ImageGrayAlpha(const size_t num_rows,
                                    const size_t num_cols)
  {
    ASSERT(num_rows > 0U) << "Cannot initialize with number of rows to 0!";
    ASSERT(num_cols > 0U) << "Cannot initialize with number of columns to 0!";

    data_ = new T[num_rows * num_cols * 2];
    num_rows_ = num_rows;
    num_cols_ = num_cols;
    num_element_per_channel_ = num_rows_ * num_cols_;
  }

  template <typename T>
  void ImageGrayAlpha<T>::fillBufferWithData(uint8_t *const buffer) const
  {
    const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
    const size_t num_bytes = num_rows_ * num_cols_ * 2 * sizeof(T);
    std::memcpy(buffer, internal_ptr, num_bytes);
  }

  template <typename T>
  size_t ImageGrayAlpha<T>::numBytes() const
  {
    return 2 * num_rows_ * num_cols_ * sizeof(T);
  }

  template <typename T>
  size_t ImageGrayAlpha<T>::numElements() const
  {
    return 2 * num_rows_ * num_cols_;
  }

  template <typename T>
  size_t ImageGrayAlpha<T>::numRows() const
  {
    return num_rows_;
  }

  template <typename T>
  size_t ImageGrayAlpha<T>::numCols() const
  {
    return num_cols_;
  }

  template <typename T>
  T *ImageGrayAlpha<T>::data() const { return data_; }

  template <typename T>
  T &ImageGrayAlpha<T>::operator()(const size_t r, const size_t c,
                                   const size_t ch)
  {
    assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
    assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
    assert((ch < 2) && "Channel index is larger than 1!");

    return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
  }

  template <typename T>
  const T &ImageGrayAlpha<T>::operator()(const size_t r, const size_t c,
                                         const size_t ch) const
  {
    assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
    assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
    assert((ch < 2) && "Channel index is larger than 1!");

    return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
  }

} // namespace lumos

#endif // LUMOS_MATH_IMAGE_IMAGE_GRAY_ALPHA_H_
