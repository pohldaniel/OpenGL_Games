#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'operator_overload_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "math_common.h"

#include "int.h"
#include "float.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#include "quat.h"

#include "mat2.h"
#include "mat3.h"
#include "mat4.h"

#include "impl/simd_common.h"

namespace MathCore
{

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec4<_type, _simd> operator*(const mat4<_type, _simd> &mat, const vec4<_type, _simd> &vec) noexcept
    {
#if defined(ITK_SSE2)
        __m128 v0 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 v1 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 v2 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 v3 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 m0 = _mm_mul_ps(mat.array_sse[0], v0);
        __m128 m1 = _mm_mul_ps(mat.array_sse[1], v1);
        __m128 m2 = _mm_mul_ps(mat.array_sse[2], v2);
        __m128 m3 = _mm_mul_ps(mat.array_sse[3], v3);

        __m128 a0 = _mm_add_ps(m0, m1);
        __m128 a1 = _mm_add_ps(m2, m3);
        __m128 a2 = _mm_add_ps(a0, a1);

        return a2;
#elif defined(ITK_NEON)
        float32x4_t v0 = vshuffle_0000(vec.array_neon);
        float32x4_t v1 = vshuffle_1111(vec.array_neon);
        float32x4_t v2 = vshuffle_2222(vec.array_neon);
        float32x4_t v3 = vshuffle_3333(vec.array_neon);

        float32x4_t m0 = vmulq_f32(mat.array_neon[0], v0);
        float32x4_t m1 = vmulq_f32(mat.array_neon[1], v1);
        float32x4_t m2 = vmulq_f32(mat.array_neon[2], v2);
        float32x4_t m3 = vmulq_f32(mat.array_neon[3], v3);

        float32x4_t a0 = vaddq_f32(m0, m1);
        float32x4_t a1 = vaddq_f32(m2, m3);
        float32x4_t a2 = vaddq_f32(a0, a1);

        return a2;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec4<_type, _simd> operator*(const vec4<_type, _simd> &vec, const mat4<_type, _simd> &mat) noexcept
    {
#if defined(ITK_SSE2)
        __m128 m0 = _mm_mul_ps(vec.array_sse, mat.array_sse[0]);
        __m128 m1 = _mm_mul_ps(vec.array_sse, mat.array_sse[1]);
        __m128 m2 = _mm_mul_ps(vec.array_sse, mat.array_sse[2]);
        __m128 m3 = _mm_mul_ps(vec.array_sse, mat.array_sse[3]);

        __m128 u0 = _mm_unpacklo_ps(m0, m1);
        __m128 u1 = _mm_unpackhi_ps(m0, m1);
        __m128 a0 = _mm_add_ps(u0, u1);

        __m128 u2 = _mm_unpacklo_ps(m2, m3);
        __m128 u3 = _mm_unpackhi_ps(m2, m3);
        __m128 a1 = _mm_add_ps(u2, u3);

        __m128 f0 = _mm_movelh_ps(a0, a1);
        __m128 f1 = _mm_movehl_ps(a1, a0);
        __m128 f2 = _mm_add_ps(f0, f1);

        return f2;
#elif defined(ITK_NEON)
        float32x4_t a = vmulq_f32(vec.array_neon, mat.array_neon[0]);
        float32x4_t b = vmulq_f32(vec.array_neon, mat.array_neon[1]);
        float32x4_t c = vmulq_f32(vec.array_neon, mat.array_neon[2]);
        float32x4_t d = vmulq_f32(vec.array_neon, mat.array_neon[3]);

        //
        // transpose ops
        //
        float32x4x2_t ab = vtrnq_f32(a, b);
        float32x4x2_t cd = vtrnq_f32(c, d);
        float32x4_t a_ = vcombine_f32(vget_low_f32(ab.val[0]), vget_low_f32(cd.val[0]));
        float32x4_t b_ = vcombine_f32(vget_low_f32(ab.val[1]), vget_low_f32(cd.val[1]));
        float32x4_t c_ = vcombine_f32(vget_high_f32(ab.val[0]), vget_high_f32(cd.val[0]));
        float32x4_t d_ = vcombine_f32(vget_high_f32(ab.val[1]), vget_high_f32(cd.val[1]));

        float32x4_t add0 = vaddq_f32(a_, b_);
        float32x4_t add1 = vaddq_f32(c_, d_);

        float32x4_t add2 = vaddq_f32(add0, add1);

        return add2;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec3<_type, _simd> operator*(const mat3<_type, _simd> &mat, const vec3<_type, _simd> &vec) noexcept
    {
#if defined(ITK_SSE2)
        __m128 v0 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 v1 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 v2 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(2, 2, 2, 2));
        //__m128 v3 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 m0 = _mm_mul_ps(mat.array_sse[0], v0);
        __m128 m1 = _mm_mul_ps(mat.array_sse[1], v1);
        __m128 m2 = _mm_mul_ps(mat.array_sse[2], v2);
        //__m128 m3 = _mm_mul_ps(mat.array_sse[3], v3);

        __m128 a0 = _mm_add_ps(m0, m1);
        //__m128 a1 = _mm_add_ps(m2, m3);
        __m128 a2 = _mm_add_ps(a0, m2);

        return a2;
#elif defined(ITK_NEON)
        float32x4_t v0 = vshuffle_0000(vec.array_neon);
        float32x4_t v1 = vshuffle_1111(vec.array_neon);
        float32x4_t v2 = vshuffle_2222(vec.array_neon);
        // float32x4_t v3 = vshuffle_3333(vec.array_neon);

        float32x4_t m0 = vmulq_f32(mat.array_neon[0], v0);
        float32x4_t m1 = vmulq_f32(mat.array_neon[1], v1);
        float32x4_t m2 = vmulq_f32(mat.array_neon[2], v2);
        // float32x4_t m3 = vmulq_f32(mat.array_neon[3], v3);

        float32x4_t a0 = vaddq_f32(m0, m1);
        // float32x4_t a1 = vaddq_f32(m2, m3);
        float32x4_t a2 = vaddq_f32(a0, m2);

        return a2;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec3<_type, _simd> operator*(const vec3<_type, _simd> &vec, const mat3<_type, _simd> &mat) noexcept
    {
#if defined(ITK_SSE2)
        __m128 m0 = _mm_mul_ps(vec.array_sse, mat.array_sse[0]);
        __m128 m1 = _mm_mul_ps(vec.array_sse, mat.array_sse[1]);
        __m128 m2 = _mm_mul_ps(vec.array_sse, mat.array_sse[2]);
        //__m128 m3 = _mm_mul_ps(vec.array_sse, mat.array_sse[3]);
        __m128 m3 = _vec4_zero_sse;

        __m128 u0 = _mm_unpacklo_ps(m0, m1);
        __m128 u1 = _mm_unpackhi_ps(m0, m1);
        __m128 a0 = _mm_add_ps(u0, u1);

        __m128 u2 = _mm_unpacklo_ps(m2, m3);
        __m128 u3 = _mm_unpackhi_ps(m2, m3);
        __m128 a1 = _mm_add_ps(u2, u3);

        __m128 f0 = _mm_movelh_ps(a0, a1);
        __m128 f1 = _mm_movehl_ps(a1, a0);
        __m128 f2 = _mm_add_ps(f0, f1);

        return f2;
#elif defined(ITK_NEON)
        float32x4_t a = vmulq_f32(vec.array_neon, mat.array_neon[0]);
        float32x4_t b = vmulq_f32(vec.array_neon, mat.array_neon[1]);
        float32x4_t c = vmulq_f32(vec.array_neon, mat.array_neon[2]);
        // float32x4_t d = vmulq_f32(vec.array_neon, mat.array_neon[3]);
        float32x4_t d = _vec4_zero;

        //
        // transpose ops
        //
        float32x4x2_t ab = vtrnq_f32(a, b);
        float32x4x2_t cd = vtrnq_f32(c, d);
        float32x4_t a_ = vcombine_f32(vget_low_f32(ab.val[0]), vget_low_f32(cd.val[0]));
        float32x4_t b_ = vcombine_f32(vget_low_f32(ab.val[1]), vget_low_f32(cd.val[1]));
        float32x4_t c_ = vcombine_f32(vget_high_f32(ab.val[0]), vget_high_f32(cd.val[0]));
        float32x4_t d_ = vcombine_f32(vget_high_f32(ab.val[1]), vget_high_f32(cd.val[1]));

        float32x4_t add0 = vaddq_f32(a_, b_);
        float32x4_t add1 = vaddq_f32(c_, d_);

        float32x4_t add2 = vaddq_f32(add0, add1);

        return add2;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec2<_type, _simd> operator*(const mat2<_type, _simd> &mat, const vec2<_type, _simd> &vec) noexcept
    {
#if defined(ITK_SSE2)
        __m128 transp = _mm_shuffle_ps(mat.array_sse, mat.array_sse, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 a0 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 a_mul = _mm_mul_ps(a0, transp);

        // __m128 b0 = _mm_shuffle_ps(a_mul, a_mul, _MM_SHUFFLE(3, 1, 2, 0));
        // __m128 b1 = _mm_shuffle_ps(a_mul, a_mul, _MM_SHUFFLE(2, 0, 3, 1));
        // __m128 a_add = _mm_add_ps(b0, b1);

        __m128 a_add = _mm_hadd_ps(a_mul, a_mul);

        return _mm_and_ps(a_add, _vec2_valid_bits_sse);
#elif defined(ITK_NEON)
        vec2<_type, _simd> result;
        result.x = mat.a1 * vec.x + mat.b1 * vec.y;
        result.y = mat.a2 * vec.x + mat.b2 * vec.y;
        return result;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec2<_type, _simd> operator*(const vec2<_type, _simd> &vec, const mat2<_type, _simd> &mat) noexcept
    {
#if defined(ITK_SSE2)
        //__m128 transp = _mm_shuffle_ps(mat.array_sse, mat.array_sse, _MM_SHUFFLE(3, 1, 2, 0));
        __m128 a0 = _mm_shuffle_ps(vec.array_sse, vec.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 a_mul = _mm_mul_ps(a0, mat.array_sse);

        // __m128 b0 = _mm_shuffle_ps(a_mul, a_mul, _MM_SHUFFLE(3, 1, 2, 0));
        // __m128 b1 = _mm_shuffle_ps(a_mul, a_mul, _MM_SHUFFLE(2, 0, 3, 1));
        // __m128 a_add = _mm_add_ps(b0, b1);

        __m128 a_add = _mm_hadd_ps(a_mul, a_mul);

        return _mm_and_ps(a_add, _vec2_valid_bits_sse);
#elif defined(ITK_NEON)
        vec2<_type, _simd> result;
        result.x = mat.a1 * vec.x + mat.a2 * vec.y;
        result.y = mat.b1 * vec.x + mat.b2 * vec.y;
        return result;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline quat<_type, _simd> operator^(const quat<_type, _simd> &a, const quat<_type, _simd> &b) noexcept
    {
#if defined(ITK_SSE2)
        __m128 a0 = _mm_shuffle_ps(a.array_sse, a.array_sse, _MM_SHUFFLE(3, 3, 3, 3));
        //__m128 b0 = _mm_shuffle_ps(b.array_sse, b.array_sse, _MM_SHUFFLE(3,2,1,0));

        __m128 a1 = _mm_shuffle_ps(a.array_sse, a.array_sse, _MM_SHUFFLE(0, 2, 1, 0));
        __m128 b1 = _mm_shuffle_ps(b.array_sse, b.array_sse, _MM_SHUFFLE(0, 3, 3, 3));

        __m128 a2 = _mm_shuffle_ps(a.array_sse, a.array_sse, _MM_SHUFFLE(1, 0, 2, 1));
        __m128 b2 = _mm_shuffle_ps(b.array_sse, b.array_sse, _MM_SHUFFLE(1, 1, 0, 2));

        __m128 a3 = _mm_shuffle_ps(a.array_sse, a.array_sse, _MM_SHUFFLE(2, 1, 0, 2));
        __m128 b3 = _mm_shuffle_ps(b.array_sse, b.array_sse, _MM_SHUFFLE(2, 0, 2, 1));

        const __m128 signMask0 = _mm_setr_ps(0.0f, 0.0f, 0.0f, -0.0f);
        // const __m128 signMask0 = _mm_setr_ps(1.0f,1.0f,1.0f,-1.0f);

        //__m128 mul0 = _mm_mul_ps(a0, b0);
        __m128 mul0 = _mm_mul_ps(a0, b.array_sse);

        __m128 mul1 = _mm_mul_ps(a1, b1);
        mul1 = _mm_xor_ps(mul1, signMask0); // much faster
        // mul1 = _mm_mul_ps(mul1, signMask0);

        __m128 mul2 = _mm_mul_ps(a2, b2);
        mul2 = _mm_xor_ps(mul2, signMask0); // much faster
        // mul2 = _mm_mul_ps(mul2, signMask0);

        __m128 mul3 = _mm_mul_ps(a3, b3);

        __m128 add0 = _mm_add_ps(mul0, mul1);
        add0 = _mm_add_ps(add0, mul2);
        add0 = _mm_sub_ps(add0, mul3);

        return add0;
#elif defined(ITK_NEON)
        float32x4_t a0 = vshuffle_3333(a.array_neon);
        //__m128 b0 = _mm_shuffle_ps(b.array_sse, b.array_sse, _MM_SHUFFLE(3,2,1,0));

        float32x4_t a1 = vshuffle_0210(a.array_neon);
        float32x4_t b1 = vshuffle_0333(b.array_neon);

        float32x4_t a2 = vshuffle_1021(a.array_neon);
        float32x4_t b2 = vshuffle_1102(b.array_neon);

        float32x4_t a3 = vshuffle_2102(a.array_neon);
        float32x4_t b3 = vshuffle_2021(b.array_neon);

        const float32x4_t signMask0 = (float32x4_t){1.0f, 1.0f, 1.0f, -1.0f};

        //__m128 mul0 = _mm_mul_ps(a0, b0);
        float32x4_t mul0 = vmulq_f32(a0, b.array_neon);

        float32x4_t mul1 = vmulq_f32(a1, b1);
        mul1 = vmulq_f32(mul1, signMask0);

        float32x4_t mul2 = vmulq_f32(a2, b2);
        mul2 = vmulq_f32(mul2, signMask0);

        float32x4_t mul3 = vmulq_f32(a3, b3);

        float32x4_t add0 = vaddq_f32(mul0, mul1);
        add0 = vaddq_f32(add0, mul2);
        add0 = vsubq_f32(add0, mul3);

        return add0;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline quat<_type, _simd> operator*(const quat<_type, _simd> &a, const quat<_type, _simd> &b) noexcept
    {
        return OP<quat<_type, _simd>>::normalize(a ^ b);
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec3<_type, _simd> operator*(const quat<_type, _simd> &a, const vec3<_type, _simd> &v) noexcept
    {
        //
        // non normalized multiplication of the quaternion
        //
#if defined(ITK_SSE2)
        quat<_type, _simd> result;
        result.array_sse = _mm_and_ps(v.array_sse, _vec3_valid_bits_sse);
        result = a ^ result ^ OP<quat<_type, _simd>>::conjugate(a);
        result.array_sse = _mm_and_ps(result.array_sse, _vec3_valid_bits_sse);
        return result.array_sse;
#elif defined(ITK_NEON)
        quat<_type, _simd> result(v.array_neon);
        result.w = (_type)0;
        result = a ^ result ^ OP<quat<_type, _simd>>::conjugate(a);
        result.w = (_type)0;
        return result.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec4<_type, _simd> operator*(const quat<_type, _simd> &a, const vec4<_type, _simd> &v) noexcept
    {
        //
        // non normalized multiplication of the quaternion
        //
#if defined(ITK_SSE2)
        quat<_type, _simd> result;
        result.array_sse = _mm_and_ps(v.array_sse, _vec3_valid_bits_sse);
        result = a ^ result ^ OP<quat<_type, _simd>>::conjugate(a);
#if defined(ITK_SSE_SKIP_SSE41)
        _mm_f32_(result.array_sse, 3) = _mm_f32_(v.array_sse, 3);
#else
        result.array_sse = _mm_blend_ps(result.array_sse, v.array_sse, 0x8);
#endif
        return result.array_sse;
#elif defined(ITK_NEON)
        quat<_type, _simd> result(v.array_neon);
        result.w = (_type)0;
        result = a ^ result ^ OP<quat<_type, _simd>>::conjugate(a);
        result.w = v.w;
        return result.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }



    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec3<_type, _simd> operator*(const vec3<_type, _simd> &v, const quat<_type, _simd> &a) noexcept
    {
        //
        // non normalized multiplication of the quaternion
        //
#if defined(ITK_SSE2)
        quat<_type, _simd> result;
        result.array_sse = _mm_and_ps(v.array_sse, _vec3_valid_bits_sse);
        result = OP<quat<_type, _simd>>::conjugate(a) ^ result ^ a;
        result.array_sse = _mm_and_ps(result.array_sse, _vec3_valid_bits_sse);
        return result.array_sse;
#elif defined(ITK_NEON)
        quat<_type, _simd> result(v.array_neon);
        result.w = (_type)0;
        result = a ^ result ^ OP<quat<_type, _simd>>::conjugate(a);
        result.w = (_type)0;
        return result.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                      (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                       std::is_same<_simd, SIMD_TYPE::NEON>::value),
                  bool>::type = true>
    static __forceinline vec4<_type, _simd> operator*(const vec4<_type, _simd> &v, const quat<_type, _simd> &a) noexcept
    {
        //
        // non normalized multiplication of the quaternion
        //
#if defined(ITK_SSE2)
        quat<_type, _simd> result;
        result.array_sse = _mm_and_ps(v.array_sse, _vec3_valid_bits_sse);
        result = OP<quat<_type, _simd>>::conjugate(a) ^ result ^ a;
#if defined(ITK_SSE_SKIP_SSE41)
        _mm_f32_(result.array_sse, 3) = _mm_f32_(v.array_sse, 3);
#else
        result.array_sse = _mm_blend_ps(result.array_sse, v.array_sse, 0x8);
#endif
        return result.array_sse;
#elif defined(ITK_NEON)
        quat<_type, _simd> result(v.array_neon);
        result.w = (_type)0;
        result = a ^ result ^ OP<quat<_type, _simd>>::conjugate(a);
        result.w = v.w;
        return result.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
    }


}