// Build check for the fixed-size math on a bare-metal target: it must compile
// as strict standard C++17 without exceptions and RTTI, and must not pull in
// <iostream>, <fstream>, <thread> or <mutex>. Run from the repo root, with GXX
// set to an arm-none-eabi-g++:
//
//   F="-std=c++17 -pedantic-errors -fno-exceptions -fno-rtti"
//   $GXX -fsyntax-only $F -mcpu=cortex-m0plus -mthumb -Isrc check.cpp
//   $GXX -M $F -mcpu=cortex-m0plus -mthumb -Isrc check.cpp | tr ' \\' '\n\n' | grep -E '/(iostream|fstream|thread|mutex)$'
//   c++ -fsyntax-only $F -Isrc check.cpp
//
// The first and third must give no errors, the second must print nothing
#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/math/transformations/quaternion.h"

float check()
{
  const lumos::Quaternion<float> q =
      lumos::Quaternion<float>::fromAxisAngle(lumos::Vec3<float>(0.0f, 0.0f, 1.0f), 0.5f);
  const lumos::Vec3<float> v = q.rotate(lumos::Vec3<float>(1.0f, 2.0f, 3.0f));
  const lumos::FixedSizeMatrix<float, 3, 3> r = q.toRotationMatrix();
  const auto r_inv = r.inverse();

  return v.x + (r_inv ? (*r_inv)(0, 0) : 0.0f);
}

int main()
{
  return check() > 0.0f ? 0 : 1;
}
