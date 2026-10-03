#ifndef LUMOS_MATH_MISC_ASSERT_H_
#define LUMOS_MATH_MISC_ASSERT_H_

// ASSERT(condition) << "message" << value;
//
// By default this is a small assert in standard C++ with no dependency on an
// operating system or on streams. When the condition is false it calls the
// assert handler and then halts with std::abort(). The streamed message is
// only there to document the assert in the code: it is not evaluated when the
// condition holds, and it is discarded when it fails. Reporting the failure is
// up to the application, in the handler it installs with setAssertHandler().
//
// Defining LUMOS_ASSERT_USE_LOGGING makes ASSERT the one from
// lumos/logging.h, which prints the message. LumosAlgo's own CMake build
// defines it.

#if defined(LUMOS_ASSERT_USE_LOGGING)

#include "lumos/logging.h"

#else

#include <cstdlib>

namespace lumos
{
  // Called when an assert fails, with the source location and the text of the
  // condition. If it returns, the program is halted with std::abort()
  using AssertHandler = void (*)(const char *file, int line, const char *condition);

  namespace internal
  {
    inline void defaultAssertHandler(const char *, int, const char *)
    {
      std::abort();
    }

    inline AssertHandler assert_handler = defaultAssertHandler;

    // Calls the handler when it goes out of scope, which is after the message
    // has been streamed into it
    class AssertFailure
    {
    public:
      AssertFailure(const char *file, int line, const char *condition)
          : file_(file), line_(line), condition_(condition) {}

      AssertFailure(const AssertFailure &) = delete;
      AssertFailure &operator=(const AssertFailure &) = delete;

      ~AssertFailure()
      {
        if (assert_handler != nullptr)
        {
          assert_handler(file_, line_, condition_);
        }
        std::abort();
      }

      // Accepts and discards anything
      template <typename U>
      AssertFailure &operator<<(const U &)
      {
        return *this;
      }

    private:
      const char *file_;
      int line_;
      const char *condition_;
    };

    // Turns the streamed expression into void, so that both branches of the
    // conditional expression in ASSERT have the same type. The operator has
    // lower precedence than <<, so it applies after the whole message
    struct AssertVoidify
    {
      void operator&(const AssertFailure &) const {}
    };

  } // namespace internal

  // Installs the handler that is called when an assert fails. Passing nullptr
  // restores the default, which calls std::abort()
  inline void setAssertHandler(AssertHandler handler)
  {
    internal::assert_handler =
        (handler != nullptr) ? handler : internal::defaultAssertHandler;
  }

  inline AssertHandler getAssertHandler()
  {
    return internal::assert_handler;
  }

} // namespace lumos

#define ASSERT(cond) \
  (cond) ? static_cast<void>(0) : ::lumos::internal::AssertVoidify() & ::lumos::internal::AssertFailure(__FILE__, __LINE__, #cond)

#endif // LUMOS_ASSERT_USE_LOGGING

#endif // LUMOS_MATH_MISC_ASSERT_H_
