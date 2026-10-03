#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>

#include "lumos/math/lin_alg/matrix_dynamic/matrix_dynamic.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/math/misc/assert.h"

#if defined(LUMOS_ASSERT_USE_LOGGING)
#error "This test is for the default assert, without LUMOS_ASSERT_USE_LOGGING"
#endif

namespace lumos
{
  namespace
  {
    int handler_calls = 0;

    void countingHandler(const char *, int, const char *)
    {
      ++handler_calls;
    }

    // Reports the failure and returns, as a handler in an application may do
    void reportingHandler(const char *file, int line, const char *condition)
    {
      std::fprintf(stderr, "assert handler: %s:%d: %s\n", file, line, condition);
    }

    int evaluations = 0;

    int countEvaluation()
    {
      ++evaluations;
      return 0;
    }

    void multiplyWithWrongSize()
    {
      Matrix<double> m(2, 2);
      m.fill(1.0);
      const Vec3<double> v = m * Vec3<double>(1.0, 2.0, 3.0);
      std::exit(v.x > 0.0 ? 0 : 1);
    }

    class AssertTest : public ::testing::Test
    {
    protected:
      void SetUp() override
      {
        handler_calls = 0;
        evaluations = 0;
        setAssertHandler(nullptr);
      }

      void TearDown() override
      {
        setAssertHandler(nullptr);
      }
    };

  } // namespace

  TEST_F(AssertTest, PassingAssertDoesNothing)
  {
    setAssertHandler(countingHandler);

    ASSERT(true);
    ASSERT(1 + 1 == 2) << "with a message";
    ASSERT(true) << "several " << 3 << " parts " << 1.5 << 'c';

    EXPECT_EQ(handler_calls, 0);
  }

  TEST_F(AssertTest, MessageIsNotEvaluatedWhenPassing)
  {
    ASSERT(true) << "value: " << countEvaluation();
    EXPECT_EQ(evaluations, 0);
  }

  TEST_F(AssertTest, ConditionIsEvaluatedOnce)
  {
    ASSERT(countEvaluation() == 0) << "message";
    EXPECT_EQ(evaluations, 1);
  }

  TEST_F(AssertTest, WorksAsASingleStatement)
  {
    setAssertHandler(countingHandler);
    int branch = 0;

    // Without braces, the else must still belong to the outer if
    if (handler_calls == 0)
      ASSERT(true) << "first";
    else
      branch = 1;

    for (int i = 0; i < 3; ++i)
      ASSERT(i < 3) << "index " << i;

    EXPECT_EQ(branch, 0);
    EXPECT_EQ(handler_calls, 0);
  }

  TEST_F(AssertTest, HandlerCanBeSetAndRestored)
  {
    const AssertHandler default_handler = getAssertHandler();
    EXPECT_NE(default_handler, nullptr);

    setAssertHandler(countingHandler);
    EXPECT_EQ(getAssertHandler(), countingHandler);

    setAssertHandler(nullptr);
    EXPECT_EQ(getAssertHandler(), default_handler);
  }

  TEST_F(AssertTest, FailingAssertAbortsByDefault)
  {
    EXPECT_DEATH(ASSERT(1 == 2) << "message", "");
  }

  TEST_F(AssertTest, FailingAssertCallsHandlerWithLocationAndCondition)
  {
    setAssertHandler(reportingHandler);
    EXPECT_DEATH(ASSERT(1 == 2) << "message " << 5,
                 "assert handler: .*assert_test.cpp:[0-9]+: 1 == 2");
  }

  TEST_F(AssertTest, HaltsEvenIfHandlerReturns)
  {
    setAssertHandler(countingHandler);
    // countingHandler returns, so reaching the statement after the assert
    // would exit normally and fail the death test
    EXPECT_DEATH(
        {
          ASSERT(false) << "message";
          std::exit(0);
        },
        "");
  }

  TEST_F(AssertTest, LibraryAssertsUseTheHandler)
  {
    setAssertHandler(reportingHandler);
    // Multiplying a Vec3 with a matrix that is not 3x3 fails an assert in
    // vec3.h
    EXPECT_DEATH(multiplyWithWrongSize(), "assert handler: .*vec3.h:[0-9]+: m.numRows\\(\\) == 3");
  }

} // namespace lumos
