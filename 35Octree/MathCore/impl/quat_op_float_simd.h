#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'quat_op_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "quat_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

#include "mat3_op.h"
#include "vec4_op_float_simd.h"

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<quat<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                  (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                   std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type,
              _algorithm>
    {
    private:
        using quatT = quat<_type, _simd>;
        using type2 = vec2<_type, _simd>;
        using type3 = vec3<_type, _simd>;
        using type4 = vec4<_type, _simd>;

        using typeMat3 = mat3<_type, _simd>;

        using self_type = OP<quatT>;

    public:
        /// \brief Constructs a conjugate quaternion
        ///
        /// The conjugate is (-x,-y,-z,w).
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat result = quatFromEuler( DEG2RAD(15.0f), DEG2RAD(0.0f), DEG2RAD(50.0f) ) ;
        ///
        /// // the conjugate is the inverse of a quaternion
        /// quat result_inv = conjugate( result );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Input quaternion
        /// \return The quat
        ///
        static __forceinline quatT conjugate(const quatT &a) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_xor_ps(_quat_conjugate_mask_sse, a.array_sse);
#elif defined(ITK_NEON)
            return vmulq_f32(a.array_neon, _quat_conjugate_sign);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Computes the dot product between two quaternions
        ///
        /// In the quaternion space, the dot can be used to <br />
        /// compute the angle between the two quaternions
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat a, b;
        /// float angle = acos( clamp( dot( a, b ), -1.0f, 1.0f ) ) * 2.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first vector
        /// \param b The second vector
        /// \return The dot product between the two quaternions
        ///
        static __forceinline _type dot(const quatT &a, const quatT &b) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_f32_(dot_sse_4(a.array_sse, b.array_sse), 0);
#elif defined(ITK_NEON)
            return dot_neon_4(a.array_neon, b.array_neon)[0];
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Make que quaterniom from parameter to become a unity quaternion
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat a,b,c;
        ///
        /// // 'c' may result in a non unit quaternion
        /// c = a ^ b;
        ///
        /// // make 'c' a unit quaternion
        /// c = normalize( c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param q The quaternion to normalize
        /// \return The normalized quaternion
        ///
        static __forceinline quatT normalize(const quatT &vec) noexcept
        {
#if defined(ITK_SSE2)
            __m128 mag2 = dot_sse_4(vec.array_sse, vec.array_sse);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(_mm_f32_(mag2, 0));
            __m128 magInv = _mm_set1_ps(mag2_rsqrt);
            return _mm_mul_ps(vec.array_sse, magInv);
#elif defined(ITK_NEON)
            float32x4_t mag2 = dot_neon_4(vec.array_neon, vec.array_neon);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(mag2[0]);
            float32x4_t magInv = vset1(mag2_rsqrt);
            return vmulq_f32(vec.array_neon, magInv);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Computes the angle in radians between two quaternions
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat a = quatFromEuler( DEG2RAD(30.0f), DEG2RAD(3.0f), DEG2RAD(20.0f) );
        /// quat b = quatFromEuler( DEG2RAD(90.0f), DEG2RAD(0.0f), DEG2RAD(0.0f) );
        ///
        /// float angle_radians = angleBetween(a, b);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first quat
        /// \param b The second quat
        /// \return The angle in radians between the two quaternions
        ///
        static __forceinline _type angleBetween(const quatT &a, const quatT &b) noexcept
        {
            /*
        There are three ways to implement quaternion minimum angle:

        MathCore::quatf b_to_a = aRibeiro::normalize(a) ^ aRibeiro::inv(aRibeiro::normalize(b));
        float way1 = acos(aRibeiro::absv(aRibeiro::clamp(b_to_a.w, -1.0f, 1.0f))) * 2.0f;

        float way2 = asin(aRibeiro::clamp(sqrt( b_to_a.x*b_to_a.x+ b_to_a.y*b_to_a.y + b_to_a.z*b_to_a.z ), -1.0f, 1.0f)) * 2.0f;

        float way3 = acos(aRibeiro::clamp(aRibeiro::absv(_dot(aRibeiro::normalize(a), aRibeiro::normalize(b))), -1.0f, 1.0f)) * 2.0f;

        */
#if defined(ITK_SSE2)
            __m128 dot0 = dot_sse_4(
                self_type::normalize(a).array_sse,
                self_type::normalize(b).array_sse);
            dot0 = _mm_andnot_ps(_vec4_sign_mask_sse, dot0);
            __m128 cosA = _mm_min_ps(dot0, _vec4_one_sse);
            return OP<_type>::acos(_mm_f32_(cosA, 0)) * 2.0f;

            // __m128 dot0 = dot_sse_4(
            //     OP<type3>::normalize(a.array_sse).array_sse,
            //     OP<type3>::normalize(b.array_sse).array_sse);
            // __m128 cosA = clamp_sse_4(dot0, _vec4_minus_one_sse, _vec4_one_sse);
            // return OP<_type>::acos(_mm_f32_(cosA, 0));
#elif defined(ITK_NEON)
            // float32x4_t dot0 = dot_neon_4(
            //     OP<type3>::normalize(a.array_neon).array_neon,
            //     OP<type3>::normalize(b.array_neon).array_neon);
            // float32x4_t cosA = clamp_neon_4(dot0, _vec4_minus_one, _vec4_one);
            // return OP<_type>::acos(cosA[0]);

            float32x4_t dot0 = dot_neon_4(
                self_type::normalize(a).array_neon,
                self_type::normalize(b).array_neon);
            dot0 = vabsq_f32(dot0);
            float32x4_t cosA = vminq_f32(dot0, _vec4_one);
            return OP<_type>::acos(cosA[0]) * 2.0f;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Compute the squared length of the quaternion
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat input;
        ///
        /// float result = sqrLength(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param q The quaternion
        /// \return The squared length
        ///
        static __forceinline _type sqrLength(const quatT &a) noexcept
        {
            return self_type::dot(a, a);
        }

        /// \brief Compute the length of the quaternion
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat input;
        ///
        /// float result = length(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param q The quaternion
        /// \return The length
        ///
        static __forceinline _type length(const quatT &a) noexcept
        {
            return OP<_type>::sqrt(self_type::dot(a, a));
        }

        /// \brief Compute the spherical linear interpolation between 2 quaternions.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat a = quatFromEuler( DEG2RAD(30.0f), DEG2RAD(3.0f), DEG2RAD(20.0f) );
        /// quat b = quatFromEuler( DEG2RAD(90.0f), DEG2RAD(0.0f), DEG2RAD(0.0f) );
        ///
        /// // 75% spherical interpolation from a to b
        /// quat result = slerp(a, b, 0.75f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Source quaternion a
        /// \param b Target quaternion b
        /// \param lerp the amount(%) of slerp to apply from a to b
        /// \return The spherical interpolation from a to b
        ///
        static __forceinline quatT slerp(const quatT &a, const quatT &b, const _type &_lerp)
        {
            _type lerp = OP<_type>::clamp(_lerp, (_type)0, (_type)1);

            _type _cos = OP<_type>::clamp( self_type::dot(a, b), (_type)-1, (_type)1);
            quatT _new_b_(b);

            _type sign_cos = OP<_type>::sign(_cos);

            // if (_cos < 0.0f)
            // {
            // 	_new_b_ = -b;
            // 	_cos = -_cos;
            // }

            //
            // Branch-less implementation
            //
            _new_b_ *= sign_cos;
            _cos *= sign_cos;

            _type a_factor, b_factor;

            // if (_cos > ((_type)1 - EPSILON<_type>::high_precision))
            // {
            //     a_factor = (_type)1 - lerp;
            //     b_factor = lerp;
            // }
            // else
            // {
            //     _type _sin = OP<_type>::sqrt(1.0f - _cos * _cos);
            //     _type _angle_rad = OP<_type>::atan2(_sin, _cos);
            //     _type _1_over_sin = (_type)1 / _sin;
            //     a_factor = OP<_type>::sin(((_type)1 - lerp) * _angle_rad) * _1_over_sin;
            //     b_factor = OP<_type>::sin((lerp)*_angle_rad) * _1_over_sin;
            // }

            //
            // Branch-less implementation
            //
            _type _sin = OP<_type>::sqrt((_type)1 - _cos * _cos); // sqrt faster than use sin calculation

            // avoid div by 0
            _type select = OP<_type>::step(_sin, EPSILON<_type>::high_precision);
            _sin = OP<_type>::maximum(_sin, FloatTypeInfo<_type>::min);

            //_type _angle_rad = OP<_type>::atan2(_sin, _cos);
            _type _angle_rad = OP<_type>::acos(_cos); // faster than atan2
            _type _1_over_sin = (_type)1 / _sin;
            a_factor = OP<_type>::sin(((_type)1 - lerp) * _angle_rad) * _1_over_sin;
            b_factor = OP<_type>::sin((lerp)*_angle_rad) * _1_over_sin;

            a_factor = OP<_type>::lerp(a_factor, (_type)1 - lerp, select);
            b_factor = OP<_type>::lerp(b_factor, lerp, select);

            // _type select = OP<float>::step(_cos, (_type)1 - EPSILON<_type>::high_precision);
            // a_factor = OP<_type>::lerp((_type)1 - lerp, a_factor, select);
            // b_factor = OP<_type>::lerp(lerp, b_factor, select);

#if defined(ITK_SSE2)
            __m128 aResult = _mm_mul_ps(a.array_sse, _mm_set1_ps(a_factor));
            __m128 _new_b_Result = _mm_mul_ps(_new_b_.array_sse, _mm_set1_ps(b_factor));
            return self_type::normalize(quatT(_mm_add_ps(aResult, _new_b_Result)));
#elif defined(ITK_NEON)
            float32x4_t aResult = vmulq_f32(a.array_neon, vset1(a_factor));
            float32x4_t _new_b_Result = vmulq_f32(_new_b_.array_neon, vset1(b_factor));
            return self_type::normalize(quatT(vaddq_f32(aResult, _new_b_Result)));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif

            // return self_type::normalize(quatT(
            //     a_factor * a.x + b_factor * _new_b_.x,
            //     a_factor * a.y + b_factor * _new_b_.y,
            //     a_factor * a.z + b_factor * _new_b_.z,
            //     a_factor * a.w + b_factor * _new_b_.w));
        }

        /// \brief Convert the quaternion to an axis angle representation. Notice: Not tested
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat rotation = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
        ///
        /// vec3 axis;
        /// float angle;
        ///
        /// extractAxisAngle(rotation, &axis, &angle);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param q The rotation quaternion
        /// \param axis Output - The axis
        /// \param angle Output - The angle arount the axis in radians
        ///
        static __forceinline void extractAxisAngle(const quatT &q, type3 *axis, _type *angle) noexcept
        {
#if defined(ITK_SSE2)
            __m128 dot0 = dot_sse_3(q.array_sse, q.array_sse);
            _type _rsqrt = OP<_type, void, _algorithm>::rsqrt(_mm_f32_(dot0, 0));
            __m128 inv_length = _mm_set1_ps(_rsqrt);
            __m128 result = _mm_mul_ps(q.array_sse, inv_length);
            axis->array_sse = result;
            *angle = OP<_type>::acos(OP<_type>::clamp(q.w, (_type)-1, (_type)1)) * (_type)2;
#elif defined(ITK_NEON)
            float32x4_t dot0 = dot_neon_3(q.array_neon, q.array_neon);
            _type _rsqrt = OP<_type, void, _algorithm>::rsqrt(dot0[0]);
            float32x4_t inv_length = vset1(_rsqrt);
            float32x4_t result = vmulq_f32(q.array_neon, inv_length);
            axis->array_neon = result;
            *angle = OP<_type>::acos(OP<_type>::clamp(q.w, (_type)-1, (_type)1)) * (_type)2;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            // _type scale_inv = OP<_type, void, _algorithm>::rsqrt(q.x * q.x + q.y * q.y + q.z * q.z);
            // axis->x = q.x * scale_inv;
            // axis->y = q.y * scale_inv;
            // axis->z = q.z * scale_inv;
            // *angle = OP<_type>::acos(OP<_type>::clamp(q.w, (_type)-1, (_type)1)) * (_type)2;
        }

        /// \brief Convert the quaternion to Euler representation. Notice: Not found an algorithm that works...
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat rotation = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
        ///
        /// float roll, pitch, yaw;
        ///
        /// extractEuler(rotation, &roll, &pitch, &yaw);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param q The rotation quaternion
        /// \param roll radians
        /// \param pitch radians
        /// \param yaw radians
        ///
        static __forceinline void extractEuler(const quatT &q, _type *roll, _type *pitch, _type *yaw) noexcept
        {
#if defined(ITK_SSE2)

            __m128 elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 0, 1));
            __m128 elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 2, 1, 1));
            __m128 op0 = _mm_mul_ps(elm0, elm1);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 3, 3, 2));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 2, 2));
            __m128 op1 = _mm_mul_ps(elm0, elm1);
            const __m128 mask01 = _mm_setr_ps(1.0f, 1.0f, -1.0f, 0.0f);
            op1 = _mm_mul_ps(mask01, op1);

            __m128 m0_ = _mm_add_ps(op0, op1);
            const __m128 mask02 = _mm_setr_ps(-2.0f, 2.0f, 2.0f, 0.0f);
            m0_ = _mm_mul_ps(mask02, m0_);
            // m0_[0] += 1.0f;
            m0_ = _mm_add_ps(m0_, _vec4_1000_sse);

            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 0, 0));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 2, 0, 1));
            op0 = _mm_mul_ps(elm0, elm1);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 3, 2, 3));
            __m128 elm1_s = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 2, 2));
            op1 = _mm_mul_ps(elm0, elm1_s);
            const __m128 mask03 = _mm_setr_ps(-1.0f, 1.0f, 1.0f, 0.0f);
            op1 = _mm_mul_ps(mask03, op1);

            __m128 m1_ = _mm_add_ps(op0, op1);
            const __m128 mask04 = _mm_setr_ps(2.0f, -2.0f, 2.0f, 0.0f);
            m1_ = _mm_mul_ps(mask04, m1_);
            // m1_[1] += 1.0f;
            m1_ = _mm_add_ps(m1_, _vec4_0100_sse);

            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 1, 0));
            // elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 2, 2));
            op0 = _mm_mul_ps(elm0, elm1_s);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 3, 3));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 0, 1));
            op1 = _mm_mul_ps(elm0, elm1);
            const __m128 mask05 = _mm_setr_ps(1.0f, -1.0f, 1.0f, 0.0f);
            op1 = _mm_mul_ps(mask05, op1);

            __m128 m2_ = _mm_add_ps(op0, op1);
            const __m128 mask06 = _mm_setr_ps(2.0f, 2.0f, -2.0f, 0.0f);
            m2_ = _mm_mul_ps(mask06, m2_);
            // m2_[2] += 1.0f;
            m2_ = _mm_add_ps(m2_, _vec4_0010_sse);

            //__m128 m3_ = (__m128){0,0,0,1.0f};

            typeMat3 m = typeMat3(m0_, m1_, m2_);
#elif defined(ITK_NEON)

            float32x4_t x2y2z2 = vmulq_f32(q.array_neon, q.array_neon);

            float32x4_t op0 = vshuffle_0100(q.array_neon);
            float32x4_t op1 = vshuffle_0221(q.array_neon);

            float32x4_t xy_xz_yz = vmulq_f32(op0, op1);
            float32x4_t wx_wy_wz = vmulq_f32(vshuffle_3333(q.array_neon), q.array_neon);

            const float32x4_t _2 = vset1(2.0f);

            float32x4_t m0 =
                vmulq_f32(_2, (float32x4_t){
                                  (x2y2z2[1] + x2y2z2[2]),
                                  (xy_xz_yz[0] + wx_wy_wz[2]),
                                  (xy_xz_yz[1] - wx_wy_wz[1]),
                                  0});

            m0[0] = 1.0f - m0[0];

            float32x4_t m1 =
                vmulq_f32(_2, (float32x4_t){
                                  (xy_xz_yz[0] - wx_wy_wz[2]),
                                  (x2y2z2[0] + x2y2z2[2]),
                                  (xy_xz_yz[2] + wx_wy_wz[0]),
                                  0});
            m1[1] = 1.0f - m1[1];

            float32x4_t m2 =
                vmulq_f32(_2, (float32x4_t){
                                  (xy_xz_yz[1] + wx_wy_wz[1]),
                                  (xy_xz_yz[2] - wx_wy_wz[0]),
                                  (x2y2z2[0] + x2y2z2[1]),
                                  0});

            m2[2] = 1.0f - m2[2];

            // const float32x4_t m3 = (float32x4_t){0,0,0,1.0f};

            typeMat3 m = typeMat3(m0, m1, m2);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif

            // //
            // // CVT<quatT>::toMat3()
            // //
            // _type x2 = q.x * q.x;
            // _type y2 = q.y * q.y;
            // _type z2 = q.z * q.z;
            // _type xy = q.x * q.y;
            // _type xz = q.x * q.z;
            // _type yz = q.y * q.z;
            // _type wx = q.w * q.x;
            // _type wy = q.w * q.y;
            // _type wz = q.w * q.z;

            // // This calculation would be a lot more complicated for non-unit length quaternions
            // // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
            // //   OpenGL
            // typeMat3 m = typeMat3((_type)1 - (_type)2 * (y2 + z2), (_type)2 * (xy - wz), (_type)2 * (xz + wy),
            //                       (_type)2 * (xy + wz), (_type)1 - (_type)2 * (x2 + z2), (_type)2 * (yz - wx),
            //                       (_type)2 * (xz - wy), (_type)2 * (yz + wx), (_type)1 - (_type)2 * (x2 + y2));

            OP<typeMat3>::extractEuler(m, roll, pitch, yaw);
        }

        /// \brief Computes the inverse of a quaternion. Notice
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat rotation = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
        ///
        /// quat rotation_inv = inv( rotation );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param q The rotation quaternion
        /// \return The inverse of the quaternion
        ///
        static __forceinline quatT inverse(const quatT &q) noexcept
        {
            return self_type::conjugate(q);
        }

        /// \brief Move from current to target, considering the max variation in angle.
        ///
        /// This function could be used as a constant angular motion interpolation<br />
        /// between two values considering the delta time and max angle speed variation.
        ///
        /// Uses the spherical linear interpolation function.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// PlatformTime timer;
        /// float moveSpeedAngle;
        /// quat current;
        /// quat target;
        ///
        /// {
        ///     timer.update();
        ///     ...
        ///     // current will be modified to be the target,
        ///     // but the delta time and move speed will make
        ///     // this transition smoother.
        ///     current = moveSlerp( current, target, time.deltaTime * moveSpeedAngle );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param currentParam The current state
        /// \param target The target state
        /// \param maxAngleVariation The max amount the current can be modified to reach target
        /// \return the slerp from current to target according max variation
        ///
        static __forceinline quatT moveSlerp(const quatT &currentParam, const quatT &target, const _type &maxAngleVariation) noexcept
        {
            quatT current = currentParam;

            // const float EPSILON = 1e-6f;
            _type deltaAngle = self_type::angleBetween(current, target);
            // if (deltaAngle < maxAngleVariation + EPSILON)
            // 	return target;
            deltaAngle = OP<_type>::maximum(deltaAngle, maxAngleVariation);
            deltaAngle = OP<_type>::maximum(deltaAngle, FloatTypeInfo<_type>::min);

            // 180º case interpolation -- force one orientation based on eulerAngle
            // if (deltaAngle >= CONSTANT<_type>::PI - EPSILON<_type>::high_precision)
            // if (deltaAngle >= CONSTANT<quatT>::_180_move_slerp_case_deltaAngle)
            // {
            //     current = CONSTANT<quatT>::_180_move_slerp_case() * current;
            //     deltaAngle = CONSTANT<quatT>::_180_move_slerp_case_deltaAngle;//OP<type3>::angleBetween(current, target);
            // }

            // branch-less implementation
            _type select_delta_angle = OP<_type>::step(CONSTANT<quatT>::_180_move_slerp_case_deltaAngle, deltaAngle);
            quatT current_180_ = CONSTANT<quatT>::_180_move_slerp_case() * current;

#if defined(ITK_SSE2)
            current.array_sse = OP<type4>::lerp(current.array_sse, current_180_.array_sse, select_delta_angle).array_sse;
#elif defined(ITK_NEON)
            current.array_neon = OP<type4>::lerp(current.array_neon, current_180_.array_neon, select_delta_angle).array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            // current.x = OP<_type>::lerp(current.x, current_180_.x, select_delta_angle);
            // current.y = OP<_type>::lerp(current.y, current_180_.y, select_delta_angle);
            // current.z = OP<_type>::lerp(current.z, current_180_.z, select_delta_angle);
            // current.w = OP<_type>::lerp(current.w, current_180_.w, select_delta_angle);

            deltaAngle = OP<_type>::lerp(deltaAngle, CONSTANT<quatT>::_180_move_slerp_case_deltaAngle, select_delta_angle);

            return self_type::slerp(current, target, maxAngleVariation / deltaAngle);
        }
    };

}