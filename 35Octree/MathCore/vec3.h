#pragma once

#include "math_common.h"

#include "impl/vec3_all_no_optimization.h"

#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/vec3_float_simd.h"

#endif

namespace MathCore
{
    //
    // common definitions
    //
    using vec3f32 = vec3<float, SIMD_TYPE::SIMD>;
    using vec3f64 = vec3<double>;

    using vec3i8 = vec3<int8_t>;
    using vec3i16 = vec3<int16_t>;
    using vec3i32 = vec3<int32_t>;
    using vec3i64 = vec3<int64_t>;

    using vec3u8 = vec3<uint8_t>;
    using vec3u16 = vec3<uint16_t>;
    using vec3u32 = vec3<uint32_t>;
    using vec3u64 = vec3<uint64_t>;

    //
    // alias
    //
    using vec3f = vec3f32;
    using vec3d = vec3f64;

    using vec3i = vec3i32;
    using vec3u = vec3u32;

    //
    // Operator Overload Template
    //
    INLINE_STATIC_ARITHMETIC_OPERATION_IMPLEMENTATION(vec3)

    template <typename _type, typename _simd>
    struct CONSTANT<vec3<_type, _simd>>
    {
        using type = vec3<_type, _simd>;

        // // C++17
        // static constexpr type Zero = type((_type)0, (_type)0, (_type)0);
        // static constexpr type One = type((_type)1, (_type)1, (_type)1);

        // C++11
        static constexpr __forceinline type Zero() noexcept { return type(); }
        static constexpr __forceinline type One() noexcept { return type((_type)1); }
    };



}
