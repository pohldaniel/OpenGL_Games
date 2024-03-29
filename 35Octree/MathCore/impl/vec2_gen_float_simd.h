#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'vec2_gen_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "vec2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd>
    struct GEN<vec2<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_type, float>::value &&
                   (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                    std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type>
    {
    private:
        using typeVec2 = vec2<_type, _simd>;
        using self_type = GEN<typeVec2>;

    public:
        static __forceinline typeVec2 fromPolar(const _type &pAngle, const _type &pRadius) noexcept
        {
            _type angleRad = OP<_type>::deg_2_rad(pAngle);
            return typeVec2(OP<_type>::cos(angleRad),
                            OP<_type>::sin(angleRad)) *
                   pRadius;
        }
    };

}