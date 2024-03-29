#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat4_op_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "mat4_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<mat4<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                  (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                   std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type,
              _algorithm>
    {
    private:
        using typeMat4 = mat4<_type, _simd>;
        using type4 = vec4<_type, _simd>;
        using self_type = OP<typeMat4>;

    public:
        static __forceinline typeMat4 clamp(const typeMat4 &value, const typeMat4 &min, const typeMat4 &max) noexcept
        {
            return typeMat4(
                OP<type4>::clamp(value[0], min[0], max[0]),
                OP<type4>::clamp(value[1], min[1], max[1]),
                OP<type4>::clamp(value[2], min[2], max[2]),
                OP<type4>::clamp(value[3], min[3], max[3]));
        }

        static __forceinline _type dot(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            _type dota = OP<type4>::dot(a[0], b[0]);
            _type dotb = OP<type4>::dot(a[1], b[1]);
            _type dotc = OP<type4>::dot(a[2], b[2]);
            _type dotd = OP<type4>::dot(a[3], b[3]);
            return dota + dotb + dotc + dotd;
        }

        static __forceinline typeMat4 normalize(const typeMat4 &m) noexcept
        {
            _type mag2 = self_type::dot(m, m);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(mag2);
            return m * mag2_rsqrt;
        }

        static __forceinline _type sqrLength(const typeMat4 &a) noexcept
        {
            return self_type::dot(a, a);
        }

        static __forceinline _type length(const typeMat4 &a) noexcept
        {
            return OP<_type>::sqrt(self_type::dot(a, a));
        }

        static __forceinline _type sqrDistance(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            typeMat4 ab = b - a;
            return self_type::dot(ab, ab);
        }

        static __forceinline _type distance(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            typeMat4 ab = b - a;
            return OP<_type>::sqrt(self_type::dot(ab, ab));
        }

        static __forceinline _type maximum(const typeMat4 &a) noexcept
        {
            type4 max_a = OP<type4>::maximum(a[0], a[1]);
            type4 max_b = OP<type4>::maximum(a[2], a[3]);
            type4 max_c = OP<type4>::maximum(max_a, max_b);
            return OP<type4>::maximum(max_c);
        }

        static __forceinline typeMat4 maximum(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            return typeMat4(OP<type4>::maximum(a[0], b[0]),
                            OP<type4>::maximum(a[1], b[1]),
                            OP<type4>::maximum(a[2], b[2]),
                            OP<type4>::maximum(a[3], b[3]));
        }

        static __forceinline _type minimum(const typeMat4 &a) noexcept
        {
            type4 max_a = OP<type4>::minimum(a[0], a[1]);
            type4 max_b = OP<type4>::minimum(a[2], a[3]);
            type4 max_c = OP<type4>::minimum(max_a, max_b);
            return OP<type4>::minimum(max_c);
        }

        static __forceinline typeMat4 minimum(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            return typeMat4(OP<type4>::minimum(a[0], b[0]),
                            OP<type4>::minimum(a[1], b[1]),
                            OP<type4>::minimum(a[2], b[2]),
                            OP<type4>::minimum(a[3], b[3]));
        }

        static __forceinline typeMat4 abs(const typeMat4 &a) noexcept
        {
            return typeMat4(OP<type4>::abs(a[0]),
                            OP<type4>::abs(a[1]),
                            OP<type4>::abs(a[2]),
                            OP<type4>::abs(a[3]));
        }

        static __forceinline typeMat4 lerp(const typeMat4 &a, const typeMat4 &b, const _type &factor) noexcept
        {
            //  return a+(b-a)*fator;
            return a * ((_type)1 - factor) + (b * factor);
        }

        static __forceinline typeMat4 barylerp(const _type &u, const _type &v, const typeMat4 &v0, const typeMat4 &v1, const typeMat4 &v2) noexcept
        {
            // return v0*(1-uv[0]-uv[1])+v1*uv[0]+v2*uv[1];
            return v0 * ((_type)1 - u - v) + v1 * u + v2 * v;
        }

        static __forceinline typeMat4 blerp(const typeMat4 &A, const typeMat4 &B, const typeMat4 &C, const typeMat4 &D,
                                          const _type &dx, const _type &dy) noexcept
        {
            _type omdx = (_type)1 - dx,
                  omdy = (_type)1 - dy;
            return (omdx * omdy) * A + (omdx * dy) * D + (dx * omdy) * B + (dx * dy) * C;
        }

        static __forceinline typeMat4 extractRotation(const typeMat4 &m) noexcept
        {
#if defined(ITK_SSE2)
            __m128 a = m.array_sse[0];
            __m128 b = m.array_sse[1];
            __m128 c = m.array_sse[2];

            a = _mm_and_ps(a, _vec3_valid_bits_sse);
            b = _mm_and_ps(b, _vec3_valid_bits_sse);
            c = _mm_and_ps(c, _vec3_valid_bits_sse);

            return typeMat4(a, b, c, _vec4_0001_sse);
#elif defined(ITK_NEON)
            typeMat4 r(
                m.array_neon[0],
                m.array_neon[1],
                m.array_neon[2],
                _neon_0001);

            r.array_neon[0][3] = 0;
            r.array_neon[1][3] = 0;
            r.array_neon[2][3] = 0;

            return r;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline type4 extractXaxis(const typeMat4 &m) noexcept
        {
            return m[0];
        }

        static __forceinline type4 extractYaxis(const typeMat4 &m) noexcept
        {
            return m[1];
        }

        static __forceinline type4 extractZaxis(const typeMat4 &m) noexcept
        {
            return m[2];
        }

        static __forceinline type4 extractTranslation(const typeMat4 &m) noexcept
        {
            return m[3];
        }

        static __forceinline typeMat4 transpose(const typeMat4 &m) noexcept
        {
#if defined(ITK_SSE2)
            __m128 tmp0 = _mm_shuffle_ps(m.array_sse[0], m.array_sse[1], _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp2 = _mm_shuffle_ps(m.array_sse[0], m.array_sse[1], _MM_SHUFFLE(3, 2, 3, 2));
            __m128 tmp1 = _mm_shuffle_ps(m.array_sse[2], m.array_sse[3], _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp3 = _mm_shuffle_ps(m.array_sse[2], m.array_sse[3], _MM_SHUFFLE(3, 2, 3, 2));

            return typeMat4(
                _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0)),
                _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1)),
                _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0)),
                _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(3, 1, 3, 1)));
#elif defined(ITK_NEON)
            float32x4x2_t ab = vtrnq_f32(m.array_neon[0], m.array_neon[1]);
            float32x4x2_t cd = vtrnq_f32(m.array_neon[2], m.array_neon[3]);
            float32x4_t a_ = vcombine_f32(vget_low_f32(ab.val[0]), vget_low_f32(cd.val[0]));
            float32x4_t b_ = vcombine_f32(vget_low_f32(ab.val[1]), vget_low_f32(cd.val[1]));
            float32x4_t c_ = vcombine_f32(vget_high_f32(ab.val[0]), vget_high_f32(cd.val[0]));
            float32x4_t d_ = vcombine_f32(vget_high_f32(ab.val[1]), vget_high_f32(cd.val[1]));

            return typeMat4(a_, b_, c_, d_);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline _type determinant(const typeMat4 &m) noexcept
        {
#if defined(ITK_SSE2)
            // SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            // SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            // SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];

            // SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            // SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            // SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

            // First 2 columns
            __m128 Swp2A = _mm_shuffle_ps(m.array_sse[2], m.array_sse[2], _MM_SHUFFLE(0, 1, 1, 2));
            __m128 Swp3A = _mm_shuffle_ps(m.array_sse[3], m.array_sse[3], _MM_SHUFFLE(3, 2, 3, 3));
            __m128 MulA = _mm_mul_ps(Swp2A, Swp3A);

            // Second 2 columns
            __m128 Swp2B = _mm_shuffle_ps(m.array_sse[2], m.array_sse[2], _MM_SHUFFLE(3, 2, 3, 3));
            __m128 Swp3B = _mm_shuffle_ps(m.array_sse[3], m.array_sse[3], _MM_SHUFFLE(0, 1, 1, 2));
            __m128 MulB = _mm_mul_ps(Swp2B, Swp3B);

            // Columns subtraction
            __m128 SubE = _mm_sub_ps(MulA, MulB);

            // Last 2 rows
            __m128 Swp2C = _mm_shuffle_ps(m.array_sse[2], m.array_sse[2], _MM_SHUFFLE(0, 0, 1, 2));
            __m128 Swp3C = _mm_shuffle_ps(m.array_sse[3], m.array_sse[3], _MM_SHUFFLE(1, 2, 0, 0));
            __m128 MulC = _mm_mul_ps(Swp2C, Swp3C);
            __m128 SubF = _mm_sub_ps(_mm_movehl_ps(MulC, MulC), MulC);

            // vec4(
            //	+ (m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02),
            //	- (m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04),
            //	+ (m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05),
            //	- (m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05));

            __m128 SubFacA = _mm_shuffle_ps(SubE, SubE, _MM_SHUFFLE(2, 1, 0, 0));
            __m128 SwpFacA = _mm_shuffle_ps(m.array_sse[1], m.array_sse[1], _MM_SHUFFLE(0, 0, 0, 1));
            __m128 MulFacA = _mm_mul_ps(SwpFacA, SubFacA);

            __m128 SubTmpB = _mm_shuffle_ps(SubE, SubF, _MM_SHUFFLE(0, 0, 3, 1));
            __m128 SubFacB = _mm_shuffle_ps(SubTmpB, SubTmpB, _MM_SHUFFLE(3, 1, 1, 0)); // SubF[0], SubE[3], SubE[3], SubE[1];
            __m128 SwpFacB = _mm_shuffle_ps(m.array_sse[1], m.array_sse[1], _MM_SHUFFLE(1, 1, 2, 2));
            __m128 MulFacB = _mm_mul_ps(SwpFacB, SubFacB);

            __m128 SubRes = _mm_sub_ps(MulFacA, MulFacB);

            __m128 SubTmpC = _mm_shuffle_ps(SubE, SubF, _MM_SHUFFLE(1, 0, 2, 2));
            __m128 SubFacC = _mm_shuffle_ps(SubTmpC, SubTmpC, _MM_SHUFFLE(3, 3, 2, 0));
            __m128 SwpFacC = _mm_shuffle_ps(m.array_sse[1], m.array_sse[1], _MM_SHUFFLE(2, 3, 3, 3));
            __m128 MulFacC = _mm_mul_ps(SwpFacC, SubFacC);

            __m128 AddRes = _mm_add_ps(SubRes, MulFacC);
            //__m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));

            const __m128 SignMask = _mm_set_ps(-0.0f, 0.0f, -0.0f, 0.0f);

            //__m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));
            __m128 DetCof = _mm_xor_ps(AddRes, SignMask);

            // return m[0][0] * DetCof[0]
            //	 + m[0][1] * DetCof[1]
            //	 + m[0][2] * DetCof[2]
            //	 + m[0][3] * DetCof[3];

            __m128 Det0 = dot_sse_4(m.array_sse[0], DetCof);

            return _mm_f32_(Det0, 0);
#elif defined(ITK_NEON)

            // T SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            // T SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            // T SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            // T SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            // T SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            // T SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

            // First 2 columns
            float32x4_t Swp2A = vshuffle_0112(m.array_neon[2]);
            float32x4_t Swp3A = vshuffle_3233(m.array_neon[3]);
            float32x4_t MulA = vmulq_f32(Swp2A, Swp3A);

            // Second 2 columns
            float32x4_t Swp2B = vshuffle_3233(m.array_neon[2]);
            float32x4_t Swp3B = vshuffle_0112(m.array_neon[3]);
            float32x4_t MulB = vmulq_f32(Swp2B, Swp3B);

            // Columns subtraction
            float32x4_t SubE = vsubq_f32(MulA, MulB);

            // Last 2 rows
            float32x4_t Swp2C = vshuffle_0012(m.array_neon[2]);
            float32x4_t Swp3C = vshuffle_1200(m.array_neon[3]);
            float32x4_t MulC = vmulq_f32(Swp2C, Swp3C);
            float32x4_t SubF = vsubq_f32(vmovehl(MulC, MulC), MulC);

            // vec4(
            //	+ (m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02),
            //	- (m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04),
            //	+ (m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05),
            //	- (m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05));

            float32x4_t SubFacA = vshuffle_2100(SubE);
            float32x4_t SwpFacA = vshuffle_0001(m.array_neon[1]);
            float32x4_t MulFacA = vmulq_f32(SwpFacA, SubFacA);

            float32x4_t SubTmpB = vshuffle_0031(SubE, SubF);
            float32x4_t SubFacB = vshuffle_3110(SubTmpB); // SubF[0], SubE[3], SubE[3], SubE[1];
            float32x4_t SwpFacB = vshuffle_1122(m.array_neon[1]);
            float32x4_t MulFacB = vmulq_f32(SwpFacB, SubFacB);

            float32x4_t SubRes = vsubq_f32(MulFacA, MulFacB);

            float32x4_t SubTmpC = vshuffle_1022(SubE, SubF);
            float32x4_t SubFacC = vshuffle_3320(SubTmpC);
            float32x4_t SwpFacC = vshuffle_2333(m.array_neon[1]);
            float32x4_t MulFacC = vmulq_f32(SwpFacC, SubFacC);

            float32x4_t AddRes = vaddq_f32(SubRes, MulFacC);
            //__m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));

            const float32x4_t SignMask = (float32x4_t){1.0f, -1.0f, 1.0f, -1.0f};

            //__m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));
            float32x4_t DetCof = vmulq_f32(AddRes, SignMask);

            // return m[0][0] * DetCof[0]
            //	 + m[0][1] * DetCof[1]
            //	 + m[0][2] * DetCof[2]
            //	 + m[0][3] * DetCof[3];

            float32x4_t Det0 = dot_neon_4(m.array_neon[0], DetCof);

            return Det0[0];

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline void extractEuler(const typeMat4 &m, _type *roll, _type *pitch, _type *yaw) noexcept
        {
            //
            // https://www.learnopencv.com/rotation-matrix-to-euler-angles/
            //
            _type sy = OP<_type>::sqrt(m.a1 * m.a1 + m.a2 * m.a2);

            bool singular = sy < EPSILON<_type>::high_precision; // 1e-6f; // If

            float x, y, z;
            if (!singular)
            {
                x = OP<_type>::atan2(m.b3, m.c3);
                y = OP<_type>::atan2(-m.a3, sy);
                z = OP<_type>::atan2(m.a2, m.a1);
            }
            else
            {
                x = OP<_type>::atan2(-m.c2, m.b2);
                y = OP<_type>::atan2(-m.a3, sy);
                z = 0;
            }

            *roll = x;
            *pitch = y;
            *yaw = z;
        }

        static __forceinline typeMat4 move(const typeMat4 &current, const typeMat4 &target, const _type &maxDistanceVariation) noexcept
        {
            _type deltaDistance = self_type::distance(current, target);

            deltaDistance = OP<_type>::maximum(deltaDistance, maxDistanceVariation);
            // avoid division by zero
            // deltaDistance = self_type::maximum(deltaDistance, EPSILON<float>::high_precision);
            deltaDistance = OP<_type>::maximum(deltaDistance, FloatTypeInfo<_type>::min);
            return self_type::lerp(current, target, maxDistanceVariation / deltaDistance);

            // if (deltaDistance < maxDistanceVariation + EPSILON<_type>::high_precision)
            //     return target;
            // return lerp(current, target, maxDistanceVariation / deltaDistance);
        }

        static __forceinline typeMat4 sign(const typeMat4 &m) noexcept
        {
            return typeMat4(
                OP<type4>::sign(m[0]),
                OP<type4>::sign(m[1]),
                OP<type4>::sign(m[2]),
                OP<type4>::sign(m[3]));
        }

        static __forceinline typeMat4 floor(const typeMat4 &m) noexcept
        {
            return typeMat4(
                OP<type4>::floor(m[0]),
                OP<type4>::floor(m[1]),
                OP<type4>::floor(m[2]),
                OP<type4>::floor(m[3]));
        }

        static __forceinline typeMat4 ceil(const typeMat4 &m) noexcept
        {
            return typeMat4(
                OP<type4>::ceil(m[0]),
                OP<type4>::ceil(m[1]),
                OP<type4>::ceil(m[2]),
                OP<type4>::ceil(m[3]));
        }

        static __forceinline typeMat4 round(const typeMat4 &m) noexcept
        {
            return typeMat4(
                OP<type4>::round(m[0]),
                OP<type4>::round(m[1]),
                OP<type4>::round(m[2]),
                OP<type4>::round(m[3]));
        }

        static __forceinline typeMat4 fmod(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            return typeMat4(
                OP<type4>::fmod(a[0], b[0]),
                OP<type4>::fmod(a[1], b[1]),
                OP<type4>::fmod(a[2], b[2]),
                OP<type4>::fmod(a[3], b[3]));
        }

        static __forceinline typeMat4 step(const typeMat4 &threshould, const typeMat4 &v) noexcept
        {
            return typeMat4(
                OP<type4>::step(threshould[0], v[0]),
                OP<type4>::step(threshould[1], v[1]),
                OP<type4>::step(threshould[2], v[2]),
                OP<type4>::step(threshould[3], v[3]));
        }

        static __forceinline typeMat4 smoothstep(const typeMat4 &edge0, const typeMat4 &edge1, const typeMat4 &x) noexcept
        {
            return typeMat4(
                OP<type4>::smoothstep(edge0[0], edge1[0], x[0]),
                OP<type4>::smoothstep(edge0[1], edge1[1], x[1]),
                OP<type4>::smoothstep(edge0[2], edge1[2], x[2]),
                OP<type4>::smoothstep(edge0[3], edge1[3], x[3]));
        }
    };

}