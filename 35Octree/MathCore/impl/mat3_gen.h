#pragma once

#include "mat3_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd>
    struct GEN<mat3<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_simd, SIMD_TYPE::NONE>::value>::type>
    {
        private:
        using typeMat3 = mat3<_type, _simd>;
        using typeMat2 = mat2<_type, _simd>;
        using typeMat4 = mat4<_type, _simd>;
        using typeVec4 = vec4<_type, _simd>;
        using typeVec3 = vec3<_type, _simd>;
        using typeVec2 = vec2<_type, _simd>;
        using quatT = quat<_type, _simd>;
        using self_type = GEN<typeMat3>;
        public:

        static __forceinline typeMat3 translateHomogeneous(const _type &_x_, const _type &_y_) noexcept
        {
            return typeMat3(
                1, 0, _x_,
                0, 1, _y_,
                0, 0, 1);
        }

        static __forceinline typeMat3 translateHomogeneous(const typeVec2 &_v_) noexcept
        {
            return typeMat3(
                1, 0, _v_.x,
                0, 1, _v_.y,
                0, 0, 1);
        }

        static __forceinline typeMat3 translateHomogeneous(const typeVec3 &_v_) noexcept
        {
            return typeMat3(
                1, 0, _v_.x,
                0, 1, _v_.y,
                0, 0, 1);
        }

        static __forceinline typeMat3 translateHomogeneous(const typeVec4 &_v_) noexcept
        {
            return typeMat3(
                1, 0, _v_.x,
                0, 1, _v_.y,
                0, 0, 1);
        }

        static __forceinline typeMat3 scaleHomogeneous(const _type &_x_, const _type &_y_) noexcept
        {
            return typeMat3(
                _x_, 0, 0,
                0, _y_, 0,
                0, 0, 1);
        }

        static __forceinline typeMat3 scaleHomogeneous(const typeVec2 &_v_) noexcept
        {
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, 1);
        }

        static __forceinline typeMat3 scaleHomogeneous(const typeVec3 &_v_) noexcept
        {
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, 1);
        }

        static __forceinline typeMat3 scaleHomogeneous(const typeVec4 &_v_) noexcept
        {
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, 1);
        }

        static __forceinline typeMat3 zRotateHomogeneous(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
            return typeMat3(
                c, -s, 0,
                s, c, 0,
                0, 0, 1);
        }

        static __forceinline typeMat3 scale(const _type &_x_, const _type &_y_, const _type &_z_ = (_type)1) noexcept
        {
            return typeMat4(
                _x_, 0, 0,
                0, _y_, 0,
                0, 0, _z_);
        }

        static __forceinline typeMat3 scale(const typeVec2 &_v_) noexcept
        {
            return typeMat4(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, 1);
        }

        static __forceinline typeMat3 scale(const typeVec3 &_v_) noexcept
        {
            return typeMat4(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, _v_.z);
        }

        static __forceinline typeMat3 scale(const typeVec4 &_v_) noexcept
        {
            return typeMat4(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, _v_.z);
        }

        static __forceinline typeMat3 xRotate(const _type &_phi_) noexcept
        {
            _type c = OP<_type>::cos(_phi_);
            _type s = OP<_type>::sin(_phi_);
            return typeMat3(
                1, 0, 0,
                0, c, -s,
                0, s, c);
        }

        static __forceinline typeMat3 yRotate(const _type &_theta_) noexcept
        {
            _type c = OP<_type>::cos(_theta_);
            _type s = OP<_type>::sin(_theta_);
            return typeMat3(
                c, 0, s,
                0, 1, 0,
                -s, 0, c);
        }

        static __forceinline typeMat3 zRotate(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
            return typeMat3(
                c, -s, 0,
                s, c, 0,
                0, 0, 1);
        }

        static __forceinline typeMat3 fromEuler(const _type &roll, const _type &pitch, const _type &yaw) noexcept
        {
            return self_type::zRotate(yaw) * self_type::yRotate(pitch) * self_type::xRotate(roll);
        }

        static __forceinline typeMat3 rotate(const _type &_ang_, const _type &_x, const _type &_y, const _type &_z) noexcept
        {
            _type x = _x;
            _type y = _y;
            _type z = _z;

            using type_info = FloatTypeInfo<_type>;
            // depois tem como otimizar esta rotação
            _type length_inv = x * x + y * y + z * z;
            length_inv = OP<_type>::sqrt(length_inv);
            length_inv = OP<_type>::maximum(length_inv, type_info::min);
            // ARIBEIRO_ABORT(length_inv == 0, "division by zero\n");
            length_inv = (_type)1 / length_inv;

            x *= length_inv;
            y *= length_inv;
            z *= length_inv;

            _type c = OP<_type>::cos(_ang_);
            _type s = OP<_type>::sin(_ang_);

            _type _1_m_c = (_type)1 - c;
            // original -- rotacao em sentido anti-horario
            return typeMat3(x * x * _1_m_c + c, x * y * _1_m_c - z * s, x * z * _1_m_c + y * s,
                            y * x * _1_m_c + z * s, y * y * _1_m_c + c, y * z * _1_m_c - x * s,
                            x * z * _1_m_c - y * s, y * z * _1_m_c + x * s, z * z * _1_m_c + c);

            // transposto -- rotacao em sentido horario
            //   return  mat4(x*x*(1-c)+c  ,  y*x*(1-c)+z*s  ,  x*z*(1-c)-y*s,   0  ,
            //                x*y*(1-c)-z*s,  y*y*(1-c)+c    ,  y*z*(1-c)+x*s,   0  ,
            //                x*z*(1-c)+y*s,  y*z*(1-c)-x*s  ,  z*z*(1-c)+c  ,   0  ,
            //                    0        ,        0        ,      0        ,   1  );
        }

        static __forceinline typeMat3 rotate(const _type &_ang_, const typeVec2 &axis) noexcept
        {
            return self_type::rotate(_ang_, axis.x, axis.y, 0);
        }

        static __forceinline typeMat3 rotate(const _type &_ang_, const typeVec3 &axis) noexcept
        {
            return self_type::rotate(_ang_, axis.x, axis.y, axis.z);
        }

        static __forceinline typeMat3 rotate(const _type &_ang_, const typeVec4 &axis) noexcept
        {
            return self_type::rotate(_ang_, axis.x, axis.y, axis.z);
        }

        static __forceinline typeMat3 lookAtRotationRH(const typeVec2 &_front, const typeVec2 &position) noexcept
        {
            typeVec2 front = OP<typeVec2>::normalize(_front);
            typeVec2 side = OP<typeVec2>::cross_z_up(front);

            typeVec3 x, y, z;
            z = typeVec3(position, 1);
            x = typeVec3(front, 0) * -1;
            y = typeVec3(side, 0);

            return typeMat3(x, y, z);
        }

        static __forceinline typeMat3 lookAtRotationLH(const typeVec3 &_front, const typeVec2 &position) noexcept
        {
            typeVec2 front = OP<typeVec2>::normalize(_front);
            typeVec2 side = OP<typeVec2>::cross_z_up(front);

            typeVec3 x, y, z;
            z = typeVec3(position, 1);
            x = typeVec3(front, 0);
            y = typeVec3(side, 0);

            return typeMat3(x, y, z);
        }

        static __forceinline typeMat3 lookAtRotationRH(const typeVec3 &front, const typeVec3 &up) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(lookTo) * -1;
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat3(x, y, z);
        }

        static __forceinline typeMat3 lookAtRotationLH(const typeVec3 &front, const typeVec3 &up) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat3(x, y, z);
        }

        static __forceinline typeMat3 lookAtRotationRH(const typeVec4 &front, const typeVec4 &up) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(*(const typeVec3 *)&lookTo) * -1;
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(*(const typeVec3 *)&up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat3(x, y, z);
        }

        static __forceinline typeMat3 lookAtRotationLH(const typeVec4 &front, const typeVec4 &up) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(*(const typeVec3 *)&lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(*(const typeVec3 *)&up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat3(x, y, z);
        }

        static __forceinline typeMat3 fromQuat(const quatT &q) noexcept
        {
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
            return typeMat3((_type)1 - (_type)2 * (y2 + z2), (_type)2 * (xy - wz), (_type)2 * (xz + wy),
                            (_type)2 * (xy + wz), (_type)1 - (_type)2 * (x2 + z2), (_type)2 * (yz - wx),
                            (_type)2 * (xz - wy), (_type)2 * (yz + wx), (_type)1 - (_type)2 * (x2 + y2));
        }

        static __forceinline typeMat3 fromMat2(const typeMat2 &v) noexcept
        {
            return typeMat3(
                typeVec3(v[0], 0),
                typeVec3(v[1], 0),
                typeVec3(0, 0, (_type)1));
        }

        static __forceinline typeMat3 fromMat4(const typeMat4 &v) noexcept
        {
            return typeMat3(
                *(const typeVec3 *)&(v[0]),
                *(const typeVec3 *)&(v[1]),
                *(const typeVec3 *)&(v[2]));
        }

    };

}