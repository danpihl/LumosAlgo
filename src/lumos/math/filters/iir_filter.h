#ifndef LUMOS_MATH_FILTERS_IIR_FILTER_H_
#define LUMOS_MATH_FILTERS_IIR_FILTER_H_

#include "lumos/math/filters/class_def/iir_filter.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace lumos
{

  template <typename T>
  IIRFilter<T>::IIRFilter() : numerator_order_(0), denominator_order_(0)
  {
  }

  template <typename T>
  IIRFilter<T>::IIRFilter(const std::vector<T> &b_coeffs,
                          const std::vector<T> &a_coeffs)
      : b_coefficients_(b_coeffs), a_coefficients_(a_coeffs),
        numerator_order_(b_coeffs.size() > 0 ? b_coeffs.size() - 1 : 0),
        denominator_order_(a_coeffs.size() > 0 ? a_coeffs.size() - 1 : 0)
  {

    if (a_coefficients_.empty() || a_coefficients_[0] == T(0))
    {
      throw std::invalid_argument(
          "First denominator coefficient (a[0]) must be non-zero");
    }

    x_delay_line_.resize(b_coefficients_.size(), T(0));
    y_delay_line_.resize(a_coefficients_.size(), T(0));
  }

  template <typename T>
  IIRFilter<T>::IIRFilter(const Vector<T> &b_coeffs, const Vector<T> &a_coeffs)
      : IIRFilter(std::vector<T>(b_coeffs.begin(), b_coeffs.end()),
                  std::vector<T>(a_coeffs.begin(), a_coeffs.end()))
  {
  }

  template <typename T>
  IIRFilter<T>::IIRFilter(std::initializer_list<T> b_coeffs,
                          std::initializer_list<T> a_coeffs)
      : b_coefficients_(b_coeffs), a_coefficients_(a_coeffs),
        numerator_order_(b_coeffs.size() > 0 ? b_coeffs.size() - 1 : 0),
        denominator_order_(a_coeffs.size() > 0 ? a_coeffs.size() - 1 : 0)
  {

    if (a_coefficients_.empty() || a_coefficients_[0] == T(0))
    {
      throw std::invalid_argument(
          "First denominator coefficient (a[0]) must be non-zero");
    }

    x_delay_line_.resize(b_coefficients_.size(), T(0));
    y_delay_line_.resize(a_coefficients_.size(), T(0));
  }

  template <typename T>
  IIRFilter<T>::IIRFilter(size_t b_order, const T *b_coeffs, size_t a_order,
                          const T *a_coeffs)
      : b_coefficients_(b_coeffs, b_coeffs + b_order + 1),
        a_coefficients_(a_coeffs, a_coeffs + a_order + 1),
        numerator_order_(b_order), denominator_order_(a_order)
  {

    if (a_coefficients_.empty() || a_coefficients_[0] == T(0))
    {
      throw std::invalid_argument(
          "First denominator coefficient (a[0]) must be non-zero");
    }

    x_delay_line_.resize(b_coefficients_.size(), T(0));
    y_delay_line_.resize(a_coefficients_.size(), T(0));
  }

  template <typename T>
  IIRFilter<T>::IIRFilter(const IIRFilter &other)
      : b_coefficients_(other.b_coefficients_),
        a_coefficients_(other.a_coefficients_),
        x_delay_line_(other.x_delay_line_), y_delay_line_(other.y_delay_line_),
        numerator_order_(other.numerator_order_),
        denominator_order_(other.denominator_order_)
  {
  }

  template <typename T>
  IIRFilter<T>::IIRFilter(IIRFilter &&other) noexcept
      : b_coefficients_(std::move(other.b_coefficients_)),
        a_coefficients_(std::move(other.a_coefficients_)),
        x_delay_line_(std::move(other.x_delay_line_)),
        y_delay_line_(std::move(other.y_delay_line_)),
        numerator_order_(other.numerator_order_),
        denominator_order_(other.denominator_order_)
  {
  }

  template <typename T>
  IIRFilter<T> &IIRFilter<T>::operator=(const IIRFilter &other)
  {
    if (this != &other)
    {
      b_coefficients_ = other.b_coefficients_;
      a_coefficients_ = other.a_coefficients_;
      x_delay_line_ = other.x_delay_line_;
      y_delay_line_ = other.y_delay_line_;
      numerator_order_ = other.numerator_order_;
      denominator_order_ = other.denominator_order_;
    }
    return *this;
  }

  template <typename T>
  IIRFilter<T> &IIRFilter<T>::operator=(IIRFilter &&other) noexcept
  {
    if (this != &other)
    {
      b_coefficients_ = std::move(other.b_coefficients_);
      a_coefficients_ = std::move(other.a_coefficients_);
      x_delay_line_ = std::move(other.x_delay_line_);
      y_delay_line_ = std::move(other.y_delay_line_);
      numerator_order_ = other.numerator_order_;
      denominator_order_ = other.denominator_order_;
    }
    return *this;
  }

  template <typename T>
  T IIRFilter<T>::filter(T input)
  {
    if (b_coefficients_.empty() || a_coefficients_.empty())
    {
      return T(0);
    }

    // Add new input to delay line
    x_delay_line_.push_front(input);
    if (x_delay_line_.size() > b_coefficients_.size())
    {
      x_delay_line_.pop_back();
    }

    // Compute feedforward part (numerator)
    T output = T(0);
    for (size_t i = 0; i < std::min(x_delay_line_.size(), b_coefficients_.size());
         ++i)
    {
      output += b_coefficients_[i] * x_delay_line_[i];
    }

    // Compute feedback part (denominator, excluding a[0])
    for (size_t i = 1; i < std::min(y_delay_line_.size(), a_coefficients_.size());
         ++i)
    {
      output -= a_coefficients_[i] * y_delay_line_[i - 1];
    }

    // Normalize by a[0]
    if (a_coefficients_[0] != T(1))
    {
      output /= a_coefficients_[0];
    }

    // Add output to delay line
    y_delay_line_.push_front(output);
    if (y_delay_line_.size() > a_coefficients_.size())
    {
      y_delay_line_.pop_back();
    }

    return output;
  }

  template <typename T>
  std::vector<T> IIRFilter<T>::filter(const std::vector<T> &input)
  {
    std::vector<T> output;
    output.reserve(input.size());

    for (const T &sample : input)
    {
      output.push_back(filter(sample));
    }

    return output;
  }

  template <typename T>
  Vector<T> IIRFilter<T>::filter(const Vector<T> &input)
  {
    Vector<T> output(input.size());

    for (size_t i = 0; i < input.size(); ++i)
    {
      output(i) = filter(input(i));
    }

    return output;
  }

  template <typename T>
  void IIRFilter<T>::filter(const T *input, T *output, size_t length)
  {
    for (size_t i = 0; i < length; ++i)
    {
      output[i] = filter(input[i]);
    }
  }

  template <typename T>
  void IIRFilter<T>::reset()
  {
    std::fill(x_delay_line_.begin(), x_delay_line_.end(), T(0));
    std::fill(y_delay_line_.begin(), y_delay_line_.end(), T(0));
  }

  template <typename T>
  void IIRFilter<T>::setInitialConditions(const std::vector<T> &x_initial,
                                          const std::vector<T> &y_initial)
  {
    if (x_initial.size() != b_coefficients_.size())
    {
      throw std::invalid_argument(
          "Input initial state size must match numerator order");
    }
    if (y_initial.size() != a_coefficients_.size())
    {
      throw std::invalid_argument(
          "Output initial state size must match denominator order");
    }

    x_delay_line_.assign(x_initial.begin(), x_initial.end());
    y_delay_line_.assign(y_initial.begin(), y_initial.end());
  }

  template <typename T>
  void IIRFilter<T>::setCoefficients(const std::vector<T> &b_coeffs,
                                     const std::vector<T> &a_coeffs)
  {
    if (a_coeffs.empty() || a_coeffs[0] == T(0))
    {
      throw std::invalid_argument(
          "First denominator coefficient (a[0]) must be non-zero");
    }

    b_coefficients_ = b_coeffs;
    a_coefficients_ = a_coeffs;
    numerator_order_ = b_coeffs.size() > 0 ? b_coeffs.size() - 1 : 0;
    denominator_order_ = a_coeffs.size() > 0 ? a_coeffs.size() - 1 : 0;

    x_delay_line_.resize(b_coefficients_.size(), T(0));
    y_delay_line_.resize(a_coefficients_.size(), T(0));
  }

  template <typename T>
  void IIRFilter<T>::setCoefficients(const Vector<T> &b_coeffs,
                                     const Vector<T> &a_coeffs)
  {
    setCoefficients(std::vector<T>(b_coeffs.begin(), b_coeffs.end()),
                    std::vector<T>(a_coeffs.begin(), a_coeffs.end()));
  }

  template <typename T>
  const std::vector<T> &IIRFilter<T>::getNumeratorCoefficients() const
  {
    return b_coefficients_;
  }

  template <typename T>
  const std::vector<T> &IIRFilter<T>::getDenominatorCoefficients() const
  {
    return a_coefficients_;
  }

  template <typename T>
  size_t IIRFilter<T>::getNumeratorOrder() const
  {
    return numerator_order_;
  }

  template <typename T>
  size_t IIRFilter<T>::getDenominatorOrder() const
  {
    return denominator_order_;
  }

  template <typename T>
  size_t IIRFilter<T>::getOrder() const
  {
    return std::max(numerator_order_, denominator_order_);
  }

  template <typename T>
  bool IIRFilter<T>::isEmpty() const
  {
    return b_coefficients_.empty() && a_coefficients_.empty();
  }

  template <typename T>
  std::complex<T> IIRFilter<T>::frequencyResponse(T frequency,
                                                  T sample_rate) const
  {
    if (b_coefficients_.empty() || a_coefficients_.empty())
    {
      return std::complex<T>(0, 0);
    }

    T omega = 2 * M_PI * frequency / sample_rate;

    // Compute numerator response
    std::complex<T> numerator(0, 0);
    for (size_t n = 0; n < b_coefficients_.size(); ++n)
    {
      T phase = -omega * n;
      numerator +=
          b_coefficients_[n] * std::complex<T>(std::cos(phase), std::sin(phase));
    }

    // Compute denominator response
    std::complex<T> denominator(0, 0);
    for (size_t n = 0; n < a_coefficients_.size(); ++n)
    {
      T phase = -omega * n;
      denominator +=
          a_coefficients_[n] * std::complex<T>(std::cos(phase), std::sin(phase));
    }

    return numerator / denominator;
  }

  template <typename T>
  void IIRFilter<T>::printCoefficients() const
  {
    std::cout << "IIR Filter Coefficients:" << std::endl;
    std::cout << "Numerator (b): ";
    for (size_t i = 0; i < b_coefficients_.size(); ++i)
    {
      std::cout << b_coefficients_[i];
      if (i < b_coefficients_.size() - 1)
        std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << "Denominator (a): ";
    for (size_t i = 0; i < a_coefficients_.size(); ++i)
    {
      std::cout << a_coefficients_[i];
      if (i < a_coefficients_.size() - 1)
        std::cout << ", ";
    }
    std::cout << std::endl;
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::firstOrderLowPass(T cutoff_freq, T sample_rate)
  {
    T omega_c = 2 * M_PI * cutoff_freq / sample_rate;
    T alpha = std::exp(-omega_c);

    std::vector<T> b_coeffs = {1 - alpha};
    std::vector<T> a_coeffs = {1, -alpha};

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::firstOrderHighPass(T cutoff_freq, T sample_rate)
  {
    T omega_c = 2 * M_PI * cutoff_freq / sample_rate;
    T alpha = std::exp(-omega_c);

    std::vector<T> b_coeffs = {(1 + alpha) / 2, -(1 + alpha) / 2};
    std::vector<T> a_coeffs = {1, -alpha};

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::secondOrderLowPass(T cutoff_freq, T q_factor,
                                                T sample_rate)
  {
    T omega_c = 2 * M_PI * cutoff_freq / sample_rate;
    T alpha = std::sin(omega_c) / (2 * q_factor);

    T b0 = (1 - std::cos(omega_c)) / 2;
    T b1 = 1 - std::cos(omega_c);
    T b2 = (1 - std::cos(omega_c)) / 2;
    T a0 = 1 + alpha;
    T a1 = -2 * std::cos(omega_c);
    T a2 = 1 - alpha;

    std::vector<T> b_coeffs = {b0 / a0, b1 / a0, b2 / a0};
    std::vector<T> a_coeffs = {1, a1 / a0, a2 / a0};

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::secondOrderHighPass(T cutoff_freq, T q_factor,
                                                 T sample_rate)
  {
    T omega_c = 2 * M_PI * cutoff_freq / sample_rate;
    T alpha = std::sin(omega_c) / (2 * q_factor);

    T b0 = (1 + std::cos(omega_c)) / 2;
    T b1 = -(1 + std::cos(omega_c));
    T b2 = (1 + std::cos(omega_c)) / 2;
    T a0 = 1 + alpha;
    T a1 = -2 * std::cos(omega_c);
    T a2 = 1 - alpha;

    std::vector<T> b_coeffs = {b0 / a0, b1 / a0, b2 / a0};
    std::vector<T> a_coeffs = {1, a1 / a0, a2 / a0};

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::secondOrderBandPass(T center_freq, T q_factor,
                                                 T sample_rate)
  {
    T omega_c = 2 * M_PI * center_freq / sample_rate;
    T alpha = std::sin(omega_c) / (2 * q_factor);

    T b0 = q_factor * alpha;
    T b1 = 0;
    T b2 = -q_factor * alpha;
    T a0 = 1 + alpha;
    T a1 = -2 * std::cos(omega_c);
    T a2 = 1 - alpha;

    std::vector<T> b_coeffs = {b0 / a0, b1 / a0, b2 / a0};
    std::vector<T> a_coeffs = {1, a1 / a0, a2 / a0};

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::secondOrderNotch(T center_freq, T q_factor,
                                              T sample_rate)
  {
    T omega_c = 2 * M_PI * center_freq / sample_rate;
    T alpha = std::sin(omega_c) / (2 * q_factor);

    T b0 = 1;
    T b1 = -2 * std::cos(omega_c);
    T b2 = 1;
    T a0 = 1 + alpha;
    T a1 = -2 * std::cos(omega_c);
    T a2 = 1 - alpha;

    std::vector<T> b_coeffs = {b0 / a0, b1 / a0, b2 / a0};
    std::vector<T> a_coeffs = {1, a1 / a0, a2 / a0};

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::integrator(T sample_rate)
  {
    T Ts = 1.0 / sample_rate;
    std::vector<T> b_coeffs = {Ts / 2, Ts / 2};
    std::vector<T> a_coeffs = {1, -1};

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::differentiator(T sample_rate)
  {
    T Ts = 1.0 / sample_rate;
    std::vector<T> b_coeffs = {1 / Ts, -1 / Ts};
    std::vector<T> a_coeffs = {1};

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  template <typename T>
  IIRFilter<T> IIRFilter<T>::dcBlocker(T cutoff_freq, T sample_rate)
  {
    return firstOrderHighPass(cutoff_freq, sample_rate);
  }

  namespace internal
  {

    // Product of two polynomials given by their coefficients
    template <typename T>
    std::vector<T> polynomialMultiply(const std::vector<T> &p0,
                                      const std::vector<T> &p1)
    {
      std::vector<T> res(p0.size() + p1.size() - 1, T(0));
      for (size_t i = 0; i < p0.size(); ++i)
      {
        for (size_t j = 0; j < p1.size(); ++j)
        {
          res[i + j] += p0[i] * p1[j];
        }
      }
      return res;
    }

    // Roots of c[0] * z^n + c[1] * z^(n - 1) + ... + c[n]. Leading zero
    // coefficients are ignored. Roots of multiplicity m are only accurate to
    // about the m-th root of the machine precision, which is inherent to finding
    // roots from coefficients
    template <typename T>
    std::vector<std::complex<T>> polynomialRoots(const std::vector<T> &coeffs)
    {
      std::vector<std::complex<T>> roots;

      size_t first = 0;
      while (first < coeffs.size() && coeffs[first] == T(0))
      {
        ++first;
      }
      size_t last = coeffs.size();
      // Trailing zero coefficients are roots at the origin
      while (last > first + 1 && coeffs[last - 1] == T(0))
      {
        roots.push_back(std::complex<T>(0, 0));
        --last;
      }
      if (last - first < 2)
      {
        return roots;
      }

      // Monic polynomial z^n + c[1] * z^(n - 1) + ... + c[n], with c[n] != 0
      const size_t n = last - first - 1;
      std::vector<T> c(n + 1);
      for (size_t i = 0; i <= n; ++i)
      {
        c[i] = coeffs[first + i] / coeffs[first];
      }

      if (n == 1)
      {
        roots.push_back(std::complex<T>(-c[1], 0));
        return roots;
      }

      if (n == 2)
      {
        const T disc = c[1] * c[1] - T(4) * c[2];
        if (disc >= T(0))
        {
          // Avoid cancellation by computing the larger root first
          const T sign = (c[1] < T(0)) ? T(-1) : T(1);
          const T r0 = -(c[1] + sign * std::sqrt(disc)) / T(2);
          roots.push_back(std::complex<T>(r0, 0));
          roots.push_back(std::complex<T>(c[2] / r0, 0));
        }
        else
        {
          const T re = -c[1] / T(2);
          const T im = std::sqrt(-disc) / T(2);
          roots.push_back(std::complex<T>(re, im));
          roots.push_back(std::complex<T>(re, -im));
        }
        return roots;
      }

      // Aberth-Ehrlich iteration, which finds all roots simultaneously. Start on
      // a circle with the geometric mean of the root magnitudes as radius
      const T radius = std::pow(std::abs(c[n]), T(1) / T(n));
      std::vector<std::complex<T>> z(n);
      for (size_t k = 0; k < n; ++k)
      {
        const T angle = T(2) * T(M_PI) * T(k) / T(n) + T(0.4);
        z[k] = std::polar(radius, angle);
      }

      const size_t max_iterations = 500;
      const T tolerance = T(4) * std::numeric_limits<T>::epsilon();
      for (size_t iteration = 0; iteration < max_iterations; ++iteration)
      {
        bool converged = true;
        for (size_t k = 0; k < n; ++k)
        {
          // Evaluate the polynomial and its derivative with Horner's method
          std::complex<T> p(c[0], 0);
          std::complex<T> dp(0, 0);
          for (size_t i = 1; i <= n; ++i)
          {
            dp = dp * z[k] + p;
            p = p * z[k] + c[i];
          }
          if (p == std::complex<T>(0, 0))
          {
            continue;
          }
          if (dp == std::complex<T>(0, 0))
          {
            // Stationary point that is not a root, nudge away from it
            z[k] += std::complex<T>(radius * T(1e-3), radius * T(1e-3));
            converged = false;
            continue;
          }

          const std::complex<T> newton_step = p / dp;
          std::complex<T> repulsion(0, 0);
          for (size_t j = 0; j < n; ++j)
          {
            if (j != k && z[j] != z[k])
            {
              repulsion += T(1) / (z[k] - z[j]);
            }
          }
          const std::complex<T> step =
              newton_step / (std::complex<T>(1, 0) - newton_step * repulsion);
          z[k] -= step;

          if (std::abs(step) > tolerance * std::max(T(1), std::abs(z[k])))
          {
            converged = false;
          }
        }
        if (converged)
        {
          break;
        }
      }

      roots.insert(roots.end(), z.begin(), z.end());
      return roots;
    }

    // The functions below build a digital filter as a product of first and second
    // order sections. Each section is an analog prototype mapped with the
    // bilinear transform s = (1 - z^-1) / (1 + z^-1), for which the analog
    // frequency that corresponds to the digital frequency f is tan(pi * f / fs)

    template <typename T>
    T prewarpedCutoff(const T cutoff_freq, const T sample_rate)
    {
      if (!(sample_rate > T(0)))
      {
        throw std::invalid_argument("Sample rate must be greater than 0");
      }
      if (!(cutoff_freq > T(0)) || !(cutoff_freq < sample_rate / T(2)))
      {
        throw std::invalid_argument(
            "Cutoff frequency must be between 0 and half the sample rate");
      }
      return std::tan(T(M_PI) * cutoff_freq / sample_rate);
    }

    // Multiplies in the section w2 / (s^2 + c1 * s + w2)
    template <typename T>
    void appendLowPassBiquad(std::vector<T> &b, std::vector<T> &a, const T c1,
                             const T w2)
    {
      const T norm = T(1) + c1 + w2;
      b = polynomialMultiply(b, {w2 / norm, T(2) * w2 / norm, w2 / norm});
      a = polynomialMultiply(
          a, {T(1), T(2) * (w2 - T(1)) / norm, (T(1) - c1 + w2) / norm});
    }

    // Multiplies in the section s^2 / (s^2 + c1 * s + w2)
    template <typename T>
    void appendHighPassBiquad(std::vector<T> &b, std::vector<T> &a, const T c1,
                              const T w2)
    {
      const T norm = T(1) + c1 + w2;
      b = polynomialMultiply(b, {T(1) / norm, T(-2) / norm, T(1) / norm});
      a = polynomialMultiply(
          a, {T(1), T(2) * (w2 - T(1)) / norm, (T(1) - c1 + w2) / norm});
    }

    // Multiplies in the section w / (s + w)
    template <typename T>
    void appendLowPassFirstOrder(std::vector<T> &b, std::vector<T> &a, const T w)
    {
      const T norm = T(1) + w;
      b = polynomialMultiply(b, {w / norm, w / norm});
      a = polynomialMultiply(a, {T(1), (w - T(1)) / norm});
    }

    // Multiplies in the section s / (s + w)
    template <typename T>
    void appendHighPassFirstOrder(std::vector<T> &b, std::vector<T> &a, const T w)
    {
      const T norm = T(1) + w;
      b = polynomialMultiply(b, {T(1) / norm, T(-1) / norm});
      a = polynomialMultiply(a, {T(1), (w - T(1)) / norm});
    }

  } // namespace internal

  // A filter is stable if all its poles are strictly inside the unit circle.
  // Poles on the unit circle (e.g. an integrator) are reported as not stable
  template <typename T>
  bool IIRFilter<T>::isStable() const
  {
    if (a_coefficients_.empty())
    {
      return true;
    }

    // Schur-Cohn test: the step-down recursion gives the reflection
    // coefficients of the denominator, which all have magnitude below one
    // exactly when all roots are inside the unit circle. This avoids finding
    // the roots
    std::vector<T> c(a_coefficients_.size());
    for (size_t i = 0; i < c.size(); ++i)
    {
      c[i] = a_coefficients_[i] / a_coefficients_[0];
    }

    std::vector<T> next(c.size());
    for (size_t m = c.size() - 1; m >= 1; --m)
    {
      const T k = c[m];
      if (!(std::abs(k) < T(1)))
      {
        return false;
      }
      const T den = T(1) - k * k;
      for (size_t i = 1; i < m; ++i)
      {
        next[i] = (c[i] - k * c[m - i]) / den;
      }
      for (size_t i = 1; i < m; ++i)
      {
        c[i] = next[i];
      }
    }

    return true;
  }

  // Poles of the transfer function in the z-plane. Includes the poles at the
  // origin that a filter with more numerator than denominator coefficients has
  template <typename T>
  std::vector<std::complex<T>> IIRFilter<T>::getPoles() const
  {
    if (a_coefficients_.empty())
    {
      return {};
    }
    std::vector<T> a = a_coefficients_;
    a.resize(std::max(a_coefficients_.size(), b_coefficients_.size()), T(0));
    return internal::polynomialRoots(a);
  }

  // Zeros of the transfer function in the z-plane. Includes the zeros at the
  // origin that a filter with more denominator than numerator coefficients has
  template <typename T>
  std::vector<std::complex<T>> IIRFilter<T>::getZeros() const
  {
    if (b_coefficients_.empty())
    {
      return {};
    }
    std::vector<T> b = b_coefficients_;
    b.resize(std::max(a_coefficients_.size(), b_coefficients_.size()), T(0));
    return internal::polynomialRoots(b);
  }

  // Maximally flat low-pass filter with -3 dB gain at cutoff_freq
  template <typename T>
  IIRFilter<T> IIRFilter<T>::butterworthLowPass(size_t order, T cutoff_freq,
                                                T sample_rate)
  {
    if (order == 0)
    {
      throw std::invalid_argument("Filter order must be greater than 0");
    }
    const T wc = internal::prewarpedCutoff(cutoff_freq, sample_rate);

    std::vector<T> b_coeffs = {T(1)};
    std::vector<T> a_coeffs = {T(1)};

    // One second order section per complex conjugate pole pair
    for (size_t k = 0; k < order / 2; ++k)
    {
      const T damping =
          T(2) * std::sin(T(M_PI) * T(2 * k + 1) / T(2 * order));
      internal::appendLowPassBiquad(b_coeffs, a_coeffs, damping * wc, wc * wc);
    }
    // Odd orders have one real pole
    if (order % 2 == 1)
    {
      internal::appendLowPassFirstOrder(b_coeffs, a_coeffs, wc);
    }

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  // Maximally flat high-pass filter with -3 dB gain at cutoff_freq
  template <typename T>
  IIRFilter<T> IIRFilter<T>::butterworthHighPass(size_t order, T cutoff_freq,
                                                 T sample_rate)
  {
    if (order == 0)
    {
      throw std::invalid_argument("Filter order must be greater than 0");
    }
    const T wc = internal::prewarpedCutoff(cutoff_freq, sample_rate);

    std::vector<T> b_coeffs = {T(1)};
    std::vector<T> a_coeffs = {T(1)};

    for (size_t k = 0; k < order / 2; ++k)
    {
      const T damping =
          T(2) * std::sin(T(M_PI) * T(2 * k + 1) / T(2 * order));
      internal::appendHighPassBiquad(b_coeffs, a_coeffs, damping * wc, wc * wc);
    }
    if (order % 2 == 1)
    {
      internal::appendHighPassFirstOrder(b_coeffs, a_coeffs, wc);
    }

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

  // Chebyshev type I low-pass filter: equiripple in the passband, with the gain
  // staying between -ripple_db and 0 dB up to cutoff_freq, where it is
  // -ripple_db
  template <typename T>
  IIRFilter<T> IIRFilter<T>::chebyshevLowPass(size_t order, T cutoff_freq,
                                              T ripple_db, T sample_rate)
  {
    if (order == 0)
    {
      throw std::invalid_argument("Filter order must be greater than 0");
    }
    if (!(ripple_db > T(0)))
    {
      throw std::invalid_argument("Passband ripple must be greater than 0 dB");
    }
    const T wc = internal::prewarpedCutoff(cutoff_freq, sample_rate);

    // The analog prototype poles lie on an ellipse with these semi-axes
    const T epsilon = std::sqrt(std::pow(T(10), ripple_db / T(10)) - T(1));
    const T mu = std::asinh(T(1) / epsilon) / T(order);
    const T sinh_mu = std::sinh(mu);
    const T cosh_mu = std::cosh(mu);

    std::vector<T> b_coeffs = {T(1)};
    std::vector<T> a_coeffs = {T(1)};

    for (size_t k = 0; k < order / 2; ++k)
    {
      const T theta = T(M_PI) * T(2 * k + 1) / T(2 * order);
      const T sigma = sinh_mu * std::sin(theta) * wc;
      const T omega = cosh_mu * std::cos(theta) * wc;
      internal::appendLowPassBiquad(b_coeffs, a_coeffs, T(2) * sigma,
                                    sigma * sigma + omega * omega);
    }
    if (order % 2 == 1)
    {
      internal::appendLowPassFirstOrder(b_coeffs, a_coeffs, sinh_mu * wc);
    }

    // The sections have unit gain at DC. For even orders DC is at the bottom
    // of the ripple
    if (order % 2 == 0)
    {
      const T dc_gain = std::pow(T(10), -ripple_db / T(20));
      for (T &b : b_coeffs)
      {
        b *= dc_gain;
      }
    }

    return IIRFilter<T>(b_coeffs, a_coeffs);
  }

} // namespace lumos

#endif // LUMOS_MATH_FILTERS_IIR_FILTER_H_
