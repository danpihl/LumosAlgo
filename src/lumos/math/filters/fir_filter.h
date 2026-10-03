#ifndef LUMOS_MATH_FILTERS_FIR_FILTER_H_
#define LUMOS_MATH_FILTERS_FIR_FILTER_H_

#include "lumos/math/filters/class_def/fir_filter.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace lumos
{

  template <typename T>
  FIRFilter<T>::FIRFilter() : order_(0)
  {
  }

  template <typename T>
  FIRFilter<T>::FIRFilter(const std::vector<T> &coefficients)
      : coefficients_(coefficients),
        order_(coefficients.size() > 0 ? coefficients.size() - 1 : 0)
  {
    delay_line_.resize(coefficients_.size(), T(0));
  }

  template <typename T>
  FIRFilter<T>::FIRFilter(const Vector<T> &coefficients)
      : FIRFilter(std::vector<T>(coefficients.begin(), coefficients.end()))
  {
  }

  template <typename T>
  FIRFilter<T>::FIRFilter(std::initializer_list<T> coefficients)
      : coefficients_(coefficients),
        order_(coefficients.size() > 0 ? coefficients.size() - 1 : 0)
  {
    delay_line_.resize(coefficients_.size(), T(0));
  }

  template <typename T>
  FIRFilter<T>::FIRFilter(size_t order, const T *coefficients)
      : coefficients_(coefficients, coefficients + order + 1), order_(order)
  {
    delay_line_.resize(order + 1, T(0));
  }

  template <typename T>
  FIRFilter<T>::FIRFilter(const FIRFilter &other)
      : coefficients_(other.coefficients_), delay_line_(other.delay_line_),
        order_(other.order_)
  {
  }

  template <typename T>
  FIRFilter<T>::FIRFilter(FIRFilter &&other) noexcept
      : coefficients_(std::move(other.coefficients_)),
        delay_line_(std::move(other.delay_line_)), order_(other.order_)
  {
  }

  template <typename T>
  FIRFilter<T> &FIRFilter<T>::operator=(const FIRFilter &other)
  {
    if (this != &other)
    {
      coefficients_ = other.coefficients_;
      delay_line_ = other.delay_line_;
      order_ = other.order_;
    }
    return *this;
  }

  template <typename T>
  FIRFilter<T> &FIRFilter<T>::operator=(FIRFilter &&other) noexcept
  {
    if (this != &other)
    {
      coefficients_ = std::move(other.coefficients_);
      delay_line_ = std::move(other.delay_line_);
      order_ = other.order_;
    }
    return *this;
  }

  template <typename T>
  T FIRFilter<T>::filter(T input)
  {
    if (coefficients_.empty())
    {
      return T(0);
    }

    // Add new input to delay line
    delay_line_.push_front(input);
    if (delay_line_.size() > coefficients_.size())
    {
      delay_line_.pop_back();
    }

    // Compute output using convolution
    T output = T(0);
    for (size_t i = 0; i < std::min(delay_line_.size(), coefficients_.size());
         ++i)
    {
      output += coefficients_[i] * delay_line_[i];
    }

    return output;
  }

  template <typename T>
  std::vector<T> FIRFilter<T>::filter(const std::vector<T> &input)
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
  Vector<T> FIRFilter<T>::filter(const Vector<T> &input)
  {
    Vector<T> output(input.size());

    for (size_t i = 0; i < input.size(); ++i)
    {
      output(i) = filter(input(i));
    }

    return output;
  }

  template <typename T>
  void FIRFilter<T>::filter(const T *input, T *output, size_t length)
  {
    for (size_t i = 0; i < length; ++i)
    {
      output[i] = filter(input[i]);
    }
  }

  template <typename T>
  void FIRFilter<T>::reset()
  {
    std::fill(delay_line_.begin(), delay_line_.end(), T(0));
  }

  template <typename T>
  void FIRFilter<T>::setInitialConditions(const std::vector<T> &initial_state)
  {
    if (initial_state.size() != coefficients_.size())
    {
      throw std::invalid_argument("Initial state size must match filter order");
    }
    delay_line_.assign(initial_state.begin(), initial_state.end());
  }

  template <typename T>
  void FIRFilter<T>::setCoefficients(const std::vector<T> &coefficients)
  {
    coefficients_ = coefficients;
    order_ = coefficients.size() > 0 ? coefficients.size() - 1 : 0;
    delay_line_.resize(coefficients_.size(), T(0));
  }

  template <typename T>
  void FIRFilter<T>::setCoefficients(const Vector<T> &coefficients)
  {
    setCoefficients(std::vector<T>(coefficients.begin(), coefficients.end()));
  }

  template <typename T>
  const std::vector<T> &FIRFilter<T>::getCoefficients() const
  {
    return coefficients_;
  }

  template <typename T>
  size_t FIRFilter<T>::getOrder() const
  {
    return order_;
  }

  template <typename T>
  size_t FIRFilter<T>::getNumCoefficients() const
  {
    return coefficients_.size();
  }

  template <typename T>
  bool FIRFilter<T>::isEmpty() const
  {
    return coefficients_.empty();
  }

  template <typename T>
  std::complex<T> FIRFilter<T>::frequencyResponse(T frequency,
                                                  T sample_rate) const
  {
    if (coefficients_.empty())
    {
      return std::complex<T>(0, 0);
    }

    T omega = 2 * M_PI * frequency / sample_rate;
    std::complex<T> response(0, 0);

    for (size_t n = 0; n < coefficients_.size(); ++n)
    {
      T phase = -omega * n;
      response +=
          coefficients_[n] * std::complex<T>(std::cos(phase), std::sin(phase));
    }

    return response;
  }

  template <typename T>
  void FIRFilter<T>::printCoefficients() const
  {
    std::cout << "FIR Filter Coefficients: ";
    for (size_t i = 0; i < coefficients_.size(); ++i)
    {
      std::cout << coefficients_[i];
      if (i < coefficients_.size() - 1)
        std::cout << ", ";
    }
    std::cout << std::endl;
  }

  template <typename T>
  T FIRFilter<T>::getGroupDelay() const
  {
    // For linear phase FIR filters, group delay is (N-1)/2
    return order_ > 0 ? T(order_) / T(2) : T(0);
  }

  template <typename T>
  FIRFilter<T> FIRFilter<T>::movingAverage(size_t window_size)
  {
    if (window_size == 0)
    {
      throw std::invalid_argument("Window size must be greater than 0");
    }

    std::vector<T> coeffs(window_size, T(1) / T(window_size));
    return FIRFilter<T>(coeffs);
  }

  template <typename T>
  FIRFilter<T> FIRFilter<T>::lowPass(size_t order, T cutoff_freq, T sample_rate)
  {
    if (order == 0)
    {
      throw std::invalid_argument("Filter order must be greater than 0");
    }

    std::vector<T> coeffs(order + 1);
    T omega_c = 2 * M_PI * cutoff_freq / sample_rate;

    // Windowed sinc function (rectangular window)
    int M = static_cast<int>(order);
    for (int n = 0; n <= M; ++n)
    {
      int k = n - M / 2;
      if (k == 0)
      {
        coeffs[n] = omega_c / M_PI;
      }
      else
      {
        coeffs[n] = std::sin(omega_c * k) / (M_PI * k);
      }
    }

    return FIRFilter<T>(coeffs);
  }

  template <typename T>
  FIRFilter<T> FIRFilter<T>::highPass(size_t order, T cutoff_freq,
                                      T sample_rate)
  {
    // High-pass = All-pass - Low-pass
    FIRFilter<T> lpf = lowPass(order, cutoff_freq, sample_rate);
    std::vector<T> coeffs = lpf.getCoefficients();

    // Spectral inversion
    for (size_t i = 0; i < coeffs.size(); ++i)
    {
      coeffs[i] = -coeffs[i];
    }

    // Add 1 to center coefficient
    if (coeffs.size() > 0)
    {
      coeffs[coeffs.size() / 2] += T(1);
    }

    return FIRFilter<T>(coeffs);
  }

  template <typename T>
  FIRFilter<T> FIRFilter<T>::bandPass(size_t order, T low_freq, T high_freq,
                                      T sample_rate)
  {
    // Band-pass = High-pass(low_freq) - High-pass(high_freq)
    FIRFilter<T> hpf_low = highPass(order, low_freq, sample_rate);
    FIRFilter<T> hpf_high = highPass(order, high_freq, sample_rate);

    std::vector<T> coeffs_low = hpf_low.getCoefficients();
    std::vector<T> coeffs_high = hpf_high.getCoefficients();

    std::vector<T> coeffs(coeffs_low.size());
    for (size_t i = 0; i < coeffs.size(); ++i)
    {
      coeffs[i] = coeffs_low[i] - coeffs_high[i];
    }

    return FIRFilter<T>(coeffs);
  }

  template <typename T>
  FIRFilter<T> FIRFilter<T>::differentiator(size_t order)
  {
    if (order == 0)
    {
      throw std::invalid_argument("Filter order must be greater than 0");
    }

    std::vector<T> coeffs(order + 1);
    int M = static_cast<int>(order);

    for (int n = 0; n <= M; ++n)
    {
      int k = n - M / 2;
      if (k == 0)
      {
        coeffs[n] = T(0);
      }
      else
      {
        coeffs[n] = std::cos(M_PI * k) / k;
      }
    }

    return FIRFilter<T>(coeffs);
  }

  template <typename T>
  FIRFilter<T> FIRFilter<T>::integrator(size_t order)
  {
    if (order == 0)
    {
      throw std::invalid_argument("Filter order must be greater than 0");
    }

    std::vector<T> coeffs(order + 1);
    int M = static_cast<int>(order);

    for (int n = 0; n <= M; ++n)
    {
      int k = n - M / 2;
      if (k == 0)
      {
        coeffs[n] = T(1);
      }
      else
      {
        coeffs[n] = std::sin(M_PI * k) / (M_PI * k);
      }
    }

    return FIRFilter<T>(coeffs);
  }

} // namespace lumos

#endif // LUMOS_MATH_FILTERS_FIR_FILTER_H_
