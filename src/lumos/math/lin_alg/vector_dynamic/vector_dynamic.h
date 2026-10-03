#ifndef LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_VECTOR_DYNAMIC_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_VECTOR_DYNAMIC_H_

#include <assert.h>

#include <cmath>
#include <cstring>
#include <type_traits>
#include <utility>

#include <iostream>
#include <string>
#include "lumos/math/misc/assert.h"
#include "lumos/math/lin_alg/vector_dynamic/class_def/vector_dynamic.h"
#include "lumos/math/misc/math_macros.h"

namespace lumos
{
  template <typename T>
  Vector<T>::Vector() : data_(nullptr), size_(0) {}

  template <typename T>
  Vector<T>::Vector(const size_t vector_length)
  {
    ASSERT(vector_length > 0U) << "Length of vector can't be 0!";
    size_ = vector_length;

    DATA_ALLOCATION(data_, vector_length, T, "Vector");
  }

  template <typename T>
  Vector<T>::Vector(const Vector<T> &v)
  {
    size_ = v.size();

    DATA_ALLOCATION(data_, v.size(), T, "Vector");

    for (size_t k = 0; k < v.size(); k++)
    {
      data_[k] = v(k);
    }
  }

  template <typename T>
  Vector<T>::Vector(Vector<T> &&v)
  {
    ASSERT(v.size() > 0U) << "Input vector size is 0!";
    data_ = v.data();
    size_ = v.size();

    v.data_ = nullptr;
    v.size_ = 0U;
  }

  template <typename T>
  Vector<T> &Vector<T>::operator=(Vector<T> &&v)
  {
    if (this != &v)
    {
      ASSERT(v.size() > 0U) << "Input vector size is 0!";

      if (size_ > 0U)
      {
        delete[] data_;
      }

      size_ = v.size();
      data_ = v.data();

      v.data_ = nullptr;
      v.size_ = 0U;
    }

    return *this;
  }

  template <typename T>
  Vector<T>::~Vector()
  {
    if (size_ > 0U)
    {
      delete[] data_;
    }
  }

  template <typename T>
  void Vector<T>::fillBufferWithData(uint8_t *const buffer) const
  {
    const uint8_t *const internal_ptr = reinterpret_cast<uint8_t *>(data_);
    const size_t num_bytes = size_ * sizeof(T);

    std::memcpy(buffer, internal_ptr, num_bytes);
  }

  template <typename T>
  Vector<T> &Vector<T>::operator=(const Vector<T> &v)
  {
    ASSERT(v.size() > 0U) << "Input vector size is 0!";

    if (this != &v)
    {
      if (size_ > 0U)
      {
        if (v.size() != size_)
        {
          delete[] data_;
          DATA_ALLOCATION(data_, v.size(), T, "Vector");
        }
      }
      else
      {
        DATA_ALLOCATION(data_, v.size(), T, "Vector");
      }
      size_ = v.size();

      for (size_t k = 0; k < v.size(); k++)
      {
        data_[k] = v(k);
      }
    }

    return *this;
  }

  template <typename T>
  template <typename Y>
  Vector<T>::Vector(const Vector<Y> &v)
  {
    size_ = v.size();

    DATA_ALLOCATION(data_, v.size(), T, "Vector");

    for (size_t k = 0; k < v.size(); k++)
    {
      data_[k] = static_cast<T>(v(k));
    }
  }

  template <typename T>
  template <typename Y>
  Vector<T> &Vector<T>::operator=(const Vector<Y> &v)
  {
    if (size_ > 0U)
    {
      if (v.size() != size_)
      {
        delete[] data_;
        DATA_ALLOCATION(data_, v.size(), T, "Vector");
      }
    }
    else
    {
      DATA_ALLOCATION(data_, v.size(), T, "Vector");
    }
    size_ = v.size();

    for (size_t k = 0; k < v.size(); k++)
    {
      data_[k] = static_cast<T>(v(k));
    }

    return *this;
  }

  template <typename T>
  Vector<T>::Vector(const std::vector<T> &v)
  {
    if (v.size() == 0)
    {
      size_ = 0;
      data_ = nullptr;
    }
    else
    {
      size_ = v.size();

      DATA_ALLOCATION(data_, v.size(), T, "Vector");

      size_t idx = 0;
      for (auto vec_element : v)
      {
        data_[idx] = vec_element;
        idx++;
      }
    }
  }

  template <typename T>
  T &Vector<T>::operator()(const size_t idx)
  {
    assert(idx < size_);
    return data_[idx];
  }

  template <typename T>
  const T &Vector<T>::operator()(const size_t idx) const
  {
    assert(idx < size_);
    return data_[idx];
  }

  template <typename T>
  size_t Vector<T>::size() const { return size_; }

  template <typename T>
  size_t Vector<T>::numElements() const { return size_; }

  template <typename T>
  size_t Vector<T>::numBytes() const
  {
    return size_ * sizeof(T);
  }

  template <typename T>
  void Vector<T>::fill(const T &val)
  {
    ASSERT(size_ > 0U) << "Tried to fill unallocated vector!";
    for (size_t k = 0; k < size_; k++)
    {
      data_[k] = val;
    }
  }

  template <typename T>
  void Vector<T>::resize(const size_t new_size)
  {
    ASSERT(new_size > 0U) << "Length of vector can't be 0!";
    if (new_size != size_)
    {
      if (size_ > 0U)
      {
        delete[] data_;
      }

      DATA_ALLOCATION(data_, new_size, T, "Vector");
      size_ = new_size;
    }
  }

  template <typename T>
  size_t Vector<T>::endIndex() const
  {
    ASSERT(size_ != 0U);
    return size_ - 1;
  }

  template <typename T>
  T *Vector<T>::data() const { return data_; }

  template <typename T>
  T *Vector<T>::begin() const { return &data_[0]; }

  template <typename T>
  T *Vector<T>::end() const { return &data_[size_]; }

  template <typename T>
  T Vector<T>::min() const
  {
    ASSERT_VEC_VALID_INTERNAL();
    T min_val = data_[0];

    for (size_t k = 1; k < size_; k++)
    {
      min_val = std::min(data_[k], min_val);
    }

    return min_val;
  }

  template <typename T>
  T Vector<T>::max() const
  {
    ASSERT_VEC_VALID_INTERNAL();
    T max_val = data_[0];

    for (size_t k = 1; k < size_; k++)
    {
      max_val = std::max(data_[k], max_val);
    }

    return max_val;
  }

  template <typename T>
  T Vector<T>::sum() const
  {
    ASSERT_VEC_VALID_INTERNAL();
    T s = 0.0;

    for (size_t k = 0; k < size_; k++)
    {
      s = s + data_[k];
    }

    return s;
  }

  // Non class methods

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const Vector<T> &v)
  {
    std::string s = "[ ";
    for (size_t k = 0; k < v.size(); k++)
    {
      s = s + std::to_string(v(k));
      if (k != v.size() - 1)
      {
        s = s + ", ";
      }
    }
    s = s + " ]";
    os << s;

    return os;
  }

  template <typename T>
  T operator*(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    T d = 0.0;
    for (size_t k = 0; k < v0.size(); k++)
    {
      d = d + v0(k) * v1(k);
    }
    return d;
  }

  template <typename T>
  Vector<T> operator%(const Vector<T> &v, const int i)
  {
    Vector<T> vout(v.size());

    for (size_t k = 0; k < v.size(); k++)
    {
      vout(k) = static_cast<T>(static_cast<int>(v(k)) % i);
    }

    return vout;
  }

  template <typename T>
  Vector<T> operator*(const T f, const Vector<T> &v)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = f * v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator*(const Vector<T> &v, const T f)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = f * v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator^(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<T> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) * v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator/(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<T> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) / v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator/(const Vector<T> &v, const T f)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) / f;
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator/(const T f, const Vector<T> &v)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = f / v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator+(const Vector<T> &v, const T f)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) + f;
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator+(const T f, const Vector<T> &v)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) + f;
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator-(const Vector<T> &v, const T f)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) - f;
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator-(const T f, const Vector<T> &v)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = f - v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator-(const Vector<T> &v)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = -v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator+(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<T> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) + v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator-(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<T> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) - v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator==(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<bool> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) == v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator==(const Vector<T> &v, const T s)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) == s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator==(const T s, const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) == s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator!=(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<bool> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) != v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator!=(const Vector<T> &v, const T s)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) != s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator!=(const T s, const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) != s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator<(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<bool> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) < v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator<(const Vector<T> &v, const T s)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) < s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator<(const T s, const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = s < v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator>(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<bool> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) > v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator>(const Vector<T> &v, const T s)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) > s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator>(const T s, const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = s > v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator<=(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<bool> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) <= v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator<=(const Vector<T> &v, const T s)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) <= s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator<=(const T s, const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = s <= v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator>=(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<bool> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) >= v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator>=(const Vector<T> &v, const T s)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) >= s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator>=(const T s, const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = s >= v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator&(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<T> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) & v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator&(const Vector<T> &v, const T s)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) & s;
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator&(const T s, const Vector<T> &v)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = s & v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator|(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<T> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) | v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator|(const Vector<T> &v, const T s)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) | s;
    }
    return v_res;
  }

  template <typename T>
  Vector<T> operator|(const T s, const Vector<T> &v)
  {
    Vector<T> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = s | v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator&&(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<bool> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) && v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator&&(const Vector<T> &v, const T s)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) && s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator&&(const T s, const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = s && v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator||(const Vector<T> &v0, const Vector<T> &v1)
  {
    ASSERT(v0.size() == v1.size());
    Vector<bool> v_res(v0.size());
    for (size_t k = 0; k < v0.size(); k++)
    {
      v_res(k) = v0(k) || v1(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator||(const Vector<T> &v, const T s)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = v(k) || s;
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator||(const T s, const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = s || v(k);
    }
    return v_res;
  }

  template <typename T>
  Vector<bool> operator!(const Vector<T> &v)
  {
    Vector<bool> v_res(v.size());
    for (size_t k = 0; k < v.size(); k++)
    {
      v_res(k) = !v(k);
    }
    return v_res;
  }

  template <typename Y>
  void fillWithPtr(Vector<Y> &v, const void *const ptr,
                   const size_t vector_length)
  {
    v.size_ = vector_length;
    DATA_ALLOCATION(v.data_, vector_length, Y, "Vector");

    std::memcpy(v.data_, ptr, sizeof(Y) * vector_length);
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_VECTOR_DYNAMIC_H_
