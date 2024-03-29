#pragma once

#include "mat4_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<mat4<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value>::type,
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
            return typeMat4(m.a1, m.b1, m.c1, 0,
                            m.a2, m.b2, m.c2, 0,
                            m.a3, m.b3, m.c3, 0,
                            0, 0, 0, 1);
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
            return typeMat4(m.a1, m.a2, m.a3, m.a4,
                            m.b1, m.b2, m.b3, m.b4,
                            m.c1, m.c2, m.c3, m.c4,
                            m.d1, m.d2, m.d3, m.d4);
        }

        static __forceinline _type determinant(const typeMat4 &m) noexcept
        {
            _type SubFactor00 = m.c3 * m.d4 - m.d3 * m.c4;
            _type SubFactor01 = m.c2 * m.d4 - m.d2 * m.c4;
            _type SubFactor02 = m.c2 * m.d3 - m.d2 * m.c3;
            _type SubFactor03 = m.c1 * m.d4 - m.d1 * m.c4;
            _type SubFactor04 = m.c1 * m.d3 - m.d1 * m.c3;
            _type SubFactor05 = m.c1 * m.d2 - m.d1 * m.c2;

            type4 aux = type4(
                +(m.b2 * SubFactor00 - m.b3 * SubFactor01 + m.b4 * SubFactor02),
                -(m.b1 * SubFactor00 - m.b3 * SubFactor03 + m.b4 * SubFactor04),
                +(m.b1 * SubFactor01 - m.b2 * SubFactor03 + m.b4 * SubFactor05),
                -(m.b1 * SubFactor02 - m.b2 * SubFactor04 + m.b3 * SubFactor05));

            return OP<type4>::dot(m[0], aux);
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