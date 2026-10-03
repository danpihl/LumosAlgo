#ifndef LUMOS_MATH_TRANSFORMATIONS_ANGLES_H_
#define LUMOS_MATH_TRANSFORMATIONS_ANGLES_H_

#include <cmath>

namespace lumos
{

  template <typename T>
  constexpr T pi()
  {
    return static_cast<T>(3.14159265358979323846264338327950288L);
  }

  template <typename T>
  constexpr T degToRad(const T deg)
  {
    return deg * pi<T>() / static_cast<T>(180);
  }

  template <typename T>
  constexpr T radToDeg(const T rad)
  {
    return rad * static_cast<T>(180) / pi<T>();
  }

  // Wraps angle to the interval [0, 2*pi)
  template <typename T>
  T wrapTo2Pi(const T angle)
  {
    const T two_pi = static_cast<T>(2) * pi<T>();
    T wrapped = std::fmod(angle, two_pi);
    if (wrapped < static_cast<T>(0))
    {
      wrapped += two_pi;
    }
    // Rounding in the addition above can land exactly on 2*pi
    if (wrapped >= two_pi)
    {
      wrapped = static_cast<T>(0);
    }
    return wrapped;
  }

  // Wraps angle to the interval [-pi, pi)
  template <typename T>
  T wrapToPi(const T angle)
  {
    return wrapTo2Pi(angle + pi<T>()) - pi<T>();
  }

  // Signed smallest rotation that takes angle "from" to angle "to", in [-pi, pi)
  template <typename T>
  T shortestAngularDistance(const T from, const T to)
  {
    return wrapToPi(to - from);
  }

  // Interpolates along the shortest arc between a0 (t = 0) and a1 (t = 1).
  // The result is wrapped to [-pi, pi)
  template <typename T>
  T interpolateAngle(const T a0, const T a1, const T t)
  {
    return wrapToPi(a0 + t * shortestAngularDistance(a0, a1));
  }

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_ANGLES_H_
