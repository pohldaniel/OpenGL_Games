#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'quat_gen_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "quat_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

#include "../op.h"

namespace MathCore
{

    template <typename _type, typename _simd>
    struct GEN<quat<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_type, float>::value &&
                   (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                    std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type>
    {
    private:
        using quatT = quat<_type, _simd>;
        using type2 = vec2<_type, _simd>;
        using type3 = vec3<_type, _simd>;
        using type4 = vec4<_type, _simd>;

        using typeMat3 = mat3<_type, _simd>;
        using typeMat4 = mat4<_type, _simd>;

        using self_type = GEN<quatT>;

    public:
        /// \brief Convert a vec3 to a unity quaternion pointing to the vec3 axis.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 axis = normalize( vec3( 1.0f, 1.0f, 0.0f ) );
        ///
        /// quat result = fromAxis( axis );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v A 3 component vector
        /// \return The quaternion
        ///
        static __forceinline quatT fromAxis(const type3 &vp) noexcept
        {
            quatT rc;
            type3 &v = *(type3 *)&rc;
            v = OP<type3>::normalize(vp);
            _type t = (_type)1 - OP<type3>::dot(v);
            rc.w = OP<_type>::sqrt(t);
            return rc;
        }

        /// \brief Convert a vec3 to a unity quaternion pointing to the vec3 axis.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec4 axis = normalize( vec4( 1.0f, 1.0f, 0.0f, 0.0f ) );
        ///
        /// quat result = fromAxis( axis );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v A 4 component vector
        /// \return The quaternion
        ///
        static __forceinline quatT fromAxis(const type4 &vp) noexcept
        {
            return self_type::fromAxis(*(const type3 *)&vp);
        }

        /// \brief Convert a vec2 to a unity quaternion pointing to the vec2 axis.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 axis = normalize( vec2( 1.0f, 1.0f ) );
        ///
        /// quat result = fromAxis( axis );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v A 2 component vector
        /// \return The quaternion
        ///
        static __forceinline quatT fromAxis(const type2 &vp) noexcept
        {
            return self_type::fromAxis(type3(vp, 0));
        }

        /// \brief Constructs a quaternion from an axis and an angle in radians.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 axis = normalize( vec3( 1.0f, 1.0f, 0.0f ) );
        /// float angle = DEG2RAD( 30.0f );
        ///
        /// quat result = fromAxisAngle( axis, angle );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param axis The reference axis
        /// \param angle_rad The angle to rotate around the axis
        /// \return The quat
        ///
        static __forceinline quatT fromAxisAngle(const type3 &axis, const _type &angle_rad) noexcept
        {
            _type sinAngle;
            _type angle = angle_rad * (_type)0.5;
            type3 vn = OP<type3>::normalize(axis);
            sinAngle = OP<_type>::sin(angle);
            vn *= sinAngle;

#if defined(ITK_SSE2)
            _mm_f32_(vn.array_sse, 3) = OP<_type>::cos(angle);
            return vn.array_sse;
#elif defined(ITK_NEON)
            vn.array_neon[3] = OP<_type>::cos(angle);
            return vn.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a quaternion from an axis and an angle in radians.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec4 axis = normalize( vec4( 1.0f, 1.0f, 0.0f, 0.0f ) );
        /// float angle = DEG2RAD( 30.0f );
        ///
        /// quat result = fromAxisAngle( axis, angle );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param axis The reference axis
        /// \param angle_rad The angle to rotate around the axis
        /// \return The quat
        ///
        static __forceinline quatT fromAxisAngle(const type4 &axis, const _type &angle_rad) noexcept
        {
            return fromAxisAngle(*(const type3 *)&axis, angle_rad);
        }

        /// \brief Constructs a quaternion from an axis and an angle in radians.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 axis = normalize( vec2( 1.0f, 1.0f ) );
        /// float angle = DEG2RAD( 30.0f );
        ///
        /// quat result = fromAxisAngle( axis, angle );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param axis The reference axis
        /// \param angle_rad The angle to rotate around the axis
        /// \return The quat
        ///
        static __forceinline quatT fromAxisAngle(const type2 &axis, const _type &angle_rad) noexcept
        {
            return fromAxisAngle(type3(axis, 0), angle_rad);
        }

        /// \brief Constructs a quaternion from euler angles in radians.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat result = fromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param pitch radians
        /// \param yaw radians
        /// \param roll radians
        /// \return The quaternion
        ///
        static __forceinline quatT fromEuler(_type roll, _type pitch, _type yaw) noexcept
        {

            pitch *= (_type)0.5;
            yaw *= (_type)0.5;
            roll *= (_type)0.5;

#if defined(ITK_SSE2)
            __m128 rollSinCos = _mm_setr_ps(OP<_type>::sin(roll), OP<_type>::cos(roll), 0, 0);
            __m128 pitchSinCos = _mm_setr_ps(OP<_type>::sin(pitch), OP<_type>::cos(pitch), 0, 0);
            __m128 yawSinCos = _mm_setr_ps(OP<_type>::sin(yaw), OP<_type>::cos(yaw), 0, 0);

            __m128 row0 = _mm_shuffle_ps(rollSinCos, rollSinCos, _MM_SHUFFLE(1, 1, 1, 0));
            __m128 row1 = _mm_shuffle_ps(rollSinCos, rollSinCos, _MM_SHUFFLE(0, 0, 0, 1));

            __m128 pitch0 = _mm_shuffle_ps(pitchSinCos, pitchSinCos, _MM_SHUFFLE(1, 1, 0, 1));
            __m128 pitch1 = _mm_shuffle_ps(pitchSinCos, pitchSinCos, _MM_SHUFFLE(0, 0, 1, 0));

            __m128 yaw0 = _mm_shuffle_ps(yawSinCos, yawSinCos, _MM_SHUFFLE(1, 0, 1, 1));
            __m128 yaw1 = _mm_shuffle_ps(yawSinCos, yawSinCos, _MM_SHUFFLE(0, 1, 0, 0));

            __m128 mul0 = _mm_mul_ps(row0, pitch0);
            mul0 = _mm_mul_ps(mul0, yaw0);

            __m128 mul1 = _mm_mul_ps(row1, pitch1);
            mul1 = _mm_mul_ps(mul1, yaw1);

            // const __m128 _mask = _mm_setr_ps(-1.0f,1.0f,-1.0f,1.0f);
            const __m128 _mask_xor = _mm_setr_ps(-0.0f, 0.0f, -0.0f, 0.0f);

            // mul1 = _mm_mul_ps(mul1, _mask );
            mul1 = _mm_xor_ps(mul1, _mask_xor); // much faster

            return _mm_add_ps(mul0, mul1);
#elif defined(ITK_NEON)
            float32x4_t rollSinCos = (float32x4_t){OP<_type>::sin(roll), OP<_type>::cos(roll), 0, 0};
            float32x4_t pitchSinCos = (float32x4_t){OP<_type>::sin(pitch), OP<_type>::cos(pitch), 0, 0};
            float32x4_t yawSinCos = (float32x4_t){OP<_type>::sin(yaw), OP<_type>::cos(yaw), 0, 0};

            float32x4_t row0 = vshuffle_1110(rollSinCos);
            float32x4_t row1 = vshuffle_0001(rollSinCos);

            float32x4_t pitch0 = vshuffle_1101(pitchSinCos);
            float32x4_t pitch1 = vshuffle_0010(pitchSinCos);

            float32x4_t yaw0 = vshuffle_1011(yawSinCos);
            float32x4_t yaw1 = vshuffle_0100(yawSinCos);

            float32x4_t mul0 = vmulq_f32(row0, pitch0);
            mul0 = vmulq_f32(mul0, yaw0);

            float32x4_t mul1 = vmulq_f32(row1, pitch1);
            mul1 = vmulq_f32(mul1, yaw1);
            mul1 = vmulq_f32(mul1, (float32x4_t){-1.0f, 1.0f, -1.0f, 1.0f});

            return vaddq_f32(mul0, mul1);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Creates a quaternion looking to any direction
        ///
        /// This matrix can be used as base to an object node
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 otherObjPos;
        /// vec3 objPos;
        ///
        /// vec3 front = normalize( otherObjPos - objPos );
        /// vec3 up = vec3(0,1,0);
        ///
        /// quat object_rotation = lookAtRotationRH(front, up, objPos);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param front A vector pointing to the direction you want
        /// \param up A vector to indicate the up orientation
        /// \param position A point to be used as origin
        /// \return A 4x4 matrix
        ///
        static __forceinline quatT lookAtRotationRH(const type3 &front, const type3 &up) noexcept
        {
            type3 x, y, z;
            z = front * (_type)-1;
            x = OP<type3>::cross(up, z);
            y = OP<type3>::cross(z, x);
            // all parameters will be normalized in the fromMat3 implementation
            return self_type::fromMat3(typeMat3(x, y, z));
        }

        /// \brief Creates a quaternion looking to any direction
        ///
        /// This matrix can be used as base to an object node
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 otherObjPos;
        /// vec3 objPos;
        ///
        /// vec3 front = normalize( otherObjPos - objPos );
        /// vec3 up = vec3(0,1,0);
        ///
        /// quat object_rotation = lookAtRotationLH(front, up, objPos);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param front A vector pointing to the direction you want
        /// \param up A vector to indicate the up orientation
        /// \param position A point to be used as origin
        /// \return A 4x4 matrix
        ///
        static __forceinline quatT lookAtRotationLH(const type3 &front, const type3 &up) noexcept
        {
            type3 x, y, z;
            z = front;
            x = OP<type3>::cross(up, z);
            y = OP<type3>::cross(z, x);
            // all parameters will be normalized in the fromMat3 implementation
            return self_type::fromMat3(typeMat3(x, y, z));
        }

        /// \brief Creates a quaternion looking to any direction
        ///
        /// This matrix can be used as base to an object node
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec4 otherObjPos;
        /// vec4 objPos;
        ///
        /// vec4 front = normalize( otherObjPos - objPos );
        /// vec4 up = vec4(0,1,0,0);
        ///
        /// quat object_rotation = lookAtRotationRH(front, up, objPos);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param front A vector pointing to the direction you want
        /// \param up A vector to indicate the up orientation
        /// \param position A point to be used as origin
        /// \return A 4x4 matrix
        ///
        static __forceinline quatT lookAtRotationRH(const type4 &front, const type4 &up) noexcept
        {
            type3 x, y, z;
            z = *(const type3 *)&front * (_type)-1;
            x = OP<type3>::cross(*(const type3 *)&up, z);
            y = OP<type3>::cross(z, x);
            // all parameters will be normalized in the fromMat3 implementation
            return self_type::fromMat3(typeMat3(x, y, z));
        }

        /// \brief Creates a quaternion looking to any direction
        ///
        /// This matrix can be used as base to an object node
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec4 otherObjPos;
        /// vec4 objPos;
        ///
        /// vec4 front = normalize( otherObjPos - objPos );
        /// vec4 up = vec4(0,1,0,0);
        ///
        /// quat object_rotation = lookAtRotationLH(front, up, objPos);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param front A vector pointing to the direction you want
        /// \param up A vector to indicate the up orientation
        /// \param position A point to be used as origin
        /// \return A 4x4 matrix
        ///
        static __forceinline quatT lookAtRotationLH(const type4 &front, const type4 &up) noexcept
        {
            type3 x, y, z;
            z = *(const type3 *)&front;
            x = OP<type3>::cross(*(const type3 *)&up, z);
            y = OP<type3>::cross(z, x);
            // all parameters will be normalized in the fromMat3 implementation
            return self_type::fromMat3(typeMat3(x, y, z));
        }

        /// \brief Extracts a quaternion from any matrix that have rotation information
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// mat3 transformation = eulerRotate( DEG2RAD(30.0f), DEG2RAD(3.0f), DEG2RAD(20.0f) );
        ///
        /// // converts the transform matrix to a quaternion representation
        /// quat transformation_quaternion = fromMat3( transformation );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The transformation 4x4 matrix
        /// \return The quat with the rotation information
        ///
        static __forceinline quatT fromMat3(const typeMat3 &mp) noexcept
        {

            typeMat3 m = mp;

            // normalize rotation part
            m[0] = OP<type3>::normalize(m[0]);
            m[1] = OP<type3>::normalize(m[1]);
            m[2] = OP<type3>::normalize(m[2]);

#if defined(ITK_SSE2)

            const __m128 _inva = _mm_setr_ps(.0f, .0f, -.0f, -.0f);
            const __m128 _invb = _mm_setr_ps(.0f, -.0f, .0f, -.0f);
            const __m128 _invc = _mm_setr_ps(.0f, -.0f, -.0f, .0f);

            __m128 _1st = _mm_xor_ps(_mm_set1_ps(m.a1), _inva);
            __m128 _2nd = _mm_xor_ps(_mm_set1_ps(m.b2), _invb);
            __m128 _3nd = _mm_xor_ps(_mm_set1_ps(m.c3), _invc);

            __m128 sum_a = _mm_add_ps(_vec4_one_sse, _1st);
            __m128 sum_b = _mm_add_ps(_2nd, _3nd);

            __m128 sum = _mm_add_ps(sum_a, sum_b);

            // large enough
            if (_mm_f32_(sum, 0) > (_type)0.001)
            {
                _type s = OP<_type>::sqrt(_mm_f32_(sum, 0)) * (_type)2;
                _type _1_o_s = 1.0f / s;

                __m128 _el1 = _mm_setr_ps(m.b3, m.c1, m.a2, 0.25f);
                __m128 _el2 = _mm_setr_ps(m.c2, m.a3, m.b1, 0.0f);

                __m128 _el_sub = _mm_sub_ps(_el1, _el2);

                __m128 _mul_pattern = _mm_setr_ps(_1_o_s, _1_o_s, _1_o_s, s);
                __m128 _el_mul = _mm_mul_ps(_el_sub, _mul_pattern);

                return _el_mul;

            } // else we have to check several cases
            else if (m.a1 > m.b2 && m.a1 > m.c3)
            {
                // Column 0:
                _type s = OP<_type>::sqrt(_mm_f32_(sum, 1)) * (_type)2;
                _type _1_o_s = 1.0f / s;

                __m128 _el1 = _mm_setr_ps(0.25f, m.a2, m.c1, m.b3);
                __m128 _el2 = _mm_setr_ps(0.0f, m.b1, m.a3, -m.c2);

                __m128 _el_sub = _mm_add_ps(_el1, _el2);

                __m128 _mul_pattern = _mm_setr_ps(s, _1_o_s, _1_o_s, _1_o_s);
                __m128 _el_mul = _mm_mul_ps(_el_sub, _mul_pattern);

                return _el_mul;
            }
            else if (m.b2 > m.c3)
            {
                // Column 1:
                _type s = OP<_type>::sqrt(_mm_f32_(sum, 2)) * (_type)2;
                _type _1_o_s = 1.0f / s;

                __m128 _el1 = _mm_setr_ps(m.a2, 0.25f, m.b3, m.c1);
                __m128 _el2 = _mm_setr_ps(m.b1, 0.0f, m.c2, -m.a3);

                __m128 _el_sub = _mm_add_ps(_el1, _el2);

                __m128 _mul_pattern = _mm_setr_ps(_1_o_s, s, _1_o_s, _1_o_s);
                __m128 _el_mul = _mm_mul_ps(_el_sub, _mul_pattern);

                return _el_mul;
            }
            else
            {
                // Column 2:
                _type s = OP<_type>::sqrt(_mm_f32_(sum, 3)) * (_type)2;
                _type _1_o_s = 1.0f / s;

                __m128 _el1 = _mm_setr_ps(m.c1, m.b3, 0.25f, m.a2);
                __m128 _el2 = _mm_setr_ps(m.a3, m.c2, 0.0f, -m.b1);

                __m128 _el_sub = _mm_add_ps(_el1, _el2);

                __m128 _mul_pattern = _mm_setr_ps(_1_o_s, _1_o_s, s, _1_o_s);
                __m128 _el_mul = _mm_mul_ps(_el_sub, _mul_pattern);

                return _el_mul;
            }

#elif defined(ITK_NEON)
            type4 _1st(m.a1, m.a1, -m.a1, -m.a1);
            type4 _2nd(m.b2, -m.b2, m.b2, -m.b2);
            type4 _3nd(m.c3, -m.c3, -m.c3, m.c3);

            type4 sum_a = type4(1.0f) + _1st;
            type4 sum_b = _2nd + _3nd;

            type4 sum = sum_a + sum_b;

            // large enough
            if (sum.x > (_type)0.001)
            {
                _type s = OP<_type>::sqrt(sum.x) * (_type)2;
                _type _1_o_s = 1.0f / s;

                type4 _el1(m.b3, m.c1, m.a2, 0.25f);
                type4 _el2(m.c2, m.a3, m.b1, 0.0f);

                type4 _el_sub = _el1 - _el2;

                type4 _mul_pattern(_1_o_s, _1_o_s, _1_o_s, s);
                type4 _el_mul = _el_sub * _mul_pattern;

                return _el_mul.array_neon;

            } // else we have to check several cases
            else if (m.a1 > m.b2 && m.a1 > m.c3)
            {
                // Column 0:
                _type s = OP<_type>::sqrt(sum.y) * (_type)2;
                _type _1_o_s = 1.0f / s;

                type4 _el1(0.25f, m.a2, m.c1, m.b3);
                type4 _el2(0.0f, m.b1, m.a3, -m.c2);

                type4 _el_sub = _el1 + _el2;

                type4 _mul_pattern(s, _1_o_s, _1_o_s, _1_o_s);
                type4 _el_mul = _el_sub * _mul_pattern;

                return _el_mul.array_neon;
            }
            else if (m.b2 > m.c3)
            {
                // Column 1:
                _type s = OP<_type>::sqrt(sum.z) * (_type)2;
                _type _1_o_s = 1.0f / s;

                type4 _el1(m.a2, 0.25f, m.b3, m.c1);
                type4 _el2(m.b1, 0.0f, m.c2, -m.a3);

                type4 _el_sub = _el1 + _el2;

                type4 _mul_pattern(_1_o_s, s, _1_o_s, _1_o_s);
                type4 _el_mul = _el_sub * _mul_pattern;

                return _el_mul.array_neon;
            }
            else
            {
                // Column 2:
                _type s = OP<_type>::sqrt(sum.w) * (_type)2;
                _type _1_o_s = 1.0f / s;

                type4 _el1(m.c1, m.b3, 0.25f, m.a2);
                type4 _el2(m.a3, m.c2, 0.0f, -m.b1);

                type4 _el_sub = _el1 + _el2;

                type4 _mul_pattern(_1_o_s, _1_o_s, s, _1_o_s);
                type4 _el_mul = _el_sub * _mul_pattern;

                return _el_mul.array_neon;
            }
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Extracts a quaternion from any matrix that have rotation information
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 transformation = eulerRotate( DEG2RAD(30.0f), DEG2RAD(3.0f), DEG2RAD(20.0f) );
        ///
        /// // converts the transform matrix to a quaternion representation
        /// quat transformation_quaternion = fromMat4( transformation );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The transformation 4x4 matrix
        /// \return The quat with the rotation information
        ///
        static __forceinline quatT fromMat4(const typeMat4 &mp) noexcept
        {
            typeMat4 m = mp;

            // normalize rotation part
            *(type3 *)&(m[0]) = OP<type3>::normalize(*(type3 *)&(m[0]));
            *(type3 *)&(m[1]) = OP<type3>::normalize(*(type3 *)&(m[1]));
            *(type3 *)&(m[2]) = OP<type3>::normalize(*(type3 *)&(m[2]));

#if defined(ITK_SSE2)

            const __m128 _inva = _mm_setr_ps(.0f, .0f, -.0f, -.0f);
            const __m128 _invb = _mm_setr_ps(.0f, -.0f, .0f, -.0f);
            const __m128 _invc = _mm_setr_ps(.0f, -.0f, -.0f, .0f);

            __m128 _1st = _mm_xor_ps(_mm_set1_ps(m.a1), _inva);
            __m128 _2nd = _mm_xor_ps(_mm_set1_ps(m.b2), _invb);
            __m128 _3nd = _mm_xor_ps(_mm_set1_ps(m.c3), _invc);

            __m128 sum_a = _mm_add_ps(_vec4_one_sse, _1st);
            __m128 sum_b = _mm_add_ps(_2nd, _3nd);

            __m128 sum = _mm_add_ps(sum_a, sum_b);

            // large enough
            if (_mm_f32_(sum, 0) > (_type)0.001)
            {
                _type s = OP<_type>::sqrt(_mm_f32_(sum, 0)) * (_type)2;
                _type _1_o_s = 1.0f / s;

                __m128 _el1 = _mm_setr_ps(m.b3, m.c1, m.a2, 0.25f);
                __m128 _el2 = _mm_setr_ps(m.c2, m.a3, m.b1, 0.0f);

                __m128 _el_sub = _mm_sub_ps(_el1, _el2);

                __m128 _mul_pattern = _mm_setr_ps(_1_o_s, _1_o_s, _1_o_s, s);
                __m128 _el_mul = _mm_mul_ps(_el_sub, _mul_pattern);

                return _el_mul;

            } // else we have to check several cases
            else if (m.a1 > m.b2 && m.a1 > m.c3)
            {
                // Column 0:
                _type s = OP<_type>::sqrt(_mm_f32_(sum, 1)) * (_type)2;
                _type _1_o_s = 1.0f / s;

                __m128 _el1 = _mm_setr_ps(0.25f, m.a2, m.c1, m.b3);
                __m128 _el2 = _mm_setr_ps(0.0f, m.b1, m.a3, -m.c2);

                __m128 _el_sub = _mm_add_ps(_el1, _el2);

                __m128 _mul_pattern = _mm_setr_ps(s, _1_o_s, _1_o_s, _1_o_s);
                __m128 _el_mul = _mm_mul_ps(_el_sub, _mul_pattern);

                return _el_mul;
            }
            else if (m.b2 > m.c3)
            {
                // Column 1:
                _type s = OP<_type>::sqrt(_mm_f32_(sum, 2)) * (_type)2;
                _type _1_o_s = 1.0f / s;

                __m128 _el1 = _mm_setr_ps(m.a2, 0.25f, m.b3, m.c1);
                __m128 _el2 = _mm_setr_ps(m.b1, 0.0f, m.c2, -m.a3);

                __m128 _el_sub = _mm_add_ps(_el1, _el2);

                __m128 _mul_pattern = _mm_setr_ps(_1_o_s, s, _1_o_s, _1_o_s);
                __m128 _el_mul = _mm_mul_ps(_el_sub, _mul_pattern);

                return _el_mul;
            }
            else
            {
                // Column 2:
                _type s = OP<_type>::sqrt(_mm_f32_(sum, 3)) * (_type)2;
                _type _1_o_s = 1.0f / s;

                __m128 _el1 = _mm_setr_ps(m.c1, m.b3, 0.25f, m.a2);
                __m128 _el2 = _mm_setr_ps(m.a3, m.c2, 0.0f, -m.b1);

                __m128 _el_sub = _mm_add_ps(_el1, _el2);

                __m128 _mul_pattern = _mm_setr_ps(_1_o_s, _1_o_s, s, _1_o_s);
                __m128 _el_mul = _mm_mul_ps(_el_sub, _mul_pattern);

                return _el_mul;
            }

#elif defined(ITK_NEON)
            type4 _1st(m.a1, m.a1, -m.a1, -m.a1);
            type4 _2nd(m.b2, -m.b2, m.b2, -m.b2);
            type4 _3nd(m.c3, -m.c3, -m.c3, m.c3);

            type4 sum_a = type4(1.0f) + _1st;
            type4 sum_b = _2nd + _3nd;

            type4 sum = sum_a + sum_b;

            // large enough
            if (sum.x > (_type)0.001)
            {
                _type s = OP<_type>::sqrt(sum.x) * (_type)2;
                _type _1_o_s = 1.0f / s;

                type4 _el1(m.b3, m.c1, m.a2, 0.25f);
                type4 _el2(m.c2, m.a3, m.b1, 0.0f);

                type4 _el_sub = _el1 - _el2;

                type4 _mul_pattern(_1_o_s, _1_o_s, _1_o_s, s);
                type4 _el_mul = _el_sub * _mul_pattern;

                return _el_mul.array_neon;

            } // else we have to check several cases
            else if (m.a1 > m.b2 && m.a1 > m.c3)
            {
                // Column 0:
                _type s = OP<_type>::sqrt(sum.y) * (_type)2;
                _type _1_o_s = 1.0f / s;

                type4 _el1(0.25f, m.a2, m.c1, m.b3);
                type4 _el2(0.0f, m.b1, m.a3, -m.c2);

                type4 _el_sub = _el1 + _el2;

                type4 _mul_pattern(s, _1_o_s, _1_o_s, _1_o_s);
                type4 _el_mul = _el_sub * _mul_pattern;

                return _el_mul.array_neon;
            }
            else if (m.b2 > m.c3)
            {
                // Column 1:
                _type s = OP<_type>::sqrt(sum.z) * (_type)2;
                _type _1_o_s = 1.0f / s;

                type4 _el1(m.a2, 0.25f, m.b3, m.c1);
                type4 _el2(m.b1, 0.0f, m.c2, -m.a3);

                type4 _el_sub = _el1 + _el2;

                type4 _mul_pattern(_1_o_s, s, _1_o_s, _1_o_s);
                type4 _el_mul = _el_sub * _mul_pattern;

                return _el_mul.array_neon;
            }
            else
            {
                // Column 2:
                _type s = OP<_type>::sqrt(sum.w) * (_type)2;
                _type _1_o_s = 1.0f / s;

                type4 _el1(m.c1, m.b3, 0.25f, m.a2);
                type4 _el2(m.a3, m.c2, 0.0f, -m.b1);

                type4 _el_sub = _el1 + _el2;

                type4 _mul_pattern(_1_o_s, _1_o_s, s, _1_o_s);
                type4 _el_mul = _el_sub * _mul_pattern;

                return _el_mul.array_neon;
            }
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
    };

}