#pragma once

#include "../vec2.h"

#include "vec3_base.h"

namespace MathCore
{

    /// \brief Vector 3D (vec3)
    ///
    /// Stores three components(x,y,z) to represent a tridimensional vector. <br/>
    /// It can be used as points or vectors in 3D.
    /// \warning The class is not designed to represent 2D homogeneous space.
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// It is possible to use any arithmetic with vec3 and float combinations.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aRibeiroCore/aRibeiroCore.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 a, b, result;
    ///
    /// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename _BaseType, typename _SimdType>
    class vec3<_BaseType, _SimdType,
               typename std::enable_if<
                   std::is_same<_SimdType, SIMD_TYPE::NONE>::value>::type>
    {
        using self_type = vec3<_BaseType, _SimdType>;
        using vec2_compatible_type = vec2<_BaseType, _SimdType>;

    public:
        
        static constexpr int array_count = 3;
        using type = self_type;
		using element_type = _BaseType;

        union
        {
            _BaseType array[3];
            struct
            {
                _BaseType x, y, z;
            };
            struct
            {
                _BaseType r, g, b;
            };
        };

        /// \brief Construct a ZERO vec3 class
        ///
        /// The ZERO vec3 class have the point information in the origin (x=0,y=0,z=0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        /*__forceinline vec3()
        {
            x = y = z = _BaseType();
        }*/
        constexpr __forceinline vec3() :array{0, 0, 0} {}
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components with the same float value (by scalar)
        ///
        /// X = v, Y = v and Z = v
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3( 0.5f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to initialize the components
        ///
        /*__forceinline vec3(const _BaseType &v)
        {
            x = y = z = v;
        }*/
        constexpr __forceinline vec3(const _BaseType& _v) :array{_v, _v, _v} {}
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from the parameters
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3( 0.1f, 0.2f, 0.3f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        /// \param z Value to assign to the Z component of the vector
        ///
        /*__forceinline vec3(const _BaseType &x, const _BaseType &y, const _BaseType &z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }*/
        constexpr __forceinline vec3(const _BaseType& _x, const _BaseType& _y, const _BaseType& _z) :array{_x, _y, _z} {}
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from a vec2 xy and an isolated z value
        ///
        /// this->xy = xy <br />
        /// this->z = z
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3( vec2( 0.1f, 0.2f ), 0.3f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param xy Vector 2D to assign to the components x and y of the instance respectively
        /// \param z Value to assign to the component z of the instance
        ///
        /*__forceinline vec3(const vec2_compatible_type &xy, const _BaseType &z)
        {
            x = xy.x;
            y = xy.y;
            this->z = z;
        }*/
        constexpr __forceinline vec3(const vec2_compatible_type& _xy, const _BaseType& _z) :array{_xy.x, _xy.y, _z} {}
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from an isolated x value and a vec2 yz
        ///
        /// this->x = x <br />
        /// this->yz = yz
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3( 0.1f, vec2( 0.2f, 0.3f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the component x of the instance
        /// \param yz Vector 2D to assign to the components y and z of the instance respectively
        ///
        /*__forceinline vec3(const _BaseType &x, const vec2_compatible_type &yz)
        {
            this->x = x;
            y = yz.x;
            z = yz.y;
        }*/
        constexpr __forceinline vec3(const _BaseType& _x, const vec2_compatible_type& _yz) :array{x, _yz.x, _yz.y} {}
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from other vec3 instance by copy
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec_source;
        ///
        /// vec3 vec = vec3( vec_source );
        ///
        /// vec3 veca = vec_source;
        ///
        /// vec3 vecb;
        /// vecb = vec_source;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to assign to the instance
        ///
        __forceinline vec3(const self_type &v)
        {
            *this = v;
        }
        //constexpr __forceinline vec3(const self_type& _v) :array{_v.x, _v.y, _v.z} {}
        /// \brief Constructs a tridimensional Vector from the subtraction b-a
        ///
        /// Initialize the vec3 components from two other vectors using the equation: <br />
        /// this = b - a
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec_a, vec_b;
        ///
        /// vec3 vec_a_to_b = vec3( vec_a, vec_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Orign vector
        /// \param b Destiny vector
        ///
        /*__forceinline vec3(const self_type &a, const self_type &b)
        {
            x = b.x - a.x;
            y = b.y - a.y;
            z = b.z - a.z;
        }*/
        constexpr __forceinline vec3(const self_type& a, const self_type& b) :array{b.x - a.x, b.y - a.y, b.z - a.z} {}
        /// \brief Compare vectors considering #EPSILON (equal)
        ///
        /// Compare two vectors using #EPSILON to see if they are the same.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec_a, vec_b;
        ///
        /// if ( vec_a == vec_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to compare against
        /// \return true if the values are the same considering #EPSILON
        ///
        template<class _Type = _BaseType,
            typename std::enable_if<
                std::is_floating_point<_Type>::value
            ,bool>::type = true>
        __forceinline bool operator==(const self_type &v) const
        {
            _BaseType accumulator = _BaseType();
            for (int i = 0; i < 3; i++)
                accumulator += OP<_BaseType>::abs(array[i] - v.array[i]);
                //accumulator += (std::abs)(array[i] - v.array[i]);
            return accumulator <= EPSILON<_BaseType>::high_precision;
        }

        template<class _Type = _BaseType,
            typename std::enable_if<
                std::is_integral<_Type>::value
            ,bool>::type = true>
        __forceinline bool operator==(const self_type &v) const
        {
            for (int i = 0; i < 3; i++)
                if (array[i] != v.array[i])
                    return false;
            return true;
        }

        // inter SIMD types converting...
        template <typename _InputType, typename _InputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          (!std::is_same<_InputSimdTypeAux, _SimdType>::value||
                          !std::is_same<_InputType, _BaseType>::value),
                      bool>::type = true>
        __forceinline void operator=(const vec3<_InputType, _InputSimdTypeAux> &vec)
        {
            *this = self_type((_BaseType)vec.x, (_BaseType)vec.y, (_BaseType)vec.z);
        }
        // inter SIMD types converting...
        template <typename _OutputType, typename _OutputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_BaseType, _OutputType>::value &&
                      !(std::is_same<_OutputSimdTypeAux, _SimdType>::value &&
                          std::is_same<_OutputType, _BaseType>::value),
                      bool>::type = true>
        __forceinline operator vec3<_OutputType, _OutputSimdTypeAux>() const
        {
            return vec3<_OutputType, _OutputSimdTypeAux>(
                (_OutputType)x, (_OutputType)y, (_OutputType)z);
        }

        /// \brief Compare vectors considering #EPSILON (not equal)
        ///
        /// Compare two vectors using #EPSILON to see if they are the same.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec_a, vec_b;
        ///
        /// if ( vec_a != vec_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to compare against
        /// \return true if the values are not the same considering #EPSILON
        ///
        __forceinline bool operator!=(const self_type &v) const
        {
            return !((*this) == v);
        }
        /// \brief Component-wise sum (add) operator overload
        ///
        /// Increment the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec, vec_b;
        ///
        /// vec += vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to increment the current vector instance
        /// \return A reference to the current instance after the increment
        ///
        __forceinline self_type &operator+=(const self_type &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return (*this);
        }
        /// \brief Component-wise subtract operator overload
        ///
        /// Decrement the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec, vec_b;
        ///
        /// vec -= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to decrement the current vector instance
        /// \return A reference to the current instance after the decrement
        ///
        __forceinline self_type &operator-=(const self_type &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return (*this);
        }
        /// \brief Component-wise minus operator overload
        ///
        /// Negates the vector components with the operator minus
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec;
        ///
        /// vec = -vec;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A copy of the current instance after the negation operation
        ///
        __forceinline self_type operator-() const
        {
            return self_type(-x, -y, -z);
        }
        /// \brief Component-wise multiply operator overload
        ///
        /// Multiply the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec, vec_b;
        ///
        /// vec *= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to multiply the current vector instance
        /// \return A reference to the current instance after the multiplication
        ///
        __forceinline self_type &operator*=(const self_type &v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return (*this);
        }
        /// \brief Component-wise division operator overload
        ///
        /// Divides the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec, vec_b;
        ///
        /// vec /= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to divide the current vector instance
        /// \return A reference to the current instance after the division
        ///
        __forceinline self_type &operator/=(const self_type &v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return (*this);
        }
        /// \brief Single value increment (add, sum) operator overload
        ///
        /// Increment the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec;
        ///
        /// vec += 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to increment all components of the current vector instance
        /// \return A reference to the current instance after the increment
        ///
        __forceinline self_type &operator+=(const _BaseType &v)
        {
            x += v;
            y += v;
            z += v;
            return (*this);
        }
        /// \brief Single value decrement (subtract) operator overload
        ///
        /// Decrement the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec;
        ///
        /// vec -= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to decrement all components of the current vector instance
        /// \return A reference to the current instance after the decrement
        ///
        __forceinline self_type &operator-=(const _BaseType &v)
        {
            x -= v;
            y -= v;
            z -= v;
            return (*this);
        }
        /// \brief Single value multiply operator overload
        ///
        /// Decrement the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec;
        ///
        /// vec *= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to decrement all components of the current vector instance
        /// \return A reference to the current instance after the decrement
        ///
        __forceinline self_type &operator*=(const _BaseType &v)
        {
            x *= v;
            y *= v;
            z *= v;
            return (*this);
        }
        /// \brief Single value division operator overload
        ///
        /// Divides the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec;
        ///
        /// vec /= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to divide all components of the current vector instance
        /// \return A reference to the current instance after the division
        ///
        __forceinline self_type &operator/=(const _BaseType &v)
        {
            x /= v;
            y /= v;
            z /= v;
            return (*this);
        }
        /// \brief Index the components of the vec3 as a C array
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec;
        ///
        /// float x = vec[0];
        ///
        /// vec[1] = 1.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The index of the components starting by 0
        /// \return A reference to the element at the index v
        ///
        __forceinline _BaseType &operator[](const int v)
        {
            return array[v];
        }

        /// \brief Index the components of the vec3 as a C array
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// void process_vec( const vec3 &vec ) {
        ///     float x = vec[0];
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The index of the components starting by 0
        /// \return A reference to the element at the index v
        ///
        __forceinline const _BaseType &operator[](const int v) const
        {
            return array[v];
        }
    };

}
