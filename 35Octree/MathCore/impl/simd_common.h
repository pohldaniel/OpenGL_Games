#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'simd_common.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "../math_common.h"

namespace MathCore
{

#if defined(ITK_SSE2)

    const __m128 _quat_conjugate_mask_sse = _mm_setr_ps(-0.f, -0.f, -0.f, 0.f);

    const __m128 _vec4_zero_sse = _mm_set1_ps(0.0f);
    const __m128 _vec4_sign_mask_sse = _mm_set1_ps(-0.f);
    const __m128 _vec4_one_sse = _mm_set1_ps(1.0f);
    const __m128 _vec4_two_sse = _mm_set1_ps(2.0f);
    const __m128 _vec4_minus_one_sse = _mm_set1_ps(-1.0f);
    //const __m128 _vec4_valid_bits_sse = _mm_castsi128_ps(_mm_set1_epi32((int)0xffffffff));

    const __m128 _vec4_1000_sse = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
    const __m128 _vec4_0100_sse = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
    const __m128 _vec4_0010_sse = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    const __m128 _vec4_0001_sse = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    //const __m128 _vec3_zero_sse = _mm_set1_ps(0.0f);
    const __m128 _vec3_sign_mask_sse = _mm_setr_ps(-0.f, -0.f, -0.f, 0.0f);
    const __m128 _vec3_one_sse = _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f);
    const __m128 _vec3_two_sse = _mm_setr_ps(2.0f, 2.0f, 2.0f, 0.0f);
    const __m128 _vec3_minus_one_sse = _mm_setr_ps(-1.0f, -1.0f, -1.0f, 0.0f);
    const __m128 _vec3_valid_bits_sse = _mm_castsi128_ps(_mm_set_epi32(0, (int)0xffffffff, (int)0xffffffff, (int)0xffffffff));

    //const __m128 _vec2_sign_mask = _mm_setr_ps(-0.f, -0.f, 0.f, 0.0f);
    //const __m128 _vec2_valid_bits = _mm_castsi128_ps(_mm_set_epi32(0, 0, (int)0xffffffff, (int)0xffffffff));

    const __m128 _vec2_zero_sse = _mm_set1_ps(0.0f);
    const __m128 _vec2_sign_mask_sse = _mm_setr_ps(-0.f, -0.f, 0.f, 0.0f);
    const __m128 _vec2_one_sse = _mm_setr_ps(1.0f, 1.0f, 0.0f, 0.0f);
    const __m128 _vec2_minus_one_sse = _mm_setr_ps(-1.0f, -1.0f, 0.0f, 0.0f);
    const __m128 _vec2_valid_bits_sse = _mm_castsi128_ps(_mm_set_epi32(0, 0, (int)0xffffffff, (int)0xffffffff));


#elif defined(ITK_NEON)

    __forceinline float32x4_t vset1(const float32_t &a)
    {
        return vdupq_n_f32(a);
    }

    const float32x4_t _vec4_minus_one = vset1(-1.0f);
    const float32x4_t _vec4_one = vset1(1.0f);
    const float32x4_t _vec4_two = vset1(2.0f);

    const float32x4_t _vec4_zero = vset1(0.0f);

    const float32x4_t _neon_1000 = (float32x4_t){ 1,0,0,0 };
    const float32x4_t _neon_0100 = (float32x4_t){ 0,1,0,0 };
    const float32x4_t _neon_0010 = (float32x4_t){ 0,0,1,0 };
    const float32x4_t _neon_0001 = (float32x4_t){ 0,0,0,1 };

    const float32x4_t _vec3_two = (float32x4_t) { 2.0f, 2.0f, 2.0f, 0 };

    const float32x4_t _quat_conjugate_sign = (float32x4_t) { -1, -1, -1, 1 };

#endif

#if defined(ITK_SSE2)

    __forceinline __m128 dot_sse_3(const __m128 &a, const __m128 &b)
    {
#if true
#if defined(ITK_SSE_SKIP_SSE41)
        __m128 mul0 = _mm_mul_ps(a, b);

        _mm_f32_(mul0, 3) = 0;
        mul0 = _mm_hadd_ps(mul0, mul0);
        mul0 = _mm_hadd_ps(mul0, mul0);

        return mul0;
#else
        return _mm_dp_ps(a, b, 0x77);
#endif

#elif defined(_MSC_VER) || true

        __m128 mul0 = _mm_mul_ps(a, b);

        _mm_f32_(mul0, 3) = 0;
        mul0 = _mm_hadd_ps(mul0, mul0);
        mul0 = _mm_hadd_ps(mul0, mul0);

        return mul0;
#else

        __m128 mul0 = _mm_mul_ps(a, b);

        // swp0 = [1,0,0,3]
        __m128 swp0 = _mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(3, 0, 0, 1));
        // add0 = [0+1,1+0,2+0,3+3]
        __m128 add0 = _mm_add_ps(mul0, swp0);
        // swp1 = [2,2,1,3]
        __m128 swp1 = _mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(3, 1, 2, 2));
        // add1 = [0+1+2,1+0+2,2+0+1,3+3+3]
        __m128 add1 = _mm_add_ps(add0, swp1);
        return add1;
#endif
    }

    __forceinline __m128 dot_sse_4(const __m128 &a, const __m128 &b)
    {
#if true
#if defined(ITK_SSE_SKIP_SSE41)
        __m128 mul0 = _mm_mul_ps(a, b);

        mul0 = _mm_hadd_ps(mul0, mul0);
        mul0 = _mm_hadd_ps(mul0, mul0);

        return mul0;
#else
        return _mm_dp_ps(a, b, 0xff);
#endif

#elif defined(_MSC_VER) || true

        __m128 mul0 = _mm_mul_ps(a, b);

        mul0 = _mm_hadd_ps(mul0, mul0);
        mul0 = _mm_hadd_ps(mul0, mul0);

        return mul0;
#else

        __m128 mul0 = _mm_mul_ps(a, b);

        // swp0 = [1,0,3,2]
        __m128 swp0 = _mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));
        // add0 = [0+1,1+0,2+3,3+2]
        __m128 add0 = _mm_add_ps(mul0, swp0);
        // swp1 = [3+2,2+3,1+0,0+1]
        __m128 swp1 = _mm_shuffle_ps(add0, add0, _MM_SHUFFLE(0, 1, 2, 3));
        // add1 = [0+1+3+2,1+0+2+3,2+3+1+0,3+2+0+1]
        __m128 add1 = _mm_add_ps(add0, swp1);
        return add1;
#endif
    }

    __forceinline __m128 max_sse_4(const __m128 &a)
    {
        __m128 swp1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 3, 2));
        __m128 max1 = _mm_max_ps(a, swp1);
        __m128 swp2 = _mm_shuffle_ps(max1, max1, _MM_SHUFFLE(0, 1, 0, 1));
        __m128 max2 = _mm_max_ps(max1, swp2);
        return max2;
    }

    __forceinline __m128 max_sse_3(const __m128 &_a)
    {
        __m128 aux = _mm_shuffle_ps(_a, _a, _MM_SHUFFLE(0, 2, 1, 0));
        return max_sse_4(aux);
    }

    __forceinline __m128 max_sse_2(const __m128 &a)
    {
        __m128 swp1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 swp2 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 1, 0, 1));
        __m128 max = _mm_max_ps(swp1, swp2);
        return max;
    }

    __forceinline __m128 min_sse_4(const __m128 &a)
    {
        __m128 swp1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 3, 2));
        __m128 min1 = _mm_min_ps(a, swp1);
        __m128 swp2 = _mm_shuffle_ps(min1, min1, _MM_SHUFFLE(0, 1, 0, 1));
        __m128 min2 = _mm_min_ps(min1, swp2);
        return min2;
    }

    __forceinline __m128 min_sse_3(const __m128 &_a)
    {
        __m128 aux = _mm_shuffle_ps(_a, _a, _MM_SHUFFLE(0, 2, 1, 0));
        return min_sse_4(aux);
    }

    __forceinline __m128 min_sse_2(const __m128 &a)
    {
        __m128 swp1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 swp2 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 1, 0, 1));
        __m128 min = _mm_min_ps(swp1, swp2);
        return min;
    }

    __forceinline __m128 clamp_sse_4(const __m128 &value, const __m128 &min, const __m128 &max)
    {
        __m128 maxStep = _mm_max_ps(value, min);
        __m128 minStep = _mm_min_ps(maxStep, max);
        return minStep;
    }

#elif defined(ITK_NEON)

    __forceinline float32x4_t vdivq_f32(const float32x4_t &a, const float32x4_t &b)
    {
        float32x4_t recip0 = vrecpeq_f32(b);
        float32x4_t recip1 = vmulq_f32(recip0, vrecpsq_f32(recip0, b));
        return vmulq_f32(a, recip1);
    }

    __forceinline float32x4_t vshuffle_2301(const float32x4_t &a)
    {
        return vrev64q_f32(a);
    }

    __forceinline float32x4_t vshuffle_1032(const float32x4_t &a)
    {
        return vcombine_f32(vget_high_f32(a), vget_low_f32(a));
    }

    __forceinline float32x4_t vshuffle_0123(const float32x4_t &a)
    {
        return vshuffle_2301(vshuffle_1032(a));
    }

    __forceinline float32x4_t vshuffle_0000(const float32x4_t &a)
    {
        return vdupq_lane_f32(vget_low_f32(a), 0); // a[0]
    }

    __forceinline float32x4_t vshuffle_1111(const float32x4_t &a)
    {
        return vdupq_lane_f32(vget_low_f32(a), 1); // a[1]
    }

    __forceinline float32x4_t vshuffle_2222(const float32x4_t &a)
    {
        return vdupq_lane_f32(vget_high_f32(a), 0); // a[2]
    }

    __forceinline float32x4_t vshuffle_3333(const float32x4_t &a)
    {
        return vdupq_lane_f32(vget_high_f32(a), 1); // a[3]
    }

    __forceinline float32x4_t vshuffle_0210(const float32x4_t &a)
    {
        float32x2_t l0 = vget_low_f32(a);
        float32x2x2_t r = vtrn_f32(vget_high_f32(a), l0);
        return vcombine_f32(l0, r.val[0]);
    }

    __forceinline float32x4_t vshuffle_0333(const float32x4_t &a)
    {
        float32x4_t r = vshuffle_3333(a);
        // const float32x4_t _zero = vset1(0);
        float32x4_t _zero = vshuffle_0000(a);
        return vextq_f32(r, _zero, 1);
    }

    __forceinline float32x4_t vshuffle_1021(const float32x4_t &a)
    {
        return vcombine_f32(vget_low_f32(vextq_f32(a, a, 1)), vget_low_f32(a));
    }

    __forceinline float32x4_t vshuffle_1102(const float32x4_t &a)
    {
        float32x2_t l0 = vget_low_f32(a);
        float32x2x2_t r = vtrn_f32(vget_high_f32(a), l0);
        return vcombine_f32(r.val[0], vdup_lane_f32(l0, 1));
    }

    __forceinline float32x4_t vshuffle_2102(const float32x4_t &a)
    {
        float32x4_t r = vshuffle_2222(a);
        return vextq_f32(r, a, 3);
    }

    __forceinline float32x4_t vshuffle_2021(const float32x4_t &a)
    {
        float32x4_t _21 = vextq_f32(a, a, 1);
        float32x2x2_t _trn = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        return vcombine_f32(vget_low_f32(_21), _trn.val[0]);
    }

    __forceinline float32x4_t vshuffle_3021(const float32x4_t &a)
    {
        float32x2x2_t r = vtrn_f32(vrev64_f32(vget_low_f32(a)), vget_high_f32(a));
        return vcombine_f32(r.val[0], r.val[1]);
    }

    // TODO: need test this shuffle...
    __forceinline float32x4_t vshuffle_3120(const float32x4_t &a)
    {
        float32x2x2_t r = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        return vcombine_f32(r.val[0], r.val[1]);
    }

    __forceinline float32x4_t vshuffle_3102(const float32x4_t &a)
    {
        float32x2x2_t r = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        return vcombine_f32(vrev64_f32(r.val[0]), r.val[1]);
    }

    __forceinline float32x4_t vshuffle_2000(const float32x4_t &a)
    {
        float32x4_t _zero = vdupq_lane_f32(vget_low_f32(a), 0);
        float32x4_t _two = vdupq_lane_f32(vget_high_f32(a), 0);
        return vextq_f32(_zero, _two, 1);
    }

    __forceinline float32x4_t vshuffle_2220(const float32x4_t &a)
    {
        float32x4_t _zero = vdupq_lane_f32(vget_low_f32(a), 0);
        float32x4_t _two = vdupq_lane_f32(vget_high_f32(a), 0);
        return vextq_f32(_zero, _two, 3);
    }

    __forceinline float32x4_t vshuffle_1110_test(const float32x4_t &a)
    {
        float32x4_t _a = vdupq_lane_f32(vget_low_f32(a), 0);
        float32x4_t _b = vdupq_lane_f32(vget_high_f32(a), 1);
        return vextq_f32(_a, _b, 3);
    }

    __forceinline float32x4_t vshuffle_1110(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 1);
        return vcombine_f32(_a_low, _b);
    }

    __forceinline float32x4_t vshuffle_0001(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 0);
        return vcombine_f32(vrev64_f32(_a_low), _b);
    }

    __forceinline float32x4_t vshuffle_1101(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 1);
        return vcombine_f32(vrev64_f32(_a_low), _b);
    }

    __forceinline float32x4_t vshuffle_0010(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 0);
        return vcombine_f32(_a_low, _b);
    }

    __forceinline float32x4_t vshuffle_1011(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 1);
        return vcombine_f32(_b, _a_low);
    }

    __forceinline float32x4_t vshuffle_0100(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 0);
        return vcombine_f32(_b, vrev64_f32(_a_low));
    }

    __forceinline float32x4_t vshuffle_0221(const float32x4_t &a)
    {
        float32x2_t l0 = vget_low_f32(a);
        float32x2x2_t r = vtrn_f32(vget_high_f32(a), l0);
        return vcombine_f32(vget_low_f32(vextq_f32(a, a, 1)), r.val[0]);
    }

    __forceinline float32x4_t vshuffle_0000(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t a_ = vdup_lane_f32(vget_low_f32(a), 0);
        float32x2_t b_ = vdup_lane_f32(vget_low_f32(b), 0);
        return vcombine_f32(a_, b_);
    }

    __forceinline float32x4_t vshuffle_1111(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t a_ = vdup_lane_f32(vget_low_f32(a), 1);
        float32x2_t b_ = vdup_lane_f32(vget_low_f32(b), 1);
        return vcombine_f32(a_, b_);
    }

    __forceinline float32x4_t vshuffle_2222(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t a_ = vdup_lane_f32(vget_high_f32(a), 0);
        float32x2_t b_ = vdup_lane_f32(vget_high_f32(b), 0);
        return vcombine_f32(a_, b_);
    }

    __forceinline float32x4_t vshuffle_3333(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t a_ = vdup_lane_f32(vget_high_f32(a), 1);
        float32x2_t b_ = vdup_lane_f32(vget_high_f32(b), 1);
        return vcombine_f32(a_, b_);
    }

    __forceinline float32x4_t vshuffle_2020(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2x2_t a_ = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        float32x2x2_t b_ = vtrn_f32(vget_low_f32(b), vget_high_f32(b));
        return vcombine_f32(a_.val[0], b_.val[0]);
    }

    /*
    __m128 a = _mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 b = _mm_set_ps(4.0f, 5.0f, 6.0f, 7.0f);
    __m128 result = _mm_movehl_ps(a, b);
    printf("%f %f %f %f\n", _mm_f32_(result,0), _mm_f32_(result, 1), _mm_f32_(result, 2), _mm_f32_(result, 3));
    //5 4 2 1

    */
    __forceinline float32x4_t vmovehl(const float32x4_t &__A, const float32x4_t &__B)
    {
        return vcombine_f32(vget_high_f32(__B), vget_high_f32(__A));
    }

    __forceinline float32x4_t vshuffle_0112(const float32x4_t &a)
    {
        float32x2_t _01_ = vrev64_f32(vget_low_f32(a));
        float32x2_t _12_ = vrev64_f32(vget_low_f32(vextq_f32(a, a, 1)));
        return vcombine_f32(_12_, _01_);
    }

    __forceinline float32x4_t vshuffle_3233(const float32x4_t &a)
    {
        float32x2_t _32_ = vget_high_f32(a);
        float32x2_t _33_ = vdup_lane_f32(vget_high_f32(a), 1);
        return vcombine_f32(_33_, _32_);
    }

    __forceinline float32x4_t vshuffle_0012(const float32x4_t &a)
    {
        float32x2_t _12_ = vrev64_f32(vget_low_f32(vextq_f32(a, a, 1)));
        float32x2_t _00_ = vdup_lane_f32(vget_low_f32(a), 0);
        return vcombine_f32(_12_, _00_);
    }

    __forceinline float32x4_t vshuffle_1200(const float32x4_t &a)
    {
        float32x2_t _12_ = vrev64_f32(vget_low_f32(vextq_f32(a, a, 1)));
        float32x2_t _00_ = vdup_lane_f32(vget_low_f32(a), 0);
        return vcombine_f32(_00_, _12_);
    }

    __forceinline float32x4_t vshuffle_2100(const float32x4_t &a)
    {
        float32x2_t _21_ = vget_low_f32(vextq_f32(a, a, 1));
        float32x2_t _00_ = vdup_lane_f32(vget_low_f32(a), 0);
        return vcombine_f32(_00_, _21_);
    }

    __forceinline float32x4_t vshuffle_0031(const float32x4_t &a)
    {
        float32x2x2_t _31_a = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        return vcombine_f32(_31_a.val[1], vdup_lane_f32(vget_low_f32(a), 0));
    }

    __forceinline float32x4_t vshuffle_3110(const float32x4_t &a)
    {
        float32x2x2_t _31_a = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        return vcombine_f32(vget_low_f32(a), _31_a.val[1]);
    }

    __forceinline float32x4_t vshuffle_1122(const float32x4_t &a)
    {
        float32x4_t _two = vshuffle_2222(a);
        float32x4_t _one = vshuffle_1111(a);
        return vextq_f32(_two, _one, 2);
    }

    __forceinline float32x4_t vshuffle_1022(const float32x4_t &a)
    {
        float32x2_t _10_ = vget_low_f32(a);
        float32x2_t _22_ = vdup_lane_f32(vget_high_f32(a), 0);
        return vcombine_f32(_22_, _10_);
    }

    __forceinline float32x4_t vshuffle_3320(const float32x4_t &a)
    {
        float32x2x2_t _20_ = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        float32x2_t _33_ = vdup_lane_f32(vget_high_f32(a), 1);
        return vcombine_f32(_20_.val[0], _33_);
    }

    __forceinline float32x4_t vshuffle_2333(const float32x4_t &a)
    {
        float32x4_t r = vshuffle_3333(a);
        float32x4_t _two = vshuffle_2222(a);
        return vextq_f32(r, _two, 1);
    }

    __forceinline float32x4_t vshuffle_0031(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2x2_t _31_a = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        return vcombine_f32(_31_a.val[1], vdup_lane_f32(vget_low_f32(b), 0));
    }

    __forceinline float32x4_t vshuffle_1022(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t _22_ = vdup_lane_f32(vget_high_f32(a), 0);
        float32x2_t _10_ = vget_low_f32(b);
        return vcombine_f32(_22_, _10_);
    }

    __forceinline float32x4_t dot_neon_4(const float32x4_t &a, const float32x4_t &b)
    {
        float32x4_t prod = vmulq_f32(a, b);
        // sum1 = [ 0+2, 1+3, 2+0, 3+1 ]
        float32x4_t sum1 = vaddq_f32(prod, vshuffle_1032(prod));
        // sum2 = [ 0+2 3+1, 1+3  2+0, 2+0 1+3, 3+1 0+2 ]
        float32x4_t sum2 = vaddq_f32(sum1, vshuffle_0123(sum1));

        return sum2;
    }

    __forceinline float32x4_t dot_neon_3(const float32x4_t &a, const float32x4_t &b)
    {
        float32x4_t aux = a; // 2x faster
        aux[3] = 0;
        return dot_neon_4(aux, b);
        // const float32x4_t _const_n = (float32x4_t){ 1,1,1,0 };
        // return dot_neon_4(vmulq_f32(a,_const_n),b);
    }

    __forceinline float32x4_t clamp_neon_4(const float32x4_t &value, const float32x4_t &min, const float32x4_t &max)
    {
        float32x4_t maxStep = vmaxq_f32(value, min);
        float32x4_t minStep = vminq_f32(maxStep, max);
        return minStep;
    }

#endif

}
