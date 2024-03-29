#pragma once

#include "math_common.h"

#include "impl/mat2_gen.h"
#include "impl/mat3_gen.h"
#include "impl/mat4_gen.h"

#include "impl/quat_gen.h"

#include "impl/vec2_gen.h"


#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/mat2_gen_float_simd.h"
#include "impl/mat3_gen_float_simd.h"
#include "impl/mat4_gen_float_simd.h"

#include "impl/quat_gen_float_simd.h"

#include "impl/vec2_gen_float_simd.h"

#endif

