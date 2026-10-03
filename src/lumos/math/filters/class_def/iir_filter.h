#ifndef LUMOS_MATH_FILTERS_CLASS_DEF_IIR_FILTER_H_
#define LUMOS_MATH_FILTERS_CLASS_DEF_IIR_FILTER_H_

#include "lumos/math/lin_alg/vector_dynamic/vector_dynamic.h"
#include "lumos/math/misc/forward_decl.h"
#include <complex>
#include <deque>
#include <initializer_list>
#include <vector>

namespace lumos
{

  template <typename T>
  class IIRFilter
  {
  private:
    std::vector<T> b_coefficients_; // Numerator coefficients (feedforward)
    std::vector<T> a_coefficients_; // Denominator coefficients (feedback)
    std::deque<T> x_delay_line_;    // Input delay line
    std::deque<T> y_delay_line_;    // Output delay line
    size_t numerator_order_;
    size_t denominator_order_;

  public:
    // Constructors
    IIRFilter();
    IIRFilter(const std::vector<T> &b_coeffs, const std::vector<T> &a_coeffs);
    IIRFilter(const Vector<T> &b_coeffs, const Vector<T> &a_coeffs);
    IIRFilter(std::initializer_list<T> b_coeffs,
              std::initializer_list<T> a_coeffs);
    IIRFilter(size_t b_order, const T *b_coeffs, size_t a_order,
              const T *a_coeffs);

    // Copy and move constructors
    IIRFilter(const IIRFilter &other);
    IIRFilter(IIRFilter &&other) noexcept;

    // Assignment operators
    IIRFilter &operator=(const IIRFilter &other);
    IIRFilter &operator=(IIRFilter &&other) noexcept;

    // Core filtering operations
    T filter(T input);
    std::vector<T> filter(const std::vector<T> &input);
    Vector<T> filter(const Vector<T> &input);

    // Batch processing with output buffer
    void filter(const T *input, T *output, size_t length);

    // Filter state management
    void reset();
    void setInitialConditions(const std::vector<T> &x_initial,
                              const std::vector<T> &y_initial);

    // Filter configuration
    void setCoefficients(const std::vector<T> &b_coeffs,
                         const std::vector<T> &a_coeffs);
    void setCoefficients(const Vector<T> &b_coeffs, const Vector<T> &a_coeffs);
    const std::vector<T> &getNumeratorCoefficients() const;
    const std::vector<T> &getDenominatorCoefficients() const;

    // Filter properties
    size_t getNumeratorOrder() const;
    size_t getDenominatorOrder() const;
    size_t getOrder() const; // Max of numerator and denominator order
    bool isEmpty() const;
    bool isStable() const; // Check if poles are inside unit circle

    // Frequency response
    std::complex<T> frequencyResponse(T frequency, T sample_rate) const;

    // Utility methods
    void printCoefficients() const;
    std::vector<std::complex<T>> getPoles() const;
    std::vector<std::complex<T>> getZeros() const;

    // Static factory methods for common filters
    static IIRFilter<T> firstOrderLowPass(T cutoff_freq, T sample_rate);
    static IIRFilter<T> firstOrderHighPass(T cutoff_freq, T sample_rate);
    static IIRFilter<T> secondOrderLowPass(T cutoff_freq, T q_factor,
                                           T sample_rate);
    static IIRFilter<T> secondOrderHighPass(T cutoff_freq, T q_factor,
                                            T sample_rate);
    static IIRFilter<T> secondOrderBandPass(T center_freq, T q_factor,
                                            T sample_rate);
    static IIRFilter<T> secondOrderNotch(T center_freq, T q_factor,
                                         T sample_rate);
    static IIRFilter<T> butterworthLowPass(size_t order, T cutoff_freq,
                                           T sample_rate);
    static IIRFilter<T> butterworthHighPass(size_t order, T cutoff_freq,
                                            T sample_rate);
    static IIRFilter<T> chebyshevLowPass(size_t order, T cutoff_freq, T ripple_db,
                                         T sample_rate);
    static IIRFilter<T> integrator(T sample_rate);
    static IIRFilter<T> differentiator(T sample_rate);
    static IIRFilter<T> dcBlocker(T cutoff_freq, T sample_rate);
  };

  // Type aliases for common use cases
  using IIRFilterd = IIRFilter<double>;
  using IIRFilterf = IIRFilter<float>;

} // namespace lumos

#endif // LUMOS_MATH_FILTERS_CLASS_DEF_IIR_FILTER_H_
