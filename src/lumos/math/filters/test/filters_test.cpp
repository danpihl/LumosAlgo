#include <algorithm>
#include <cmath>
#include <complex>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

#include "lumos/math/filters/filters.h"

namespace lumos
{

  // Test fixture for FIR Filter tests
  class FIRFilterTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Common test coefficients
      simple_coeffs = {1.0, 0.5, 0.25};
      moving_avg_coeffs = {0.25, 0.25, 0.25, 0.25};
      low_pass_coeffs = {0.1, 0.2, 0.4, 0.2, 0.1};

      // Test input signals
      impulse_input = {1.0, 0.0, 0.0, 0.0, 0.0};
      step_input = {1.0, 1.0, 1.0, 1.0, 1.0};
      ramp_input = {1.0, 2.0, 3.0, 4.0, 5.0};

      // Filter instances
      simple_filter = FIRFilter<double>(simple_coeffs);
      moving_avg_filter = FIRFilter<double>(moving_avg_coeffs);
    }

    std::vector<double> simple_coeffs, moving_avg_coeffs, low_pass_coeffs;
    std::vector<double> impulse_input, step_input, ramp_input;
    FIRFilter<double> simple_filter, moving_avg_filter;
  };

  // Test fixture for IIR Filter tests
  class IIRFilterTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      // Common test coefficients
      b_coeffs = {1.0, 0.5};
      a_coeffs = {1.0, -0.5};

      // Test input signals
      impulse_input = {1.0, 0.0, 0.0, 0.0, 0.0};
      step_input = {1.0, 1.0, 1.0, 1.0, 1.0};
      ramp_input = {1.0, 2.0, 3.0, 4.0, 5.0};

      // Filter instances
      simple_filter = IIRFilter<double>(b_coeffs, a_coeffs);
      sample_rate = 1000.0;
      cutoff_freq = 100.0;
      q_factor = 0.707;
    }

    std::vector<double> b_coeffs, a_coeffs;
    std::vector<double> impulse_input, step_input, ramp_input;
    IIRFilter<double> simple_filter;
    double sample_rate, cutoff_freq, q_factor;
  };

  // =============================================================================
  // FIR FILTER TESTS
  // =============================================================================

  // CONSTRUCTOR TESTS

  TEST_F(FIRFilterTest, DefaultConstructor)
  {
    FIRFilter<double> filter;

    EXPECT_EQ(filter.getOrder(), 0U);
    EXPECT_EQ(filter.getNumCoefficients(), 0U);
    EXPECT_TRUE(filter.isEmpty());
    EXPECT_DOUBLE_EQ(filter.filter(1.0), 0.0);
  }

  TEST_F(FIRFilterTest, VectorConstructor)
  {
    FIRFilter<double> filter(simple_coeffs);

    EXPECT_EQ(filter.getOrder(), 2U);
    EXPECT_EQ(filter.getNumCoefficients(), 3U);
    EXPECT_FALSE(filter.isEmpty());

    const auto &coeffs = filter.getCoefficients();
    EXPECT_EQ(coeffs.size(), 3U);
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  TEST_F(FIRFilterTest, InitializerListConstructor)
  {
    FIRFilter<double> filter({1.0, 0.5, 0.25});

    EXPECT_EQ(filter.getOrder(), 2U);
    EXPECT_EQ(filter.getNumCoefficients(), 3U);

    const auto &coeffs = filter.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  TEST_F(FIRFilterTest, ArrayConstructor)
  {
    double coeffs_array[] = {1.0, 0.5, 0.25};
    FIRFilter<double> filter(2, coeffs_array);

    EXPECT_EQ(filter.getOrder(), 2U);
    EXPECT_EQ(filter.getNumCoefficients(), 3U);

    const auto &coeffs = filter.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  TEST_F(FIRFilterTest, CopyConstructor)
  {
    FIRFilter<double> filter_copy(simple_filter);

    EXPECT_EQ(filter_copy.getOrder(), simple_filter.getOrder());
    EXPECT_EQ(filter_copy.getNumCoefficients(),
              simple_filter.getNumCoefficients());

    const auto &orig_coeffs = simple_filter.getCoefficients();
    const auto &copy_coeffs = filter_copy.getCoefficients();

    EXPECT_EQ(orig_coeffs.size(), copy_coeffs.size());
    for (size_t i = 0; i < orig_coeffs.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(orig_coeffs[i], copy_coeffs[i]);
    }
  }

  TEST_F(FIRFilterTest, MoveConstructor)
  {
    FIRFilter<double> filter_original(simple_coeffs);
    FIRFilter<double> filter_moved(std::move(filter_original));

    EXPECT_EQ(filter_moved.getOrder(), 2U);
    EXPECT_EQ(filter_moved.getNumCoefficients(), 3U);

    const auto &coeffs = filter_moved.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  TEST_F(FIRFilterTest, CopyAssignment)
  {
    FIRFilter<double> filter;
    filter = simple_filter;

    EXPECT_EQ(filter.getOrder(), simple_filter.getOrder());
    EXPECT_EQ(filter.getNumCoefficients(), simple_filter.getNumCoefficients());

    const auto &orig_coeffs = simple_filter.getCoefficients();
    const auto &copy_coeffs = filter.getCoefficients();

    for (size_t i = 0; i < orig_coeffs.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(orig_coeffs[i], copy_coeffs[i]);
    }
  }

  TEST_F(FIRFilterTest, MoveAssignment)
  {
    FIRFilter<double> filter_original(simple_coeffs);
    FIRFilter<double> filter;
    filter = std::move(filter_original);

    EXPECT_EQ(filter.getOrder(), 2U);
    EXPECT_EQ(filter.getNumCoefficients(), 3U);

    const auto &coeffs = filter.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 0.5);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.25);
  }

  // SINGLE SAMPLE FILTERING TESTS

  TEST_F(FIRFilterTest, SingleSampleFiltering)
  {
    // Test impulse response
    double out1 = simple_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 1.0); // First coefficient

    double out2 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out2, 0.5); // Second coefficient

    double out3 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out3, 0.25); // Third coefficient

    double out4 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out4, 0.0); // Should be zero now
  }

  TEST_F(FIRFilterTest, MovingAverageFiltering)
  {
    // Test moving average with step input
    double out1 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 0.25);

    double out2 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out2, 0.5);

    double out3 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out3, 0.75);

    double out4 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out4, 1.0);

    double out5 = moving_avg_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out5, 1.0); // Should stay at 1.0
  }

  // VECTOR FILTERING TESTS

  TEST_F(FIRFilterTest, VectorFiltering)
  {
    FIRFilter<double> filter({1.0, 0.5});
    std::vector<double> input = {1.0, 2.0, 3.0};
    std::vector<double> output = filter.filter(input);

    EXPECT_EQ(output.size(), 3U);
    EXPECT_DOUBLE_EQ(output[0], 1.0); // 1.0 * 1.0
    EXPECT_DOUBLE_EQ(output[1], 2.5); // 1.0 * 2.0 + 0.5 * 1.0
    EXPECT_DOUBLE_EQ(output[2], 4.0); // 1.0 * 3.0 + 0.5 * 2.0
  }

  TEST_F(FIRFilterTest, BatchFiltering)
  {
    FIRFilter<double> filter({1.0, 0.5});
    std::vector<double> input = {1.0, 2.0, 3.0};
    std::vector<double> output(3);

    filter.filter(input.data(), output.data(), input.size());

    EXPECT_DOUBLE_EQ(output[0], 1.0);
    EXPECT_DOUBLE_EQ(output[1], 2.5);
    EXPECT_DOUBLE_EQ(output[2], 4.0);
  }

  // FILTER STATE MANAGEMENT TESTS

  TEST_F(FIRFilterTest, ResetFilter)
  {
    // Apply some input to change internal state
    simple_filter.filter(1.0);
    simple_filter.filter(2.0);

    // Reset filter
    simple_filter.reset();

    // Test that filter behaves as if newly constructed
    double out1 = simple_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 1.0);

    double out2 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out2, 0.5);
  }

  TEST_F(FIRFilterTest, SetInitialConditions)
  {
    std::vector<double> initial_state = {1.0, 2.0, 3.0};
    simple_filter.setInitialConditions(initial_state);

    // After setting initial conditions, the first output should be:
    // coeffs[0] * 0.0 + coeffs[1] * initial_state[0] + coeffs[2] *
    // initial_state[1] = 1.0 * 0.0 + 0.5 * 1.0 + 0.25 * 2.0 = 0.0 + 0.5 + 0.5
    // = 1.0
    double output = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(output, 1.0);
  }

  TEST_F(FIRFilterTest, SetInitialConditionsWrongSize)
  {
    std::vector<double> wrong_size_state = {1.0, 2.0}; // Too few elements
    EXPECT_THROW(simple_filter.setInitialConditions(wrong_size_state),
                 std::invalid_argument);
  }

  // FILTER CONFIGURATION TESTS

  TEST_F(FIRFilterTest, SetCoefficients)
  {
    std::vector<double> new_coeffs = {2.0, 1.0, 0.5};
    simple_filter.setCoefficients(new_coeffs);

    EXPECT_EQ(simple_filter.getOrder(), 2U);
    EXPECT_EQ(simple_filter.getNumCoefficients(), 3U);

    const auto &coeffs = simple_filter.getCoefficients();
    EXPECT_DOUBLE_EQ(coeffs[0], 2.0);
    EXPECT_DOUBLE_EQ(coeffs[1], 1.0);
    EXPECT_DOUBLE_EQ(coeffs[2], 0.5);
  }

  // FREQUENCY RESPONSE TESTS

  TEST_F(FIRFilterTest, FrequencyResponse)
  {
    FIRFilter<double> filter({1.0, 0.0}); // Simple delay
    double sample_rate = 1000.0;
    double frequency = 100.0;

    std::complex<double> response =
        filter.frequencyResponse(frequency, sample_rate);

    // For a simple delay, magnitude should be 1
    EXPECT_NEAR(std::abs(response), 1.0, 1e-10);
  }

  TEST_F(FIRFilterTest, FrequencyResponseEmptyFilter)
  {
    FIRFilter<double> empty_filter;
    std::complex<double> response = empty_filter.frequencyResponse(100.0, 1000.0);

    EXPECT_DOUBLE_EQ(response.real(), 0.0);
    EXPECT_DOUBLE_EQ(response.imag(), 0.0);
  }

  // GROUP DELAY TESTS

  TEST_F(FIRFilterTest, GroupDelay)
  {
    FIRFilter<double> filter({1.0, 0.0, 0.0, 0.0, 1.0}); // Order 4
    double group_delay = filter.getGroupDelay();
    EXPECT_DOUBLE_EQ(group_delay,
                     2.0); // (4-1)/2 = 1.5, wait that's wrong... (4)/2 = 2
  }

  TEST_F(FIRFilterTest, GroupDelayEmptyFilter)
  {
    FIRFilter<double> empty_filter;
    double group_delay = empty_filter.getGroupDelay();
    EXPECT_DOUBLE_EQ(group_delay, 0.0);
  }

  // FACTORY METHOD TESTS

  TEST_F(FIRFilterTest, MovingAverageFactory)
  {
    FIRFilter<double> ma_filter = FIRFilter<double>::movingAverage(4);

    EXPECT_EQ(ma_filter.getOrder(), 3U);
    EXPECT_EQ(ma_filter.getNumCoefficients(), 4U);

    const auto &coeffs = ma_filter.getCoefficients();
    for (size_t i = 0; i < coeffs.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(coeffs[i], 0.25);
    }
  }

  TEST_F(FIRFilterTest, MovingAverageFactoryZeroSize)
  {
    EXPECT_THROW(FIRFilter<double>::movingAverage(0), std::invalid_argument);
  }

  TEST_F(FIRFilterTest, LowPassFactory)
  {
    FIRFilter<double> lp_filter = FIRFilter<double>::lowPass(10, 100.0, 1000.0);

    EXPECT_EQ(lp_filter.getOrder(), 10U);
    EXPECT_EQ(lp_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(lp_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, LowPassFactoryZeroOrder)
  {
    EXPECT_THROW(FIRFilter<double>::lowPass(0, 100.0, 1000.0),
                 std::invalid_argument);
  }

  TEST_F(FIRFilterTest, HighPassFactory)
  {
    FIRFilter<double> hp_filter = FIRFilter<double>::highPass(10, 100.0, 1000.0);

    EXPECT_EQ(hp_filter.getOrder(), 10U);
    EXPECT_EQ(hp_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(hp_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, BandPassFactory)
  {
    FIRFilter<double> bp_filter =
        FIRFilter<double>::bandPass(10, 50.0, 150.0, 1000.0);

    EXPECT_EQ(bp_filter.getOrder(), 10U);
    EXPECT_EQ(bp_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(bp_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, DifferentiatorFactory)
  {
    FIRFilter<double> diff_filter = FIRFilter<double>::differentiator(10);

    EXPECT_EQ(diff_filter.getOrder(), 10U);
    EXPECT_EQ(diff_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(diff_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, DifferentiatorFactoryZeroOrder)
  {
    EXPECT_THROW(FIRFilter<double>::differentiator(0), std::invalid_argument);
  }

  TEST_F(FIRFilterTest, IntegratorFactory)
  {
    FIRFilter<double> int_filter = FIRFilter<double>::integrator(10);

    EXPECT_EQ(int_filter.getOrder(), 10U);
    EXPECT_EQ(int_filter.getNumCoefficients(), 11U);
    EXPECT_FALSE(int_filter.isEmpty());
  }

  TEST_F(FIRFilterTest, IntegratorFactoryZeroOrder)
  {
    EXPECT_THROW(FIRFilter<double>::integrator(0), std::invalid_argument);
  }

  // TYPE ALIASES TESTS

  TEST_F(FIRFilterTest, TypeAliases)
  {
    FIRFilterd filter_d({1.0, 0.5});
    FIRFilterf filter_f({1.0f, 0.5f});

    EXPECT_EQ(filter_d.getOrder(), 1U);
    EXPECT_EQ(filter_f.getOrder(), 1U);

    EXPECT_DOUBLE_EQ(filter_d.filter(1.0), 1.0);
    EXPECT_FLOAT_EQ(filter_f.filter(1.0f), 1.0f);
  }

  // =============================================================================
  // IIR FILTER TESTS
  // =============================================================================

  // CONSTRUCTOR TESTS

  TEST_F(IIRFilterTest, DefaultConstructor)
  {
    IIRFilter<double> filter;

    EXPECT_EQ(filter.getNumeratorOrder(), 0U);
    EXPECT_EQ(filter.getDenominatorOrder(), 0U);
    EXPECT_EQ(filter.getOrder(), 0U);
    EXPECT_TRUE(filter.isEmpty());
    EXPECT_DOUBLE_EQ(filter.filter(1.0), 0.0);
  }

  TEST_F(IIRFilterTest, VectorConstructor)
  {
    IIRFilter<double> filter(b_coeffs, a_coeffs);

    EXPECT_EQ(filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(filter.getDenominatorOrder(), 1U);
    EXPECT_EQ(filter.getOrder(), 1U);
    EXPECT_FALSE(filter.isEmpty());

    const auto &b = filter.getNumeratorCoefficients();
    const auto &a = filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(b[0], 1.0);
    EXPECT_DOUBLE_EQ(b[1], 0.5);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.5);
  }

  TEST_F(IIRFilterTest, InitializerListConstructor)
  {
    IIRFilter<double> filter({1.0, 0.5}, {1.0, -0.5});

    EXPECT_EQ(filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(filter.getDenominatorOrder(), 1U);

    const auto &b = filter.getNumeratorCoefficients();
    const auto &a = filter.getDenominatorCoefficients();

    EXPECT_DOUBLE_EQ(b[0], 1.0);
    EXPECT_DOUBLE_EQ(b[1], 0.5);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.5);
  }

  TEST_F(IIRFilterTest, ArrayConstructor)
  {
    double b_array[] = {1.0, 0.5};
    double a_array[] = {1.0, -0.5};
    IIRFilter<double> filter(1, b_array, 1, a_array);

    EXPECT_EQ(filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(filter.getDenominatorOrder(), 1U);

    const auto &b = filter.getNumeratorCoefficients();
    const auto &a = filter.getDenominatorCoefficients();

    EXPECT_DOUBLE_EQ(b[0], 1.0);
    EXPECT_DOUBLE_EQ(b[1], 0.5);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.5);
  }

  TEST_F(IIRFilterTest, ConstructorInvalidDenominator)
  {
    std::vector<double> invalid_a = {0.0, 1.0}; // First coefficient is zero
    EXPECT_THROW(IIRFilter<double>(b_coeffs, invalid_a), std::invalid_argument);
  }

  TEST_F(IIRFilterTest, ConstructorEmptyDenominator)
  {
    std::vector<double> empty_a;
    EXPECT_THROW(IIRFilter<double>(b_coeffs, empty_a), std::invalid_argument);
  }

  TEST_F(IIRFilterTest, CopyConstructor)
  {
    IIRFilter<double> filter_copy(simple_filter);

    EXPECT_EQ(filter_copy.getNumeratorOrder(), simple_filter.getNumeratorOrder());
    EXPECT_EQ(filter_copy.getDenominatorOrder(),
              simple_filter.getDenominatorOrder());

    const auto &orig_b = simple_filter.getNumeratorCoefficients();
    const auto &copy_b = filter_copy.getNumeratorCoefficients();
    const auto &orig_a = simple_filter.getDenominatorCoefficients();
    const auto &copy_a = filter_copy.getDenominatorCoefficients();

    EXPECT_EQ(orig_b.size(), copy_b.size());
    EXPECT_EQ(orig_a.size(), copy_a.size());

    for (size_t i = 0; i < orig_b.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(orig_b[i], copy_b[i]);
    }
    for (size_t i = 0; i < orig_a.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(orig_a[i], copy_a[i]);
    }
  }

  TEST_F(IIRFilterTest, MoveConstructor)
  {
    IIRFilter<double> filter_original(b_coeffs, a_coeffs);
    IIRFilter<double> filter_moved(std::move(filter_original));

    EXPECT_EQ(filter_moved.getNumeratorOrder(), 1U);
    EXPECT_EQ(filter_moved.getDenominatorOrder(), 1U);

    const auto &b = filter_moved.getNumeratorCoefficients();
    const auto &a = filter_moved.getDenominatorCoefficients();

    EXPECT_DOUBLE_EQ(b[0], 1.0);
    EXPECT_DOUBLE_EQ(b[1], 0.5);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.5);
  }

  // SINGLE SAMPLE FILTERING TESTS

  TEST_F(IIRFilterTest, SingleSampleFiltering)
  {
    // Simple first-order filter: y[n] = x[n] + 0.5*x[n-1] + 0.5*y[n-1]
    double out1 = simple_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 1.0); // 1.0 + 0.5*0 + 0.5*0

    double out2 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out2, 1.0); // 0 + 0.5*1 + 0.5*1

    double out3 = simple_filter.filter(0.0);
    EXPECT_DOUBLE_EQ(out3, 0.5); // 0 + 0.5*0 + 0.5*1
  }

  TEST_F(IIRFilterTest, VectorFiltering)
  {
    IIRFilter<double> filter({1.0}, {1.0, -0.5});
    std::vector<double> input = {1.0, 0.0, 0.0};
    std::vector<double> output = filter.filter(input);

    EXPECT_EQ(output.size(), 3U);
    EXPECT_DOUBLE_EQ(output[0], 1.0);
    EXPECT_DOUBLE_EQ(output[1], 0.5);  // 0 + 0.5*1
    EXPECT_DOUBLE_EQ(output[2], 0.25); // 0 + 0.5*0.5
  }

  TEST_F(IIRFilterTest, BatchFiltering)
  {
    IIRFilter<double> filter({1.0}, {1.0, -0.5});
    std::vector<double> input = {1.0, 0.0, 0.0};
    std::vector<double> output(3);

    filter.filter(input.data(), output.data(), input.size());

    EXPECT_DOUBLE_EQ(output[0], 1.0);
    EXPECT_DOUBLE_EQ(output[1], 0.5);
    EXPECT_DOUBLE_EQ(output[2], 0.25);
  }

  // FILTER STATE MANAGEMENT TESTS

  TEST_F(IIRFilterTest, ResetFilter)
  {
    // Apply some input to change internal state
    simple_filter.filter(1.0);
    simple_filter.filter(2.0);

    // Reset filter
    simple_filter.reset();

    // Test that filter behaves as if newly constructed
    double out1 = simple_filter.filter(1.0);
    EXPECT_DOUBLE_EQ(out1, 1.0);
  }

  TEST_F(IIRFilterTest, SetInitialConditions)
  {
    std::vector<double> x_initial = {1.0, 2.0};
    std::vector<double> y_initial = {0.5, 1.0};

    simple_filter.setInitialConditions(x_initial, y_initial);

    double output = simple_filter.filter(0.0);
    // Expected: 0*1 + 0.5*1 + 0.5*0.5 = 0.75
    EXPECT_DOUBLE_EQ(output, 0.75);
  }

  TEST_F(IIRFilterTest, SetInitialConditionsWrongSize)
  {
    std::vector<double> x_wrong = {1.0}; // Too few elements
    std::vector<double> y_wrong = {0.5}; // Too few elements
    std::vector<double> y_correct = {0.5, 1.0};

    EXPECT_THROW(simple_filter.setInitialConditions(x_wrong, y_correct),
                 std::invalid_argument);
    EXPECT_THROW(simple_filter.setInitialConditions(b_coeffs, y_wrong),
                 std::invalid_argument);
  }

  // FILTER CONFIGURATION TESTS

  TEST_F(IIRFilterTest, SetCoefficients)
  {
    std::vector<double> new_b = {2.0, 1.0};
    std::vector<double> new_a = {1.0, -0.25};

    simple_filter.setCoefficients(new_b, new_a);

    EXPECT_EQ(simple_filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(simple_filter.getDenominatorOrder(), 1U);

    const auto &b = simple_filter.getNumeratorCoefficients();
    const auto &a = simple_filter.getDenominatorCoefficients();

    EXPECT_DOUBLE_EQ(b[0], 2.0);
    EXPECT_DOUBLE_EQ(b[1], 1.0);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -0.25);
  }

  TEST_F(IIRFilterTest, SetCoefficientsInvalidDenominator)
  {
    std::vector<double> new_b = {1.0};
    std::vector<double> invalid_a = {0.0, 1.0}; // First coefficient is zero

    EXPECT_THROW(simple_filter.setCoefficients(new_b, invalid_a),
                 std::invalid_argument);
  }

  // FREQUENCY RESPONSE TESTS

  TEST_F(IIRFilterTest, FrequencyResponse)
  {
    IIRFilter<double> filter({1.0}, {1.0, 0.0}); // Simple passthrough
    double sample_rate = 1000.0;
    double frequency = 100.0;

    std::complex<double> response =
        filter.frequencyResponse(frequency, sample_rate);

    // For a simple passthrough, magnitude should be 1
    EXPECT_NEAR(std::abs(response), 1.0, 1e-10);
  }

  TEST_F(IIRFilterTest, FrequencyResponseEmptyFilter)
  {
    IIRFilter<double> empty_filter;
    std::complex<double> response = empty_filter.frequencyResponse(100.0, 1000.0);

    EXPECT_DOUBLE_EQ(response.real(), 0.0);
    EXPECT_DOUBLE_EQ(response.imag(), 0.0);
  }

  // FACTORY METHOD TESTS

  TEST_F(IIRFilterTest, FirstOrderLowPassFactory)
  {
    IIRFilter<double> lp_filter =
        IIRFilter<double>::firstOrderLowPass(cutoff_freq, sample_rate);

    EXPECT_EQ(lp_filter.getNumeratorOrder(), 0U);
    EXPECT_EQ(lp_filter.getDenominatorOrder(), 1U);
    EXPECT_FALSE(lp_filter.isEmpty());

    const auto &b = lp_filter.getNumeratorCoefficients();
    const auto &a = lp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 1U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, FirstOrderHighPassFactory)
  {
    IIRFilter<double> hp_filter =
        IIRFilter<double>::firstOrderHighPass(cutoff_freq, sample_rate);

    EXPECT_EQ(hp_filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(hp_filter.getDenominatorOrder(), 1U);
    EXPECT_FALSE(hp_filter.isEmpty());

    const auto &b = hp_filter.getNumeratorCoefficients();
    const auto &a = hp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, SecondOrderLowPassFactory)
  {
    IIRFilter<double> lp_filter =
        IIRFilter<double>::secondOrderLowPass(cutoff_freq, q_factor, sample_rate);

    EXPECT_EQ(lp_filter.getNumeratorOrder(), 2U);
    EXPECT_EQ(lp_filter.getDenominatorOrder(), 2U);
    EXPECT_FALSE(lp_filter.isEmpty());

    const auto &b = lp_filter.getNumeratorCoefficients();
    const auto &a = lp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(a.size(), 3U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, SecondOrderHighPassFactory)
  {
    IIRFilter<double> hp_filter = IIRFilter<double>::secondOrderHighPass(
        cutoff_freq, q_factor, sample_rate);

    EXPECT_EQ(hp_filter.getNumeratorOrder(), 2U);
    EXPECT_EQ(hp_filter.getDenominatorOrder(), 2U);
    EXPECT_FALSE(hp_filter.isEmpty());

    const auto &b = hp_filter.getNumeratorCoefficients();
    const auto &a = hp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(a.size(), 3U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, SecondOrderBandPassFactory)
  {
    double center_freq = 100.0;
    IIRFilter<double> bp_filter = IIRFilter<double>::secondOrderBandPass(
        center_freq, q_factor, sample_rate);

    EXPECT_EQ(bp_filter.getNumeratorOrder(), 2U);
    EXPECT_EQ(bp_filter.getDenominatorOrder(), 2U);
    EXPECT_FALSE(bp_filter.isEmpty());

    const auto &b = bp_filter.getNumeratorCoefficients();
    const auto &a = bp_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(a.size(), 3U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, SecondOrderNotchFactory)
  {
    double center_freq = 100.0;
    IIRFilter<double> notch_filter =
        IIRFilter<double>::secondOrderNotch(center_freq, q_factor, sample_rate);

    EXPECT_EQ(notch_filter.getNumeratorOrder(), 2U);
    EXPECT_EQ(notch_filter.getDenominatorOrder(), 2U);
    EXPECT_FALSE(notch_filter.isEmpty());

    const auto &b = notch_filter.getNumeratorCoefficients();
    const auto &a = notch_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 3U);
    EXPECT_EQ(a.size(), 3U);
    EXPECT_DOUBLE_EQ(a[0], 1.0); // Normalized form
  }

  TEST_F(IIRFilterTest, IntegratorFactory)
  {
    IIRFilter<double> int_filter = IIRFilter<double>::integrator(sample_rate);

    EXPECT_EQ(int_filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(int_filter.getDenominatorOrder(), 1U);
    EXPECT_FALSE(int_filter.isEmpty());

    const auto &b = int_filter.getNumeratorCoefficients();
    const auto &a = int_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
    EXPECT_DOUBLE_EQ(a[1], -1.0);
  }

  TEST_F(IIRFilterTest, DifferentiatorFactory)
  {
    IIRFilter<double> diff_filter =
        IIRFilter<double>::differentiator(sample_rate);

    EXPECT_EQ(diff_filter.getNumeratorOrder(), 1U);
    EXPECT_EQ(diff_filter.getDenominatorOrder(), 0U);
    EXPECT_FALSE(diff_filter.isEmpty());

    const auto &b = diff_filter.getNumeratorCoefficients();
    const auto &a = diff_filter.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 1U);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
  }

  TEST_F(IIRFilterTest, DCBlockerFactory)
  {
    IIRFilter<double> dc_blocker = IIRFilter<double>::dcBlocker(1.0, sample_rate);

    EXPECT_EQ(dc_blocker.getNumeratorOrder(), 1U);
    EXPECT_EQ(dc_blocker.getDenominatorOrder(), 1U);
    EXPECT_FALSE(dc_blocker.isEmpty());

    const auto &b = dc_blocker.getNumeratorCoefficients();
    const auto &a = dc_blocker.getDenominatorCoefficients();

    EXPECT_EQ(b.size(), 2U);
    EXPECT_EQ(a.size(), 2U);
    EXPECT_DOUBLE_EQ(a[0], 1.0);
  }

  // TYPE ALIASES TESTS

  TEST_F(IIRFilterTest, TypeAliases)
  {
    IIRFilterd filter_d({1.0}, {1.0, -0.5});
    IIRFilterf filter_f({1.0f}, {1.0f, -0.5f});

    EXPECT_EQ(filter_d.getNumeratorOrder(), 0U);
    EXPECT_EQ(filter_f.getNumeratorOrder(), 0U);

    EXPECT_DOUBLE_EQ(filter_d.filter(1.0), 1.0);
    EXPECT_FLOAT_EQ(filter_f.filter(1.0f), 1.0f);
  }

  // EDGE CASES AND ERROR HANDLING

  TEST_F(IIRFilterTest, EmptyFilterBehavior)
  {
    IIRFilter<double> empty_filter;

    EXPECT_DOUBLE_EQ(empty_filter.filter(1.0), 0.0);
    EXPECT_DOUBLE_EQ(empty_filter.filter(5.0), 0.0);

    std::vector<double> input = {1.0, 2.0, 3.0};
    std::vector<double> output = empty_filter.filter(input);

    EXPECT_EQ(output.size(), 3U);
    for (double val : output)
    {
      EXPECT_DOUBLE_EQ(val, 0.0);
    }
  }

  TEST_F(FIRFilterTest, EmptyFilterBehavior)
  {
    FIRFilter<double> empty_filter;

    EXPECT_DOUBLE_EQ(empty_filter.filter(1.0), 0.0);
    EXPECT_DOUBLE_EQ(empty_filter.filter(5.0), 0.0);

    std::vector<double> input = {1.0, 2.0, 3.0};
    std::vector<double> output = empty_filter.filter(input);

    EXPECT_EQ(output.size(), 3U);
    for (double val : output)
    {
      EXPECT_DOUBLE_EQ(val, 0.0);
    }
  }

  // FILTERING STABILITY TESTS

  TEST_F(IIRFilterTest, StableLowPassFiltering)
  {
    IIRFilter<double> stable_filter =
        IIRFilter<double>::firstOrderLowPass(100.0, 1000.0);

    // Apply a constant input - should converge to a stable value
    std::vector<double> outputs;
    for (int i = 0; i < 100; ++i)
    {
      outputs.push_back(stable_filter.filter(1.0));
    }

    // Check that the output doesn't explode
    for (double output : outputs)
    {
      EXPECT_LT(std::abs(output), 10.0); // Should be bounded
    }

    // Check that it converges (last few values should be similar)
    EXPECT_NEAR(outputs[99], outputs[98], 1e-6);
    EXPECT_NEAR(outputs[98], outputs[97], 1e-6);
  }

  // COMPREHENSIVE INTEGRATION TESTS

  TEST_F(FIRFilterTest, MovingAverageIntegrationTest)
  {
    // Test moving average with known signal
    FIRFilter<double> ma_filter = FIRFilter<double>::movingAverage(3);

    std::vector<double> input = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> output = ma_filter.filter(input);

    // Expected outputs for 3-point moving average
    EXPECT_DOUBLE_EQ(output[0], 1.0 / 3.0);               // [1]/3
    EXPECT_DOUBLE_EQ(output[1], (1.0 + 2.0) / 3.0);       // [1,2]/3
    EXPECT_DOUBLE_EQ(output[2], (1.0 + 2.0 + 3.0) / 3.0); // [1,2,3]/3
    EXPECT_DOUBLE_EQ(output[3], (2.0 + 3.0 + 4.0) / 3.0); // [2,3,4]/3
    EXPECT_DOUBLE_EQ(output[4], (3.0 + 4.0 + 5.0) / 3.0); // [3,4,5]/3
  }

  TEST_F(IIRFilterTest, IntegratorIntegrationTest)
  {
    // Test integrator with known signal
    IIRFilter<double> integrator =
        IIRFilter<double>::integrator(1.0); // 1 Hz sample rate

    std::vector<double> input = {1.0, 1.0, 1.0, 1.0}; // Step input
    std::vector<double> output = integrator.filter(input);

    // Output should be approximately a ramp (cumulative sum scaled by Ts/2)
    EXPECT_GT(output[0], 0.0);
    EXPECT_GT(output[1], output[0]);
    EXPECT_GT(output[2], output[1]);
    EXPECT_GT(output[3], output[2]);
  }

  // PERFORMANCE AND PRECISION TESTS

  TEST_F(FIRFilterTest, PrecisionTest)
  {
    // Test with very small coefficients
    FIRFilter<double> precise_filter({1e-10, 1e-10, 1e-10});

    double output = precise_filter.filter(1e10);
    EXPECT_NEAR(output, 1e-10 * 1e10, 1e-15); // Should be 1.0
  }

  TEST_F(IIRFilterTest, PrecisionTest)
  {
    // Test with very small coefficients
    IIRFilter<double> precise_filter({1e-10}, {1.0, 1e-10});

    double output = precise_filter.filter(1e10);
    EXPECT_NEAR(output, 1e-10 * 1e10, 1e-15); // Should be 1.0
  }

  // Stability, Poles and Zeros Tests

  // Sorts by real part, then imaginary part, so that roots can be compared
  // against expected values
  static std::vector<std::complex<double>>
  sortedRoots(std::vector<std::complex<double>> roots)
  {
    std::sort(roots.begin(), roots.end(),
              [](const std::complex<double> &r0, const std::complex<double> &r1) {
                if (std::abs(r0.real() - r1.real()) > 1e-6)
                {
                  return r0.real() < r1.real();
                }
                return r0.imag() < r1.imag();
              });
    return roots;
  }

  // Coefficients of the monic polynomial with the given roots
  static std::vector<double>
  polynomialFromRoots(const std::vector<std::complex<double>> &roots)
  {
    std::vector<std::complex<double>> coeffs = {1.0};
    for (const std::complex<double> &root : roots)
    {
      coeffs.push_back(0.0);
      for (size_t i = coeffs.size() - 1; i >= 1; --i)
      {
        coeffs[i] -= root * coeffs[i - 1];
      }
    }
    std::vector<double> real_coeffs;
    for (const std::complex<double> &coeff : coeffs)
    {
      real_coeffs.push_back(coeff.real());
    }
    return real_coeffs;
  }

  static double gainAt(const IIRFilter<double> &filter, const double frequency,
                       const double sample_rate)
  {
    return std::abs(filter.frequencyResponse(frequency, sample_rate));
  }

  TEST_F(IIRFilterTest, IsStable)
  {
    // Pole at 0.5
    EXPECT_TRUE(IIRFilter<double>({1.0}, {1.0, -0.5}).isStable());
    // Pole at -0.99
    EXPECT_TRUE(IIRFilter<double>({1.0}, {1.0, 0.99}).isStable());
    // Pole at 1.5
    EXPECT_FALSE(IIRFilter<double>({1.0}, {1.0, -1.5}).isStable());
    // Pole on the unit circle
    EXPECT_FALSE(IIRFilter<double>({1.0}, {1.0, -1.0}).isStable());
    EXPECT_FALSE(IIRFilter<double>::integrator(100.0).isStable());
    // No feedback
    EXPECT_TRUE(IIRFilter<double>({1.0, 2.0, 3.0}, {1.0}).isStable());
    // Complex poles 0.5 +- 0.5i, magnitude 0.707
    EXPECT_TRUE(IIRFilter<double>({1.0}, {1.0, -1.0, 0.5}).isStable());
    // Complex poles 1 +- 0.5i, magnitude 1.118
    EXPECT_FALSE(IIRFilter<double>({1.0}, {1.0, -2.0, 1.25}).isStable());
    // Real poles 0.5 and 2: the product hides the unstable one in a[2]
    EXPECT_FALSE(IIRFilter<double>({1.0}, {1.0, -2.5, 1.0}).isStable());
    // a[0] different from one: pole at 0.5
    EXPECT_TRUE(IIRFilter<double>({1.0}, {2.0, -1.0}).isStable());
    // a[0] different from one: pole at 2
    EXPECT_FALSE(IIRFilter<double>({1.0}, {0.5, -1.0}).isStable());

    EXPECT_TRUE(IIRFilter<double>().isStable());
  }

  TEST_F(IIRFilterTest, IsStableAgreesWithPoles)
  {
    // Higher order denominators built from known roots
    const std::vector<std::complex<double>> stable_roots = {
        {0.9, 0.0}, {-0.3, 0.0}, {0.5, 0.5}, {0.5, -0.5}, {-0.2, 0.9}, {-0.2, -0.9}};
    EXPECT_TRUE(
        IIRFilter<double>({1.0}, polynomialFromRoots(stable_roots)).isStable());

    std::vector<std::complex<double>> unstable_roots = stable_roots;
    unstable_roots[4] = {-0.5, 0.9};
    unstable_roots[5] = {-0.5, -0.9};
    EXPECT_FALSE(
        IIRFilter<double>({1.0}, polynomialFromRoots(unstable_roots)).isStable());
  }

  TEST_F(IIRFilterTest, FactoryFiltersAreStable)
  {
    EXPECT_TRUE(IIRFilter<double>::firstOrderLowPass(100.0, 1000.0).isStable());
    EXPECT_TRUE(IIRFilter<double>::firstOrderHighPass(100.0, 1000.0).isStable());
    EXPECT_TRUE(
        IIRFilter<double>::secondOrderLowPass(100.0, 0.707, 1000.0).isStable());
    EXPECT_TRUE(
        IIRFilter<double>::secondOrderNotch(100.0, 5.0, 1000.0).isStable());
    EXPECT_TRUE(IIRFilter<double>::differentiator(1000.0).isStable());
  }

  TEST_F(IIRFilterTest, PolesFirstAndSecondOrder)
  {
    const std::vector<std::complex<double>> poles1 =
        IIRFilter<double>({1.0}, {1.0, -0.5}).getPoles();
    ASSERT_EQ(poles1.size(), 1u);
    EXPECT_NEAR(poles1[0].real(), 0.5, 1e-12);
    EXPECT_NEAR(poles1[0].imag(), 0.0, 1e-12);

    // z^2 - z + 0.5 has roots 0.5 +- 0.5i
    const std::vector<std::complex<double>> poles2 =
        sortedRoots(IIRFilter<double>({1.0}, {1.0, -1.0, 0.5}).getPoles());
    ASSERT_EQ(poles2.size(), 2u);
    EXPECT_NEAR(poles2[0].real(), 0.5, 1e-12);
    EXPECT_NEAR(poles2[0].imag(), -0.5, 1e-12);
    EXPECT_NEAR(poles2[1].real(), 0.5, 1e-12);
    EXPECT_NEAR(poles2[1].imag(), 0.5, 1e-12);

    // z^2 - 2.5 * z + 1 has roots 0.5 and 2
    const std::vector<std::complex<double>> poles3 =
        sortedRoots(IIRFilter<double>({1.0}, {2.0, -5.0, 2.0}).getPoles());
    ASSERT_EQ(poles3.size(), 2u);
    EXPECT_NEAR(poles3[0].real(), 0.5, 1e-12);
    EXPECT_NEAR(poles3[1].real(), 2.0, 1e-12);
    EXPECT_NEAR(poles3[0].imag(), 0.0, 1e-12);
    EXPECT_NEAR(poles3[1].imag(), 0.0, 1e-12);
  }

  TEST_F(IIRFilterTest, PolesHigherOrder)
  {
    const std::vector<std::complex<double>> expected = sortedRoots(
        {{0.9, 0.0}, {-0.3, 0.0}, {0.5, 0.5}, {0.5, -0.5}, {-0.2, 0.9}, {-0.2, -0.9}, {1.5, 0.0}});
    const IIRFilter<double> filter({1.0}, polynomialFromRoots(expected));
    const std::vector<std::complex<double>> poles =
        sortedRoots(filter.getPoles());

    ASSERT_EQ(poles.size(), expected.size());
    for (size_t i = 0; i < poles.size(); ++i)
    {
      EXPECT_NEAR(poles[i].real(), expected[i].real(), 1e-9);
      EXPECT_NEAR(poles[i].imag(), expected[i].imag(), 1e-9);
    }
  }

  TEST_F(IIRFilterTest, ZerosOfNotchAreOnUnitCircleAtCenterFrequency)
  {
    const double fs = 1000.0;
    const double f0 = 100.0;
    const std::vector<std::complex<double>> zeros =
        sortedRoots(IIRFilter<double>::secondOrderNotch(f0, 5.0, fs).getZeros());

    ASSERT_EQ(zeros.size(), 2u);
    const double omega = 2.0 * M_PI * f0 / fs;
    EXPECT_NEAR(zeros[0].real(), std::cos(omega), 1e-12);
    EXPECT_NEAR(zeros[0].imag(), -std::sin(omega), 1e-12);
    EXPECT_NEAR(zeros[1].real(), std::cos(omega), 1e-12);
    EXPECT_NEAR(zeros[1].imag(), std::sin(omega), 1e-12);
  }

  TEST_F(IIRFilterTest, PolesAndZerosAtOrigin)
  {
    // H(z) = 1 + z^-1 + z^-2 = (z^2 + z + 1) / z^2: two poles at the origin
    const IIRFilter<double> fir_like({1.0, 1.0, 1.0}, {1.0});
    const std::vector<std::complex<double>> poles = fir_like.getPoles();
    ASSERT_EQ(poles.size(), 2u);
    EXPECT_EQ(std::abs(poles[0]), 0.0);
    EXPECT_EQ(std::abs(poles[1]), 0.0);
    EXPECT_EQ(fir_like.getZeros().size(), 2u);

    // H(z) = 1 / (1 - 0.5 * z^-1) = z / (z - 0.5): one zero at the origin
    const IIRFilter<double> all_pole({1.0}, {1.0, -0.5});
    const std::vector<std::complex<double>> zeros = all_pole.getZeros();
    ASSERT_EQ(zeros.size(), 1u);
    EXPECT_EQ(std::abs(zeros[0]), 0.0);

    // Leading zero numerator coefficient (pure delay): H(z) = z^-1 = 1 / z
    const IIRFilter<double> delay({0.0, 1.0}, {1.0});
    EXPECT_EQ(delay.getZeros().size(), 0u);
    ASSERT_EQ(delay.getPoles().size(), 1u);
    EXPECT_EQ(std::abs(delay.getPoles()[0]), 0.0);

    EXPECT_TRUE(IIRFilter<double>().getPoles().empty());
    EXPECT_TRUE(IIRFilter<double>().getZeros().empty());
  }

  // Butterworth Tests

  TEST_F(IIRFilterTest, ButterworthLowPassKnownCoefficients)
  {
    // Reference: scipy.signal.butter(2, 0.2) and scipy.signal.butter(4, 0.2)
    const IIRFilter<double> order2 =
        IIRFilter<double>::butterworthLowPass(2, 100.0, 1000.0);
    const std::vector<double> b2 = {0.06745527, 0.13491055, 0.06745527};
    const std::vector<double> a2 = {1.0, -1.1429805, 0.4128016};
    ASSERT_EQ(order2.getNumeratorCoefficients().size(), 3u);
    ASSERT_EQ(order2.getDenominatorCoefficients().size(), 3u);
    for (size_t i = 0; i < 3; ++i)
    {
      EXPECT_NEAR(order2.getNumeratorCoefficients()[i], b2[i], 1e-7);
      EXPECT_NEAR(order2.getDenominatorCoefficients()[i], a2[i], 1e-7);
    }

    const IIRFilter<double> order4 =
        IIRFilter<double>::butterworthLowPass(4, 100.0, 1000.0);
    const std::vector<double> b4 = {0.00482434, 0.01929737, 0.02894606,
                                    0.01929737, 0.00482434};
    const std::vector<double> a4 = {1.0, -2.36951301, 2.31398841, -1.05466541,
                                    0.18737949};
    ASSERT_EQ(order4.getNumeratorCoefficients().size(), 5u);
    ASSERT_EQ(order4.getDenominatorCoefficients().size(), 5u);
    for (size_t i = 0; i < 5; ++i)
    {
      EXPECT_NEAR(order4.getNumeratorCoefficients()[i], b4[i], 1e-7);
      EXPECT_NEAR(order4.getDenominatorCoefficients()[i], a4[i], 1e-7);
    }
  }

  TEST_F(IIRFilterTest, ButterworthLowPassFrequencyResponse)
  {
    const double fs = 1000.0;
    const double fc = 80.0;
    for (size_t order = 1; order <= 8; ++order)
    {
      const IIRFilter<double> filter =
          IIRFilter<double>::butterworthLowPass(order, fc, fs);
      EXPECT_EQ(filter.getOrder(), order);
      EXPECT_TRUE(filter.isStable());

      EXPECT_NEAR(gainAt(filter, 0.0, fs), 1.0, 1e-9);
      // -3 dB at the cutoff, for every order
      EXPECT_NEAR(gainAt(filter, fc, fs), 1.0 / std::sqrt(2.0), 1e-9);
      EXPECT_NEAR(gainAt(filter, fs / 2.0, fs), 0.0, 1e-9);

      // Monotonically decreasing
      double previous_gain = gainAt(filter, 0.0, fs);
      for (double f = 5.0; f < fs / 2.0; f += 5.0)
      {
        const double gain = gainAt(filter, f, fs);
        EXPECT_LE(gain, previous_gain + 1e-9);
        previous_gain = gain;
      }
    }
  }

  TEST_F(IIRFilterTest, ButterworthLowPassMatchesAnalogMagnitude)
  {
    // |H(f)|^2 = 1 / (1 + (W / Wc)^(2 * N)), with W the prewarped frequency
    const double fs = 1000.0;
    const double fc = 120.0;
    const size_t order = 5;
    const IIRFilter<double> filter =
        IIRFilter<double>::butterworthLowPass(order, fc, fs);
    const double wc = std::tan(M_PI * fc / fs);
    for (double f = 10.0; f < 450.0; f += 20.0)
    {
      const double w = std::tan(M_PI * f / fs);
      const double expected =
          1.0 / std::sqrt(1.0 + std::pow(w / wc, 2.0 * order));
      EXPECT_NEAR(gainAt(filter, f, fs), expected, 1e-9);
    }
  }

  TEST_F(IIRFilterTest, ButterworthHighPassFrequencyResponse)
  {
    const double fs = 1000.0;
    const double fc = 80.0;
    for (size_t order = 1; order <= 8; ++order)
    {
      const IIRFilter<double> filter =
          IIRFilter<double>::butterworthHighPass(order, fc, fs);
      EXPECT_EQ(filter.getOrder(), order);
      EXPECT_TRUE(filter.isStable());

      // The tolerance at DC grows with the order, since the response there is
      // the sum of large coefficients of alternating sign
      EXPECT_NEAR(gainAt(filter, 0.0, fs), 0.0, 1e-7);
      EXPECT_NEAR(gainAt(filter, fc, fs), 1.0 / std::sqrt(2.0), 1e-9);
      EXPECT_NEAR(gainAt(filter, fs / 2.0, fs), 1.0, 1e-9);

      // Monotonically increasing
      double previous_gain = gainAt(filter, 5.0, fs);
      for (double f = 10.0; f < fs / 2.0; f += 5.0)
      {
        const double gain = gainAt(filter, f, fs);
        EXPECT_GE(gain, previous_gain - 1e-9);
        previous_gain = gain;
      }
    }
  }

  TEST_F(IIRFilterTest, ButterworthHighPassKnownCoefficients)
  {
    // Reference: scipy.signal.butter(2, 0.2, 'highpass')
    const IIRFilter<double> filter =
        IIRFilter<double>::butterworthHighPass(2, 100.0, 1000.0);
    const std::vector<double> b = {0.63894553, -1.27789105, 0.63894553};
    const std::vector<double> a = {1.0, -1.1429805, 0.4128016};
    for (size_t i = 0; i < 3; ++i)
    {
      EXPECT_NEAR(filter.getNumeratorCoefficients()[i], b[i], 1e-7);
      EXPECT_NEAR(filter.getDenominatorCoefficients()[i], a[i], 1e-7);
    }
  }

  TEST_F(IIRFilterTest, ButterworthLowAndHighPassArePowerComplementary)
  {
    // |H_lp|^2 + |H_hp|^2 = 1 at every frequency
    const double fs = 1000.0;
    const IIRFilter<double> lpf =
        IIRFilter<double>::butterworthLowPass(3, 150.0, fs);
    const IIRFilter<double> hpf =
        IIRFilter<double>::butterworthHighPass(3, 150.0, fs);
    for (double f = 0.0; f <= 500.0; f += 25.0)
    {
      const double g_lp = gainAt(lpf, f, fs);
      const double g_hp = gainAt(hpf, f, fs);
      EXPECT_NEAR(g_lp * g_lp + g_hp * g_hp, 1.0, 1e-9);
    }
  }

  TEST_F(IIRFilterTest, ButterworthPolesAndZeros)
  {
    const size_t order = 4;
    const IIRFilter<double> filter =
        IIRFilter<double>::butterworthLowPass(order, 100.0, 1000.0);

    const std::vector<std::complex<double>> poles = filter.getPoles();
    ASSERT_EQ(poles.size(), order);
    for (const std::complex<double> &pole : poles)
    {
      EXPECT_LT(std::abs(pole), 1.0);
    }
    // The poles reproduce the denominator
    const std::vector<double> a = polynomialFromRoots(poles);
    for (size_t i = 0; i <= order; ++i)
    {
      EXPECT_NEAR(a[i], filter.getDenominatorCoefficients()[i], 1e-9);
    }

    // All zeros are at z = -1. A root of multiplicity 4 is only accurate to
    // about the fourth root of the machine precision
    const std::vector<std::complex<double>> zeros = filter.getZeros();
    ASSERT_EQ(zeros.size(), order);
    for (const std::complex<double> &zero : zeros)
    {
      EXPECT_NEAR(zero.real(), -1.0, 1e-2);
      EXPECT_NEAR(zero.imag(), 0.0, 1e-2);
    }
  }

  TEST_F(IIRFilterTest, ButterworthStepResponseSettlesAtOne)
  {
    IIRFilter<double> filter =
        IIRFilter<double>::butterworthLowPass(4, 50.0, 1000.0);
    double output = 0.0;
    for (int i = 0; i < 500; ++i)
    {
      output = filter.filter(1.0);
    }
    EXPECT_NEAR(output, 1.0, 1e-6);
  }

  TEST_F(IIRFilterTest, ButterworthInvalidArguments)
  {
    EXPECT_THROW(IIRFilter<double>::butterworthLowPass(0, 100.0, 1000.0),
                 std::invalid_argument);
    EXPECT_THROW(IIRFilter<double>::butterworthHighPass(0, 100.0, 1000.0),
                 std::invalid_argument);
    // Cutoff must be strictly between 0 and the Nyquist frequency
    EXPECT_THROW(IIRFilter<double>::butterworthLowPass(2, 0.0, 1000.0),
                 std::invalid_argument);
    EXPECT_THROW(IIRFilter<double>::butterworthLowPass(2, 500.0, 1000.0),
                 std::invalid_argument);
    EXPECT_THROW(IIRFilter<double>::butterworthHighPass(2, 600.0, 1000.0),
                 std::invalid_argument);
    EXPECT_THROW(IIRFilter<double>::butterworthLowPass(2, -10.0, 1000.0),
                 std::invalid_argument);
    EXPECT_THROW(IIRFilter<double>::butterworthLowPass(2, 100.0, 0.0),
                 std::invalid_argument);
  }

  // Chebyshev Tests

  TEST_F(IIRFilterTest, ChebyshevLowPassFrequencyResponse)
  {
    const double fs = 1000.0;
    const double fc = 100.0;
    for (const double ripple_db : {0.1, 0.5, 1.0, 3.0})
    {
      const double ripple_gain = std::pow(10.0, -ripple_db / 20.0);
      for (size_t order = 1; order <= 8; ++order)
      {
        const IIRFilter<double> filter =
            IIRFilter<double>::chebyshevLowPass(order, fc, ripple_db, fs);
        EXPECT_EQ(filter.getOrder(), order);
        EXPECT_TRUE(filter.isStable());

        // DC is at the top of the ripple for odd orders, at the bottom for even
        const double expected_dc = (order % 2 == 1) ? 1.0 : ripple_gain;
        EXPECT_NEAR(gainAt(filter, 0.0, fs), expected_dc, 1e-9);
        // The passband edge is at the bottom of the ripple
        EXPECT_NEAR(gainAt(filter, fc, fs), ripple_gain, 1e-9);
        EXPECT_NEAR(gainAt(filter, fs / 2.0, fs), 0.0, 1e-9);

        // The gain stays within the ripple band over the whole passband
        for (double f = 0.0; f <= fc; f += 1.0)
        {
          const double gain = gainAt(filter, f, fs);
          EXPECT_LE(gain, 1.0 + 1e-9);
          EXPECT_GE(gain, ripple_gain - 1e-9);
        }

        // Monotonically decreasing in the stopband
        double previous_gain = gainAt(filter, fc, fs);
        for (double f = fc + 5.0; f < fs / 2.0; f += 5.0)
        {
          const double gain = gainAt(filter, f, fs);
          EXPECT_LE(gain, previous_gain + 1e-9);
          previous_gain = gain;
        }
      }
    }
  }

  TEST_F(IIRFilterTest, ChebyshevLowPassMatchesAnalogMagnitude)
  {
    // |H(f)|^2 = 1 / (1 + eps^2 * T_N(W / Wc)^2), with T_N the Chebyshev
    // polynomial and W the prewarped frequency
    const double fs = 1000.0;
    const double fc = 100.0;
    const double ripple_db = 1.0;
    const size_t order = 5;
    const IIRFilter<double> filter =
        IIRFilter<double>::chebyshevLowPass(order, fc, ripple_db, fs);

    const double eps2 = std::pow(10.0, ripple_db / 10.0) - 1.0;
    const double wc = std::tan(M_PI * fc / fs);
    for (double f = 5.0; f < 450.0; f += 15.0)
    {
      const double x = std::tan(M_PI * f / fs) / wc;
      const double tn = (x <= 1.0) ? std::cos(order * std::acos(x))
                                   : std::cosh(order * std::acosh(x));
      const double expected = 1.0 / std::sqrt(1.0 + eps2 * tn * tn);
      EXPECT_NEAR(gainAt(filter, f, fs), expected, 1e-9);
    }
  }

  TEST_F(IIRFilterTest, ChebyshevRollsOffFasterThanButterworth)
  {
    const double fs = 1000.0;
    const IIRFilter<double> chebyshev =
        IIRFilter<double>::chebyshevLowPass(4, 100.0, 1.0, fs);
    const IIRFilter<double> butterworth =
        IIRFilter<double>::butterworthLowPass(4, 100.0, fs);
    EXPECT_LT(gainAt(chebyshev, 200.0, fs), gainAt(butterworth, 200.0, fs));
    EXPECT_LT(gainAt(chebyshev, 300.0, fs), gainAt(butterworth, 300.0, fs));
  }

  TEST_F(IIRFilterTest, ChebyshevInvalidArguments)
  {
    EXPECT_THROW(IIRFilter<double>::chebyshevLowPass(0, 100.0, 1.0, 1000.0),
                 std::invalid_argument);
    EXPECT_THROW(IIRFilter<double>::chebyshevLowPass(2, 100.0, 0.0, 1000.0),
                 std::invalid_argument);
    EXPECT_THROW(IIRFilter<double>::chebyshevLowPass(2, 100.0, -1.0, 1000.0),
                 std::invalid_argument);
    EXPECT_THROW(IIRFilter<double>::chebyshevLowPass(2, 500.0, 1.0, 1000.0),
                 std::invalid_argument);
  }

  TEST_F(IIRFilterTest, DesignFactoriesFloatInstantiation)
  {
    const IIRFilter<float> butterworth =
        IIRFilter<float>::butterworthLowPass(3, 100.0f, 1000.0f);
    EXPECT_TRUE(butterworth.isStable());
    EXPECT_NEAR(std::abs(butterworth.frequencyResponse(100.0f, 1000.0f)),
                1.0f / std::sqrt(2.0f), 1e-4f);

    const IIRFilter<float> chebyshev =
        IIRFilter<float>::chebyshevLowPass(3, 100.0f, 1.0f, 1000.0f);
    EXPECT_TRUE(chebyshev.isStable());
    EXPECT_EQ(chebyshev.getPoles().size(), 3u);

    const IIRFilter<float> high_pass =
        IIRFilter<float>::butterworthHighPass(2, 100.0f, 1000.0f);
    EXPECT_EQ(high_pass.getZeros().size(), 2u);
  }

  // Interoperability with the library's Vector type

  TEST_F(FIRFilterTest, DynamicVectorInterface)
  {
    const std::vector<double> std_coeffs = {0.5, 0.3, 0.2};
    const std::vector<double> std_input = {1.0, 2.0, 3.0, 4.0, 5.0};
    const Vector<double> coeffs(std_coeffs);
    const Vector<double> input(std_input);

    FIRFilter<double> std_filter(std_coeffs);
    FIRFilter<double> vector_filter(coeffs);
    EXPECT_EQ(vector_filter.getCoefficients(), std_coeffs);

    const std::vector<double> expected = std_filter.filter(std_input);
    const Vector<double> output = vector_filter.filter(input);
    ASSERT_EQ(output.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(output(i), expected[i]);
    }

    FIRFilter<double> filter;
    filter.setCoefficients(coeffs);
    EXPECT_EQ(filter.getCoefficients(), std_coeffs);
  }

  TEST_F(IIRFilterTest, DynamicVectorInterface)
  {
    const std::vector<double> std_b = {0.2, 0.1};
    const std::vector<double> std_a = {1.0, -0.5};
    const std::vector<double> std_input = {1.0, 2.0, 3.0, 4.0, 5.0};
    const Vector<double> b(std_b);
    const Vector<double> a(std_a);
    const Vector<double> input(std_input);

    IIRFilter<double> std_filter(std_b, std_a);
    IIRFilter<double> vector_filter(b, a);
    EXPECT_EQ(vector_filter.getNumeratorCoefficients(), std_b);
    EXPECT_EQ(vector_filter.getDenominatorCoefficients(), std_a);

    const std::vector<double> expected = std_filter.filter(std_input);
    const Vector<double> output = vector_filter.filter(input);
    ASSERT_EQ(output.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(output(i), expected[i]);
    }

    IIRFilter<double> filter;
    filter.setCoefficients(b, a);
    EXPECT_EQ(filter.getNumeratorCoefficients(), std_b);
    EXPECT_EQ(filter.getDenominatorCoefficients(), std_a);
  }

} // namespace lumos
