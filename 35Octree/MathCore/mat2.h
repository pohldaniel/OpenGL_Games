#pragma once

#include "math_common.h"

#include "impl/mat2_all_no_optimization.h"

#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/mat2_float_simd.h"

#endif

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

namespace MathCore
{
    //
    // common definitions
    //
    using mat2f32 = mat2<float, SIMD_TYPE::SIMD>;
    using mat2f64 = mat2<double>;

    // using mat2i8 = mat2<int8_t>;
    // using mat2i16 = mat2<int16_t>;
    // using mat2i32 = mat2<int32_t>;
    // using mat2i64 = mat2<int64_t>;

    // using mat2u8 = mat2<uint8_t>;
    // using mat2u16 = mat2<uint16_t>;
    // using mat2u32 = mat2<uint32_t>;
    // using mat2u64 = mat2<uint64_t>;

    //
    // alias
    //
    using mat2f = mat2f32;
    using mat2d = mat2f64;

    // using mat2i = mat2i32;
    // using mat2u = mat2u32;

    //
    // Operator Overload Template
    //
    INLINE_STATIC_ARITHMETIC_OPERATION_IMPLEMENTATION(mat2)

    //
    // templated constants
    //

    template <typename _type, typename _simd>
    struct CONSTANT<mat2<_type, _simd>>
    {
        using type = mat2<_type, _simd>;

        // // C++17
        // static constexpr __forceinline type Identity = type(
        //     1, 0,
        //     0, 1
        // );
        // C++11
        static constexpr __forceinline type Identity() noexcept
        {
            return type();
        }
    };

    

}
