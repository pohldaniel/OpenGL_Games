#pragma once

#include "math_common.h"

#include "impl/mat4_all_no_optimization.h"

#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/mat4_float_simd.h"

#endif

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

namespace MathCore
{
    //
    // common definitions
    //
    using mat4f32 = mat4<float, SIMD_TYPE::SIMD>;
    using mat4f64 = mat4<double>;

    // using mat4i8 = mat4<int8_t>;
    // using mat4i16 = mat4<int16_t>;
    // using mat4i32 = mat4<int32_t>;
    // using mat4i64 = mat4<int64_t>;

    // using mat4u8 = mat4<uint8_t>;
    // using mat4u16 = mat4<uint16_t>;
    // using mat4u32 = mat4<uint32_t>;
    // using mat4u64 = mat4<uint64_t>;

    //
    // alias
    //
    using mat4f = mat4f32;
    using mat4d = mat4f64;

    // using mat4i = mat4i32;
    // using mat4u = mat4u32;

    //
    // Operator Overload Template
    //
    INLINE_STATIC_ARITHMETIC_OPERATION_IMPLEMENTATION(mat4)

    //
    // templated constants
    //

    template <typename _type, typename _simd>
    struct CONSTANT<mat4<_type, _simd>>
    {
        using type = mat4<_type, _simd>;
        // // C++17
        // static constexpr type Identity = type(
        //     1, 0, 0, 0,
        //     0, 1, 0, 0,
        //     0, 0, 1, 0,
        //     0, 0, 0, 1
        // );
        // C++11
        static constexpr __forceinline type Identity() noexcept
        {
            return type();
        }
    };

    

    

}
