#pragma once

#include "quat_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

#include "../op.h"

namespace MathCore
{

    template <typename _type, typename _simd>
    struct GEN<quat<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_simd, SIMD_TYPE::NONE>::value>::type>
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

        /// \brief Convert a vec4 to a unity quaternion pointing to the vec3 axis.
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
            return quatT((vn.x * sinAngle),
                         (vn.y * sinAngle),
                         (vn.z * sinAngle),
                         OP<_type>::cos(angle));
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
        /// \return The quat
        ///
        static __forceinline quatT fromEuler(_type roll, _type pitch, _type yaw) noexcept
        {

            pitch *= (_type)0.5;
            yaw *= (_type)0.5;
            roll *= (_type)0.5;

            _type sinPitch = OP<_type>::sin(pitch);
            _type cosPitch = OP<_type>::cos(pitch);
            _type sinYaw = OP<_type>::sin(yaw);
            _type cosYaw = OP<_type>::cos(yaw);
            _type sinRoll = OP<_type>::sin(roll);
            _type cosRoll = OP<_type>::cos(roll);

            _type cosPitchCosYaw = cosPitch * cosYaw;
            _type sinPitchSinYaw = sinPitch * sinYaw;

            return quatT(
                sinRoll * cosPitchCosYaw - cosRoll * sinPitchSinYaw,
                cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
                cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
                cosRoll * cosPitchCosYaw + sinRoll * sinPitchSinYaw);
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

            _type t = (_type)1 + m.a1 + m.b2 + m.c3;
            // large enough
            if (t > (_type)0.001)
            {
                _type s = OP<_type>::sqrt(t) * (_type)2;
                return quatT((m.b3 - m.c2) / s,
                             (m.c1 - m.a3) / s,
                             (m.a2 - m.b1) / s,
                             (_type)0.25 * s);
            } // else we have to check several cases
            else if (m.a1 > m.b2 && m.a1 > m.c3)
            {
                // Column 0:
                _type s = OP<_type>::sqrt((_type)1 + m.a1 - m.b2 - m.c3) * (_type)2;
                return quatT((_type)0.25 * s,
                             (m.a2 + m.b1) / s,
                             (m.c1 + m.a3) / s,
                             (m.b3 - m.c2) / s);
            }
            else if (m.b2 > m.c3)
            {
                // Column 1:
                _type s = OP<_type>::sqrt((_type)1 + m.b2 - m.a1 - m.c3) * (_type)2;
                return quatT((m.a2 + m.b1) / s,
                             (_type)0.25 * s,
                             (m.b3 + m.c2) / s,
                             (m.c1 - m.a3) / s);
            }
            else
            {
                // Column 2:
                _type s = OP<_type>::sqrt((_type)1 + m.c3 - m.a1 - m.b2) * (_type)2;
                return quatT((m.c1 + m.a3) / s,
                             (m.b3 + m.c2) / s,
                             (_type)0.25 * s,
                             (m.a2 - m.b1) / s);
            }
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

            _type t = (_type)1 + m.a1 + m.b2 + m.c3;
            // large enough
            if (t > (_type)0.001)
            {
                _type s = OP<_type>::sqrt(t) * (_type)2;
                return quatT((m.b3 - m.c2) / s,
                             (m.c1 - m.a3) / s,
                             (m.a2 - m.b1) / s,
                             (_type)0.25 * s);
            } // else we have to check several cases
            else if (m.a1 > m.b2 && m.a1 > m.c3)
            {
                // Column 0:
                _type s = OP<_type>::sqrt((_type)1 + m.a1 - m.b2 - m.c3) * (_type)2;
                return quatT((_type)0.25 * s,
                             (m.a2 + m.b1) / s,
                             (m.c1 + m.a3) / s,
                             (m.b3 - m.c2) / s);
            }
            else if (m.b2 > m.c3)
            {
                // Column 1:
                _type s = OP<_type>::sqrt((_type)1 + m.b2 - m.a1 - m.c3) * (_type)2;
                return quatT((m.a2 + m.b1) / s,
                             (_type)0.25 * s,
                             (m.b3 + m.c2) / s,
                             (m.c1 - m.a3) / s);
            }
            else
            {
                // Column 2:
                _type s = OP<_type>::sqrt((_type)1 + m.c3 - m.a1 - m.b2) * (_type)2;
                return quatT((m.c1 + m.a3) / s,
                             (m.b3 + m.c2) / s,
                             (_type)0.25 * s,
                             (m.a2 - m.b1) / s);
            }
        }
    };

}