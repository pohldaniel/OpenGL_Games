#pragma once

#include "../vec4.h"

#include "mat4_base.h"

namespace MathCore
{

	/*
	struct  {
				float _11, _21, _31, _41,
					_12, _22, _32, _42,
					_13, _23, _33, _43,
					_14, _24, _34, _44;
			};
			struct  {
				float a1, a2, a3, a4,
					b1, b2, b3, b4,
					c1, c2, c3, c4,
					d1, d2, d3, d4;
			};
	*/

	/// \brief Matrix with 4x4 components
	///
	/// Matrix definition to work with rigid transformations
	///
	/// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
	///
	/// It is possible to use any arithmetic with mat4 and _BaseType Combinations.
	///
	/// Example:
	///
	/// \code
	/// #include <aRibeiroCore/aRibeiroCore.h>
	/// using namespace aRibeiro;
	///
	/// mat4 a, b, result;
	///
	/// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
	/// \endcode
	///
	///
	/// \author Alessandro Ribeiro
	///
	template <typename _BaseType, typename _SimdType>
	class mat4<_BaseType, _SimdType,
			   typename std::enable_if<
				   std::is_same<_SimdType, SIMD_TYPE::NONE>::value>::type>
	{
		using self_type = mat4<_BaseType, _SimdType>;
		using vec4_compatible_type = vec4<_BaseType, _SimdType>;

	public:
		static constexpr int rows = 4;
		static constexpr int cols = 4;

		static constexpr int array_count = 16;
		static constexpr int array_stride = 4;

		using type = self_type;
		using element_type = _BaseType;

		union
		{
			struct
			{
				_BaseType a1, a2, a3, a4,
					b1, b2, b3, b4,
					c1, c2, c3, c4,
					d1, d2, d3, d4;
			};
			_BaseType array[16];
			// column-major (OpenGL like matrix byte order)
			//  x  y  z  w
			//  0  4  8 12
			//  1  5  9 13
			//  2  6 10 14
			//  3  7 11 15
		};

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
		/// mat4 matrix = mat4();
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		///
		/*__forceinline mat4()
		{
			const self_type mat4_IdentityMatrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
			*this = mat4_IdentityMatrix;
		}*/
		constexpr __forceinline mat4() : array{1, 0, 0, 0,
											0, 1, 0, 0,
											0, 0, 1, 0,
											0, 0, 0, 1} {}
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
		/// mat4 matrix = mat4( 10.0f );
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param value Value to initialize the components
		///
		/*__forceinline mat4(const _BaseType& value)
		{
			a1 = a2 = a3 = a4 =
				b1 = b2 = b3 = b4 =
				c1 = c2 = c3 = c4 =
				d1 = d2 = d3 = d4 = value;
		}*/
		constexpr __forceinline mat4(const _BaseType &v) : array{v, v, v, v,
															  v, v, v, v,
															  v, v, v, v,
															  v, v, v, v} {}
		//---------------------------------------------------------------------------
		/// \brief Constructs a 4x4 matrix
		///
		/// Initialize the mat4 components from the parameters
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
		/// mat4 matrix = mat4( 1.0f, 0.0f, 0.0f, 0.0f,
		///                     0.0f, 1.0f, 0.0f, 0.0f,
		///                     0.0f, 0.0f, 1.0f, 0.0f,
		///                     0.0f, 0.0f, 0.0f, 1.0f);
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		///
		/*__forceinline mat4(
			const _BaseType& _a1, const _BaseType& _b1, const _BaseType& _c1, const _BaseType& _d1,
			const _BaseType& _a2, const _BaseType& _b2, const _BaseType& _c2, const _BaseType& _d2,
			const _BaseType& _a3, const _BaseType& _b3, const _BaseType& _c3, const _BaseType& _d3,
			const _BaseType& _a4, const _BaseType& _b4, const _BaseType& _c4, const _BaseType& _d4)
		{
			a1 = _a1;
			a2 = _a2;
			a3 = _a3;
			a4 = _a4;

			b1 = _b1;
			b2 = _b2;
			b3 = _b3;
			b4 = _b4;

			c1 = _c1;
			c2 = _c2;
			c3 = _c3;
			c4 = _c4;

			d1 = _d1;
			d2 = _d2;
			d3 = _d3;
			d4 = _d4;
		}*/
		constexpr __forceinline mat4(const _BaseType &_a1, const _BaseType &_b1, const _BaseType &_c1, const _BaseType &_d1,
								  const _BaseType &_a2, const _BaseType &_b2, const _BaseType &_c2, const _BaseType &_d2,
								  const _BaseType &_a3, const _BaseType &_b3, const _BaseType &_c3, const _BaseType &_d3,
								  const _BaseType &_a4, const _BaseType &_b4, const _BaseType &_c4, const _BaseType &_d4) : array{_a1, _a2, _a3, _a4,
																																  _b1, _b2, _b3, _b4,
																																  _c1, _c2, _c3, _c4,
																																  _d1, _d2, _d3, _d4} {}
		//---------------------------------------------------------------------------
		/// \brief Constructs a 4x4 matrix
		///
		/// Initialize the mat4 components by copying other mat4 instance
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat4 matrix_src = mat4( 1.0f, 0.0f, 0.0f, 0.0f,
		///                         0.0f, 1.0f, 0.0f, 0.0f,
		///                         0.0f, 0.0f, 1.0f, 0.0f,
		///                         0.0f, 0.0f, 0.0f, 1.0f);
		///
		/// mat4 matrix = mat4( matrix_src );
		///
		/// mat4 matrix_a = matrix_src;
		///
		/// mat4 matrix_b;
		/// matrix_b = matrix_src;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param m Matrix to assign to the instance
		///
		__forceinline mat4(const self_type &m)
		{
			*this = m;
		}
		/*constexpr __forceinline mat4(const self_type& m) :
			array{ m.a1, m.a2, m.a3, m.a4,
			m.b1, m.b2, m.b3, m.b4,
			m.c1, m.c2, m.c3, m.c4,
			m.d1, m.d2, m.d3, m.d4 }
		{}*/

		//---------------------------------------------------------------------------
		/// \brief Constructs a 4x4 matrix
		///
		/// Initialize the mat4 components from vec4 parameters
		///
		/// \author Alessandro Ribeiro
		/// \param m Matrix to assign to the instance
		///
		/*__forceinline mat4(const vec4_compatible_type& a, const vec4_compatible_type& b, const vec4_compatible_type& c, const vec4_compatible_type& d)
		{
			a1 = a.x;
			a2 = a.y;
			a3 = a.z;
			a4 = a.w;

			b1 = b.x;
			b2 = b.y;
			b3 = b.z;
			b4 = b.w;

			c1 = c.x;
			c2 = c.y;
			c3 = c.z;
			c4 = c.w;

			d1 = d.x;
			d2 = d.y;
			d3 = d.z;
			d4 = d.w;
		}*/

		constexpr __forceinline mat4(const vec4_compatible_type &a, const vec4_compatible_type &b, const vec4_compatible_type &c, const vec4_compatible_type &d) : array{a.x, a.y, a.z, a.w,
																																									  b.x, b.y, b.z, b.w,
																																									  c.x, c.y, c.z, c.w,
																																									  d.x, d.y, d.z, d.w} {}

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
		/// mat4 matrix, other_matrix;
		///
		/// matrix *= other_matrix;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param M the matrix to be multiplied by the current instance
		/// \return A reference to the multiplied matrix current instance
		///
		__forceinline self_type &operator*=(const self_type &M)
		{
			_BaseType a, b, c, d;
			a = a1;
			b = b1;
			c = c1;
			d = d1;
			a1 = (a * M.a1 + b * M.a2 + c * M.a3 + d * M.a4);
			b1 = (a * M.b1 + b * M.b2 + c * M.b3 + d * M.b4);
			c1 = (a * M.c1 + b * M.c2 + c * M.c3 + d * M.c4);
			d1 = (a * M.d1 + b * M.d2 + c * M.d3 + d * M.d4);

			a = a2;
			b = b2;
			c = c2;
			d = d2;
			a2 = (a * M.a1 + b * M.a2 + c * M.a3 + d * M.a4);
			b2 = (a * M.b1 + b * M.b2 + c * M.b3 + d * M.b4);
			c2 = (a * M.c1 + b * M.c2 + c * M.c3 + d * M.c4);
			d2 = (a * M.d1 + b * M.d2 + c * M.d3 + d * M.d4);

			a = a3;
			b = b3;
			c = c3;
			d = d3;
			a3 = (a * M.a1 + b * M.a2 + c * M.a3 + d * M.a4);
			b3 = (a * M.b1 + b * M.b2 + c * M.b3 + d * M.b4);
			c3 = (a * M.c1 + b * M.c2 + c * M.c3 + d * M.c4);
			d3 = (a * M.d1 + b * M.d2 + c * M.d3 + d * M.d4);

			a = a4;
			b = b4;
			c = c4;
			d = d4;
			a4 = (a * M.a1 + b * M.a2 + c * M.a3 + d * M.a4);
			b4 = (a * M.b1 + b * M.b2 + c * M.b3 + d * M.b4);
			c4 = (a * M.c1 + b * M.c2 + c * M.c3 + d * M.c4);
			d4 = (a * M.d1 + b * M.d2 + c * M.d3 + d * M.d4);

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
		/// mat4 matrix;
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
		__forceinline _BaseType &operator()(const int _row, const int _col)
		{
			return array[_col * 4 + _row];
		}
		__forceinline const _BaseType &operator()(const int _row, const int _col) const
		{
			return array[_col * 4 + _row];
		}
		//---------------------------------------------------------------------------
		/// \brief Matrix row access based
		///
		/// Acess one of the 4 columns of the matrix as a vec4 type
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat4 matrix;
		/// vec3 translate_vec;
		///
		/// vec4 forward = matrix[2];
		///
		/// matrix[3] = toPtn4( translate_vec );
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param _col The column to get
		/// \return A reference to the matrix row as vec4
		///
		__forceinline vec4_compatible_type &operator[](const int _col)
		{
			return *((vec4_compatible_type *)&array[_col * 4]);
		}

		/// \brief Matrix row access based
		///
		/// Acess one of the 4 columns of the matrix as a vec4 type
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// void process_matrix( const mat4 &matrix ) {
		///     vec4 forward = matrix[2];
		///     ...
		/// }
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param _col The column to get
		/// \return A reference to the matrix row as vec4
		///
		__forceinline const vec4_compatible_type &operator[](const int _col) const
		{
			return *((vec4_compatible_type *)&array[_col * 4]);
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
		/// mat4 matrix_a, matrix_b;
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
		template <class _Type = _BaseType,
				  typename std::enable_if<
					  std::is_floating_point<_Type>::value, bool>::type = true>
		__forceinline bool operator==(const self_type &v) const
		{
			_BaseType accumulator = _BaseType();
			for (int i = 0; i < 16; i++)
				accumulator += OP<_BaseType>::abs(array[i] - v.array[i]);
			// accumulator += (std::abs)(array[i] - v.array[i]);
			return accumulator <= EPSILON<_BaseType>::high_precision;
		}

		template <class _Type = _BaseType,
				  typename std::enable_if<
					  std::is_integral<_Type>::value, bool>::type = true>
		__forceinline bool operator==(const self_type &v) const
		{
			for (int i = 0; i < 16; i++)
				if (array[i] != v.array[i])
					return false;
			return true;
		}

		// inter SIMD types converting...
		template <typename _InputType, typename _InputSimdTypeAux,
				  typename std::enable_if<
					  std::is_convertible<_InputType, _BaseType>::value &&
						  (!std::is_same<_InputSimdTypeAux, _SimdType>::value ||
						   !std::is_same<_InputType, _BaseType>::value),
					  bool>::type = true>
		__forceinline void operator=(const mat4<_InputType, _InputSimdTypeAux> &m)
		{
			*this = self_type(
				(_BaseType)m.a1, (_BaseType)m.b1, (_BaseType)m.c1, (_BaseType)m.d1,
				(_BaseType)m.a2, (_BaseType)m.b2, (_BaseType)m.c2, (_BaseType)m.d2,
				(_BaseType)m.a3, (_BaseType)m.b3, (_BaseType)m.c3, (_BaseType)m.d3,
				(_BaseType)m.a4, (_BaseType)m.b4, (_BaseType)m.c4, (_BaseType)m.d4);
		}
		// inter SIMD types converting...
		template <typename _OutputType, typename _OutputSimdTypeAux,
				  typename std::enable_if<
					  std::is_convertible<_BaseType, _OutputType>::value &&
						  !(std::is_same<_OutputSimdTypeAux, _SimdType>::value &&
							std::is_same<_OutputType, _BaseType>::value),
					  bool>::type = true>
		__forceinline operator mat4<_OutputType, _OutputSimdTypeAux>() const
		{
			return mat4<_OutputType, _OutputSimdTypeAux>(
				(_OutputType)a1, (_OutputType)b1, (_OutputType)c1, (_OutputType)d1,
				(_OutputType)a2, (_OutputType)b2, (_OutputType)c2, (_OutputType)d2,
				(_OutputType)a3, (_OutputType)b3, (_OutputType)c3, (_OutputType)d3,
				(_OutputType)a4, (_OutputType)b4, (_OutputType)c4, (_OutputType)d4);
		}

		/// \brief Compare two matrix using the #EPSILON constant
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat4 matrix_a, matrix_b;
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
		__forceinline bool operator!=(const self_type &v) const
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
		/// mat4 matrix, matrix_b;
		///
		/// matrix += matrix_b;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v The other matrix used to add values
		/// \return The matrix with the sum result
		///
		__forceinline self_type &operator+=(const self_type &v)
		{
			a1 += v.a1;
			a2 += v.a2;
			a3 += v.a3;
			a4 += v.a4;

			b1 += v.b1;
			b2 += v.b2;
			b3 += v.b3;
			b4 += v.b4;

			c1 += v.c1;
			c2 += v.c2;
			c3 += v.c3;
			c4 += v.c4;

			d1 += v.d1;
			d2 += v.d2;
			d3 += v.d3;
			d4 += v.d4;
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
		/// mat4 matrix, matrix_b;
		///
		/// matrix -= matrix_b;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \param v The other matrix used to subtract values
		/// \return The matrix with the subtract result
		///
		__forceinline self_type &operator-=(const self_type &v)
		{
			a1 -= v.a1;
			a2 -= v.a2;
			a3 -= v.a3;
			a4 -= v.a4;

			b1 -= v.b1;
			b2 -= v.b2;
			b3 -= v.b3;
			b4 -= v.b4;

			c1 -= v.c1;
			c2 -= v.c2;
			c3 -= v.c3;
			c4 -= v.c4;

			d1 -= v.d1;
			d2 -= v.d2;
			d3 -= v.d3;
			d4 -= v.d4;
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
		/// mat4 matrix;
		///
		/// matrix = -matrix;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the signal changed
		///
		__forceinline self_type operator-() const
		{
			return self_type(-a1, -b1, -c1, -d1,
							 -a2, -b2, -c2, -d2,
							 -a3, -b3, -c3, -d3,
							 -a4, -b4, -c4, -d4);
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
		/// mat4 matrix, matrix_b;
		///
		/// matrix /= matrix_b;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the division result
		///
		__forceinline self_type &operator/=(const self_type &v)
		{
			(*this) *= v.inverse();
			return *this;
		}

		__forceinline self_type inverse() const
		{
			// adapted from https://github.com/g-truc/glm/blob/master/glm/detail/func_matrix.inl

			_BaseType Coef00 = c3 * d4 - d3 * c4;
			_BaseType Coef02 = b3 * d4 - d3 * b4;
			_BaseType Coef03 = b3 * c4 - c3 * b4;

			_BaseType Coef04 = c2 * d4 - d2 * c4;
			_BaseType Coef06 = b2 * d4 - d2 * b4;
			_BaseType Coef07 = b2 * c4 - c2 * b4;

			_BaseType Coef08 = c2 * d3 - d2 * c3;
			_BaseType Coef10 = b2 * d3 - d2 * b3;
			_BaseType Coef11 = b2 * c3 - c2 * b3;

			_BaseType Coef12 = c1 * d4 - d1 * c4;
			_BaseType Coef14 = b1 * d4 - d1 * b4;
			_BaseType Coef15 = b1 * c4 - c1 * b4;

			_BaseType Coef16 = c1 * d3 - d1 * c3;
			_BaseType Coef18 = b1 * d3 - d1 * b3;
			_BaseType Coef19 = b1 * c3 - c1 * b3;

			_BaseType Coef20 = c1 * d2 - d1 * c2;
			_BaseType Coef22 = b1 * d2 - d1 * b2;
			_BaseType Coef23 = b1 * c2 - c1 * b2;

			vec4_compatible_type Fac0(Coef00, Coef00, Coef02, Coef03);
			vec4_compatible_type Fac1(Coef04, Coef04, Coef06, Coef07);
			vec4_compatible_type Fac2(Coef08, Coef08, Coef10, Coef11);
			vec4_compatible_type Fac3(Coef12, Coef12, Coef14, Coef15);
			vec4_compatible_type Fac4(Coef16, Coef16, Coef18, Coef19);
			vec4_compatible_type Fac5(Coef20, Coef20, Coef22, Coef23);

			vec4_compatible_type Vec0(b1, a1, a1, a1);
			vec4_compatible_type Vec1(b2, a2, a2, a2);
			vec4_compatible_type Vec2(b3, a3, a3, a3);
			vec4_compatible_type Vec3(b4, a4, a4, a4);

			vec4_compatible_type Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			vec4_compatible_type Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			vec4_compatible_type Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			vec4_compatible_type Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

			vec4_compatible_type SignA(+1, -1, +1, -1);
			vec4_compatible_type SignB(-1, +1, -1, +1);
			self_type Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

			vec4_compatible_type Row0(Inverse.a1, Inverse.b1, Inverse.c1, Inverse.d1);

			vec4_compatible_type Dot0((*this)[0] * Row0);
			_BaseType det = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

			// MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
			det = OP<_BaseType>::maximum(det, FloatTypeInfo<_BaseType>::min);

			_BaseType _1_over_det = (_BaseType)1.0 / det;

			return Inverse * _1_over_det;
		}

		//
		// can be used for inverse
		// transpose rotation+scale mat4 representations
		//
		__forceinline mat4 inverse_transpose_3x3() const
		{
			_BaseType aux1 = c3 * b2 - b3 * c2;
			_BaseType aux2 = b3 * c1 - c3 * b1;
			_BaseType aux3 = c2 * b1 - b2 * c1;

			_BaseType det = (a1 * aux1 + a2 * aux2 + a3 * aux3);

			// check det
			// MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
			det = OP<_BaseType>::maximum(det, FloatTypeInfo<_BaseType>::min);

			det = (_BaseType)1.0 / det;

			return self_type(
				det * aux1, det * (a3 * c2 - c3 * a2), det * (b3 * a2 - a3 * b2), 0,
				det * aux2, det * (c3 * a1 - a3 * c1), det * (a3 * b1 - b3 * a1), 0,
				det * aux3, det * (a2 * c1 - c2 * a1), det * (b2 * a1 - a2 * b1), 0,
				0, 0, 0, 1);
		}

		/// \brief Add (sum) matrix with a scalar
		///
		/// Example:
		///
		/// \code
		/// #include <aRibeiroCore/aRibeiroCore.h>
		/// using namespace aRibeiro;
		///
		/// mat4 matrix;
		///
		/// matrix += 5.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the sum of the elements
		///
		__forceinline self_type &operator+=(const _BaseType &v)
		{
			a1 += v;
			a2 += v;
			a3 += v;
			a4 += v;

			b1 += v;
			b2 += v;
			b3 += v;
			b4 += v;

			c1 += v;
			c2 += v;
			c3 += v;
			c4 += v;

			d1 += v;
			d2 += v;
			d3 += v;
			d4 += v;
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
		/// mat4 matrix;
		///
		/// matrix -= 5.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the subtract of the elements
		///
		__forceinline self_type &operator-=(const _BaseType &v)
		{
			a1 -= v;
			a2 -= v;
			a3 -= v;
			a4 -= v;

			b1 -= v;
			b2 -= v;
			b3 -= v;
			b4 -= v;

			c1 -= v;
			c2 -= v;
			c3 -= v;
			c4 -= v;

			d1 -= v;
			d2 -= v;
			d3 -= v;
			d4 -= v;
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
		/// mat4 matrix;
		///
		/// matrix *= 5.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the multiplication of the elements
		///
		__forceinline self_type &operator*=(const _BaseType &v)
		{
			a1 *= v;
			a2 *= v;
			a3 *= v;
			a4 *= v;

			b1 *= v;
			b2 *= v;
			b3 *= v;
			b4 *= v;

			c1 *= v;
			c2 *= v;
			c3 *= v;
			c4 *= v;

			d1 *= v;
			d2 *= v;
			d3 *= v;
			d4 *= v;
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
		/// mat4 matrix;
		///
		/// matrix /= 5.0f;
		/// \endcode
		///
		/// \author Alessandro Ribeiro
		/// \return The matrix with the division of the elements
		///
		__forceinline self_type &operator/=(const _BaseType &v)
		{
			a1 /= v;
			a2 /= v;
			a3 /= v;
			a4 /= v;

			b1 /= v;
			b2 /= v;
			b3 /= v;
			b4 /= v;

			c1 /= v;
			c2 /= v;
			c3 /= v;
			c4 /= v;

			d1 /= v;
			d2 /= v;
			d3 /= v;
			d4 /= v;
			return *this;
		}
	};
}
