#ifndef LUMOS_MATH_FILTERS_CLASS_DEF_FIR_FILTER_H_
#define LUMOS_MATH_FILTERS_CLASS_DEF_FIR_FILTER_H_

#include "lumos/math/lin_alg/vector_dynamic/vector_dynamic.h"
#include "lumos/math/misc/forward_decl.h"
#include <complex>
#include <deque>
#include <initializer_list>
#include <vector>

namespace lumos
{

  template <typename T>
  class FIRFilter
  {
  private:
    std::vector<T> coefficients_;
    std::deque<T> delay_line_;
    size_t order_;

  public:
    // Constructors
    FIRFilter();
    explicit FIRFilter(const std::vector<T> &coefficients);
    explicit FIRFilter(const Vector<T> &coefficients);
    FIRFilter(std::initializer_list<T> coefficients);
    FIRFilter(size_t order, const T *coefficients);

    // Copy and move constructors
    FIRFilter(const FIRFilter &other);
    FIRFilter(FIRFilter &&other) noexcept;

    // Assignment operators
    FIRFilter &operator=(const FIRFilter &other);
    FIRFilter &operator=(FIRFilter &&other) noexcept;

    // Core filtering operations
    T filter(T input);
    std::vector<T> filter(const std::vector<T> &input);
    Vector<T> filter(const Vector<T> &input);

    // Batch processing with output buffer
    void filter(const T *input, T *output, size_t length);

    // Filter state management
    void reset();
    void setInitialConditions(const std::vector<T> &initial_state);

    // Filter configuration
    void setCoefficients(const std::vector<T> &coefficients);
    void setCoefficients(const Vector<T> &coefficients);
    const std::vector<T> &getCoefficients() const;

    // Filter properties
    size_t getOrder() const;
    size_t getNumCoefficients() const;
    bool isEmpty() const;

    // Frequency response
    std::complex<T> frequencyResponse(T frequency, T sample_rate) const;

    // Utility methods
    void printCoefficients() const;
    T getGroupDelay() const;

    // Static factory methods for common filters
    static FIRFilter<T> movingAverage(size_t window_size);
    static FIRFilter<T> lowPass(size_t order, T cutoff_freq, T sample_rate);
    static FIRFilter<T> highPass(size_t order, T cutoff_freq, T sample_rate);
    static FIRFilter<T> bandPass(size_t order, T low_freq, T high_freq,
                                 T sample_rate);
    static FIRFilter<T> differentiator(size_t order);
    static FIRFilter<T> integrator(size_t order);
  };

  // Type aliases for common use cases
  using FIRFilterd = FIRFilter<double>;
  using FIRFilterf = FIRFilter<float>;

} // namespace lumos

#endif // LUMOS_MATH_FILTERS_CLASS_DEF_FIR_FILTER_H_
