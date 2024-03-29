#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat4_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "../vec4.h"

#include "mat4_base.h"

namespace MathCore
{

    /// \brief Matrix with 4x4 components
    ///
    /// Matrix definition to work with rigid transformations
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// It is possible to use any arithmetic with mat4 and float combinations.
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
    class alignas(16) mat4<_BaseType, _SimdType,
                           typename std::enable_if<
                               std::is_same<_BaseType, float>::value &&
                               (std::is_same<_SimdType, SIMD_TYPE::SSE>::value ||
                                std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
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
#if defined(ITK_SSE2)
            __m128 array_sse[4];
#elif defined(ITK_NEON)
            float32x4_t array_neon[4];
#endif
        };

#if defined(ITK_SSE2)
        __forceinline mat4(const __m128 &a, const __m128 &b, const __m128 &c, const __m128 &d)
        {
            array_sse[0] = a;
            array_sse[1] = b;
            array_sse[2] = c;
            array_sse[3] = d;
        }
#elif defined(ITK_NEON)
        __forceinline mat4(const float32x4_t &a, const float32x4_t &b, const float32x4_t &c, const float32x4_t &d)
        {
            array_neon[0] = a;
            array_neon[1] = b;
            array_neon[2] = c;
            array_neon[3] = d;
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
        /// mat4 matrix = mat4();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        __forceinline mat4()
        {
#if defined(ITK_SSE2)
            array_sse[0] = _vec4_1000_sse;// _mm_setr_ps(1, 0, 0, 0);
            array_sse[1] = _vec4_0100_sse;//_mm_setr_ps(0, 1, 0, 0);
            array_sse[2] = _vec4_0010_sse;//_mm_setr_ps(0, 0, 1, 0);
            array_sse[3] = _vec4_0001_sse;//_mm_setr_ps(0, 0, 1, 0);
#elif defined(ITK_NEON)
            array_neon[0] = _neon_1000;//(float32x4_t){ 1, 0, 0, 0 };
            array_neon[1] = _neon_0100;//(float32x4_t){ 0, 1, 0, 0 };
            array_neon[2] = _neon_0010;//(float32x4_t){ 0, 0, 1, 0 };
            array_neon[3] = _neon_0001;//(float32x4_t){ 0, 0, 0, 1 };
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
        /*constexpr __forceinline mat4() :array{ 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1 } {}*/
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
        __forceinline mat4(const _BaseType &value)
        {
#if defined(ITK_SSE2)
            __m128 data = _mm_set1_ps(value);
            array_sse[0] = data;
            array_sse[1] = data;
            array_sse[2] = data;
            array_sse[3] = data;
#elif defined(ITK_NEON)
            float32x4_t data = vset1(value);
            array_neon[0] = data;
            array_neon[1] = data;
            array_neon[2] = data;
            array_neon[3] = data;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
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
        __forceinline mat4(
            const _BaseType &a1, const _BaseType &b1, const _BaseType &c1, const _BaseType &d1,
            const _BaseType &a2, const _BaseType &b2, const _BaseType &c2, const _BaseType &d2,
            const _BaseType &a3, const _BaseType &b3, const _BaseType &c3, const _BaseType &d3,
            const _BaseType &a4, const _BaseType &b4, const _BaseType &c4, const _BaseType &d4)
        {
#if defined(ITK_SSE2)
            array_sse[0] = _mm_setr_ps(a1, a2, a3, a4);
            array_sse[1] = _mm_setr_ps(b1, b2, b3, b4);
            array_sse[2] = _mm_setr_ps(c1, c2, c3, c4);
            array_sse[3] = _mm_setr_ps(d1, d2, d3, d4);
#elif defined(ITK_NEON)
            array_neon[0] = (float32x4_t){a1, a2, a3, a4};
            array_neon[1] = (float32x4_t){b1, b2, b3, b4};
            array_neon[2] = (float32x4_t){c1, c2, c3, c4};
            array_neon[3] = (float32x4_t){d1, d2, d3, d4};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /*constexpr __forceinline mat4(const _BaseType& _a1, const _BaseType& _b1, const _BaseType& _c1, const _BaseType& _d1,
            const _BaseType& _a2, const _BaseType& _b2, const _BaseType& _c2, const _BaseType& _d2,
            const _BaseType& _a3, const _BaseType& _b3, const _BaseType& _c3, const _BaseType& _d3,
            const _BaseType& _a4, const _BaseType& _b4, const _BaseType& _c4, const _BaseType& _d4) :array{ _a1, _a2, _a3, _a4,
            _b1, _b2, _b3, _b4,
            _c1, _c2, _c3, _c4,
            _d1, _d2, _d3, _d4 } {}*/
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
#if defined(ITK_SSE2)
            array_sse[0] = m.array_sse[0];
            array_sse[1] = m.array_sse[1];
            array_sse[2] = m.array_sse[2];
            array_sse[3] = m.array_sse[3];
#elif defined(ITK_NEON)
            array_neon[0] = m.array_neon[0];
            array_neon[1] = m.array_neon[1];
            array_neon[2] = m.array_neon[2];
            array_neon[3] = m.array_neon[3];
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize the mat4 components from vec4 parameters
        ///
        /// \author Alessandro Ribeiro
        /// \param m Matrix to assign to the instance
        ///
        __forceinline mat4(const vec4_compatible_type &a, const vec4_compatible_type &b, const vec4_compatible_type &c, const vec4_compatible_type &d)
        {
#if defined(ITK_SSE2)
            array_sse[0] = a.array_sse;
            array_sse[1] = b.array_sse;
            array_sse[2] = c.array_sse;
            array_sse[3] = d.array_sse;
#elif defined(ITK_NEON)
            array_neon[0] = a.array_neon;
            array_neon[1] = b.array_neon;
            array_neon[2] = c.array_neon;
            array_neon[3] = d.array_neon;
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

#if defined(ITK_SSE2)
            __m128 array_sse_result[4];
            {
                __m128 e0 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 e1 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 e2 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 e3 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 m0 = _mm_mul_ps(array_sse[0], e0);
                __m128 m1 = _mm_mul_ps(array_sse[1], e1);
                __m128 m2 = _mm_mul_ps(array_sse[2], e2);
                __m128 m3 = _mm_mul_ps(array_sse[3], e3);

                __m128 a0 = _mm_add_ps(m0, m1);
                __m128 a1 = _mm_add_ps(m2, m3);
                __m128 a2 = _mm_add_ps(a0, a1);

                array_sse_result[0] = a2;
            }

            {
                __m128 e0 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 e1 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 e2 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 e3 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 m0 = _mm_mul_ps(array_sse[0], e0);
                __m128 m1 = _mm_mul_ps(array_sse[1], e1);
                __m128 m2 = _mm_mul_ps(array_sse[2], e2);
                __m128 m3 = _mm_mul_ps(array_sse[3], e3);

                __m128 a0 = _mm_add_ps(m0, m1);
                __m128 a1 = _mm_add_ps(m2, m3);
                __m128 a2 = _mm_add_ps(a0, a1);

                array_sse_result[1] = a2;
            }

            {
                __m128 e0 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 e1 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 e2 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 e3 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 m0 = _mm_mul_ps(array_sse[0], e0);
                __m128 m1 = _mm_mul_ps(array_sse[1], e1);
                __m128 m2 = _mm_mul_ps(array_sse[2], e2);
                __m128 m3 = _mm_mul_ps(array_sse[3], e3);

                __m128 a0 = _mm_add_ps(m0, m1);
                __m128 a1 = _mm_add_ps(m2, m3);
                __m128 a2 = _mm_add_ps(a0, a1);

                array_sse_result[2] = a2;
            }

            {
                //(__m128&)_mm_shuffle_epi32(__m128i&)in2[0], _MM_SHUFFLE(3, 3, 3, 3))
                __m128 e0 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 e1 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 e2 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 e3 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 m0 = _mm_mul_ps(array_sse[0], e0);
                __m128 m1 = _mm_mul_ps(array_sse[1], e1);
                __m128 m2 = _mm_mul_ps(array_sse[2], e2);
                __m128 m3 = _mm_mul_ps(array_sse[3], e3);

                __m128 a0 = _mm_add_ps(m0, m1);
                __m128 a1 = _mm_add_ps(m2, m3);
                __m128 a2 = _mm_add_ps(a0, a1);

                array_sse_result[3] = a2;
            }

            array_sse[0] = array_sse_result[0];
            array_sse[1] = array_sse_result[1];
            array_sse[2] = array_sse_result[2];
            array_sse[3] = array_sse_result[3];

#elif defined(ITK_NEON)

            float32x4_t array_neon_result[4];
            {
                float32x4_t e0 = vshuffle_0000(M.array_neon[0]);
                float32x4_t e1 = vshuffle_1111(M.array_neon[0]);
                float32x4_t e2 = vshuffle_2222(M.array_neon[0]);
                float32x4_t e3 = vshuffle_3333(M.array_neon[0]);

                float32x4_t m0 = vmulq_f32(array_neon[0], e0);
                float32x4_t m1 = vmulq_f32(array_neon[1], e1);
                float32x4_t m2 = vmulq_f32(array_neon[2], e2);
                float32x4_t m3 = vmulq_f32(array_neon[3], e3);

                float32x4_t a0 = vaddq_f32(m0, m1);
                float32x4_t a1 = vaddq_f32(m2, m3);
                float32x4_t a2 = vaddq_f32(a0, a1);

                array_neon_result[0] = a2;
            }

            {
                float32x4_t e0 = vshuffle_0000(M.array_neon[1]);
                float32x4_t e1 = vshuffle_1111(M.array_neon[1]);
                float32x4_t e2 = vshuffle_2222(M.array_neon[1]);
                float32x4_t e3 = vshuffle_3333(M.array_neon[1]);

                float32x4_t m0 = vmulq_f32(array_neon[0], e0);
                float32x4_t m1 = vmulq_f32(array_neon[1], e1);
                float32x4_t m2 = vmulq_f32(array_neon[2], e2);
                float32x4_t m3 = vmulq_f32(array_neon[3], e3);

                float32x4_t a0 = vaddq_f32(m0, m1);
                float32x4_t a1 = vaddq_f32(m2, m3);
                float32x4_t a2 = vaddq_f32(a0, a1);

                array_neon_result[1] = a2;
            }

            {
                float32x4_t e0 = vshuffle_0000(M.array_neon[2]);
                float32x4_t e1 = vshuffle_1111(M.array_neon[2]);
                float32x4_t e2 = vshuffle_2222(M.array_neon[2]);
                float32x4_t e3 = vshuffle_3333(M.array_neon[2]);

                float32x4_t m0 = vmulq_f32(array_neon[0], e0);
                float32x4_t m1 = vmulq_f32(array_neon[1], e1);
                float32x4_t m2 = vmulq_f32(array_neon[2], e2);
                float32x4_t m3 = vmulq_f32(array_neon[3], e3);

                float32x4_t a0 = vaddq_f32(m0, m1);
                float32x4_t a1 = vaddq_f32(m2, m3);
                float32x4_t a2 = vaddq_f32(a0, a1);

                array_neon_result[2] = a2;
            }

            {
                //(float32x4_t&)_mm_shuffle_epi32(float32x4_ti&)in2[0], _MM_SHUFFLE(3, 3, 3, 3))
                float32x4_t e0 = vshuffle_0000(M.array_neon[3]);
                float32x4_t e1 = vshuffle_1111(M.array_neon[3]);
                float32x4_t e2 = vshuffle_2222(M.array_neon[3]);
                float32x4_t e3 = vshuffle_3333(M.array_neon[3]);

                float32x4_t m0 = vmulq_f32(array_neon[0], e0);
                float32x4_t m1 = vmulq_f32(array_neon[1], e1);
                float32x4_t m2 = vmulq_f32(array_neon[2], e2);
                float32x4_t m3 = vmulq_f32(array_neon[3], e3);

                float32x4_t a0 = vaddq_f32(m0, m1);
                float32x4_t a1 = vaddq_f32(m2, m3);
                float32x4_t a2 = vaddq_f32(a0, a1);

                array_neon_result[3] = a2;
            }

            array_neon[0] = array_neon_result[0];
            array_neon[1] = array_neon_result[1];
            array_neon[2] = array_neon_result[2];
            array_neon[3] = array_neon_result[3];

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
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
        __forceinline bool operator==(const self_type &v) const
        {

#if defined(ITK_SSE2)

            // const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31

            __m128 diff_abs[4];
            diff_abs[0] = _mm_sub_ps(array_sse[0], v.array_sse[0]);
            diff_abs[0] = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs[0]); // abs

            diff_abs[1] = _mm_sub_ps(array_sse[1], v.array_sse[1]);
            diff_abs[1] = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs[1]); // abs

            diff_abs[2] = _mm_sub_ps(array_sse[2], v.array_sse[2]);
            diff_abs[2] = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs[2]); // abs

            diff_abs[3] = _mm_sub_ps(array_sse[3], v.array_sse[3]);
            diff_abs[3] = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs[3]); // abs

            __m128 accumulator_a = _mm_add_ps(diff_abs[0], diff_abs[1]);
            __m128 accumulator_b = _mm_add_ps(diff_abs[2], diff_abs[3]);

            __m128 accumulator = _mm_add_ps(accumulator_a, accumulator_b);

            accumulator = _mm_hadd_ps(accumulator, accumulator);
            accumulator = _mm_hadd_ps(accumulator, accumulator);

            return _mm_f32_(accumulator, 0) <= EPSILON<_BaseType>::high_precision;

#elif defined(ITK_NEON)

            float32x4_t diff_abs[4];
            diff_abs[0] = vsubq_f32(array_neon[0], v.array_neon[0]);
            diff_abs[0] = vabsq_f32(diff_abs[0]); // abs

            diff_abs[1] = vsubq_f32(array_neon[1], v.array_neon[1]);
            diff_abs[1] = vabsq_f32(diff_abs[1]); // abs

            diff_abs[2] = vsubq_f32(array_neon[2], v.array_neon[2]);
            diff_abs[2] = vabsq_f32(diff_abs[2]); // abs

            diff_abs[3] = vsubq_f32(array_neon[3], v.array_neon[3]);
            diff_abs[3] = vabsq_f32(diff_abs[3]); // abs

            float32x4_t accumulator_a = vaddq_f32(diff_abs[0], diff_abs[1]);
            float32x4_t accumulator_b = vaddq_f32(diff_abs[2], diff_abs[3]);

            float32x4_t acc_4_elements = vaddq_f32(accumulator_a, accumulator_b);

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
                          !(std::is_same<_InputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_InputType, _BaseType>::value),
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
#if defined(ITK_SSE2)
            array_sse[0] = _mm_add_ps(array_sse[0], v.array_sse[0]);
            array_sse[1] = _mm_add_ps(array_sse[1], v.array_sse[1]);
            array_sse[2] = _mm_add_ps(array_sse[2], v.array_sse[2]);
            array_sse[3] = _mm_add_ps(array_sse[3], v.array_sse[3]);

#elif defined(ITK_NEON)

            array_neon[0] = vaddq_f32(array_neon[0], v.array_neon[0]);
            array_neon[1] = vaddq_f32(array_neon[1], v.array_neon[1]);
            array_neon[2] = vaddq_f32(array_neon[2], v.array_neon[2]);
            array_neon[3] = vaddq_f32(array_neon[3], v.array_neon[3]);

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
#if defined(ITK_SSE2)
            array_sse[0] = _mm_sub_ps(array_sse[0], v.array_sse[0]);
            array_sse[1] = _mm_sub_ps(array_sse[1], v.array_sse[1]);
            array_sse[2] = _mm_sub_ps(array_sse[2], v.array_sse[2]);
            array_sse[3] = _mm_sub_ps(array_sse[3], v.array_sse[3]);

#elif defined(ITK_NEON)

            array_neon[0] = vsubq_f32(array_neon[0], v.array_neon[0]);
            array_neon[1] = vsubq_f32(array_neon[1], v.array_neon[1]);
            array_neon[2] = vsubq_f32(array_neon[2], v.array_neon[2]);
            array_neon[3] = vsubq_f32(array_neon[3], v.array_neon[3]);

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
#if defined(ITK_SSE2)
            self_type(_mm_xor_ps(_vec4_sign_mask_sse, array_sse[0]),
                      _mm_xor_ps(_vec4_sign_mask_sse, array_sse[1]),
                      _mm_xor_ps(_vec4_sign_mask_sse, array_sse[2]),
                      _mm_xor_ps(_vec4_sign_mask_sse, array_sse[3]));

#elif defined(ITK_NEON)

            //const float32x4_t neg = vset1(-1.0f);

            // return self_type(
            //     vmulq_f32(array_neon[0], _vec4_minus_one),
            //     vmulq_f32(array_neon[1], _vec4_minus_one),
            //     vmulq_f32(array_neon[2], _vec4_minus_one),
            //     vmulq_f32(array_neon[3], _vec4_minus_one));
            return self_type(
                vnegq_f32(array_neon[0]),
                vnegq_f32(array_neon[1]),
                vnegq_f32(array_neon[2]),
                vnegq_f32(array_neon[3]));

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
#if defined(ITK_SSE2)
            __m128 Fac0;
            {
                //    valType SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
                //    valType SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
                //    valType SubFactor06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
                //    valType SubFactor13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

                __m128 Swp0a = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(3, 3, 3, 3));
                __m128 Swp0b = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(2, 2, 2, 2));

                __m128 Swp00 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp03 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
                __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
                Fac0 = _mm_sub_ps(Mul00, Mul01);
            }

            __m128 Fac1;
            {
                //    valType SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
                //    valType SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
                //    valType SubFactor07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
                //    valType SubFactor14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

                __m128 Swp0a = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(3, 3, 3, 3));
                __m128 Swp0b = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(1, 1, 1, 1));

                __m128 Swp00 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp03 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
                __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
                Fac1 = _mm_sub_ps(Mul00, Mul01);
            }

            __m128 Fac2;
            {
                //    valType SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
                //    valType SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
                //    valType SubFactor08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
                //    valType SubFactor15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

                __m128 Swp0a = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 Swp0b = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(1, 1, 1, 1));

                __m128 Swp00 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp03 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(2, 2, 2, 2));

                __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
                __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
                Fac2 = _mm_sub_ps(Mul00, Mul01);
            }

            __m128 Fac3;
            {
                //    valType SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
                //    valType SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
                //    valType SubFactor09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
                //    valType SubFactor16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

                __m128 Swp0a = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(3, 3, 3, 3));
                __m128 Swp0b = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(0, 0, 0, 0));

                __m128 Swp00 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp03 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
                __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
                Fac3 = _mm_sub_ps(Mul00, Mul01);
            }

            __m128 Fac4;
            {
                //    valType SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
                //    valType SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
                //    valType SubFactor10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
                //    valType SubFactor17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

                __m128 Swp0a = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 Swp0b = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(0, 0, 0, 0));

                __m128 Swp00 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp03 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(2, 2, 2, 2));

                __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
                __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
                Fac4 = _mm_sub_ps(Mul00, Mul01);
            }

            __m128 Fac5;
            {
                //    valType SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
                //    valType SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
                //    valType SubFactor12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
                //    valType SubFactor18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

                __m128 Swp0a = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 Swp0b = _mm_shuffle_ps(array_sse[3], array_sse[2], _MM_SHUFFLE(0, 0, 0, 0));

                __m128 Swp00 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 Swp01 = _mm_shuffle_ps(Swp0a, Swp0a, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp02 = _mm_shuffle_ps(Swp0b, Swp0b, _MM_SHUFFLE(2, 0, 0, 0));
                __m128 Swp03 = _mm_shuffle_ps(array_sse[2], array_sse[1], _MM_SHUFFLE(1, 1, 1, 1));

                __m128 Mul00 = _mm_mul_ps(Swp00, Swp01);
                __m128 Mul01 = _mm_mul_ps(Swp02, Swp03);
                Fac5 = _mm_sub_ps(Mul00, Mul01);
            }

            // const __m128 SignA = _mm_set_ps( 1.0f,-1.0f, 1.0f,-1.0f);
            // const __m128 SignB = _mm_set_ps(-1.0f, 1.0f,-1.0f, 1.0f);
            const __m128 SignAMask = _mm_set_ps(0.0f, -0.0f, 0.0f, -0.0f);
            const __m128 SignBMask = _mm_set_ps(-0.0f, 0.0f, -0.0f, 0.0f);

            // m[1][0]
            // m[0][0]
            // m[0][0]
            // m[0][0]
            __m128 Temp0 = _mm_shuffle_ps(array_sse[1], array_sse[0], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 Vec0 = _mm_shuffle_ps(Temp0, Temp0, _MM_SHUFFLE(2, 2, 2, 0));

            // m[1][1]
            // m[0][1]
            // m[0][1]
            // m[0][1]
            __m128 Temp1 = _mm_shuffle_ps(array_sse[1], array_sse[0], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 Vec1 = _mm_shuffle_ps(Temp1, Temp1, _MM_SHUFFLE(2, 2, 2, 0));

            // m[1][2]
            // m[0][2]
            // m[0][2]
            // m[0][2]
            __m128 Temp2 = _mm_shuffle_ps(array_sse[1], array_sse[0], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 Vec2 = _mm_shuffle_ps(Temp2, Temp2, _MM_SHUFFLE(2, 2, 2, 0));

            // m[1][3]
            // m[0][3]
            // m[0][3]
            // m[0][3]
            __m128 Temp3 = _mm_shuffle_ps(array_sse[1], array_sse[0], _MM_SHUFFLE(3, 3, 3, 3));
            __m128 Vec3 = _mm_shuffle_ps(Temp3, Temp3, _MM_SHUFFLE(2, 2, 2, 0));

            // col0
            // + (Vec1[0] * Fac0[0] - Vec2[0] * Fac1[0] + Vec3[0] * Fac2[0]),
            // - (Vec1[1] * Fac0[1] - Vec2[1] * Fac1[1] + Vec3[1] * Fac2[1]),
            // + (Vec1[2] * Fac0[2] - Vec2[2] * Fac1[2] + Vec3[2] * Fac2[2]),
            // - (Vec1[3] * Fac0[3] - Vec2[3] * Fac1[3] + Vec3[3] * Fac2[3]),
            __m128 Mul00 = _mm_mul_ps(Vec1, Fac0);
            __m128 Mul01 = _mm_mul_ps(Vec2, Fac1);
            __m128 Mul02 = _mm_mul_ps(Vec3, Fac2);
            __m128 Sub00 = _mm_sub_ps(Mul00, Mul01);
            __m128 Add00 = _mm_add_ps(Sub00, Mul02);
            //__m128 Inv0 = _mm_mul_ps(SignB, Add00);
            __m128 Inv0 = _mm_xor_ps(SignBMask, Add00);

            // col1
            // - (Vec0[0] * Fac0[0] - Vec2[0] * Fac3[0] + Vec3[0] * Fac4[0]),
            // + (Vec0[0] * Fac0[1] - Vec2[1] * Fac3[1] + Vec3[1] * Fac4[1]),
            // - (Vec0[0] * Fac0[2] - Vec2[2] * Fac3[2] + Vec3[2] * Fac4[2]),
            // + (Vec0[0] * Fac0[3] - Vec2[3] * Fac3[3] + Vec3[3] * Fac4[3]),
            __m128 Mul03 = _mm_mul_ps(Vec0, Fac0);
            __m128 Mul04 = _mm_mul_ps(Vec2, Fac3);
            __m128 Mul05 = _mm_mul_ps(Vec3, Fac4);
            __m128 Sub01 = _mm_sub_ps(Mul03, Mul04);
            __m128 Add01 = _mm_add_ps(Sub01, Mul05);
            //__m128 Inv1 = _mm_mul_ps(SignA, Add01);
            __m128 Inv1 = _mm_xor_ps(SignAMask, Add01);

            // col2
            // + (Vec0[0] * Fac1[0] - Vec1[0] * Fac3[0] + Vec3[0] * Fac5[0]),
            // - (Vec0[0] * Fac1[1] - Vec1[1] * Fac3[1] + Vec3[1] * Fac5[1]),
            // + (Vec0[0] * Fac1[2] - Vec1[2] * Fac3[2] + Vec3[2] * Fac5[2]),
            // - (Vec0[0] * Fac1[3] - Vec1[3] * Fac3[3] + Vec3[3] * Fac5[3]),
            __m128 Mul06 = _mm_mul_ps(Vec0, Fac1);
            __m128 Mul07 = _mm_mul_ps(Vec1, Fac3);
            __m128 Mul08 = _mm_mul_ps(Vec3, Fac5);
            __m128 Sub02 = _mm_sub_ps(Mul06, Mul07);
            __m128 Add02 = _mm_add_ps(Sub02, Mul08);
            //__m128 Inv2 = _mm_mul_ps(SignB, Add02);
            __m128 Inv2 = _mm_xor_ps(SignBMask, Add02);

            // col3
            // - (Vec1[0] * Fac2[0] - Vec1[0] * Fac4[0] + Vec2[0] * Fac5[0]),
            // + (Vec1[0] * Fac2[1] - Vec1[1] * Fac4[1] + Vec2[1] * Fac5[1]),
            // - (Vec1[0] * Fac2[2] - Vec1[2] * Fac4[2] + Vec2[2] * Fac5[2]),
            // + (Vec1[0] * Fac2[3] - Vec1[3] * Fac4[3] + Vec2[3] * Fac5[3]));
            __m128 Mul09 = _mm_mul_ps(Vec0, Fac2);
            __m128 Mul10 = _mm_mul_ps(Vec1, Fac4);
            __m128 Mul11 = _mm_mul_ps(Vec2, Fac5);
            __m128 Sub03 = _mm_sub_ps(Mul09, Mul10);
            __m128 Add03 = _mm_add_ps(Sub03, Mul11);
            //__m128 Inv3 = _mm_mul_ps(SignA, Add03);
            __m128 Inv3 = _mm_xor_ps(SignAMask, Add03);

            __m128 Row0 = _mm_shuffle_ps(Inv0, Inv1, _MM_SHUFFLE(0, 0, 0, 0));
            __m128 Row1 = _mm_shuffle_ps(Inv2, Inv3, _MM_SHUFFLE(0, 0, 0, 0));
            __m128 Row2 = _mm_shuffle_ps(Row0, Row1, _MM_SHUFFLE(2, 0, 2, 0));

            //    valType Determinant = m[0][0] * Inverse[0][0]
            //                        + m[0][1] * Inverse[1][0]
            //                        + m[0][2] * Inverse[2][0]
            //                        + m[0][3] * Inverse[3][0];
            __m128 Det0 = dot_sse_4(array_sse[0], Row2);
            //__m128 Det0 = _mm_dp_ps( array_sse[0], Row2, 0xff );

            //MATH_CORE_THROW_RUNTIME_ERROR(_mm_f32_(Det0, 0) == 0, "trying to invert a singular matrix\n");
            // if (_mm_f32_( Det0, 0 ) == 0){
            //     fprintf(stderr,"trying to invert a singular matrix\n");
            //     exit(-1);
            // }

            _BaseType det = _mm_f32_(Det0, 0);
            det = OP<_BaseType>::maximum(det,FloatTypeInfo<_BaseType>::min);

            __m128 Rcp0 = _mm_set1_ps(1.0f / det);
            //__m128 Rcp0 = _mm_div_ps(_mm_set1_ps(1.0f), Det0);
            //__m128 Rcp0 = _mm_rcp_ps(Det0);

            //    Inverse /= Determinant;
            return self_type(_mm_mul_ps(Inv0, Rcp0),
                             _mm_mul_ps(Inv1, Rcp0),
                             _mm_mul_ps(Inv2, Rcp0),
                             _mm_mul_ps(Inv3, Rcp0));

#elif defined(ITK_NEON)

            float32x4_t Fac0;
            {
                //    valType SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
                //    valType SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
                //    valType SubFactor06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
                //    valType SubFactor13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

                float32x4_t Swp0a = vshuffle_3333(array_neon[3], array_neon[2]);
                float32x4_t Swp0b = vshuffle_2222(array_neon[3], array_neon[2]);

                float32x4_t Swp00 = vshuffle_2222(array_neon[2], array_neon[1]);
                float32x4_t Swp01 = vshuffle_2000(Swp0a);
                float32x4_t Swp02 = vshuffle_2000(Swp0b);
                float32x4_t Swp03 = vshuffle_3333(array_neon[2], array_neon[1]);

                float32x4_t Mul00 = vmulq_f32(Swp00, Swp01);
                float32x4_t Mul01 = vmulq_f32(Swp02, Swp03);
                Fac0 = vsubq_f32(Mul00, Mul01);
            }

            float32x4_t Fac1;
            {
                //    valType SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
                //    valType SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
                //    valType SubFactor07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
                //    valType SubFactor14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

                float32x4_t Swp0a = vshuffle_3333(array_neon[3], array_neon[2]);
                float32x4_t Swp0b = vshuffle_1111(array_neon[3], array_neon[2]);

                float32x4_t Swp00 = vshuffle_1111(array_neon[2], array_neon[1]);
                float32x4_t Swp01 = vshuffle_2000(Swp0a);
                float32x4_t Swp02 = vshuffle_2000(Swp0b);
                float32x4_t Swp03 = vshuffle_3333(array_neon[2], array_neon[1]);

                float32x4_t Mul00 = vmulq_f32(Swp00, Swp01);
                float32x4_t Mul01 = vmulq_f32(Swp02, Swp03);
                Fac1 = vsubq_f32(Mul00, Mul01);
            }

            float32x4_t Fac2;
            {
                //    valType SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
                //    valType SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
                //    valType SubFactor08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
                //    valType SubFactor15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

                float32x4_t Swp0a = vshuffle_2222(array_neon[3], array_neon[2]);
                float32x4_t Swp0b = vshuffle_1111(array_neon[3], array_neon[2]);

                float32x4_t Swp00 = vshuffle_1111(array_neon[2], array_neon[1]);
                float32x4_t Swp01 = vshuffle_2000(Swp0a);
                float32x4_t Swp02 = vshuffle_2000(Swp0b);
                float32x4_t Swp03 = vshuffle_2222(array_neon[2], array_neon[1]);

                float32x4_t Mul00 = vmulq_f32(Swp00, Swp01);
                float32x4_t Mul01 = vmulq_f32(Swp02, Swp03);
                Fac2 = vsubq_f32(Mul00, Mul01);
            }

            float32x4_t Fac3;
            {
                //    valType SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
                //    valType SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
                //    valType SubFactor09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
                //    valType SubFactor16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

                float32x4_t Swp0a = vshuffle_3333(array_neon[3], array_neon[2]);
                float32x4_t Swp0b = vshuffle_0000(array_neon[3], array_neon[2]);

                float32x4_t Swp00 = vshuffle_0000(array_neon[2], array_neon[1]);
                float32x4_t Swp01 = vshuffle_2000(Swp0a);
                float32x4_t Swp02 = vshuffle_2000(Swp0b);
                float32x4_t Swp03 = vshuffle_3333(array_neon[2], array_neon[1]);

                float32x4_t Mul00 = vmulq_f32(Swp00, Swp01);
                float32x4_t Mul01 = vmulq_f32(Swp02, Swp03);
                Fac3 = vsubq_f32(Mul00, Mul01);
            }

            float32x4_t Fac4;
            {
                //    valType SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
                //    valType SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
                //    valType SubFactor10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
                //    valType SubFactor17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

                float32x4_t Swp0a = vshuffle_2222(array_neon[3], array_neon[2]);
                float32x4_t Swp0b = vshuffle_0000(array_neon[3], array_neon[2]);

                float32x4_t Swp00 = vshuffle_0000(array_neon[2], array_neon[1]);
                float32x4_t Swp01 = vshuffle_2000(Swp0a);
                float32x4_t Swp02 = vshuffle_2000(Swp0b);
                float32x4_t Swp03 = vshuffle_2222(array_neon[2], array_neon[1]);

                float32x4_t Mul00 = vmulq_f32(Swp00, Swp01);
                float32x4_t Mul01 = vmulq_f32(Swp02, Swp03);
                Fac4 = vsubq_f32(Mul00, Mul01);
            }

            float32x4_t Fac5;
            {
                //    valType SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
                //    valType SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
                //    valType SubFactor12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
                //    valType SubFactor18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

                float32x4_t Swp0a = vshuffle_1111(array_neon[3], array_neon[2]);
                float32x4_t Swp0b = vshuffle_0000(array_neon[3], array_neon[2]);

                float32x4_t Swp00 = vshuffle_0000(array_neon[2], array_neon[1]);
                float32x4_t Swp01 = vshuffle_2000(Swp0a);
                float32x4_t Swp02 = vshuffle_2000(Swp0b);
                float32x4_t Swp03 = vshuffle_1111(array_neon[2], array_neon[1]);

                float32x4_t Mul00 = vmulq_f32(Swp00, Swp01);
                float32x4_t Mul01 = vmulq_f32(Swp02, Swp03);
                Fac5 = vsubq_f32(Mul00, Mul01);
            }

            const float32x4_t SignA = (float32x4_t){1.0f, -1.0f, 1.0f, -1.0f};
            const float32x4_t SignB = (float32x4_t){-1.0f, 1.0f, -1.0f, 1.0f};

            // m[1][0]
            // m[0][0]
            // m[0][0]
            // m[0][0]
            float32x4_t Temp0 = vshuffle_0000(array_neon[1], array_neon[0]);
            float32x4_t Vec0 = vshuffle_2220(Temp0);

            // m[1][1]
            // m[0][1]
            // m[0][1]
            // m[0][1]
            float32x4_t Temp1 = vshuffle_1111(array_neon[1], array_neon[0]);
            float32x4_t Vec1 = vshuffle_2220(Temp1);

            // m[1][2]
            // m[0][2]
            // m[0][2]
            // m[0][2]
            float32x4_t Temp2 = vshuffle_2222(array_neon[1], array_neon[0]);
            float32x4_t Vec2 = vshuffle_2220(Temp2);

            // m[1][3]
            // m[0][3]
            // m[0][3]
            // m[0][3]
            float32x4_t Temp3 = vshuffle_3333(array_neon[1], array_neon[0]);
            float32x4_t Vec3 = vshuffle_2220(Temp3);

            // col0
            // + (Vec1[0] * Fac0[0] - Vec2[0] * Fac1[0] + Vec3[0] * Fac2[0]),
            // - (Vec1[1] * Fac0[1] - Vec2[1] * Fac1[1] + Vec3[1] * Fac2[1]),
            // + (Vec1[2] * Fac0[2] - Vec2[2] * Fac1[2] + Vec3[2] * Fac2[2]),
            // - (Vec1[3] * Fac0[3] - Vec2[3] * Fac1[3] + Vec3[3] * Fac2[3]),
            float32x4_t Mul00 = vmulq_f32(Vec1, Fac0);
            float32x4_t Mul01 = vmulq_f32(Vec2, Fac1);
            float32x4_t Mul02 = vmulq_f32(Vec3, Fac2);
            float32x4_t Sub00 = vsubq_f32(Mul00, Mul01);
            float32x4_t Add00 = vaddq_f32(Sub00, Mul02);
            float32x4_t Inv0 = vmulq_f32(SignB, Add00);

            // col1
            // - (Vec0[0] * Fac0[0] - Vec2[0] * Fac3[0] + Vec3[0] * Fac4[0]),
            // + (Vec0[0] * Fac0[1] - Vec2[1] * Fac3[1] + Vec3[1] * Fac4[1]),
            // - (Vec0[0] * Fac0[2] - Vec2[2] * Fac3[2] + Vec3[2] * Fac4[2]),
            // + (Vec0[0] * Fac0[3] - Vec2[3] * Fac3[3] + Vec3[3] * Fac4[3]),
            float32x4_t Mul03 = vmulq_f32(Vec0, Fac0);
            float32x4_t Mul04 = vmulq_f32(Vec2, Fac3);
            float32x4_t Mul05 = vmulq_f32(Vec3, Fac4);
            float32x4_t Sub01 = vsubq_f32(Mul03, Mul04);
            float32x4_t Add01 = vaddq_f32(Sub01, Mul05);
            float32x4_t Inv1 = vmulq_f32(SignA, Add01);

            // col2
            // + (Vec0[0] * Fac1[0] - Vec1[0] * Fac3[0] + Vec3[0] * Fac5[0]),
            // - (Vec0[0] * Fac1[1] - Vec1[1] * Fac3[1] + Vec3[1] * Fac5[1]),
            // + (Vec0[0] * Fac1[2] - Vec1[2] * Fac3[2] + Vec3[2] * Fac5[2]),
            // - (Vec0[0] * Fac1[3] - Vec1[3] * Fac3[3] + Vec3[3] * Fac5[3]),
            float32x4_t Mul06 = vmulq_f32(Vec0, Fac1);
            float32x4_t Mul07 = vmulq_f32(Vec1, Fac3);
            float32x4_t Mul08 = vmulq_f32(Vec3, Fac5);
            float32x4_t Sub02 = vsubq_f32(Mul06, Mul07);
            float32x4_t Add02 = vaddq_f32(Sub02, Mul08);
            float32x4_t Inv2 = vmulq_f32(SignB, Add02);

            // col3
            // - (Vec1[0] * Fac2[0] - Vec1[0] * Fac4[0] + Vec2[0] * Fac5[0]),
            // + (Vec1[0] * Fac2[1] - Vec1[1] * Fac4[1] + Vec2[1] * Fac5[1]),
            // - (Vec1[0] * Fac2[2] - Vec1[2] * Fac4[2] + Vec2[2] * Fac5[2]),
            // + (Vec1[0] * Fac2[3] - Vec1[3] * Fac4[3] + Vec2[3] * Fac5[3]));
            float32x4_t Mul09 = vmulq_f32(Vec0, Fac2);
            float32x4_t Mul10 = vmulq_f32(Vec1, Fac4);
            float32x4_t Mul11 = vmulq_f32(Vec2, Fac5);
            float32x4_t Sub03 = vsubq_f32(Mul09, Mul10);
            float32x4_t Add03 = vaddq_f32(Sub03, Mul11);
            float32x4_t Inv3 = vmulq_f32(SignA, Add03);

            float32x4_t Row0 = vshuffle_0000(Inv0, Inv1);
            float32x4_t Row1 = vshuffle_0000(Inv2, Inv3);
            float32x4_t Row2 = vshuffle_2020(Row0, Row1);

            //    valType Determinant = m[0][0] * Inverse[0][0]
            //                        + m[0][1] * Inverse[1][0]
            //                        + m[0][2] * Inverse[2][0]
            //                        + m[0][3] * Inverse[3][0];
            float32x4_t Det0 = dot_neon_4(array_neon[0], Row2);

            //MATH_CORE_THROW_RUNTIME_ERROR(Det0[0] == 0, "trying to invert a singular matrix\n");
            // if (Det0[0] == 0){
            //     fprintf(stderr,"trying to invert a singular matrix\n");
            //     exit(-1);
            // }

            _BaseType det = Det0[0];
            det = OP<_BaseType>::maximum(det,FloatTypeInfo<_BaseType>::min);

            float32x4_t Rcp0 = vset1(1.0f / det); //_mm_div_ps(_mm_set1_ps(1.0f), Det0);
            //__m128 Rcp0 = _mm_rcp_ps(Det0);

            //    Inverse /= Determinant;
            return self_type(vmulq_f32(Inv0, Rcp0),
                             vmulq_f32(Inv1, Rcp0),
                             vmulq_f32(Inv2, Rcp0),
                             vmulq_f32(Inv3, Rcp0));

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        //
        // can be used for inverse
        // transpose rotation+scale mat4 representations
        //
        __forceinline mat4 inverse_transpose_3x3() const
        {

            // optimal ver 2
            // 
#if defined(ITK_SSE2)

            self_type result(
                (vec4_compatible_type(c3, a3, b3, 0) * vec4_compatible_type(b2, c2, a2, 0) - vec4_compatible_type(b3, c3, a3, 0) * vec4_compatible_type(c2, a2, b2, 0)),
                (vec4_compatible_type(b3, c3, a3, 0) * vec4_compatible_type(c1, a1, b1, 0) - vec4_compatible_type(c3, a3, b3, 0) * vec4_compatible_type(b1, c1, a1, 0)),
                (vec4_compatible_type(c2, a2, b2, 0) * vec4_compatible_type(b1, c1, a1, 0) - vec4_compatible_type(b2, c2, a2, 0) * vec4_compatible_type(c1, a1, b1, 0)),
                _vec4_0001_sse
            );

            // transpose
            __m128 tmp0 = _mm_shuffle_ps(result.array_sse[0], result.array_sse[1], _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp2 = _mm_shuffle_ps(result.array_sse[0], result.array_sse[1], _MM_SHUFFLE(3, 2, 3, 2));
            __m128 tmp1 = _mm_shuffle_ps(result.array_sse[2], result.array_sse[3], _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp3 = _mm_shuffle_ps(result.array_sse[2], result.array_sse[3], _MM_SHUFFLE(3, 2, 3, 2));


            __m128 _a = _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0));
            __m128 _b = _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1));
            __m128 _c = _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0));
            __m128 _d = _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(3, 1, 3, 1));

            _BaseType det = _mm_f32_(dot_sse_3(array_sse[0], _a), 0);

            // check det
            //MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
            det = OP<_BaseType>::maximum(det,FloatTypeInfo<_BaseType>::min);
            det = (_BaseType)1.0 / det;

            __m128 _det_result = _mm_set_ps1(det);

            _a = _mm_mul_ps(_a, _det_result);
            _b = _mm_mul_ps(_b, _det_result);
            _c = _mm_mul_ps(_c, _det_result);

            return self_type(
                _a,
                _b,
                _c,
                _vec4_0001_sse
            );

#elif defined(ITK_NEON)

            self_type result(
                (vec4_compatible_type(c3, a3, b3, 0) * vec4_compatible_type(b2, c2, a2, 0) - vec4_compatible_type(b3, c3, a3, 0) * vec4_compatible_type(c2, a2, b2, 0)),
                (vec4_compatible_type(b3, c3, a3, 0) * vec4_compatible_type(c1, a1, b1, 0) - vec4_compatible_type(c3, a3, b3, 0) * vec4_compatible_type(b1, c1, a1, 0)),
                (vec4_compatible_type(c2, a2, b2, 0) * vec4_compatible_type(b1, c1, a1, 0) - vec4_compatible_type(b2, c2, a2, 0) * vec4_compatible_type(c1, a1, b1, 0)),
                _neon_0001
            );
            

            float32x4x2_t ab = vtrnq_f32(result.array_neon[0], result.array_neon[1]);
            float32x4x2_t cd = vtrnq_f32(result.array_neon[2], result.array_neon[3]);
            float32x4_t a_ = vcombine_f32(vget_low_f32(ab.val[0]), vget_low_f32(cd.val[0]));
            float32x4_t b_ = vcombine_f32(vget_low_f32(ab.val[1]), vget_low_f32(cd.val[1]));
            float32x4_t c_ = vcombine_f32(vget_high_f32(ab.val[0]), vget_high_f32(cd.val[0]));
            float32x4_t d_ = vcombine_f32(vget_high_f32(ab.val[1]), vget_high_f32(cd.val[1]));

            _BaseType det = dot_neon_3(array_neon[0], a_)[0];

            // check det
            //MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
            det = OP<_BaseType>::maximum(det,FloatTypeInfo<_BaseType>::min);
            det = (_BaseType)1.0 / det;
            
            float32x4_t _det_result = vset1(det);

            a_ = vmulq_f32(a_, _det_result);
            b_ = vmulq_f32(b_, _det_result);
            c_ = vmulq_f32(c_, _det_result);

            return self_type(
                a_,
                b_,
                c_,
                _neon_0001
            );

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
            __m128 tmp = _mm_set1_ps(v);

            array_sse[0] = _mm_add_ps(array_sse[0], tmp);
            array_sse[1] = _mm_add_ps(array_sse[1], tmp);
            array_sse[2] = _mm_add_ps(array_sse[2], tmp);
            array_sse[3] = _mm_add_ps(array_sse[3], tmp);

#elif defined(ITK_NEON)

            float32x4_t tmp = vset1(v);

            array_neon[0] = vaddq_f32(array_neon[0], tmp);
            array_neon[1] = vaddq_f32(array_neon[1], tmp);
            array_neon[2] = vaddq_f32(array_neon[2], tmp);
            array_neon[3] = vaddq_f32(array_neon[3], tmp);

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
#if defined(ITK_SSE2)
            __m128 tmp = _mm_set1_ps(v);

            array_sse[0] = _mm_sub_ps(array_sse[0], tmp);
            array_sse[1] = _mm_sub_ps(array_sse[1], tmp);
            array_sse[2] = _mm_sub_ps(array_sse[2], tmp);
            array_sse[3] = _mm_sub_ps(array_sse[3], tmp);

#elif defined(ITK_NEON)

            float32x4_t tmp = vset1(v);

            array_neon[0] = vsubq_f32(array_neon[0], tmp);
            array_neon[1] = vsubq_f32(array_neon[1], tmp);
            array_neon[2] = vsubq_f32(array_neon[2], tmp);
            array_neon[3] = vsubq_f32(array_neon[3], tmp);

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
#if defined(ITK_SSE2)
            __m128 tmp = _mm_set1_ps(v);

            array_sse[0] = _mm_mul_ps(array_sse[0], tmp);
            array_sse[1] = _mm_mul_ps(array_sse[1], tmp);
            array_sse[2] = _mm_mul_ps(array_sse[2], tmp);
            array_sse[3] = _mm_mul_ps(array_sse[3], tmp);

#elif defined(ITK_NEON)

            float32x4_t tmp = vset1(v);

            array_neon[0] = vmulq_f32(array_neon[0], tmp);
            array_neon[1] = vmulq_f32(array_neon[1], tmp);
            array_neon[2] = vmulq_f32(array_neon[2], tmp);
            array_neon[3] = vmulq_f32(array_neon[3], tmp);

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
#if defined(ITK_SSE2)
            __m128 tmp = _mm_set1_ps(v);

            array_sse[0] = _mm_div_ps(array_sse[0], tmp);
            array_sse[1] = _mm_div_ps(array_sse[1], tmp);
            array_sse[2] = _mm_div_ps(array_sse[2], tmp);
            array_sse[3] = _mm_div_ps(array_sse[3], tmp);

#elif defined(ITK_NEON)

            float32x4_t tmp = vset1(1.0f / v);

            array_neon[0] = vmulq_f32(array_neon[0], tmp);
            array_neon[1] = vmulq_f32(array_neon[1], tmp);
            array_neon[2] = vmulq_f32(array_neon[2], tmp);
            array_neon[3] = vmulq_f32(array_neon[3], tmp);

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return *this;
        }
    };
}
