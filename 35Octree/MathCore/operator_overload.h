#pragma once

#include "math_common.h"

#include "int.h"
#include "float.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#include "quat.h"

#include "mat2.h"
#include "mat3.h"
#include "mat4.h"

namespace MathCore
{

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec4<_type, _simd> operator*(const mat4<_type, _simd> &mat, const vec4<_type, _simd> &vec) noexcept
    {
        vec4<_type, _simd> result;
        result.x = mat.a1 * vec.x + mat.b1 * vec.y + mat.c1 * vec.z + mat.d1 * vec.w;
        result.y = mat.a2 * vec.x + mat.b2 * vec.y + mat.c2 * vec.z + mat.d2 * vec.w;
        result.z = mat.a3 * vec.x + mat.b3 * vec.y + mat.c3 * vec.z + mat.d3 * vec.w;
        result.w = mat.a4 * vec.x + mat.b4 * vec.y + mat.c4 * vec.z + mat.d4 * vec.w;
        return result;
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec4<_type, _simd> operator*(const vec4<_type, _simd> &vec, const mat4<_type, _simd> &mat) noexcept
    {
        vec4<_type, _simd> result;
        result.x = mat.a1 * vec.x + mat.a2 * vec.y + mat.a3 * vec.z + mat.a4 * vec.w;
        result.y = mat.b1 * vec.x + mat.b2 * vec.y + mat.b3 * vec.z + mat.b4 * vec.w;
        result.z = mat.c1 * vec.x + mat.c2 * vec.y + mat.c3 * vec.z + mat.c4 * vec.w;
        result.w = mat.d1 * vec.x + mat.d2 * vec.y + mat.d3 * vec.z + mat.d4 * vec.w;
        return result;
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec3<_type, _simd> operator*(const mat3<_type, _simd> &mat, const vec3<_type, _simd> &vec) noexcept
    {
        vec3<_type, _simd> result;
        result.x = mat.a1 * vec.x + mat.b1 * vec.y + mat.c1 * vec.z;
        result.y = mat.a2 * vec.x + mat.b2 * vec.y + mat.c2 * vec.z;
        result.z = mat.a3 * vec.x + mat.b3 * vec.y + mat.c3 * vec.z;
        return result;
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec3<_type, _simd> operator*(const vec3<_type, _simd> &vec, const mat3<_type, _simd> &mat) noexcept
    {
        vec3<_type, _simd> result;
        result.x = mat.a1 * vec.x + mat.a2 * vec.y + mat.a3 * vec.z;
        result.y = mat.b1 * vec.x + mat.b2 * vec.y + mat.b3 * vec.z;
        result.z = mat.c1 * vec.x + mat.c2 * vec.y + mat.c3 * vec.z;
        return result;
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec2<_type, _simd> operator*(const mat2<_type, _simd> &mat, const vec2<_type, _simd> &vec) noexcept
    {
        vec2<_type, _simd> result;
        result.x = mat.a1 * vec.x + mat.b1 * vec.y;
        result.y = mat.a2 * vec.x + mat.b2 * vec.y;
        return result;
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec2<_type, _simd> operator*(const vec2<_type, _simd> &vec, const mat2<_type, _simd> &mat) noexcept
    {
        vec2<_type, _simd> result;
        result.x = mat.a1 * vec.x + mat.a2 * vec.y;
        result.y = mat.b1 * vec.x + mat.b2 * vec.y;
        return result;
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline quat<_type, _simd> operator^(const quat<_type, _simd> &a, const quat<_type, _simd> &b) noexcept
    {
        return quat<_type, _simd>(
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
            a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline quat<_type, _simd> operator*(const quat<_type, _simd> &a, const quat<_type, _simd> &b) noexcept {
        return OP<quat<_type, _simd>>::normalize(a ^ b);
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec3<_type, _simd> operator*(const quat<_type, _simd> &a, const vec3<_type, _simd> &v) noexcept
    {
        //
        // non normalized multiplication of the quaternion
        //
        quat<_type, _simd> result = a ^ quat<_type, _simd>(v.x, v.y, v.z, 0) ^ OP<quat<_type, _simd>>::conjugate(a);
        //return vec3(result.x, result.y, result.z);
        return *(vec3<_type, _simd>*)&result;
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec4<_type, _simd> operator*(const quat<_type, _simd> &a, const vec4<_type, _simd> &v) noexcept
    {
        //
        // non normalized multiplication of the quaternion
        //
        quat<_type, _simd> result = a ^ quat<_type, _simd>(v.x, v.y, v.z, 0) ^ OP<quat<_type, _simd>>::conjugate(a);
        result.w = v.w;
        //return vec3(result.x, result.y, result.z);
        return *(vec4<_type, _simd>*)&result;
    }


    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec3<_type, _simd> operator*(const vec3<_type, _simd> &v, const quat<_type, _simd> &a) noexcept
    {
        //
        // non normalized multiplication of the quaternion
        //
        quat<_type, _simd> result = OP<quat<_type, _simd>>::conjugate(a) ^ quat<_type, _simd>(v.x, v.y, v.z, 0) ^ a;
        //return vec3(result.x, result.y, result.z);
        return *(vec3<_type, _simd>*)&result;
    }

    template <typename _type, typename _simd,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value,
                  bool>::type = true>
    static __forceinline vec4<_type, _simd> operator*(const vec4<_type, _simd> &v, const quat<_type, _simd> &a) noexcept
    {
        //
        // non normalized multiplication of the quaternion
        //
        quat<_type, _simd> result = OP<quat<_type, _simd>>::conjugate(a) ^ quat<_type, _simd>(v.x, v.y, v.z, 0) ^ a;
        result.w = v.w;
        //return vec3(result.x, result.y, result.z);
        return *(vec4<_type, _simd>*)&result;
    }

    

}


#if defined(ITK_SSE2) || defined(ITK_NEON)

    #include "operator_overload_float_simd.h"

#endif