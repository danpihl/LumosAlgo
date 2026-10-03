#ifndef LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_SE2_H_
#define LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_SE2_H_

#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/lin_alg/vector_low_dim/vec2.h"
#include "lumos/math/misc/forward_decl.h"

namespace lumos
{

  // Element of the Lie algebra se(2)
  template <typename T>
  struct Twist2
  {
    Vec2<T> linear;
    T angular;

    Twist2();
    Twist2(const Vec2<T> &linear_, const T angular_);
    template <typename Y>
    Twist2(const Twist2<Y> &twist);
  };

  // Rigid transformation in 2D: p_a = R(theta) * p_b + translation
  template <typename T>
  class SE2
  {
  public:
    // Always in [-pi, pi) when set through the constructors
    T theta;
    Vec2<T> translation;

    // Identity transformation
    SE2();
    SE2(const T theta_, const Vec2<T> &translation_);
    SE2(const T theta_, const T x, const T y);
    explicit SE2(const FixedSizeMatrix<T, 3, 3> &homogeneous_matrix);
    template <typename Y>
    SE2(const SE2<Y> &pose);

    // Exponential map
    static SE2 exp(const Twist2<T> &twist);
    // Logarithmic map
    Twist2<T> log() const;

    // Composition: (a * b) * p == a * (b * p)
    SE2 operator*(const SE2 &other) const;
    // Transforms a point (applies rotation and translation)
    Vec2<T> operator*(const Vec2<T> &p) const;
    Vec2<T> transformPoint(const Vec2<T> &p) const;
    // Transforms a direction (applies rotation only)
    Vec2<T> rotateVector(const Vec2<T> &v) const;

    SE2 inverse() const;

    FixedSizeMatrix<T, 2, 2> rotationMatrix() const;
    FixedSizeMatrix<T, 3, 3> toHomogeneousMatrix() const;
  };

} // namespace lumos

#endif // LUMOS_MATH_TRANSFORMATIONS_CLASS_DEF_SE2_H_
