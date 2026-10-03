#ifndef LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_CLASS_DEF_VECTOR_DYNAMIC_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_CLASS_DEF_VECTOR_DYNAMIC_H_

#include <assert.h>

#include <iterator>

#include <cstring>
#include <iostream>
#include <new>
#include <vector>
#include "lumos/math/misc/assert.h"
#include "lumos/math/misc/forward_decl.h"

namespace lumos
{

  template <typename T>
  class VectorInitializer
  {
  private:
    T *data_;
    size_t size_;
    friend class Vector<T>;

  public:
    VectorInitializer() = delete;
    VectorInitializer(const VectorInitializer<T> &v) = delete;
    VectorInitializer(VectorInitializer<T> &&v);
    VectorInitializer<T> &operator=(const VectorInitializer<T> &v) = delete;
    VectorInitializer<T> &operator=(VectorInitializer<T> &&v) = delete;
    explicit VectorInitializer(const std::initializer_list<T> &il)
    {
      DATA_ALLOCATION(data_, il.size(), T, "VectorInitializer");

      size_ = il.size();

      size_t idx = 0;
      for (auto list_element : il)
      {
        data_[idx] = list_element;
        idx++;
      }
    }

    ~VectorInitializer() { delete[] data_; }
  };

  template <typename T>
  class VectorView
  {
  private:
    T *data_;
    size_t size_;

  public:
    VectorView() : data_{nullptr}, size_{0U} {}

    VectorView(T *const data_ptr_in, const size_t size_in)
        : data_{data_ptr_in}, size_{size_in} {}

    T *data() const { return data_; }

    size_t size() const { return size_; }

    size_t numBytes() const { return size_ * sizeof(T); }

    size_t numElements() const { return size_; }

    void fillBufferWithData(uint8_t *const buffer) const
    {
      const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
      const size_t num_bytes = size_ * sizeof(T);

      std::memcpy(buffer, internal_ptr, num_bytes);
    }

    T &operator()(const size_t idx)
    {
      assert(idx < size_);
      return data_[idx];
    }

    const T &operator()(const size_t idx) const
    {
      assert(idx < size_);
      return data_[idx];
    }

    std::pair<T, T> findMinMax() const
    {
      T min_value = data_[0], max_value = data_[0];

      for (size_t k = 0; k < size_; k++)
      {
        const T val = data_[k];
        if (val < min_value)
        {
          min_value = val;
        }
        if (val > max_value)
        {
          max_value = val;
        }
      }

      return {min_value, max_value};
    }
  };

  template <typename T>
  class VectorConstView
  {
  private:
    const T *data_;
    size_t size_;

  public:
    VectorConstView() : data_{nullptr}, size_{0U} {}

    VectorConstView(const T *const data_ptr_in, const size_t size_in)
        : data_{data_ptr_in}, size_{size_in} {}

    const T *data() const { return data_; }

    size_t size() const { return size_; }

    size_t numBytes() const { return size_ * sizeof(T); }

    size_t numElements() const { return size_; }

    void fillBufferWithData(uint8_t *const buffer) const
    {
      const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
      const size_t num_bytes = size_ * sizeof(T);

      std::memcpy(buffer, internal_ptr, num_bytes);
    }

    const T &operator()(const size_t idx) const
    {
      assert(idx < size_);
      return data_[idx];
    }

    std::pair<T, T> findMinMax() const
    {
      T min_value = data_[0], max_value = data_[0];

      for (size_t k = 0; k < size_; k++)
      {
        const T val = data_[k];
        if (val < min_value)
        {
          min_value = val;
        }
        if (val > max_value)
        {
          max_value = val;
        }
      }

      return {min_value, max_value};
    }
  };

  template <typename T>
  class Vector
  {
  protected:
    T *data_;
    size_t size_;

  public:
    Vector();
    explicit Vector(const size_t vector_length);
    Vector(const Vector<T> &v);
    Vector(Vector<T> &&v);

    Vector(VectorInitializer<T> &&v)
    {
      data_ = v.data_;
      size_ = v.size_;

      v.data_ = nullptr;
      v.size_ = 0U;
    }

    template <typename Y>
    Vector(const Vector<Y> &v);

    template <typename Y>
    friend void fillWithPtr(Vector<Y> &v, const void *const ptr,
                            const size_t vector_length);

    Vector(const std::vector<T> &v);

    ~Vector();

    VectorView<T> view() const { return VectorView{data_, size_}; }

    VectorConstView<T> constView() const { return VectorConstView{data_, size_}; }

    void fillBufferWithData(uint8_t *const buffer) const;

    Vector<T> &operator=(const Vector<T> &v);
    Vector<T> &operator=(Vector<T> &&v);
    T &operator()(const size_t idx);
    const T &operator()(const size_t idx) const;

    size_t size() const;
    size_t numElements() const;
    size_t numBytes() const;
    void fill(const T &val);
    void resize(const size_t new_size);
    size_t endIndex() const;
    T *data() const;
    T *begin() const;
    T *end() const;
    template <typename Y>
    Vector<T> &operator=(const Vector<Y> &rhs);

    T max() const;
    T min() const;
    T sum() const;

    template <uint16_t N>
    FixedSizeVector<T, N> toFixedSizeVector() const
    {
      static_assert(N <= 0xFFFF, "N must be less than or equal to 65535");
      assert(size_ == N && "Size mismatch in toFixedSizeVector conversion");

      FixedSizeVector<T, N> fixed_size_vector;
      for (uint16_t i = 0; i < N; ++i)
      {
        fixed_size_vector[i] = data_[i];
      }
      return fixed_size_vector;
    }
  };

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_CLASS_DEF_VECTOR_DYNAMIC_H_
