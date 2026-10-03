
#ifndef LUMOS_MATH_GEOMETRY_PLANE_H_
#define LUMOS_MATH_GEOMETRY_PLANE_H_

#include <cmath>
#include <utility>

#include "lumos/math/misc/assert.h"
#include "lumos/math/geometry/class_def/plane.h"
#include "lumos/math/lin_alg.h"

namespace lumos
{
  template <typename T>
  Plane<T>::Plane(const T a_, const T b_, const T c_, const T d_)
  {
    a = a_;
    b = b_;
    c = c_;
    d = d_;
  }

  template <typename T>
  Plane<T>::Plane(const Point3<T> &point, const Vec3<T> &normal_vector)
  {
    a = normal_vector.x;
    b = normal_vector.y;
    c = normal_vector.z;
    d = -(a * point.x + b * point.y + c * point.z);
  }

  template <typename T>
  Plane<T>::Plane() {}

  template <typename T>
  template <typename Y>
  Plane<T>::Plane(const Plane<Y> &p)
  {
    a = p.a;
    b = p.b;
    c = p.c;
    d = p.d;
  }

  template <typename T>
  T Plane<T>::eval(const Point3<T> &p) const
  {
    return a * p.x + b * p.y + c * p.z + d;
  }

  template <typename T>
  T Plane<T>::evalXY(const T x, const T y) const
  {
    return -(a * x + b * y + d) / c;
  }

  template <typename T>
  T Plane<T>::evalXZ(const T x, const T z) const
  {
    return -(a * x + c * z + d) / b;
  }

  template <typename T>
  T Plane<T>::evalYZ(const T y, const T z) const
  {
    return -(b * y + c * z + d) / a;
  }

  template <typename T>
  Plane<T> planeFromThreePoints(const Point3<T> &p0, const Point3<T> &p1,
                                const Point3<T> &p2)
  {
    Vec3<T> v10 = p1.normalizedVectorBetweenPoints(p0);
    Vec3<T> v12 = p1.normalizedVectorBetweenPoints(p2);
    Vec3<T> normal_vector = v10.crossProduct(v12);

    return Plane<T>(p0, normal_vector);
  }

} // namespace lumos

#endif // LUMOS_MATH_GEOMETRY_PLANE_H_
