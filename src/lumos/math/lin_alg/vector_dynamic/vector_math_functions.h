#ifndef LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_VECTOR_MATH_FUNCTIONS_H_
#define LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_VECTOR_MATH_FUNCTIONS_H_

#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <utility>

#include "lumos/math/misc/assert.h"
#include "lumos/math/lin_alg/vector_dynamic/vector_dynamic.h"

namespace lumos
{
  template <typename T>
  bool any(const Vector<T> &v)
  {
    for (size_t k = 0; k < v.size(); k++)
    {
      if (v(k))
      {
        return true;
      }
    }

    return true;
  }

  template <typename T>
  bool all(const Vector<T> &v)
  {
    for (size_t k = 0; k < v.size(); k++)
    {
      if (!v(k))
      {
        return false;
      }
    }

    return true;
  }

  template <typename T>
  Vector<T> round(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::round(vin(k));
    }

    return v;
  }

  template <typename T>
  T max(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    T max_val = vin(0);
    for (size_t k = 1; k < vin.size(); k++)
    {
      max_val = std::max(vin(k), max_val);
    }

    return max_val;
  }

  template <typename T>
  Vector<T> abs(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> vout(vin.size());

    for (size_t k = 0; k < vin.size(); k++)
    {
      vout(k) = std::fabs(vin(k));
    }

    return vout;
  }

  template <typename T>
  T min(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    T min_val = vin(0);
    for (size_t k = 1; k < vin.size(); k++)
    {
      min_val = std::min(vin(k), min_val);
    }

    return min_val;
  }

  template <typename T>
  Vector<T> log10(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::log10(vin(k));
    }

    return v;
  }

  template <typename T>
  Vector<T> pow(const Vector<T> &vin, const T e)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::pow(vin(k), e);
    }

    return v;
  }

  template <typename T>
  Vector<T> log(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::log(vin(k));
    }

    return v;
  }

  template <typename T>
  Vector<T> exp(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::exp(vin(k));
    }

    return v;
  }

  template <typename T>
  Vector<T> cos(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::cos(vin(k));
    }

    return v;
  }

  template <typename T>
  Vector<T> sin(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::sin(vin(k));
    }

    return v;
  }

  template <typename T>
  Vector<T> tan(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::tan(vin(k));
    }

    return v;
  }

  template <typename T>
  Vector<T> atan(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);
    Vector<T> v(vin.size());
    for (size_t k = 0; k < vin.size(); k++)
    {
      v(k) = std::atan(vin(k));
    }

    return v;
  }

  template <typename T>
  Vector<T> atan2(const Vector<T> &y_vec, const Vector<T> &x_vec)
  {
    ASSERT(y_vec.size() > 0);
    ASSERT(y_vec.size() == x_vec.size());

    Vector<T> v(x_vec.size());

    for (size_t k = 0; k < x_vec.size(); k++)
    {
      v(k) = std::atan2(y_vec(k), x_vec(k));
    }

    return v;
  }

  template <typename T>
  Vector<T> linspaceFromBoundariesAndCount(const T x0, const T x1,
                                           const size_t num_values)
  {
    ASSERT(num_values > 0);
    Vector<T> v(num_values);

    const T dx = (x1 - x0) / static_cast<T>(num_values - 1);
    v(0) = x0;

    for (size_t k = 1; k < num_values; k++)
    {
      v(k) = v(k - 1) + dx;
    }

    return v;
  }

  template <typename T>
  Vector<T> linspaceFromStartPointIncAndCount(const T x0, const T dx,
                                              const size_t num_values)
  {
    ASSERT(num_values > 0);
    Vector<T> v(num_values);

    v(0) = x0;

    for (size_t k = 1; k < num_values; k++)
    {
      v(k) = v(k - 1) + dx;
    }

    return v;
  }

  template <typename T>
  Vector<T> linspaceFromBoundariesAndInc(const T x0, const T x1, const T dx)
  {
    ASSERT(dx > 0);
    ASSERT(x1 > x0);

    const size_t num_values = (x1 - x0) / dx;

    return linspaceFromBoundariesAndCount(x0, x1, num_values);
  }

  template <typename T>
  T sum(const Vector<T> &vin)
  {
    ASSERT(vin.size() > 0);

    T s = vin(0);
    for (size_t k = 1; k < vin.size(); k++)
    {
      s = s + vin(k);
    }

    return s;
  }

} // namespace lumos

#endif // LUMOS_MATH_LIN_ALG_VECTOR_DYNAMIC_VECTOR_MATH_FUNCTIONS_H_
