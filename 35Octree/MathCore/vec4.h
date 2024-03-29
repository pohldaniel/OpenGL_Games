#pragma once

#include "math_common.h"

#include "impl/vec4_all_no_optimization.h"

#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/vec4_float_simd.h"

#endif

#include "vec3.h"
//#include "mat3.h"

namespace MathCore
{
    //
    // common definitions
    //
    using vec4f32 = vec4<float, SIMD_TYPE::SIMD>;
    using vec4f64 = vec4<double>;

    using vec4i8 = vec4<int8_t>;
    using vec4i16 = vec4<int16_t>;
    using vec4i32 = vec4<int32_t>;
    using vec4i64 = vec4<int64_t>;

    using vec4u8 = vec4<uint8_t>;
    using vec4u16 = vec4<uint16_t>;
    using vec4u32 = vec4<uint32_t>;
    using vec4u64 = vec4<uint64_t>;

    //
    // alias
    //
    using vec4f = vec4f32;
    using vec4d = vec4f64;

    using vec4i = vec4i32;
    using vec4u = vec4u32;

    //
    // Operator Overload Template
    //
    INLINE_STATIC_ARITHMETIC_OPERATION_IMPLEMENTATION(vec4)

    template <typename _type, typename _simd>
    struct CONSTANT<vec4<_type, _simd>>
    {
        using type = vec4<_type, _simd>;

        // // C++17
        // static constexpr type Zero = type((_type)0, (_type)0, (_type)0, (_type)0);
        // static constexpr type One = type((_type)1, (_type)1, (_type)1, (_type)1);

        // C++11
        static constexpr __forceinline type Zero() noexcept { return type(); }
        static constexpr __forceinline type One() noexcept { return type((_type)1); }
    };

    

}
