#ifndef LUMOS_MATH_MATH_H_
#define LUMOS_MATH_MATH_H_

// clang-format off
#include "lumos/math/misc/forward_decl.h"

#include "lumos/math/lin_alg/matrix_dynamic/matrix_dynamic.h"
#include "lumos/math/lin_alg/matrix_dynamic/matrix_math_functions.h"
#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed.h"
#include "lumos/math/lin_alg/fixed_size_vector/fixed_size_vector.h"
#include "lumos/math/lin_alg/matrix_vector_dynamic.h"
#include "lumos/math/lin_alg/matrix_vector_fixed.h"
#include "lumos/math/lin_alg/vector_dynamic/vector_dynamic.h"
#include "lumos/math/lin_alg/vector_dynamic/vector_math_functions.h"
#include "lumos/math/lin_alg/vector_low_dim/vec2.h"
#include "lumos/math/lin_alg/vector_low_dim/vec3.h"
#include "lumos/math/lin_alg/vector_low_dim/vec4.h"
#include "lumos/math/lin_alg/conversions.h"

#include "lumos/math/transformations/euler_angles.h"
#include "lumos/math/transformations/axis_angle.h"

#include "lumos/math/geometry/line_2d.h"
#include "lumos/math/geometry/line_3d.h"
#include "lumos/math/geometry/plane.h"
#include "lumos/math/geometry/triangle.h"

#include "lumos/math/structures/index_triplet.h"

#include "lumos/math/image/image_gray.h"
#include "lumos/math/image/image_gray_alpha.h"
#include "lumos/math/image/image_rgb.h"
#include "lumos/math/image/image_rgba.h"

#include "lumos/math/transformations/quaternion.h"
#include "lumos/math/transformations/angles.h"
#include "lumos/math/transformations/so3.h"
#include "lumos/math/transformations/se2.h"
#include "lumos/math/transformations/se3.h"
#include "lumos/math/curves/curves.h"
#include "lumos/math/filters/filters.h"
#include "lumos/math/estimation/attitude_estimator.h"

#include "lumos/math/pre_defs.h"

// Stream output for the fixed size types
#include "lumos/math/lin_alg/fixed_size_vector/fixed_size_vector_io.h"
#include "lumos/math/lin_alg/matrix_fixed/matrix_fixed_io.h"
#include "lumos/math/lin_alg/vector_low_dim/vec_io.h"
#include "lumos/math/transformations/transformations_io.h"
// clang-format on

#endif // LUMOS_MATH_MATH_H_
