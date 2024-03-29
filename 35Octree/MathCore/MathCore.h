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

#include "operator_overload.h"

#include "op.h"

#include "gen.h"

#include "cvt.h"

#include "util.h"

//
// Alias namespace
//
namespace MathCore
{
    namespace Alias
    {

        template <typename _type>
        static constexpr __forceinline _type absv(const _type &v) noexcept
        {
            return OP<_type>::abs(v);
        }

    }

    namespace MathTypeClass
    {
        struct _class_vec2{};
        struct _class_vec3{};
        struct _class_vec4{};
        struct _class_quat{};
        struct _class_mat2{};
        struct _class_mat3{};
        struct _class_mat4{};
    };

    template <typename _single_element_>
    struct MathDefaultType;

    template <>
    struct MathDefaultType<float>
    {
        using vec2 = vec2f;
        using vec3 = vec3f;
        using vec4 = vec4f;
        using quat = quatf;
        using mat2 = mat2f;
        using mat3 = mat3f;
        using mat4 = mat4f;
    };

    template <>
    struct MathDefaultType<double>
    {
        using vec2 = vec2d;
        using vec3 = vec3d;
        using vec4 = vec4d;
        using quat = quatd;
        using mat2 = mat2d;
        using mat3 = mat3d;
        using mat4 = mat4d;
    };

    // template type instrospection
    template <typename _single_element_>
    struct MathTypeInfo{
        //static constexpr bool _is_valid = false;
        using _is_valid = std::false_type;
        using _is_vec = std::false_type;
        using _type = _single_element_;

    };

    template <typename __type, typename __simd>
    struct MathTypeInfo<vec2<__type, __simd>>
    {
        using _is_valid = std::true_type;
        using _class = MathTypeClass::_class_vec2;
        using _is_vec = std::true_type;
        using _type = __type;
        using _simd = __simd;

        // compatible type definition
        using _vec2 = vec2<__type, __simd>;
        using _vec3 = vec3<__type, __simd>;
        using _vec4 = vec4<__type, __simd>;
        using _quat = quat<__type, __simd>;
        using _mat2 = mat2<__type, __simd>;
        using _mat3 = mat3<__type, __simd>;
        using _mat4 = mat4<__type, __simd>;
    };

    template <typename __type, typename __simd>
    struct MathTypeInfo<vec3<__type, __simd>>
    {
        using _is_valid = std::true_type;
        using _class = MathTypeClass::_class_vec3;
        using _is_vec = std::true_type;
        using _type = __type;
        using _simd = __simd;

        // compatible type definition
        using _vec2 = vec2<__type, __simd>;
        using _vec3 = vec3<__type, __simd>;
        using _vec4 = vec4<__type, __simd>;
        using _quat = quat<__type, __simd>;
        using _mat2 = mat2<__type, __simd>;
        using _mat3 = mat3<__type, __simd>;
        using _mat4 = mat4<__type, __simd>;
    };

    template <typename __type, typename __simd>
    struct MathTypeInfo<vec4<__type, __simd>>
    {
        using _is_valid = std::true_type;
        using _class = MathTypeClass::_class_vec4;
        using _is_vec = std::true_type;
        using _type = __type;
        using _simd = __simd;

        // compatible type definition
        using _vec2 = vec2<__type, __simd>;
        using _vec3 = vec3<__type, __simd>;
        using _vec4 = vec4<__type, __simd>;
        using _quat = quat<__type, __simd>;
        using _mat2 = mat2<__type, __simd>;
        using _mat3 = mat3<__type, __simd>;
        using _mat4 = mat4<__type, __simd>;
    };

    template <typename __type, typename __simd>
    struct MathTypeInfo<quat<__type, __simd>>
    {
        using _is_valid = std::true_type;
        using _class = MathTypeClass::_class_quat;
        using _is_vec = std::true_type;
        using _type = __type;
        using _simd = __simd;

        // compatible type definition
        using _vec2 = vec2<__type, __simd>;
        using _vec3 = vec3<__type, __simd>;
        using _vec4 = vec4<__type, __simd>;
        using _quat = quat<__type, __simd>;
        using _mat2 = mat2<__type, __simd>;
        using _mat3 = mat3<__type, __simd>;
        using _mat4 = mat4<__type, __simd>;
    };

    template <typename __type, typename __simd>
    struct MathTypeInfo<mat2<__type, __simd>>
    {
        using _is_valid = std::true_type;
        using _class = MathTypeClass::_class_mat2;
        using _is_vec = std::false_type;
        using _type = __type;
        using _simd = __simd;

        // compatible type definition
        using _vec2 = vec2<__type, __simd>;
        using _vec3 = vec3<__type, __simd>;
        using _vec4 = vec4<__type, __simd>;
        using _quat = quat<__type, __simd>;
        using _mat2 = mat2<__type, __simd>;
        using _mat3 = mat3<__type, __simd>;
        using _mat4 = mat4<__type, __simd>;
    };

    template <typename __type, typename __simd>
    struct MathTypeInfo<mat3<__type, __simd>>
    {
        using _is_valid = std::true_type;
        using _class = MathTypeClass::_class_mat3;
        using _is_vec = std::false_type;
        using _type = __type;
        using _simd = __simd;

        // compatible type definition
        using _vec2 = vec2<__type, __simd>;
        using _vec3 = vec3<__type, __simd>;
        using _vec4 = vec4<__type, __simd>;
        using _quat = quat<__type, __simd>;
        using _mat2 = mat2<__type, __simd>;
        using _mat3 = mat3<__type, __simd>;
        using _mat4 = mat4<__type, __simd>;
    };

    template <typename __type, typename __simd>
    struct MathTypeInfo<mat4<__type, __simd>>
    {
        using _is_valid = std::true_type;
        using _class = MathTypeClass::_class_mat4;
        using _is_vec = std::false_type;
        using _type = __type;
        using _simd = __simd;

        // compatible type definition
        using _vec2 = vec2<__type, __simd>;
        using _vec3 = vec3<__type, __simd>;
        using _vec4 = vec4<__type, __simd>;
        using _quat = quat<__type, __simd>;
        using _mat2 = mat2<__type, __simd>;
        using _mat3 = mat3<__type, __simd>;
        using _mat4 = mat4<__type, __simd>;
    };


    // other types filtering
    // #define MATH_CORE_DEFINE_ANY_TYPE(T_) template <> \
    // struct MathTypeInfo<T_> \
    // { \
    //     using _is_valid = std::false_type; \
    //     using _is_vec = std::false_type; \
    //     using _type = T_; \
    // };

    // MATH_CORE_DEFINE_ANY_TYPE(uint8_t)
    // MATH_CORE_DEFINE_ANY_TYPE(int8_t)
    // MATH_CORE_DEFINE_ANY_TYPE(uint16_t)
    // MATH_CORE_DEFINE_ANY_TYPE(int16_t)
    // MATH_CORE_DEFINE_ANY_TYPE(uint32_t)
    // MATH_CORE_DEFINE_ANY_TYPE(int32_t)
    // MATH_CORE_DEFINE_ANY_TYPE(float)
    // MATH_CORE_DEFINE_ANY_TYPE(double)
    // MATH_CORE_DEFINE_ANY_TYPE(bool)
    // MATH_CORE_DEFINE_ANY_TYPE(std::string)

    // #undef MATH_CORE_DEFINE_ANY_TYPE



}
