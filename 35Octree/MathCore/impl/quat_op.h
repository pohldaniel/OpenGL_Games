#pragma once

#include "quat_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

#include "mat3_op.h"

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<quat<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value>::type,
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
            return quatT(-a.x, -a.y, -a.z, a.w);
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
            return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
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
            _type mag2 = self_type::dot(vec, vec);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(mag2);
            return quatT(vec.x * mag2_rsqrt,
                         vec.y * mag2_rsqrt,
                         vec.z * mag2_rsqrt,
                         vec.w * mag2_rsqrt);
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
            _type cosA = OP<_type>::minimum(
                OP<_type>::abs(
                    self_type::dot(self_type::normalize(a), self_type::normalize(b))),
                (_type)1);
            return OP<_type>::acos(cosA) * (_type)2;
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
            // a_factor = OP<_type>::lerp((_type)1-lerp, a_factor, select);
            // b_factor = OP<_type>::lerp(lerp, b_factor, select);

            return self_type::normalize(quatT(
                a_factor * a.x + b_factor * _new_b_.x,
                a_factor * a.y + b_factor * _new_b_.y,
                a_factor * a.z + b_factor * _new_b_.z,
                a_factor * a.w + b_factor * _new_b_.w));
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
            _type scale_inv = OP<_type, void, _algorithm>::rsqrt(q.x * q.x + q.y * q.y + q.z * q.z);
            axis->x = q.x * scale_inv;
            axis->y = q.y * scale_inv;
            axis->z = q.z * scale_inv;
            *angle = OP<_type>::acos(OP<_type>::clamp(q.w, (_type)-1, (_type)1)) * (_type)2;
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

            //
            // CVT<quatT>::toMat3()
            //
            _type x2 = q.x * q.x;
            _type y2 = q.y * q.y;
            _type z2 = q.z * q.z;
            _type xy = q.x * q.y;
            _type xz = q.x * q.z;
            _type yz = q.y * q.z;
            _type wx = q.w * q.x;
            _type wy = q.w * q.y;
            _type wz = q.w * q.z;

            // This calculation would be a lot more complicated for non-unit length quaternions
            // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
            //   OpenGL
            typeMat3 m = typeMat3((_type)1 - (_type)2 * (y2 + z2), (_type)2 * (xy - wz), (_type)2 * (xz + wy),
                                  (_type)2 * (xy + wz), (_type)1 - (_type)2 * (x2 + z2), (_type)2 * (yz - wx),
                                  (_type)2 * (xz - wy), (_type)2 * (yz + wx), (_type)1 - (_type)2 * (x2 + y2));

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
            current.x = OP<_type>::lerp(current.x, current_180_.x, select_delta_angle);
            current.y = OP<_type>::lerp(current.y, current_180_.y, select_delta_angle);
            current.z = OP<_type>::lerp(current.z, current_180_.z, select_delta_angle);
            current.w = OP<_type>::lerp(current.w, current_180_.w, select_delta_angle);
            deltaAngle = OP<_type>::lerp(deltaAngle, CONSTANT<quatT>::_180_move_slerp_case_deltaAngle, select_delta_angle);

            return self_type::slerp(current, target, maxAngleVariation / deltaAngle);
        }
    };

}