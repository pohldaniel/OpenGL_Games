#pragma once

#include "mat3_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

//#include "quat_op.h" -- do not use any quat for mat3 OP

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<mat3<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value>::type,
              _algorithm>
    {
        private:
        using typeMat3 = mat3<_type, _simd>;
        using type3 = vec3<_type, _simd>;
        using self_type = OP<typeMat3>;
        public:

        static __forceinline typeMat3 clamp(const typeMat3 &value, const typeMat3 &min, const typeMat3 &max) noexcept
        {
            return typeMat3(
                OP<type3>::clamp(value[0], min[0], max[0]),
                OP<type3>::clamp(value[1], min[1], max[1]),
                OP<type3>::clamp(value[2], min[2], max[2]));
        }

        static __forceinline _type dot(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            _type dota = OP<type3>::dot(a[0], b[0]);
            _type dotb = OP<type3>::dot(a[1], b[1]);
            _type dotc = OP<type3>::dot(a[2], b[2]);
            return dota + dotb + dotc;
        }

        static __forceinline typeMat3 normalize(const typeMat3 &m) noexcept
        {
            _type mag2 = self_type::dot(m, m);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(mag2);
            return m * mag2_rsqrt;
        }

        static __forceinline _type sqrLength(const typeMat3 &a) noexcept
        {
            return self_type::dot(a, a);
        }

        static __forceinline _type length(const typeMat3 &a) noexcept
        {
            return OP<_type>::sqrt(self_type::dot(a, a));
        }

        static __forceinline _type sqrDistance(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            typeMat3 ab = b - a;
            return self_type::dot(ab, ab);
        }

        static __forceinline _type distance(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            typeMat3 ab = b - a;
            return OP<_type>::sqrt(self_type::dot(ab, ab));
        }

        static __forceinline _type maximum(const typeMat3 &a) noexcept
        {
            type3 max_a = OP<type3>::maximum(a[0], a[1]);
            type3 max_c = OP<type3>::maximum(max_a, a[2]);
            return OP<type3>::maximum(max_c);
        }

        static __forceinline typeMat3 maximum(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            return typeMat3(OP<type3>::maximum(a[0], b[0]),
                            OP<type3>::maximum(a[1], b[1]),
                            OP<type3>::maximum(a[2], b[2]));
        }

        static __forceinline _type minimum(const typeMat3 &a) noexcept
        {
            type3 max_a = OP<type3>::minimum(a[0], a[1]);
            type3 max_c = OP<type3>::minimum(max_a, a[2]);
            return OP<type3>::minimum(max_c);
        }

        static __forceinline typeMat3 minimum(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            return typeMat3(OP<type3>::minimum(a[0], b[0]),
                            OP<type3>::minimum(a[1], b[1]),
                            OP<type3>::minimum(a[2], b[2]));
        }

        static __forceinline typeMat3 abs(const typeMat3 &a) noexcept
        {
            return typeMat3(OP<type3>::abs(a[0]),
                            OP<type3>::abs(a[1]),
                            OP<type3>::abs(a[2]));
        }

        static __forceinline typeMat3 lerp(const typeMat3 &a, const typeMat3 &b, const _type &factor) noexcept
        {
            //  return a+(b-a)*fator;
            return a * ((_type)1 - factor) + (b * factor);
        }

        static __forceinline typeMat3 barylerp(const _type &u, const _type &v, const typeMat3 &v0, const typeMat3 &v1, const typeMat3 &v2) noexcept
        {
            // return v0*(1-uv[0]-uv[1])+v1*uv[0]+v2*uv[1];
            return v0 * ((_type)1 - u - v) + v1 * u + v2 * v;
        }

        static __forceinline typeMat3 blerp(const typeMat3 &A, const typeMat3 &B, const typeMat3 &C, const typeMat3 &D,
                                          const _type &dx, const _type &dy) noexcept
        {
            _type omdx = (_type)1 - dx,
                  omdy = (_type)1 - dy;
            return (omdx * omdy) * A + (omdx * dy) * D + (dx * omdy) * B + (dx * dy) * C;
        }

        static __forceinline typeMat3 extractRotation(const typeMat3 &m) noexcept
        {
            return typeMat3(m.a1, m.b1, 0,
                            m.a2, m.b2, 0,
                            0, 0, 1);
        }

        static __forceinline type3 extractXaxis(const typeMat3& m) noexcept
        {
            return m[0];
        }

        static __forceinline type3 extractYaxis(const typeMat3& m) noexcept
        {
            return m[1];
        }

        static __forceinline type3 extractZaxis(const typeMat3& m) noexcept
        {
            return m[2];
        }

        static __forceinline type3 extractTranslation(const typeMat3& m) noexcept
        {
            return m[2];
        }

        static __forceinline typeMat3 transpose(const typeMat3 &m) noexcept
        {
            return typeMat3(m.a1, m.a2, m.a3,
                            m.b1, m.b2, m.b3,
                            m.c1, m.c2, m.c3);
        }

        static __forceinline _type determinant(const typeMat3 &m) noexcept
        {
            _type aux1 = m.c3 * m.b2 - m.b3 * m.c2;
            _type aux2 = m.b3 * m.c1 - m.c3 * m.b1;
            _type aux3 = m.c2 * m.b1 - m.b2 * m.c1;

            return (m.a1 * aux1 + m.a2 * aux2 + m.a3 * aux3);
        }

        static __forceinline void extractEuler(const typeMat3 &m, _type *roll, _type *pitch, _type *yaw) noexcept
        {
            //
            // https://www.learnopencv.com/rotation-matrix-to-euler-angles/
            //
            _type sy = OP<_type>::sqrt(m.a1 * m.a1 + m.a2 * m.a2);

            bool singular = sy < EPSILON<_type>::high_precision;// 1e-6f; // If

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

        static __forceinline typeMat3 move(const typeMat3 &current, const typeMat3 &target, const _type &maxDistanceVariation) noexcept
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

        static __forceinline typeMat3 sign(const typeMat3 &m) noexcept
        {
            return typeMat3(
                OP<type3>::sign(m[0]),
                OP<type3>::sign(m[1]),
                OP<type3>::sign(m[2]));
        }

        static __forceinline typeMat3 floor(const typeMat3 &m) noexcept
        {
            return typeMat3(
                OP<type3>::floor(m[0]),
                OP<type3>::floor(m[1]),
                OP<type3>::floor(m[2]));
        }

        static __forceinline typeMat3 ceil(const typeMat3 &m) noexcept
        {
            return typeMat3(
                OP<type3>::ceil(m[0]),
                OP<type3>::ceil(m[1]),
                OP<type3>::ceil(m[2]));
        }

        static __forceinline typeMat3 round(const typeMat3 &m) noexcept
        {
            return typeMat3(
                OP<type3>::round(m[0]),
                OP<type3>::round(m[1]),
                OP<type3>::round(m[2]));
        }

        static __forceinline typeMat3 fmod(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            return typeMat3(
                OP<type3>::fmod(a[0], b[0]),
                OP<type3>::fmod(a[1], b[1]),
                OP<type3>::fmod(a[2], b[2]));
        }

        static __forceinline typeMat3 step(const typeMat3 &threshould, const typeMat3 &v) noexcept
        {
            return typeMat3(
                OP<type3>::step(threshould[0], v[0]),
                OP<type3>::step(threshould[1], v[1]),
                OP<type3>::step(threshould[2], v[2]));
        }

        static __forceinline typeMat3 smoothstep(const typeMat3 &edge0, const typeMat3 &edge1, const typeMat3 &x) noexcept
        {
            return typeMat4(
                OP<type3>::smoothstep(edge0[0], edge1[0], x[0]),
                OP<type3>::smoothstep(edge0[1], edge1[1], x[1]),
                OP<type3>::smoothstep(edge0[2], edge1[2], x[2]));
        }

    };

}