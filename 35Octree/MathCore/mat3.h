#pragma once

#include "math_common.h"

#include "impl/mat3_all_no_optimization.h"

#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/mat3_float_simd.h"

#endif

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

namespace MathCore
{
    //
    // common definitions
    //
    using mat3f32 = mat3<float, SIMD_TYPE::SIMD>;
    using mat3f64 = mat3<double>;

    // using mat3i8 = mat3<int8_t>;
    // using mat3i16 = mat3<int16_t>;
    // using mat3i32 = mat3<int32_t>;
    // using mat3i64 = mat3<int64_t>;

    // using mat3u8 = mat3<uint8_t>;
    // using mat3u16 = mat3<uint16_t>;
    // using mat3u32 = mat3<uint32_t>;
    // using mat3u64 = mat3<uint64_t>;

    //
    // alias
    //
    using mat3f = mat3f32;
    using mat3d = mat3f64;

    // using mat3i = mat3i32;
    // using mat3u = mat3u32;

    //
    // Operator Overload Template
    //
    INLINE_STATIC_ARITHMETIC_OPERATION_IMPLEMENTATION(mat3)

    //
    // templated constants
    //

    template <typename _type, typename _simd>
    struct CONSTANT<mat3<_type, _simd>>
    {
        using type = mat3<_type, _simd>;

        // // C++17
        // static constexpr type Identity = type(
        //     1, 0, 0,
        //     0, 1, 0,
        //     0, 0, 1
        // );
        // C++11
        static constexpr __forceinline type Identity() noexcept
        {
            return type();
        }
    };

    

}
