#pragma once

#include "math_common.h"

#include "impl/quat_all_no_optimization.h"

#if defined(ITK_SSE2) || defined(ITK_NEON)

#include "impl/quat_float_simd.h"

#endif

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat3.h"
#include "mat4.h"

namespace MathCore
{
	//
	// common definitions
	//
	using quatf32 = quat<float, SIMD_TYPE::SIMD>;
	using quatf64 = quat<double>;

	// using quati8 = quat<int8_t>;
	// using quati16 = quat<int16_t>;
	// using quati32 = quat<int32_t>;
	// using quati64 = quat<int64_t>;

	// using quatu8 = quat<uint8_t>;
	// using quatu16 = quat<uint16_t>;
	// using quatu32 = quat<uint32_t>;
	// using quatu64 = quat<uint64_t>;

	//
	// alias
	//
	using quatf = quatf32;
	using quatd = quatf64;

	// using quati = quati32;
	// using quatu = quatu32;

	//
	// Operator Overload Template
	//
	// INLINE_STATIC_ARITHMETIC_OPERATION_IMPLEMENTATION(quat)

	//
	// templated constants
	//

	template <typename _type, typename _simd>
	struct CONSTANT<quat<_type, _simd>>
	{
		using type = quat<_type, _simd>;

		// // C++17
		// static constexpr type Identity = type((_type)0, (_type)0, (_type)0, (_type)1);
		// static constexpr type Zero = type((_type)0, (_type)0, (_type)0, (_type)0);
		// static constexpr type One = type((_type)1, (_type)1, (_type)1, (_type)1);

		// C++11
		static constexpr __forceinline type Identity() noexcept { return type(); }
		static constexpr __forceinline type Zero() noexcept { return type((_type)0); }
		static constexpr __forceinline type One() noexcept { return type((_type)1); }

		// how to generate these values: 
		//
		// double _ang_deg = 0.125 - 0.0366115;
		// quat<double,SIMD_TYPE::NONE> fixedRotation = GEN<quat<double,SIMD_TYPE::NONE>>::fromEuler(
		// 				OP<double>::deg_2_rad(_ang_deg),
		// 				OP<double>::deg_2_rad(_ang_deg),
		// 				OP<double>::deg_2_rad(_ang_deg));
		// vec3<double> a(1,0,0), b(-1,0,0);
		// printf("_180_move_slerp_case(): (_type)%e, (_type)%e, (_type)%e, (_type)%e \n", fixedRotation.x, fixedRotation.y, fixedRotation.z, fixedRotation.w);
		// a = fixedRotation * a;
		// printf("_180_move_slerp_case_deltaAngle(rad): %f \n", OP<decltype(a)>::angleBetween(a,b));
		// printf("_180_move_slerp_case_deltaAngle(deg): %f \n", OP<double>::rad_2_deg(OP<decltype(a)>::angleBetween(a,b)));
		static constexpr __forceinline type _180_move_slerp_case() noexcept { return type((_type)7.707397e-04, (_type)7.719296e-04, (_type)7.707397e-04, (_type)9.999991e-01); }
		static constexpr _type _180_move_slerp_case_deltaAngle = (_type)3.139411;

		

	};

	

	
}
