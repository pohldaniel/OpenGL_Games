#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat3_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "../vec3.h"

#include "mat3_base.h"

namespace MathCore
{

	/// \brief Matrix with 4x4 components
	///
	/// Matrix definition to work with rigid transformations
	///
	/// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
	///
	/// It is possible to use any arithmetic with mat3 and float combinations.
	///
	/// Example:
	///
	/// \code
	/// #include <aRibeiroCore/aRibeiroCore.h>
	/// using namespace aRibeiro;
	///
	/// mat3 a, b, result;
	///
	/// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
	/// \endcode
	///
	///
	/// \author Alessandro Ribeiro
	///
	template <typename _BaseType, typename _SimdType>
	class alignas(16) mat3<_BaseType, _SimdType,
		typename std::enable_if<
		std::is_same<_BaseType, float>::value &&
		(std::is_same<_SimdType, SIMD_TYPE::SSE>::value ||
			std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
	{
		using self_type = mat3<_BaseType, _SimdType>;
		using vec3_compatible_type = vec3<_BaseType, _SimdType>;

	public:

		static constexpr int rows = 3;
		static constexpr int cols = 3;

		static constexpr int array_count = 12;
		static constexpr int array_stride = 4;

		using type = self_type;
		using element_type = _BaseType;

		union
		{
			struct
			{
				_BaseType a1, a2, a3, _dummy_a4,
					b1, b2, b3, _dummy_b4,
					c1, c2, c3, _dummy_c4;
			};
			_BaseType array[12];
			// column-major (OpenGL like matrix byte order)
			//  x  y  z  w
			//  0  4  8 12
			//  1  5  9 13
			//  2  6 10 14
			//  3  7 11 15
#if defined(ITK_SSE2)
			__m128 array_sse[3];
#elif defined(ITK_NEON)
			float32x4_t array_neon[3];
#endif
		};

#if defined(ITK_SSE2)
		__forceinline mat3(const __m128& a, const __m128& b, const __m128& c)
		{
			array_sse[0] = a;
			array_sse[1] = b;
			array_sse[2] = c;
		}
#elif defined(ITK_NEON)
		__forceinline mat3(const float32x4_t& a, const float32x4_t& b, const float32x4_t& c)
		{
			array_neon[0] = a;
			array_neon[1] = b;
			array_neon[2] = c;
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
		/// mat3 matrix = mat3();
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		///
		__forceinline mat3()
		{
#if defined(ITK_SSE2)
			array_sse[0] = _vec4_1000_sse;// _mm_setr_ps(1, 0, 0, 0);
			array_sse[1] = _vec4_0100_sse;//_mm_setr_ps(0, 1, 0, 0);
			array_sse[2] = _vec4_0010_sse;//_mm_setr_ps(0, 0, 1, 0);
#elif defined(ITK_NEON)
			array_neon[0] = _neon_1000;//(float32x4_t){ 1, 0, 0, 0 };
			array_neon[1] = _neon_0100;//(float32x4_t){ 0, 1, 0, 0 };
			array_neon[2] = _neon_0010;//(float32x4_t){ 0, 0, 1, 0 };
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}
		/*constexpr __forceinline mat3() :array{ 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0 } {}*/
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
			/// mat3 matrix = mat3( 10.0f );
			/// \endcode
			///
			/// \author Alessandro Ribeiro
			/// \param value Value to initialize the components
			///
		__forceinline mat3(const _BaseType& value)
		{
#if defined(ITK_SSE2)
			__m128 data = _mm_set1_ps(value);
			array_sse[0] = data;
			array_sse[1] = data;
			array_sse[2] = data;
#elif defined(ITK_NEON)
			float32x4_t data = vset1(value);
			array_neon[0] = data;
			array_neon[1] = data;
			array_neon[2] = data;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}
		//---------------------------------------------------------------------------
		/// \brief Constructs a 4x4 matrix
		///
		/// Initialize the mat3 components from the parameters
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
		/// mat3 matrix = mat3( 1.0f, 0.0f, 0.0f, 0.0f,
		///                     0.0f, 1.0f, 0.0f, 0.0f,
		///                     0.0f, 0.0f, 1.0f, 0.0f,
		///                     0.0f, 0.0f, 0.0f, 1.0f);
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		///
		__forceinline mat3(
			const _BaseType& a1, const _BaseType& b1, const _BaseType& c1,
			const _BaseType& a2, const _BaseType& b2, const _BaseType& c2,
			const _BaseType& a3, const _BaseType& b3, const _BaseType& c3)
		{
#if defined(ITK_SSE2)
			array_sse[0] = _mm_setr_ps(a1, a2, a3, 0);
			array_sse[1] = _mm_setr_ps(b1, b2, b3, 0);
			array_sse[2] = _mm_setr_ps(c1, c2, c3, 0);
#elif defined(ITK_NEON)
			array_neon[0] = (float32x4_t){ a1, a2, a3, 0 };
			array_neon[1] = (float32x4_t){ b1, b2, b3, 0 };
			array_neon[2] = (float32x4_t){ c1, c2, c3, 0 };
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}
		/*constexpr __forceinline mat3(const _BaseType& _a1, const _BaseType& _b1, const _BaseType& _c1,
			const _BaseType& _a2, const _BaseType& _b2, const _BaseType& _c2,
			const _BaseType& _a3, const _BaseType& _b3, const _BaseType& _c3) :array{ _a1, _a2, _a3, 0,
			_b1, _b2, _b3, 0,
			_c1, _c2, _c3, 0 } {}*/
			//---------------------------------------------------------------------------
			/// \brief Constructs a 4x4 matrix
			///
			/// Initialize the mat3 components by copying other mat3 instance
			///
			/// Example:
			///
			/// \code
			/// #include <aRibeiroCore/aRibeiroCore.h>
			/// using namespace aRibeiro;
			///
			/// mat3 matrix_src = mat3( 1.0f, 0.0f, 0.0f, 0.0f,
			///                         0.0f, 1.0f, 0.0f, 0.0f,
			///                         0.0f, 0.0f, 1.0f, 0.0f,
			///                         0.0f, 0.0f, 0.0f, 1.0f);
			///
			/// mat3 matrix = mat3( matrix_src );
			///
			/// mat3 matrix_a = matrix_src;
			///
			/// mat3 matrix_b;
			/// matrix_b = matrix_src;
			/// \endcode
			///
			/// \author Alessandro Ribeiro
			/// \param m Matrix to assign to the instance
			///
		__forceinline mat3(const self_type& m)
		{
#if defined(ITK_SSE2)
			array_sse[0] = m.array_sse[0];
			array_sse[1] = m.array_sse[1];
			array_sse[2] = m.array_sse[2];
#elif defined(ITK_NEON)
			array_neon[0] = m.array_neon[0];
			array_neon[1] = m.array_neon[1];
			array_neon[2] = m.array_neon[2];
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
		}

		//---------------------------------------------------------------------------
		/// \brief Constructs a 4x4 matrix
		///
		/// Initialize the mat3 components from vec3 parameters
		///
		/// \author Alessandro Ribeiro
		/// \param m Matrix to assign to the instance
		///
		__forceinline mat3(const vec3_compatible_type& a, const vec3_compatible_type& b, const vec3_compatible_type& c)
		{
#if defined(ITK_SSE2)
			array_sse[0] = a.array_sse;
			array_sse[1] = b.array_sse;
			array_sse[2] = c.array_sse;
#elif defined(ITK_NEON)
			array_neon[0] = a.array_neon;
			array_neon[1] = b.array_neon;
			array_neon[2] = c.array_neon;
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
		/// mat3 matrix, other_matrix;
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
#if defined(ITK_SSE2)
			__m128 array_sse_result[3];
			{
				__m128 e0 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(0, 0, 0, 0));
				__m128 e1 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(1, 1, 1, 1));
				__m128 e2 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(2, 2, 2, 2));
				//__m128 e3 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(3, 3, 3, 3));

				__m128 m0 = _mm_mul_ps(array_sse[0], e0);
				__m128 m1 = _mm_mul_ps(array_sse[1], e1);
				__m128 m2 = _mm_mul_ps(array_sse[2], e2);
				//__m128 m3 = _mm_mul_ps(array_sse[3], e3);

				__m128 a0 = _mm_add_ps(m0, m1);
				//__m128 a1 = _mm_add_ps(m2, m3);
				__m128 a2 = _mm_add_ps(a0, m2);

				array_sse_result[0] = a2;
			}

			{
				__m128 e0 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(0, 0, 0, 0));
				__m128 e1 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(1, 1, 1, 1));
				__m128 e2 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(2, 2, 2, 2));
				//__m128 e3 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(3, 3, 3, 3));

				__m128 m0 = _mm_mul_ps(array_sse[0], e0);
				__m128 m1 = _mm_mul_ps(array_sse[1], e1);
				__m128 m2 = _mm_mul_ps(array_sse[2], e2);
				//__m128 m3 = _mm_mul_ps(array_sse[3], e3);

				__m128 a0 = _mm_add_ps(m0, m1);
				//__m128 a1 = _mm_add_ps(m2, m3);
				__m128 a2 = _mm_add_ps(a0, m2);

				array_sse_result[1] = a2;
			}

			{
				__m128 e0 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(0, 0, 0, 0));
				__m128 e1 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(1, 1, 1, 1));
				__m128 e2 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(2, 2, 2, 2));
				//__m128 e3 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(3, 3, 3, 3));

				__m128 m0 = _mm_mul_ps(array_sse[0], e0);
				__m128 m1 = _mm_mul_ps(array_sse[1], e1);
				__m128 m2 = _mm_mul_ps(array_sse[2], e2);
				//__m128 m3 = _mm_mul_ps(array_sse[3], e3);

				__m128 a0 = _mm_add_ps(m0, m1);
				//__m128 a1 = _mm_add_ps(m2, m3);
				__m128 a2 = _mm_add_ps(a0, m2);

				array_sse_result[2] = a2;
			}

			// {
			//     //(__m128&)_mm_shuffle_epi32(__m128i&)in2[0], _MM_SHUFFLE(3, 3, 3, 3))
			//     __m128 e0 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(0, 0, 0, 0));
			//     __m128 e1 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(1, 1, 1, 1));
			//     __m128 e2 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(2, 2, 2, 2));
			//     __m128 e3 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(3, 3, 3, 3));

			//     __m128 m0 = _mm_mul_ps(array_sse[0], e0);
			//     __m128 m1 = _mm_mul_ps(array_sse[1], e1);
			//     __m128 m2 = _mm_mul_ps(array_sse[2], e2);
			//     __m128 m3 = _mm_mul_ps(array_sse[3], e3);

			//     __m128 a0 = _mm_add_ps(m0, m1);
			//     __m128 a1 = _mm_add_ps(m2, m3);
			//     __m128 a2 = _mm_add_ps(a0, a1);

			//     array_sse_result[3] = a2;
			// }

			array_sse[0] = array_sse_result[0];
			array_sse[1] = array_sse_result[1];
			array_sse[2] = array_sse_result[2];
			//array_sse[3] = array_sse_result[3];

#else
			float32x4_t array_neon_result[3];
			{
				float32x4_t e0 = vshuffle_0000(M.array_neon[0]);
				float32x4_t e1 = vshuffle_1111(M.array_neon[0]);
				float32x4_t e2 = vshuffle_2222(M.array_neon[0]);
				//float32x4_t e3 = vshuffle_3333(M.array_neon[0]);

				float32x4_t m0 = vmulq_f32(array_neon[0], e0);
				float32x4_t m1 = vmulq_f32(array_neon[1], e1);
				float32x4_t m2 = vmulq_f32(array_neon[2], e2);
				//float32x4_t m3 = vmulq_f32(array_neon[3], e3);

				float32x4_t a0 = vaddq_f32(m0, m1);
				//float32x4_t a1 = vaddq_f32(m2, m3);
				float32x4_t a2 = vaddq_f32(a0, m2);

				array_neon_result[0] = a2;
			}

			{
				float32x4_t e0 = vshuffle_0000(M.array_neon[1]);
				float32x4_t e1 = vshuffle_1111(M.array_neon[1]);
				float32x4_t e2 = vshuffle_2222(M.array_neon[1]);
				//float32x4_t e3 = vshuffle_3333(M.array_neon[1]);

				float32x4_t m0 = vmulq_f32(array_neon[0], e0);
				float32x4_t m1 = vmulq_f32(array_neon[1], e1);
				float32x4_t m2 = vmulq_f32(array_neon[2], e2);
				//float32x4_t m3 = vmulq_f32(array_neon[3], e3);

				float32x4_t a0 = vaddq_f32(m0, m1);
				//float32x4_t a1 = vaddq_f32(m2, m3);
				float32x4_t a2 = vaddq_f32(a0, m2);

				array_neon_result[1] = a2;
			}

			{
				float32x4_t e0 = vshuffle_0000(M.array_neon[2]);
				float32x4_t e1 = vshuffle_1111(M.array_neon[2]);
				float32x4_t e2 = vshuffle_2222(M.array_neon[2]);
				//float32x4_t e3 = vshuffle_3333(M.array_neon[2]);

				float32x4_t m0 = vmulq_f32(array_neon[0], e0);
				float32x4_t m1 = vmulq_f32(array_neon[1], e1);
				float32x4_t m2 = vmulq_f32(array_neon[2], e2);
				//float32x4_t m3 = vmulq_f32(array_neon[3], e3);

				float32x4_t a0 = vaddq_f32(m0, m1);
				//float32x4_t a1 = vaddq_f32(m2, m3);
				float32x4_t a2 = vaddq_f32(a0, m2);

				array_neon_result[2] = a2;
			}

			// {
			//     //(float32x4_t&)_mm_shuffle_epi32(float32x4_ti&)in2[0], _MM_SHUFFLE(3, 3, 3, 3))
			//     float32x4_t e0 = vshuffle_0000(M.array_neon[3]);
			//     float32x4_t e1 = vshuffle_1111(M.array_neon[3]);
			//     float32x4_t e2 = vshuffle_2222(M.array_neon[3]);
			//     //float32x4_t e3 = vshuffle_3333(M.array_neon[3]);

			//     float32x4_t m0 = vmulq_f32(array_neon[0], e0);
			//     float32x4_t m1 = vmulq_f32(array_neon[1], e1);
			//     float32x4_t m2 = vmulq_f32(array_neon[2], e2);
			//     //float32x4_t m3 = vmulq_f32(array_neon[3], e3);

			//     float32x4_t a0 = vaddq_f32(m0, m1);
			//     //float32x4_t a1 = vaddq_f32(m2, m3);
			//     float32x4_t a2 = vaddq_f32(a0, m2);

			//     array_neon_result[3] = a2;
			// }

			array_neon[0] = array_neon_result[0];
			array_neon[1] = array_neon_result[1];
			array_neon[2] = array_neon_result[2];
			//array_neon[3] = array_neon_result[3];

#endif

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
		/// mat3 matrix;
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
			return array[_col * 4 + _row];
		}
		__forceinline const _BaseType& operator()(const int _row, const int _col) const
		{
			return array[_col * 4 + _row];
		}
		//---------------------------------------------------------------------------
		/// \brief Matrix row access based
		///
		/// Acess one of the 4 columns of the matrix as a vec3 type
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat3 matrix;
		/// vec3 translate_vec;
		///
		/// vec3 forward = matrix[2];
		///
		/// matrix[3] = toPtn4( translate_vec );
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param _col The column to get
		/// \return A reference to the matrix row as vec3
		///
		__forceinline vec3_compatible_type& operator[](const int _col)
		{
			return *((vec3_compatible_type*)&array[_col * 4]);
		}

		/// \brief Matrix row access based
		///
		/// Acess one of the 4 columns of the matrix as a vec3 type
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// void process_matrix( const mat3 &matrix ) {
		///     vec3 forward = matrix[2];
		///     ...
		/// }
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param _col The column to get
		/// \return A reference to the matrix row as vec3
		///
		__forceinline const vec3_compatible_type& operator[](const int _col) const
		{
			return *((vec3_compatible_type*)&array[_col * 4]);
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
		/// mat3 matrix_a, matrix_b;
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

			// const __m128 _vec3_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31

			__m128 diff_abs[3];
			diff_abs[0] = _mm_sub_ps(array_sse[0], v.array_sse[0]);
			diff_abs[0] = _mm_andnot_ps(_vec3_sign_mask_sse, diff_abs[0]); // abs

			diff_abs[1] = _mm_sub_ps(array_sse[1], v.array_sse[1]);
			diff_abs[1] = _mm_andnot_ps(_vec3_sign_mask_sse, diff_abs[1]); // abs

			diff_abs[2] = _mm_sub_ps(array_sse[2], v.array_sse[2]);
			diff_abs[2] = _mm_andnot_ps(_vec3_sign_mask_sse, diff_abs[2]); // abs

			__m128 accumulator_a = _mm_add_ps(diff_abs[0], diff_abs[1]);
			//__m128 accumulator_b = _mm_add_ps(diff_abs[2], diff_abs[3]);

			__m128 accumulator = _mm_add_ps(accumulator_a, diff_abs[2]);

			accumulator = _mm_and_ps(_vec3_valid_bits_sse, accumulator);

			accumulator = _mm_hadd_ps(accumulator, accumulator);
			accumulator = _mm_hadd_ps(accumulator, accumulator);

			return _mm_f32_(accumulator, 0) <= EPSILON<_BaseType>::high_precision;

#elif defined(ITK_NEON)

			float32x4_t diff_abs[3];
			diff_abs[0] = vsubq_f32(array_neon[0], v.array_neon[0]);
			diff_abs[0] = vabsq_f32(diff_abs[0]); // abs

			diff_abs[1] = vsubq_f32(array_neon[1], v.array_neon[1]);
			diff_abs[1] = vabsq_f32(diff_abs[1]); // abs

			diff_abs[2] = vsubq_f32(array_neon[2], v.array_neon[2]);
			diff_abs[2] = vabsq_f32(diff_abs[2]); // abs

			// diff_abs[3] = vsubq_f32(array_neon[3], v.array_neon[3]);
			// diff_abs[3] = vabsq_f32(diff_abs[3]); // abs

			float32x4_t accumulator_a = vaddq_f32(diff_abs[0], diff_abs[1]);
			// float32x4_t accumulator_b = vaddq_f32(diff_abs[2], diff_abs[3]);

			float32x4_t acc_4_elements = vaddq_f32(accumulator_a, diff_abs[2]);

			//const float32x4_t mask = (float32x4_t){1.0,1.0,1.0,0.0};
			//acc_4_elements = vmulq_f32(acc_4_elements, mask);
			acc_4_elements[3] = 0;

			float32x2_t acc_2_elements = vadd_f32(vget_high_f32(acc_4_elements), vget_low_f32(acc_4_elements));
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
		__forceinline void operator=(const mat3<_InputType, _InputSimdTypeAux>& m)
		{
			*this = self_type(
				(_BaseType)m.a1, (_BaseType)m.b1, (_BaseType)m.c1,
				(_BaseType)m.a2, (_BaseType)m.b2, (_BaseType)m.c2,
				(_BaseType)m.a3, (_BaseType)m.b3, (_BaseType)m.c3);
		}
		// inter SIMD types converting...
		template <typename _OutputType, typename _OutputSimdTypeAux,
			typename std::enable_if<
			std::is_convertible<_BaseType, _OutputType>::value &&
			!(std::is_same<_OutputSimdTypeAux, _SimdType>::value&&
				std::is_same<_OutputType, _BaseType>::value),
			bool>::type = true>
		__forceinline operator mat3<_OutputType, _OutputSimdTypeAux>() const
		{
			return mat3<_OutputType, _OutputSimdTypeAux>(
				(_OutputType)a1, (_OutputType)b1, (_OutputType)c1,
				(_OutputType)a2, (_OutputType)b2, (_OutputType)c2,
				(_OutputType)a3, (_OutputType)b3, (_OutputType)c3);
		}

		/// \brief Compare two matrix using the #EPSILON constant
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat3 matrix_a, matrix_b;
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
		/// mat3 matrix, matrix_b;
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
			array_sse[0] = _mm_add_ps(array_sse[0], v.array_sse[0]);
			array_sse[1] = _mm_add_ps(array_sse[1], v.array_sse[1]);
			array_sse[2] = _mm_add_ps(array_sse[2], v.array_sse[2]);

#elif defined(ITK_NEON)

			array_neon[0] = vaddq_f32(array_neon[0], v.array_neon[0]);
			array_neon[1] = vaddq_f32(array_neon[1], v.array_neon[1]);
			array_neon[2] = vaddq_f32(array_neon[2], v.array_neon[2]);

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
		/// mat3 matrix, matrix_b;
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
			array_sse[0] = _mm_sub_ps(array_sse[0], v.array_sse[0]);
			array_sse[1] = _mm_sub_ps(array_sse[1], v.array_sse[1]);
			array_sse[2] = _mm_sub_ps(array_sse[2], v.array_sse[2]);

#elif defined(ITK_NEON)

			array_neon[0] = vsubq_f32(array_neon[0], v.array_neon[0]);
			array_neon[1] = vsubq_f32(array_neon[1], v.array_neon[1]);
			array_neon[2] = vsubq_f32(array_neon[2], v.array_neon[2]);

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
		/// mat3 matrix;
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
			self_type(_mm_xor_ps(_vec3_sign_mask_sse, array_sse[0]),
				_mm_xor_ps(_vec3_sign_mask_sse, array_sse[1]),
				_mm_xor_ps(_vec3_sign_mask_sse, array_sse[2]));

#elif defined(ITK_NEON)

			//const float32x4_t neg = vset1(-1.0f);

			// return self_type(
			// 	vmulq_f32(array_neon[0], _vec4_minus_one),
			// 	vmulq_f32(array_neon[1], _vec4_minus_one),
			// 	vmulq_f32(array_neon[2], _vec4_minus_one));
			return self_type(
				vnegq_f32(array_neon[0]),
				vnegq_f32(array_neon[1]),
				vnegq_f32(array_neon[2]));
				



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
		/// mat3 matrix, matrix_b;
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
			// original implementation
			// 
			//_BaseType aux1 = c3 * b2 - b3 * c2;
			//_BaseType aux2 = b3 * c1 - c3 * b1;
			//_BaseType aux3 = c2 * b1 - b2 * c1;

			//_BaseType det = (a1 * aux1 + a2 * aux2 + a3 * aux3);

			//// check det
			//MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");

			//det = (_BaseType)1.0 / det;

			//return self_type(
			//    vec3_compatible_type(aux1, (a3 * c2 - c3 * a2), (b3 * a2 - a3 * b2)) * det,
			//    vec3_compatible_type(aux2, (c3 * a1 - a3 * c1), (a3 * b1 - b3 * a1)) * det,
			//    vec3_compatible_type(aux3, (a2 * c1 - c2 * a1), (b2 * a1 - a2 * b1)) * det);

			// optimal ver 1
			// 
			//vec3_compatible_type aux = vec3_compatible_type(c3, b3, c2) * vec3_compatible_type(b2, c1, b1) - vec3_compatible_type(b3, c3, b2) * vec3_compatible_type(c2, b1, c1);
			//_BaseType det = _mm_f32_(dot_sse_3(array_sse[0], aux.array_sse), 0);
			//// check det
			////MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
			//det = (_BaseType)1.0 / det;
			//return self_type(
			//	vec3_compatible_type(aux.x, (a3 * c2 - c3 * a2), (b3 * a2 - a3 * b2)) * det,
			//	vec3_compatible_type(aux.y, (c3 * a1 - a3 * c1), (a3 * b1 - b3 * a1)) * det,
			//	vec3_compatible_type(aux.z, (a2 * c1 - c2 * a1), (b2 * a1 - a2 * b1)) * det);


			// optimal ver 2
			// 
			self_type result(
				(vec3_compatible_type(c3, a3, b3) * vec3_compatible_type(b2, c2, a2) - vec3_compatible_type(b3, c3, a3) * vec3_compatible_type(c2, a2, b2)),
				(vec3_compatible_type(b3, c3, a3) * vec3_compatible_type(c1, a1, b1) - vec3_compatible_type(c3, a3, b3) * vec3_compatible_type(b1, c1, a1)),
				(vec3_compatible_type(c2, a2, b2) * vec3_compatible_type(b1, c1, a1) - vec3_compatible_type(b2, c2, a2) * vec3_compatible_type(c1, a1, b1))
			);

#if defined(ITK_SSE2)
			__m128 tmp0 = _mm_shuffle_ps(result.array_sse[0], result.array_sse[1], _MM_SHUFFLE(1, 0, 1, 0));
			__m128 tmp1 = _mm_shuffle_ps(result.array_sse[2], _vec4_0001_sse, _MM_SHUFFLE(1, 0, 1, 0));
			__m128 aux = _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0));
			_BaseType det = _mm_f32_(dot_sse_3(array_sse[0], aux), 0);
#elif defined(ITK_NEON)
			_BaseType det = (a1 * result.a1 + a2 * result.b1 + a3 * result.c1);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			// check det
			//MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
			det = OP<_BaseType>::maximum(det,FloatTypeInfo<_BaseType>::min);
			det = (_BaseType)1.0 / det;

			return result * det;
		}

		//
		// can be used for inverse
		// transpose rotation+scale mat3 representations
		//
		__forceinline mat3 inverse_transpose_2x2() const
		{
			_BaseType det = (a1 * b2 - b1 * a2);

			//MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
			det = OP<_BaseType>::maximum(det,FloatTypeInfo<_BaseType>::min);
			det = (_BaseType)1.0 / det;

			return self_type(+b2 * det, -a2 * det, 0,
				-b1 * det, +a1 * det, 0,
				0, 0, 1);
		}

		/// \brief Add (sum) matrix with a scalar
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat3 matrix;
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
			__m128 tmp = _mm_set1_ps(v);

			array_sse[0] = _mm_add_ps(array_sse[0], tmp);
			array_sse[1] = _mm_add_ps(array_sse[1], tmp);
			array_sse[2] = _mm_add_ps(array_sse[2], tmp);

#elif defined(ITK_NEON)

			float32x4_t tmp = vset1(v);

			array_neon[0] = vaddq_f32(array_neon[0], tmp);
			array_neon[1] = vaddq_f32(array_neon[1], tmp);
			array_neon[2] = vaddq_f32(array_neon[2], tmp);

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
		/// mat3 matrix;
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
			__m128 tmp = _mm_set1_ps(v);

			array_sse[0] = _mm_sub_ps(array_sse[0], tmp);
			array_sse[1] = _mm_sub_ps(array_sse[1], tmp);
			array_sse[2] = _mm_sub_ps(array_sse[2], tmp);

#elif defined(ITK_NEON)

			float32x4_t tmp = vset1(v);

			array_neon[0] = vsubq_f32(array_neon[0], tmp);
			array_neon[1] = vsubq_f32(array_neon[1], tmp);
			array_neon[2] = vsubq_f32(array_neon[2], tmp);

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
		/// mat3 matrix;
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
			__m128 tmp = _mm_set1_ps(v);

			array_sse[0] = _mm_mul_ps(array_sse[0], tmp);
			array_sse[1] = _mm_mul_ps(array_sse[1], tmp);
			array_sse[2] = _mm_mul_ps(array_sse[2], tmp);

#elif defined(ITK_NEON)

			float32x4_t tmp = vset1(v);

			array_neon[0] = vmulq_f32(array_neon[0], tmp);
			array_neon[1] = vmulq_f32(array_neon[1], tmp);
			array_neon[2] = vmulq_f32(array_neon[2], tmp);

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
		/// mat3 matrix;
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
			__m128 tmp = _mm_set1_ps(v);

			array_sse[0] = _mm_div_ps(array_sse[0], tmp);
			array_sse[1] = _mm_div_ps(array_sse[1], tmp);
			array_sse[2] = _mm_div_ps(array_sse[2], tmp);

#elif defined(ITK_NEON)

			float32x4_t tmp = vset1(1.0f / v);

			array_neon[0] = vmulq_f32(array_neon[0], tmp);
			array_neon[1] = vmulq_f32(array_neon[1], tmp);
			array_neon[2] = vmulq_f32(array_neon[2], tmp);

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
			return *this;
		}
	};
}
