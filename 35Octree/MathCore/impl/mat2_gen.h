#pragma once

#include "mat2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd>
    struct GEN<mat2<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_simd, SIMD_TYPE::NONE>::value>::type>
    {
        private:
        using typeMat2 = mat2<_type, _simd>;
        using typeMat3 = mat3<_type, _simd>;
        using typeMat4 = mat4<_type, _simd>;
        using typeVec4 = vec4<_type, _simd>;
        using typeVec3 = vec3<_type, _simd>;
        using typeVec2 = vec2<_type, _simd>;
        using self_type = GEN<typeMat2>;
        public:

        static __forceinline typeMat2 scale(const _type &_x_, const _type &_y_) noexcept
        {
            return typeMat2(
                _x_, 0,
                0, _y_);
        }

        static __forceinline typeMat2 scale(const typeVec2 &_v_) noexcept
        {
            return typeMat2(
                _v_.x, 0,
                0, _v_.y);
        }

        static __forceinline typeMat2 scale(const typeVec3 &_v_) noexcept
        {
            return typeMat2(
                _v_.x, 0,
                0, _v_.y);
        }

        static __forceinline typeMat2 scale(const typeVec4 &_v_) noexcept
        {
            return typeMat4(
                _v_.x, 0,
                0, _v_.y);
        }

        static __forceinline typeMat2 rotate(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
            return typeMat2(
                c, -s,
                s, c);
        }


        static __forceinline typeMat2 fromMat3(const typeMat3 &v) noexcept
        {
            return typeMat2(
                *(const typeVec2 *)&(v[0]),
                *(const typeVec2 *)&(v[1]));
        }

        static __forceinline typeMat2 fromMat4(const typeMat4 &v) noexcept
        {
            return typeMat2(
                *(const typeVec2 *)&(v[0]),
                *(const typeVec2 *)&(v[1]));
        }

    };

}