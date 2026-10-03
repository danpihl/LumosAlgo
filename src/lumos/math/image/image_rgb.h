#ifndef LUMOS_MATH_IMAGE_IMAGE_RGB_H_
#define LUMOS_MATH_IMAGE_IMAGE_RGB_H_

#include <assert.h>
#include <stdint.h>

#include <cstring>
#include <iostream>

#include "lumos/math/misc/assert.h"

namespace lumos
{
  template <typename T>
  class ImageRGBView
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;
    size_t num_element_per_channel_;

  public:
    ImageRGBView()
        : data_{nullptr}, num_rows_{0U}, num_cols_{0U}, num_element_per_channel_{
                                                            0U} {}

    ImageRGBView(T *const data_ptr_in, const size_t num_rows,
                 const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols},
          num_element_per_channel_{num_rows_ * num_cols_} {}

    T *data() const { return data_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }

    size_t numBytes() const { return 3 * num_rows_ * num_cols_ * sizeof(T); }

    T &operator()(const size_t r, const size_t c, const size_t ch)
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
      assert((ch < 3) && "Channel index is larger than 2!");

      return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
    }

    const T &operator()(const size_t r, const size_t c, const size_t ch) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
      assert((ch < 3) && "Channel index is larger than 2!");

      return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
    }
  };

  template <typename T>
  class ImageRGBConstView
  {
  private:
    const T *data_;
    size_t num_rows_;
    size_t num_cols_;
    size_t num_element_per_channel_;

  public:
    ImageRGBConstView()
        : data_{nullptr}, num_rows_{0U}, num_cols_{0U}, num_element_per_channel_{
                                                            0U} {}

    ImageRGBConstView(const T *const data_ptr_in, const size_t num_rows,
                      const size_t num_cols)
        : data_{data_ptr_in}, num_rows_{num_rows}, num_cols_{num_cols},
          num_element_per_channel_{num_rows_ * num_cols_} {}

    const T *data() const { return data_; }

    size_t numRows() const { return num_rows_; }

    size_t numCols() const { return num_cols_; }

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }

    size_t numBytes() const { return 3 * num_rows_ * num_cols_ * sizeof(T); }

    size_t numElements() const { return 3 * num_rows_ * num_cols_; }

    const T &operator()(const size_t r, const size_t c, const size_t ch) const
    {
      assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
      assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
      assert((ch < 3) && "Channel index is larger than 2!");

      return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
    }
  };

  template <typename T>
  class ImageRGB
  {
  private:
    T *data_;
    size_t num_rows_;
    size_t num_cols_;
    size_t num_element_per_channel_;

  public:
    ImageRGB();
    ImageRGB(const size_t num_rows, const size_t num_cols);
    ImageRGB(const ImageRGB<T> &other);
    ImageRGB(ImageRGB<T> &&other);
    ~ImageRGB();

    T &operator()(const size_t r, const size_t c, const size_t ch);
    const T &operator()(const size_t r, const size_t c, const size_t ch) const;

    size_t numRows() const;
    size_t numCols() const;
    size_t numBytes() const;
    size_t numElements() const;
    void fillBufferWithData(uint8_t *const buffer) const;

    ImageRGBConstView<T> constView() const
    {
      return ImageRGBConstView<T>{data_, num_rows_, num_cols_};
    }

    ImageRGBView<T> view() const
    {
      return ImageRGBView<T>{data_, num_rows_, num_cols_};
    }

    size_t width() const { return num_cols_; }

    size_t height() const { return num_rows_; }

    T *data() const;
  };

  template <typename T>
  ImageRGB<T>::~ImageRGB()
  {
    if (num_rows_ > 0U)
    {
      delete[] data_;
    }
  }

  template <typename T>
  ImageRGB<T>::ImageRGB()
      : data_{nullptr}, num_rows_(0U), num_cols_(0U),
        num_element_per_channel_(0U) {}

  template <typename T>
  ImageRGB<T>::ImageRGB(const size_t num_rows, const size_t num_cols)
  {
    ASSERT(num_rows > 0U) << "Cannot initialize with number of rows to 0!";
    ASSERT(num_cols > 0U) << "Cannot initialize with number of columns to 0!";

    data_ = new T[num_rows * num_cols * 3];
    num_rows_ = num_rows;
    num_cols_ = num_cols;
    num_element_per_channel_ = num_rows_ * num_cols_;
  }

  template <typename T>
  ImageRGB<T>::ImageRGB(const ImageRGB<T> &other)
  {
    ASSERT(other.numRows() > 0U) << "Cannot initialize with number of rows to 0!";
    ASSERT(other.numCols() > 0U)
        << "Cannot initialize with number of columns to 0!";

    data_ = new T[other.numRows() * other.numCols() * 3];
    num_rows_ = other.numRows();
    num_cols_ = other.numCols();
    num_element_per_channel_ = num_rows_ * num_cols_;

    std::memcpy(data_, other.data_, num_element_per_channel_ * 3U);
  }

  template <typename T>
  ImageRGB<T>::ImageRGB(ImageRGB<T> &&other)
  {
    data_ = other.data_;
    num_rows_ = other.num_rows_;
    num_cols_ = other.num_cols_;
    num_element_per_channel_ = other.num_element_per_channel_;

    other.data_ = nullptr;
    other.num_rows_ = 0U;
    other.num_cols_ = 0U;
    other.num_element_per_channel_ = 0U;
  }

  template <typename T>
  void ImageRGB<T>::fillBufferWithData(uint8_t *const buffer) const
  {
    const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
    const size_t num_bytes = num_rows_ * num_cols_ * 3 * sizeof(T);
    std::memcpy(buffer, internal_ptr, num_bytes);
  }

  template <typename T>
  size_t ImageRGB<T>::numBytes() const
  {
    return 3 * num_rows_ * num_cols_ * sizeof(T);
  }

  template <typename T>
  size_t ImageRGB<T>::numElements() const
  {
    return 3 * num_rows_ * num_cols_;
  }

  template <typename T>
  size_t ImageRGB<T>::numRows() const { return num_rows_; }

  template <typename T>
  size_t ImageRGB<T>::numCols() const { return num_cols_; }

  template <typename T>
  T *ImageRGB<T>::data() const { return data_; }

  template <typename T>
  T &ImageRGB<T>::operator()(const size_t r, const size_t c, const size_t ch)
  {
    assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
    assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
    assert((ch < 3) && "Channel index is larger than 2!");

    return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
  }

  template <typename T>
  const T &ImageRGB<T>::operator()(const size_t r, const size_t c,
                                   const size_t ch) const
  {
    assert((r < num_rows_) && "Row index is larger than num_rows_ - 1!");
    assert((c < num_cols_) && "Column index is larger than num_cols_ - 1!");
    assert((ch < 3) && "Channel index is larger than 2!");

    return data_[ch * num_element_per_channel_ + r * num_cols_ + c];
  }

} // namespace lumos

#endif // LUMOS_MATH_IMAGE_IMAGE_RGB_H_
