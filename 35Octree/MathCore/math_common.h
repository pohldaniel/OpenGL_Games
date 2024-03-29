#pragma once

namespace MathCore
{

    namespace SIMD_TYPE
    {

        struct NONE;
        struct SSE;
        struct NEON;

        using DEFAULT = NONE;

#if defined(ITK_SSE2)
        using SIMD = SSE;
#elif defined(ITK_NEON)
        using SIMD = NEON;
#else
        using SIMD = NONE;
#endif

    }

	template <typename _type_>
	struct EPSILON2
	{
	};

    template <typename _type_, class Enable = void>
    struct CONSTANT
    {
    };

    template <typename _type_, class Enable = void>
    struct CVT
    {
    };

    namespace RSQRT
    {
        struct NORMAL;
        struct SIMD;
        struct CARMACK;

#if defined(ITK_FORCE_USE_RSQRT_CARMACK)
        using DEFAULT = CARMACK;
#elif defined(ITK_NEON) || defined(ITK_SSE2)
        using DEFAULT = SIMD;
#else
        using DEFAULT = NORMAL;
#endif

    };

    template <typename _type_, class Enable = void, typename Algorithm = RSQRT::DEFAULT>
    struct OP
    {
    };

    template <typename _type_, class Enable = void>
    struct GEN
    {
    };

#define INLINE_STATIC_ARITHMETIC_OPERATION_IMPLEMENTATION(TTYPE)                                                                                       \
    template <typename _BaseType, typename _SimdType>                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator/(const TTYPE<_BaseType, _SimdType> &vecA, const TTYPE<_BaseType, _SimdType> &vecB) noexcept \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>(vecA) /= vecB);                                                                                            \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _InputType, typename _SimdType,                                                                             \
              typename std::enable_if<                                                                                                                 \
                  std::is_convertible<_InputType, _BaseType>::value,                                                                                   \
                  bool>::type = true>                                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator/(const TTYPE<_BaseType, _SimdType> &vec, const _InputType &value) noexcept                  \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>(vec) /= (_BaseType)value);                                                                                 \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _InputType, typename _SimdType,                                                                             \
              typename std::enable_if<                                                                                                                 \
                  std::is_convertible<_InputType, _BaseType>::value,                                                                                   \
                  bool>::type = true>                                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator/(const _InputType &value, const TTYPE<_BaseType, _SimdType> &vec) noexcept                  \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>((_BaseType)value) /= vec);                                                                                 \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _SimdType>                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator*(const TTYPE<_BaseType, _SimdType> &vecA, const TTYPE<_BaseType, _SimdType> &vecB) noexcept \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>(vecA) *= vecB);                                                                                            \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _InputType, typename _SimdType,                                                                             \
              typename std::enable_if<                                                                                                                 \
                  std::is_convertible<_InputType, _BaseType>::value,                                                                                   \
                  bool>::type = true>                                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator*(const TTYPE<_BaseType, _SimdType> &vec, const _InputType &value) noexcept                  \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>(vec) *= (_BaseType)value);                                                                                 \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _InputType, typename _SimdType,                                                                             \
              typename std::enable_if<                                                                                                                 \
                  std::is_convertible<_InputType, _BaseType>::value,                                                                                   \
                  bool>::type = true>                                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator*(const _InputType &value, const TTYPE<_BaseType, _SimdType> &vec) noexcept                  \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>((_BaseType)value) *= vec);                                                                                 \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _SimdType>                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator+(const TTYPE<_BaseType, _SimdType> &vecA, const TTYPE<_BaseType, _SimdType> &vecB) noexcept \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>(vecA) += vecB);                                                                                            \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _InputType, typename _SimdType,                                                                             \
              typename std::enable_if<                                                                                                                 \
                  std::is_convertible<_InputType, _BaseType>::value,                                                                                   \
                  bool>::type = true>                                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator+(const TTYPE<_BaseType, _SimdType> &vec, const _InputType &value) noexcept                  \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>(vec) += (_BaseType)value);                                                                                 \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _InputType, typename _SimdType,                                                                             \
              typename std::enable_if<                                                                                                                 \
                  std::is_convertible<_InputType, _BaseType>::value,                                                                                   \
                  bool>::type = true>                                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator+(const _InputType &value, const TTYPE<_BaseType, _SimdType> &vec) noexcept                  \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>((_BaseType)value) += vec);                                                                                 \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _SimdType>                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator-(const TTYPE<_BaseType, _SimdType> &vecA, const TTYPE<_BaseType, _SimdType> &vecB) noexcept \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>(vecA) -= vecB);                                                                                            \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _InputType, typename _SimdType,                                                                             \
              typename std::enable_if<                                                                                                                 \
                  std::is_convertible<_InputType, _BaseType>::value,                                                                                   \
                  bool>::type = true>                                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator-(const TTYPE<_BaseType, _SimdType> &vec, const _InputType &value) noexcept                  \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>(vec) -= (_BaseType)value);                                                                                 \
    }                                                                                                                                                  \
    template <typename _BaseType, typename _InputType, typename _SimdType,                                                                             \
              typename std::enable_if<                                                                                                                 \
                  std::is_convertible<_InputType, _BaseType>::value,                                                                                   \
                  bool>::type = true>                                                                                                                  \
    static __forceinline TTYPE<_BaseType, _SimdType> operator-(const _InputType &value, const TTYPE<_BaseType, _SimdType> &vec) noexcept                  \
    {                                                                                                                                                  \
        return (TTYPE<_BaseType, _SimdType>((_BaseType)value) -= vec);                                                                                 \
    }

#define MATH_CORE_AUX_STR_(x) #x
#define MATH_CORE_AUX_STR(x) MATH_CORE_AUX_STR_(x)

#define MATH_CORE_THROW_RUNTIME_ERROR(bool_exp, _str_) \
    if (bool_exp)                                      \
        throw std::runtime_error(                      \
            "[" __FILE__ ":" MATH_CORE_AUX_STR(__LINE__) "] " _str_);

}
