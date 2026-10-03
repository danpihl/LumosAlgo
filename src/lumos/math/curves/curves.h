#ifndef LUMOS_MATH_CURVES_CURVES_H_
#define LUMOS_MATH_CURVES_CURVES_H_

#include "lumos/math/curves/bezier_curve.h"
#include "lumos/math/curves/bspline_curve.h"
#include "lumos/math/curves/quintic_polynomial.h"

namespace lumos
{

  // Common type aliases for convenience
  using BezierCurve2Dd = BezierCurve2D<double>;
  using BezierCurve3Dd = BezierCurve3D<double>;
  using BezierCurve2Df = BezierCurve2D<float>;
  using BezierCurve3Df = BezierCurve3D<float>;

  using BSplineCurve2Dd = BSplineCurve2D<double>;
  using BSplineCurve3Dd = BSplineCurve3D<double>;
  using BSplineCurve2Df = BSplineCurve2D<float>;
  using BSplineCurve3Df = BSplineCurve3D<float>;

  using QuinticPolynomial2Dd = QuinticPolynomial2D<double>;
  using QuinticPolynomial3Dd = QuinticPolynomial3D<double>;
  using QuinticPolynomial2Df = QuinticPolynomial2D<float>;
  using QuinticPolynomial3Df = QuinticPolynomial3D<float>;

  using QuinticPolynomial1Dd = QuinticPolynomial1D<double>;
  using QuinticPolynomial1Df = QuinticPolynomial1D<float>;

} // namespace lumos

#endif // LUMOS_MATH_CURVES_CURVES_H_
