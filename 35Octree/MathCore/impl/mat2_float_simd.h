#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat2_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "../vec2.h"

#include "mat2_base.h"

namespace MathCore
{

	/// \brief Matrix with 4x4 components
	///
	/// Matrix definition to work with rigid transformations
	///
	/// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
	///
	/// It is possible to use any arithmetic with mat2 and float combinations.
	///
	/// Example:
	///
	/// \code
	/// #include <aRibeiroCore/aRibeiroCore.h>
	/// using namespace aRibeiro;
	///
	/// mat2 a, b, result;
	///
	/// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
	/// \endcode
	///
	///
	/// \author Alessandro Ribeiro
	///
	template <typename _BaseType, typename _SimdType>
	class alignas(16) mat2<_BaseType, _SimdType,
		typename std::enable_if<
		std::is_same<_BaseType, float>::value &&
		(std::is_same<_SimdType, SIMD_TYPE::SSE>::value ||
			std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
	{
		using self_type = mat2<_BaseType, _SimdType>;
		using vec2_compatible_type = vec2<_BaseType, _SimdType>;
		using vec2_extract_type = vec2<_BaseType, SIMD_TYPE::NONE>;

	public:

		static constexpr int rows = 2;
		static constexpr int cols = 2;

		static constexpr int array_count = 4;
		static constexpr int array_stride = 2;

		using type = self_type;
		using element_type = _BaseType;

		union
		{
			struct
			{
				_BaseType a1, a2,
					b1, b2;
			};
			_BaseType array[4];
			// column-major (OpenGL like matrix byte order)
			//  x  y  z  w
			//  0  4  8 12
			//  1  5  9 13
			//  2  6 10 14
			//  3  7 11 15
#if defined(ITK_SSE2)
			__m128 array_sse;
#elif defined(ITK_NEON)
			float32x4_t array_neon;
#endif
		};

#if defined(ITK_SSE2)
		__forceinline mat2(const __m128& a)
		{
			array_sse = a;
		}
#elif defined(ITK_NEON)
		__forceinline mat2(const float32x4_t& a)
		{
			array_neon = a;
		}
#endif

		//---------------------------------------------------------------------------
		/// \brief Constructs an identity matrix 4x4
		///
		/// This construct an identity matrix
		///
		/// <pre>
		/// | 1 0 0 0 |
		/// | 0 1 0 0 |
		/// | 0 0 1 0 |
		/// | 0 0 0 1 |
		/// </pre>
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix = mat2();
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		///
		__forceinline mat2()
		{
#if defined(ITK_SSE2)
			array_sse = _mm_setr_ps(
				1, 0,
				0, 1
			);
#elif defined(ITK_NEON)
			array_neon = (float32x4_t){
				1, 0,
				0, 1
		};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
	}
		/*constexpr __forceinline mat2() :array{ 1, 0,
			0, 1 } {}*/
			//---------------------------------------------------------------------------
			/// \brief Constructs a 4x4 matrix
			///
			/// Initialize all components of the matrix with the same value
			///
			/// Example:
			///
			/// \code
			/// #include <aRibeiroCore/aRibeiroCore.h>
			/// using namespace aRibeiro;
			///
			/// mat2 matrix = mat2( 10.0f );
			/// \endcode
			///
			/// \author Alessandro Ribeiro
			/// \param value Value to initialize the components
			///
		__forceinline mat2(const _BaseType& value)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_set1_ps(value);
#elif defined(ITK_NEON)
			array_neon = vset1(value);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}
		//---------------------------------------------------------------------------
		/// \brief Constructs a 4x4 matrix
		///
		/// Initialize the mat2 components from the parameters
		///
		/// The visual is related to the matrix column major order.
		///
		/// <pre>
		/// | a1 b1 c1 d1 |
		/// | a2 b2 c2 d2 |
		/// | a3 b3 c3 d3 |
		/// | a4 b4 c4 d4 |
		/// </pre>
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix = mat2( 1.0f, 0.0f, 0.0f, 0.0f,
		///                     0.0f, 1.0f, 0.0f, 0.0f,
		///                     0.0f, 0.0f, 1.0f, 0.0f,
		///                     0.0f, 0.0f, 0.0f, 1.0f);
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		///
		__forceinline mat2(
			const _BaseType &a1, const _BaseType &b1,
			const _BaseType &a2, const _BaseType &b2)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_setr_ps(a1, a2, b1, b2);
#elif defined(ITK_NEON)
			array_neon = (float32x4_t){a1, a2, b1, b2};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}
		/*constexpr __forceinline mat2(const _BaseType& _a1, const _BaseType& _b1,
			const _BaseType& _a2, const _BaseType& _b2) :array{ _a1, _a2,
			_b1, _b2} {}*/
		//---------------------------------------------------------------------------
		/// \brief Constructs a 4x4 matrix
		///
		/// Initialize the mat2 components by copying other mat2 instance
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix_src = mat2( 1.0f, 0.0f, 0.0f, 0.0f,
		///                         0.0f, 1.0f, 0.0f, 0.0f,
		///                         0.0f, 0.0f, 1.0f, 0.0f,
		///                         0.0f, 0.0f, 0.0f, 1.0f);
		///
		/// mat2 matrix = mat2( matrix_src );
		///
		/// mat2 matrix_a = matrix_src;
		///
		/// mat2 matrix_b;
		/// matrix_b = matrix_src;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param m Matrix to assign to the instance
		///
		__forceinline mat2(const self_type& m)
		{
			*this = m;
		}

		//---------------------------------------------------------------------------
		/// \brief Constructs a 4x4 matrix
		///
		/// Initialize the mat2 components from vec2 parameters
		///
		/// \author Alessandro Ribeiro
		/// \param m Matrix to assign to the instance
		///
		__forceinline mat2(const vec2_compatible_type& a, const vec2_compatible_type& b)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_shuffle_ps(a.array_sse, b.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
#elif defined(ITK_NEON)
			array_neon = (float32x4_t){ a.x, a.y, b.x, b.y };
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}

		//---------------------------------------------------------------------------
		/// \brief Matrix multiplication
		///
		/// Makes the full 4x4 matrix multiplication
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix, other_matrix;
		///
		/// matrix *= other_matrix;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param M the matrix to be multiplied by the current instance
		/// \return A reference to the multiplied matrix current instance
		///
		__forceinline self_type& operator*=(const self_type& M)
		{
			_BaseType a, b, c;
			a = a1;
			b = b1;

			a1 = (a * M.a1 + b * M.a2);
			b1 = (a * M.b1 + b * M.b2);

			a = a2;
			b = b2;

			a2 = (a * M.a1 + b * M.a2);
			b2 = (a * M.b1 + b * M.b2);

			return *this;
		}
		//---------------------------------------------------------------------------
		/// \brief Matrix access based on X (row) and Y (column)
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix;
		///
		/// matrix(3,0) = 1.0f;
		///
		/// float v = matrix(3,3);
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param _row The row to get the element at index
		/// \param _col The column to get the element at index
		/// \return A reference to the matrix element
		///
		__forceinline _BaseType& operator()(const int _row, const int _col)
		{
			return array[_col * 2 + _row];
		}
		__forceinline const _BaseType& operator()(const int _row, const int _col) const
		{
			return array[_col * 2 + _row];
		}
		//---------------------------------------------------------------------------
		/// \brief Matrix row access based
		///
		/// Acess one of the 4 columns of the matrix as a vec2 type
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix;
		/// vec2 translate_vec;
		///
		/// vec2 forward = matrix[2];
		///
		/// matrix[3] = toPtn4( translate_vec );
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param _col The column to get
		/// \return A reference to the matrix row as vec2
		///
		__forceinline vec2_extract_type& operator[](const int _col)
		{
			return *((vec2_extract_type*)&array[_col * 2]);
		}

		/// \brief Matrix row access based
		///
		/// Acess one of the 4 columns of the matrix as a vec2 type
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// void process_matrix( const mat2 &matrix ) {
		///     vec2 forward = matrix[2];
		///     ...
		/// }
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param _col The column to get
		/// \return A reference to the matrix row as vec2
		///
		__forceinline const vec2_extract_type& operator[](const int _col) const
		{
			return *((vec2_extract_type*)&array[_col * 4]);
		}
		//---------------------------------------------------------------------------
		/// \brief Compare two matrix using the #EPSILON constant
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix_a, matrix_b;
		///
		/// if ( matrix_a == matrix_b ){
		///     //do something
		///     ...
		/// }
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v The other matrix to compare with
		/// \return true: the matrix is equal, considering the #EPSILON
		///
		__forceinline bool operator==(const self_type& v) const
		{

#if defined(ITK_SSE2)

			// const __m128 _vec2_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31

			__m128 diff_abs;
			diff_abs = _mm_sub_ps(array_sse, v.array_sse);
			diff_abs = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs); // abs

			__m128 accumulator = _mm_hadd_ps(diff_abs, diff_abs);
			accumulator = _mm_hadd_ps(accumulator, accumulator);

			return _mm_f32_(accumulator, 0) <= EPSILON<_BaseType>::high_precision;

#elif defined(ITK_NEON)

			float32x4_t diff_abs;
			diff_abs = vsubq_f32(array_neon, v.array_neon);
			diff_abs = vabsq_f32(diff_abs); // abs

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
		__forceinline void operator=(const mat2<_InputType, _InputSimdTypeAux>& m)
		{
			*this = self_type(
				(_BaseType)m.a1, (_BaseType)m.b1,
				(_BaseType)m.a2, (_BaseType)m.b2);
		}
		// inter SIMD types converting...
		template <typename _OutputType, typename _OutputSimdTypeAux,
			typename std::enable_if<
			std::is_convertible<_BaseType, _OutputType>::value &&
			!(std::is_same<_OutputSimdTypeAux, _SimdType>::value&&
				std::is_same<_OutputType, _BaseType>::value),
			bool>::type = true>
		__forceinline operator mat2<_OutputType, _OutputSimdTypeAux>() const
		{
			return mat2<_OutputType, _OutputSimdTypeAux>(
				(_OutputType)a1, (_OutputType)b1,
				(_OutputType)a2, (_OutputType)b2);
		}

		/// \brief Compare two matrix using the #EPSILON constant
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix_a, matrix_b;
		///
		/// if ( matrix_a != matrix_b ){
		///     //do something
		///     ...
		/// }
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v The other matrix to compare with
		/// \return true: the matrix is not equal, considering the #EPSILON
		///
		__forceinline bool operator!=(const self_type& v) const
		{
			return !((*this) == v);
		}

		/// \brief Component-wise add elements of the matrix
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix, matrix_b;
		///
		/// matrix += matrix_b;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v The other matrix used to add values
		/// \return The matrix with the sum result
		///
		__forceinline self_type& operator+=(const self_type& v)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_add_ps(array_sse, v.array_sse);

#elif defined(ITK_NEON)

			array_neon = vaddq_f32(array_neon, v.array_neon);

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			return *this;
		}

		/// \brief Component-wise subtract elements of the matrix
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix, matrix_b;
		///
		/// matrix -= matrix_b;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v The other matrix used to subtract values
		/// \return The matrix with the subtract result
		///
		__forceinline self_type& operator-=(const self_type& v)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_sub_ps(array_sse, v.array_sse);

#elif defined(ITK_NEON)

			array_neon = vsubq_f32(array_neon, v.array_neon);

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			return *this;
		}

		/// \brief Component-wise change signal
		///
		/// Change the signal of each element in the matrix
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix;
		///
		/// matrix = -matrix;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the signal changed
		///
		__forceinline self_type operator-() const
		{
#if defined(ITK_SSE2)
			self_type(_mm_xor_ps(_vec4_sign_mask_sse, array_sse));

#elif defined(ITK_NEON)

			// return self_type(
			// 	vmulq_f32(array_neon, _vec4_minus_one));
			return self_type(vnegq_f32(array_neon));

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}

		/// \brief Component-wise divide element
		///
		/// Make the division operation on each element of the matrix
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix, matrix_b;
		///
		/// matrix /= matrix_b;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the division result
		///
		__forceinline self_type& operator/=(const self_type& v)
		{
			(*this) *= v.inverse();
			return *this;
		}

		__forceinline self_type inverse() const
		{
			_BaseType det = (a1 * b2 - b1 * a2);

			//MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
			det = OP<_BaseType>::maximum(det,FloatTypeInfo<_BaseType>::min);
			det = (_BaseType)1.0 / det;

			return self_type(b2, -b1,
				-a2, a1) * det;
			// return self_type(+b2 * det, -b1 * det,
			//                  -a2 * det, +a1 * det);
		}

		/// \brief Add (sum) matrix with a scalar
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix;
		///
		/// matrix += 5.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the sum of the elements
		///
		__forceinline self_type& operator+=(const _BaseType& v)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_add_ps(array_sse, _mm_set1_ps(v));

#elif defined(ITK_NEON)
			array_neon = vaddq_f32(array_neon, vset1(v));

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			return *this;
		}

		/// \brief Subtract matrix with a scalar
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix;
		///
		/// matrix -= 5.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the subtract of the elements
		///
		__forceinline self_type& operator-=(const _BaseType& v)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_sub_ps(array_sse, _mm_set1_ps(v));

#elif defined(ITK_NEON)
			array_neon = vsubq_f32(array_neon, vset1(v));

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			return *this;
		}

		/// \brief Multiply matrix elements with a scalar
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix;
		///
		/// matrix *= 5.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the multiplication of the elements
		///
		__forceinline self_type& operator*=(const _BaseType& v)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_mul_ps(array_sse, _mm_set1_ps(v));

#elif defined(ITK_NEON)
			array_neon = vmulq_f32(array_neon, vset1(v));

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			return *this;
		}

		/// \brief Divide matrix elements with a scalar
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat2 matrix;
		///
		/// matrix /= 5.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the division of the elements
		///
		__forceinline self_type& operator/=(const _BaseType& v)
		{
#if defined(ITK_SSE2)
			array_sse = _mm_div_ps(array_sse, _mm_set1_ps(v));

#elif defined(ITK_NEON)
			array_neon = vmulq_f32(array_neon, vset1(1.0f / v));

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			return *this;
		}
};
}
