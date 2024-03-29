#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'quat_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "quat_base.h"

namespace MathCore
{

	/// \brief Quaternion (quat)
	///
	/// Stores four components(x,y,z,w) to represent a quaternion. <br/>
	/// The quaternion can be seen as a unit axis with an angle in radians in the imaginary space.
	///
	/// \author Alessandro Ribeiro
	///
	template <typename _BaseType, typename _SimdType>
	class alignas(16) quat<_BaseType, _SimdType,
		typename std::enable_if<
		std::is_same<_BaseType, float>::value &&
		(std::is_same<_SimdType, SIMD_TYPE::SSE>::value ||
			std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
	{
		using self_type = quat<_BaseType, _SimdType>;

	public:

		static constexpr int array_count = 4;
		using type = self_type;
		using element_type = _BaseType;

		union
		{
			_BaseType array[4];
			struct
			{
				_BaseType x, y, z, w;
			};
#if defined(ITK_SSE2)
			__m128 array_sse;
#elif defined(ITK_NEON)
			float32x4_t array_neon;
#endif
		};

#if defined(ITK_SSE2)
		__forceinline quat(const __m128& v)
		{
			array_sse = v;
		}
#elif defined(ITK_NEON)
		__forceinline quat(const float32x4_t& v)
		{
			array_neon = v;
		}
#endif

		/// \brief Construct an identity quaternion quat class
		///
		/// The identity quat class has the folow configuration (x=0,y=0,z=0,w=1)
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// quat rotation = quat();
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		///
		__forceinline quat()
		{
#if defined(ITK_SSE2)
			array_sse = _vec4_0001_sse;// _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
#elif defined(ITK_NEON)
			array_neon = _neon_0001;//(float32x4_t){0.0f, 0.0f, 0.0f, 1.0f};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}
		/*constexpr __forceinline quat() :array{0, 0, 0, (_BaseType)1} {}*/
		/// \brief Constructs a quaterion
		///
		/// Initialize the quat components from the parameters
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// quat rotation = quat( 0.0f, 0.0f, 0.0f, 1.0f );
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param x Value to assign to the X component
		/// \param y Value to assign to the Y component
		/// \param z Value to assign to the Z component
		/// \param w Value to assign to the W component
		///
		__forceinline quat(const _BaseType &x, const _BaseType &y, const _BaseType &z, const _BaseType &w)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_setr_ps(x, y, z, w);
#elif defined(ITK_NEON)
			array_neon = (float32x4_t){x, y, z, w};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}
		/*constexpr __forceinline quat(const _BaseType& _x, const _BaseType& _y, const _BaseType& _z, const _BaseType& _w) :array{_x, _y, _z, _w} {}*/
		/// \brief Constructs a quaternion
		///
		/// Initialize the quat components from other quat instance by copying the content of the other quat.
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// quat rotation_source;
		///
		/// quat rotation = quat( rotation_source );
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v Vector to assign to the instance
		///
		__forceinline quat(const self_type &v)
		{
#if defined(ITK_SSE2)
			array_sse = v.array_sse;
#elif defined(ITK_NEON)
			array_neon = v.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}

		//constexpr __forceinline quat(const self_type& q) :x(q.x), y(q.y), z(q.z), w(q.w) {}

		/// \brief Comparison of quaternions (equal)
		///
		/// Compare two quaternions considering #EPSILON.
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// quat rotation_a, rotation_b;
		///
		/// if ( rotation_a == rotation_b ) {
		///     ...
		/// }
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v Quaternion to compare against
		/// \return true if the quaternions are equal considering #EPSILON
		///
		__forceinline bool operator==(const quat& v) const
		{

#if defined(ITK_SSE2)

			__m128 diff_abs = _mm_sub_ps(array_sse, v.array_sse);
			// abs
			// const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
			diff_abs = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs);

			diff_abs = _mm_hadd_ps(diff_abs, diff_abs);
			diff_abs = _mm_hadd_ps(diff_abs, diff_abs);

			return _mm_f32_(diff_abs, 0) <= EPSILON<_BaseType>::high_precision;
#elif defined(ITK_NEON)

			float32x4_t diff_abs = vsubq_f32(array_neon, v.array_neon);
			// abs
			diff_abs = vabsq_f32(diff_abs);

			float32x2_t acc_2_elements = vadd_f32(vget_high_f32(diff_abs), vget_low_f32(diff_abs));
			acc_2_elements = vpadd_f32(acc_2_elements, acc_2_elements);

			return acc_2_elements[0] <= EPSILON<_BaseType>::high_precision;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}

		// inter SIMD types converting...
		template <typename _InputType, typename _InputSimdTypeAux,
			typename std::enable_if<
			std::is_convertible<_InputType, _BaseType>::value &&
			!(std::is_same<_InputSimdTypeAux, _SimdType>::value&&
				std::is_same<_InputType, _BaseType>::value),
			bool>::type = true>
		__forceinline void operator=(const quat<_InputType, _InputSimdTypeAux>& vec)
		{
			*this = self_type((_BaseType)vec.x, (_BaseType)vec.y, (_BaseType)vec.z, (_BaseType)vec.w);
		}
		// inter SIMD types converting...
		template <typename _OutputType, typename _OutputSimdTypeAux,
			typename std::enable_if<
			std::is_convertible<_BaseType, _OutputType>::value &&
			!(std::is_same<_OutputSimdTypeAux, _SimdType>::value&&
				std::is_same<_OutputType, _BaseType>::value),
			bool>::type = true>
		__forceinline operator quat<_OutputType, _OutputSimdTypeAux>() const
		{
			return quat<_OutputType, _OutputSimdTypeAux>(
				(_OutputType)x, (_OutputType)y, (_OutputType)z, (_OutputType)w);
		}

		/// \brief Invert all signs of the quaternion
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// quat rotation;
		///
		/// rotation = -rotation;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return the quaternion with sign of the elements inverted
		///
		__forceinline self_type operator-() const
		{
#if defined(ITK_SSE2)
			// const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
			return _mm_xor_ps(_vec4_sign_mask_sse, array_sse);
#elif defined(ITK_NEON)
			//const float32x4_t minus_one = (float32x4_t){ -1.0f, -1.0f, -1.0f, -1.0f };
			//return vmulq_f32(_vec4_minus_one, array_neon);
			return vnegq_f32(array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}

		/// \brief Comparison of quaternions (not equal)
		///
		/// Compare two quaternions considering #EPSILON.
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// quat rotation_a, rotation_b;
		///
		/// if ( rotation_a != rotation_b ) {
		///     ...
		/// }
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v Quaternion to compare against
		/// \return true if the quaternions are not equal considering #EPSILON
		///
		__forceinline bool operator!=(const self_type& v) const
		{
			return !((*this) == v);
		}

		/// \brief Index the components of the quat as a C array
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// quat rotation;
		///
		/// float x = rotation[0];
		///
		/// rotation[3] = 1.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v The index of the components starting by 0
		/// \return A reference to the element at the index v
		///
		__forceinline _BaseType& operator[](const int v)
		{
			return array[v];
		}

		/// \brief Index the components of the quat as a C array
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// void processQuaternion ( const quat &rotation ) {
		///     float x = rotation[0];
		///     ...
		/// }
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v The index of the components starting by 0
		/// \return A reference to the element at the index v
		///
		__forceinline const _BaseType& operator[](const int v) const
		{
			return array[v];
		}

		__forceinline self_type& operator*=(const _BaseType& v)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_mul_ps(array_sse, _mm_set1_ps(v));
#elif defined(ITK_NEON)
			array_neon = vmulq_f32(array_neon, vset1(v));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			return (*this);
		}
		
	};
}
