#ifndef LUMOS_MATH_PRE_DEFS_H_
#define LUMOS_MATH_PRE_DEFS_H_

#include "math/misc/forward_decl.h"

namespace lumos {
using AxisAngled = AxisAngle<double>;
using HomogeneousLine2Dd = HomogeneousLine2D<double>;
using ParametricLine2Dd = ParametricLine2D<double>;
using Line3Dd = Line3D<double>;
using Planed = Plane<double>;
using Triangle2Dd = Triangle2D<double>;
using Triangle3Dd = Triangle3D<double>;
using Matrixd = Matrix<double>;
using Vectord = Vector<double>;
using Pointd = Point<double>;
using Vec2d = Vec2<double>;
using Vec3d = Vec3<double>;
using Vec4d = Vec4<double>;
using Point2d = Point2<double>;
using Point3d = Point3<double>;
using Point4d = Point4<double>;

using AxisAnglef = AxisAngle<float>;
using HomogeneousLine2Df = HomogeneousLine2D<float>;
using ParametricLine2Df = ParametricLine2D<float>;
using Line3Df = Line3D<float>;
using Planef = Plane<float>;
using Triangle2Df = Triangle2D<float>;
using Triangle3Df = Triangle3D<float>;
using Matrixf = Matrix<float>;
using Vectorf = Vector<float>;
using Pointf = Point<float>;
using Vec2f = Vec2<float>;
using Vec3f = Vec3<float>;
using Vec4f = Vec4<float>;
using Point2f = Point2<float>;
using Point3f = Point3<float>;
using Point4f = Point4<float>;

using Vec2i = Vec2<int>;

template <typename T> using Matrix3x3 = FixedSizeMatrix<T, 3, 3>;

// Forward declarations for curves
template <typename T, typename VecType> class BezierCurve;
template <typename T, typename VecType> class BSplineCurve;
template <typename T, typename VecType> class QuinticPolynomial;
template <typename T> class QuinticPolynomial1D;

// Forward declarations for filters
template <typename T> class FIRFilter;
template <typename T> class IIRFilter;

} // namespace lumos

#endif // LUMOS_MATH_PRE_DEFS_H_
