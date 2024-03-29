#pragma once

#include "math_common.h"

namespace MathCore
{
	//
	// IntTypeInfo
	//
	template <typename _type, class Enable = void>
	struct IntTypeInfo
	{
	};
	template <typename _input>
	struct IntTypeInfo<_input,
					   typename std::enable_if<
						   std::is_same<_input, int8_t>::value ||
						   std::is_same<_input, uint8_t>::value>::type>
	{
		using type = IntTypeInfo<_input>;
		using type_unsigned = uint8_t;
		using type_signed = int8_t;

		static constexpr int shift_to_get_sign = 7;
		static constexpr int shift_to_get_sign_minus_one = 6;
		static constexpr int8_t first_bit_zero = 0xfe;
		// static constexpr inline int shift_to_get_sign() { return 7; }
	};
	template <typename _input>
	struct IntTypeInfo<_input,
					   typename std::enable_if<
						   std::is_same<_input, int16_t>::value ||
						   std::is_same<_input, uint16_t>::value>::type>
	{
		using type = IntTypeInfo<_input>;
		using type_unsigned = uint16_t;
		using type_signed = int16_t;

		static constexpr int shift_to_get_sign = 15;
		static constexpr int shift_to_get_sign_minus_one = 14;
		static constexpr int16_t first_bit_zero = 0xfffe;
		// static constexpr inline int shift_to_get_sign() { return 15; }
	};
	template <typename _input>
	struct IntTypeInfo<_input,
					   typename std::enable_if<
						   std::is_same<_input, int32_t>::value ||
						   std::is_same<_input, uint32_t>::value>::type>
	{
		using type = IntTypeInfo<_input>;
		using type_unsigned = uint32_t;
		using type_signed = int32_t;

		static constexpr int shift_to_get_sign = 31;
		static constexpr int shift_to_get_sign_minus_one = 30;
		static constexpr int32_t first_bit_zero = 0xfffffffe;
		// static constexpr inline int shift_to_get_sign() { return 31; }
	};
	template <typename _input>
	struct IntTypeInfo<_input,
					   typename std::enable_if<
						   std::is_same<_input, int64_t>::value ||
						   std::is_same<_input, uint64_t>::value>::type>
	{
		using type = IntTypeInfo<_input>;
		using type_unsigned = uint64_t;
		using type_signed = int64_t;

		static constexpr int shift_to_get_sign = 63;
		static constexpr int shift_to_get_sign_minus_one = 62;
		static constexpr int64_t first_bit_zero = 0xfffffffffffffffe;
		// static constexpr inline int shift_to_get_sign() { return 63; }
	};

	//
	// OP<any int>
	//
	template <typename _type>
	struct OP<_type,
			  typename std::enable_if<
				  std::is_integral<_type>::value>::type>
	{
		using type = _type;
		using self_type = OP<_type>;

		// optimized instructions
		static __forceinline _type abs(const _type &v) noexcept
		{
			using type_info = IntTypeInfo<_type>;
			using type_signed = typename IntTypeInfo<_type>::type_signed;

			// https://www.geeksforgeeks.org/compute-the-integer-absolute-value-abs-without-branching/
			_type signmask_all_bits_x = (type_signed)v >> type_info::shift_to_get_sign;
			return (v + signmask_all_bits_x) ^ signmask_all_bits_x;
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_same<_Type, int32_t>::value, bool>::type = true>
		static __forceinline _type clamp(const _type &value, const _type &min, const _type &max) noexcept
		{
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
			__m128i maxStep = _sse2_mm_max_epi32(_mm_set1_epi32(value), _mm_set1_epi32(min));
			__m128i minStep = _sse2_mm_min_epi32(maxStep, _mm_set1_epi32(max));
#else
			__m128i maxStep = _mm_max_epi32(_mm_set1_epi32(value), _mm_set1_epi32(min));
			__m128i minStep = _mm_min_epi32(maxStep, _mm_set1_epi32(max));
#endif
			return _mm_i32_(minStep, 0);
#else
			return (value < min) ? min : ((value > max) ? max : value);
#endif
		}
		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_same<_Type, uint32_t>::value, bool>::type = true>
		static __forceinline _type clamp(const _type &value, const _type &min, const _type &max) noexcept
		{
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
			__m128i maxStep = _sse2_mm_max_epu32(_mm_set1_epi32(value), _mm_set1_epi32(min));
			__m128i minStep = _sse2_mm_min_epu32(maxStep, _mm_set1_epi32(max));
#else
			__m128i maxStep = _mm_max_epu32(_mm_set1_epi32(value), _mm_set1_epi32(min));
			__m128i minStep = _mm_min_epu32(maxStep, _mm_set1_epi32(max));
#endif
			return _mm_u32_(minStep, 0);
#else
			return (value < min) ? min : ((value > max) ? max : value);
#endif
		}
		template <class _Type = _type,
				  typename std::enable_if<
					  !std::is_same<_Type, int32_t>::value &&
						  !std::is_same<_Type, uint32_t>::value,
					  bool>::type = true>
		static __forceinline _type clamp(const _type &value, const _type &min, const _type &max) noexcept
		{
			return (value < min) ? min : ((value > max) ? max : value);
		}

		static __forceinline _type sqrDistance(const _type &a, const _type &b) noexcept
		{
			_type ab = b - a;
			return ab * ab;
		}

		static __forceinline _type distance(const _type &a, const _type &b) noexcept
		{
			_type ab = b - a;
			return self_type::abs(ab);
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_same<_Type, int32_t>::value, bool>::type = true>
		static __forceinline _type maximum(const _type &a, const _type &b) noexcept
		{
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
			return _mm_i32_(_sse2_mm_max_epi32(_mm_set1_epi32(a), _mm_set1_epi32(b)), 0);
#else
			return _mm_i32_(_mm_max_epi32(_mm_set1_epi32(a), _mm_set1_epi32(b)), 0);
#endif
#else
			return (a > b) ? a : b;
#endif
		}
		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_same<_Type, uint32_t>::value, bool>::type = true>
		static __forceinline _type maximum(const _type &a, const _type &b) noexcept
		{
#if defined(ITK_SSE2)
			return _mm_u32_(_mm_max_epu32(_mm_set1_epi32(a), _mm_set1_epi32(b)), 0);
#else
			return (a > b) ? a : b;
#endif
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  !std::is_same<_Type, int32_t>::value &&
						  !std::is_same<_Type, uint32_t>::value,
					  bool>::type = true>
		static __forceinline _type maximum(const _type &a, const _type &b) noexcept
		{
			return (a > b) ? a : b;
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_same<_Type, int32_t>::value, bool>::type = true>
		static __forceinline _type minimum(const _type &a, const _type &b) noexcept
		{
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
			return _mm_i32_(_sse2_mm_min_epi32(_mm_set1_epi32(a), _mm_set1_epi32(b)), 0);
#else
			return _mm_i32_(_mm_min_epi32(_mm_set1_epi32(a), _mm_set1_epi32(b)), 0);
#endif
#else
			return (a < b) ? a : b;
#endif
		}
		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_same<_Type, uint32_t>::value, bool>::type = true>
		static __forceinline _type minimum(const _type &a, const _type &b) noexcept
		{
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
			return _mm_u32_(_sse2_mm_min_epu32(_mm_set1_epi32(a), _mm_set1_epi32(b)), 0);
#else
			return _mm_u32_(_mm_min_epu32(_mm_set1_epi32(a), _mm_set1_epi32(b)), 0);
#endif
#else
			return (a < b) ? a : b;
#endif
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  !std::is_same<_Type, int32_t>::value &&
						  !std::is_same<_Type, uint32_t>::value,
					  bool>::type = true>
		static __forceinline _type minimum(const _type &a, const _type &b) noexcept
		{
			return (a < b) ? a : b;
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_signed<_Type>::value, bool>::type = true>
		static __forceinline _type sign(const _type &a) noexcept
		{
			using type_info = IntTypeInfo<_type>;
			return ((a >> type_info::shift_to_get_sign_minus_one) & type_info::first_bit_zero) + 1;
			//return (((a >= 0) << 1) - 1);
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_signed<_Type>::value, bool>::type = true>
		static __forceinline _type step(const _type &threshould, const _type &v) noexcept
		{
			// _type _sub = v - threshould;
			// _type _sign = self_type::sign(_sub);
			// _sign = self_type::maximum(_sign, (_type)0);
			// return _sign;

			return (v >= threshould);
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_signed<_Type>::value, bool>::type = true>
		static __forceinline _type step_cmp_greater_equal(const _type &a, const _type &b) noexcept
		{
			// return step(b, a);
			return (a >= b);
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_signed<_Type>::value, bool>::type = true>
		static __forceinline _type step_cmp_less(const _type &a, const _type &b) noexcept
		{
			// xor first bit
			// return step(b, a) ^ 0x01;
			return (a < b);
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_signed<_Type>::value, bool>::type = true>
		static __forceinline _type step_cmp_less_equal(const _type &a, const _type &b) noexcept
		{
			// return step(a,b);
			return (a <= b);
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_signed<_Type>::value, bool>::type = true>
		static __forceinline _type step_cmp_greater(const _type &a, const _type &b) noexcept
		{
			// return step(a,b) ^ 0x01;
			return (a > b);
		}

		template <class _Type = _type,
				  typename std::enable_if<
					  std::is_signed<_Type>::value, bool>::type = true>
		static __forceinline _type step_cmp_equal(const _type &a, const _type &b) noexcept
		{
			// return step_cmp_less_equal(a,b) & step_cmp_greater_equal(a,b);
			return (a == b);
		}
	};

}
