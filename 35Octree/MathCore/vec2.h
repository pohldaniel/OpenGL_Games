#pragma once

#include "math_common.h"

#include "impl/vec2_all_no_optimization.h"

#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/vec2_float_simd.h"

#endif

namespace MathCore
{
    //
    // common definitions
    //
    using vec2f32 = vec2<float, SIMD_TYPE::SIMD>;
    using vec2f64 = vec2<double>;

    using vec2i8 = vec2<int8_t>;
    using vec2i16 = vec2<int16_t>;
    using vec2i32 = vec2<int32_t>;
    using vec2i64 = vec2<int64_t>;

    using vec2u8 = vec2<uint8_t>;
    using vec2u16 = vec2<uint16_t>;
    using vec2u32 = vec2<uint32_t>;
    using vec2u64 = vec2<uint64_t>;

    //
    // alias
    //
    using vec2f = vec2f32;
    using vec2d = vec2f64;

    using vec2i = vec2i32;
    using vec2u = vec2u32;

    //
    // Operator Overload Template
    //
    INLINE_STATIC_ARITHMETIC_OPERATION_IMPLEMENTATION(vec2)

    template <typename _type, typename _simd>
    struct CONSTANT<vec2<_type, _simd>>
    {
        using type = vec2<_type, _simd>;

        // // C++17
        // static constexpr type Zero = type((_type)0, (_type)0);
        // static constexpr type One = type((_type)1, (_type)1);

        // C++11
        static constexpr __forceinline type Zero() noexcept { return type(); }
        static constexpr __forceinline type One() noexcept { return type((_type)1); }
    };


}
