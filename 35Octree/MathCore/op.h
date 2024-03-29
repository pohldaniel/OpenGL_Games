#pragma once

#include "math_common.h"

#include "impl/mat2_op.h"
#include "impl/mat3_op.h"
#include "impl/mat4_op.h"

#include "impl/quat_op.h"

#include "impl/vec2_op.h"
#include "impl/vec3_op.h"
#include "impl/vec4_op.h"


#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/mat2_op_float_simd.h"
#include "impl/mat3_op_float_simd.h"
#include "impl/mat4_op_float_simd.h"

#include "impl/quat_op_float_simd.h"

#include "impl/vec2_op_float_simd.h"
#include "impl/vec3_op_float_simd.h"
#include "impl/vec4_op_float_simd.h"


#endif