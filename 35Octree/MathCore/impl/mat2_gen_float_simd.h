#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat2_gen_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "mat2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd>
    struct GEN<mat2<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_type, float>::value &&
                   (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                    std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type>
    {
    private:
        using typeMat2 = mat2<_type, _simd>;
        using typeMat3 = mat3<_type, _simd>;
        using typeMat4 = mat4<_type, _simd>;
        using typeVec4 = vec4<_type, _simd>;
        using typeVec3 = vec3<_type, _simd>;
        using typeVec2 = vec2<_type, _simd>;
        using self_type = GEN<typeMat2>;

    public:
        static __forceinline typeMat2 scale(const _type &_x_, const _type &_y_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat2(_mm_setr_ps(_x_, 0,
                                        0, _y_));
#elif defined(ITK_NEON)
            return typeMat2((float32x4_t){_x_, 0,
                                          0, _y_});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat2 scale(const typeVec2 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
                return _mm_setr_ps( 
                        _v_.x, 0, 
                        0, _v_.y);
#else
            __m128 _tmp0 = _mm_shuffle_ps(_v_.array_sse, _v_.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
            _tmp0 = _mm_blend_ps(_tmp0, _vec4_zero_sse, 0x6);

            return _tmp0;
#endif

            // return typeMat2(_mm_setr_ps(_v_.x, 0,
            //                             0, _v_.y));
#elif defined(ITK_NEON)
            return (float32x4_t){_v_.x, 0,
                                          0, _v_.y};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat2 scale(const typeVec3 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
                return _mm_setr_ps( 
                        _v_.x, 0, 
                        0, _v_.y);
#else
            __m128 _tmp0 = _mm_shuffle_ps(_v_.array_sse, _v_.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
            _tmp0 = _mm_blend_ps(_tmp0, _vec4_zero_sse, 0x6);

            return _tmp0;
#endif

            // return typeMat2(_mm_setr_ps(_v_.x, 0,
            //                             0, _v_.y));
#elif defined(ITK_NEON)
            return (float32x4_t){_v_.x, 0,
                                          0, _v_.y};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat2 scale(const typeVec4 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
                return _mm_setr_ps( 
                        _v_.x, 0, 
                        0, _v_.y);
#else
            __m128 _tmp0 = _mm_shuffle_ps(_v_.array_sse, _v_.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
            _tmp0 = _mm_blend_ps(_tmp0, _vec4_zero_sse, 0x6);

            return _tmp0;
#endif

            // return typeMat2(_mm_setr_ps(_v_.x, 0,
            //                             0, _v_.y));
#elif defined(ITK_NEON)
            return (float32x4_t){_v_.x, 0,
                                          0, _v_.y};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat2 rotate(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
#if defined(ITK_SSE2)
            return _mm_setr_ps(c, s,
                                        -s, c);
#elif defined(ITK_NEON)
            return (float32x4_t){c, s,
                                          -s, c};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat2 fromMat3(const typeMat3 &v) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat2(v.array_sse[0],
                            v.array_sse[1]);
#elif defined(ITK_NEON)
            return typeMat2(v.array_neon[0],
                            v.array_neon[1]);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat2 fromMat4(const typeMat4 &v) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat2(v.array_sse[0],
                            v.array_sse[1]);
#elif defined(ITK_NEON)
            return typeMat2(v.array_neon[0],
                            v.array_neon[1]);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
    };

}