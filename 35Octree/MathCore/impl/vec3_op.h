#pragma once

#include "vec3_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

#include "quat_op.h"

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<vec3<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value>::type,
              _algorithm>
    {
    private:
        using type3 = vec3<_type, _simd>;
        using self_type = OP<type3>;
        using quatT = quat<_type, _simd>;

    public:
        /// \brief Clamp values in a component wise fashion
        ///
        /// For each component of the vector, evaluate:
        /// ```
        ///     if min < value then return min
        ///     if max > value then return max
        ///     else return value
        /// ```
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result;
        /// // result = vec3 ( 50, 3, 10 )
        /// result = clamp( vec3( 300 , 3, 5 ), vec3( 0, -1, 10 ) , vec3( 50, 5, 15 ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param value The value to evaluate
        /// \param min The min threshold
        /// \param max The max threshold
        /// \return The evaluated value
        ///
        static __forceinline type3 clamp(const type3 &value, const type3 &min, const type3 &max) noexcept
        {
            return type3(
                OP<_type>::clamp(value.x, min.x, max.x),
                OP<_type>::clamp(value.y, min.y, max.y),
                OP<_type>::clamp(value.z, min.z, max.z));
        }

        /// \brief Computes the dot product between two vectors
        ///
        /// The dot product is a single value computed from the two vectors.
        ///
        /// <pre>
        /// It can be interpreted as:
        ///
        /// -The squared length of the vector modulos
        ///  when the two vectors are the same:
        ///    dot(a,a) = |a|^2 = x^2+y^2+z^2
        ///
        /// -The length of a projection
        ///  when some vector is an unit vector:
        ///    dot(a,UnitV) = dot(UnitV,a) =
        ///    signed length of 'a' after the projection in the UnitV
        /// =================================================================
        ///      / a
        ///     /
        ///     ----->UnitV
        ///
        ///      /
        ///     /
        ///     -- projected 'a' over UnitV
        ///    |  |
        ///    length of the projection with positive sign
        /// =================================================================
        ///   \ a
        ///    \                                                            .
        ///     ----->UnitV
        ///
        ///   \                                                             .
        ///    \                                                            .
        ///   -- projected 'a' over UnitV
        ///  |  |
        ///  length of the projection with negative sign
        /// =================================================================
        /// -The hemisfere side of two vectors:
        ///    dot(a,b) = dot(b,a) = value
        ///        -- value < 0 => they are in the opose direction.
        ///        -- value > 0 => they are in the same direction.
        ///        -- value = 0 => they are orthogonal(90 degrees) vectors.
        ///
        /// -The dot product computes the equation:
        ///    dot(a,b) = cos(angle between a and b) * |a| * |b|
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, b;
        /// float result = dot( a, b ) ;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first vector
        /// \param b The second vector
        /// \return The dot product between the two vectors
        ///
        static __forceinline _type dot(const type3 &a, const type3 &b) noexcept
        {
            return (a.x * b.x + a.y * b.y + a.z * b.z);
        }

        /// \brief Normalize a vector
        ///
        /// Returns a unit vector in the same direction of the parameter.
        ///
        /// result = vec/|vec|
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a;
        ///
        /// vec3 a_normalized = normalize( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param vec The vector to normalize
        /// \return The unit vector
        ///
        static __forceinline type3 normalize(const type3 &vec) noexcept
        {
            _type mag2 = self_type::dot(vec, vec);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(mag2);
            return vec * mag2_rsqrt;
        }

        /// \brief Compute the angle in radians between two vectors
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a = vec3( 1, 0, 0 );
        /// vec3 b = vec3( 0, 10, 0 );
        ///
        /// float angle_radians = angleBetween(a, b);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The source vector
        /// \param b The target vector
        /// \return Angle in radians
        ///
        static __forceinline _type angleBetween(const type3 &a, const type3 &b) noexcept
        {
            _type cosA = OP<_type>::clamp(
                self_type::dot(self_type::normalize(a), self_type::normalize(b)),
                (_type)-1, (_type)1);
            return OP<_type>::acos(cosA);
        }

        //------------------------------------------------------------------------------
        /// \brief Computes the cross product between two vectors
        ///
        /// The cross product is an orthogonal vector to the others two vectores, i. e. the vector have 90 degrees to each vector at the same time.
        ///
        /// The side of the vector is defined by the right hand rule.
        ///
        /// <pre>
        ///     the first vector (a)
        ///    |
        ///    |____this is the cross result
        ///   /
        ///  /
        ///  the second vector (b)
        ///
        ///     the second vector (b)
        ///    |
        ///    |____the first vector (a)
        ///   /
        ///  /
        ///  this is the cross result
        ///
        ///     ____the first vector (a)
        ///   /|
        ///  / |
        ///  the second vector (b)
        ///    |
        ///     this is the cross result
        /// </pre>
        ///
        /// The length of the cross product is the same as:
        ///
        /// |sin(a)|*|a|*|b|, a = angle between the two vectors
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a = vec3( 1 ,0 ,0 );
        /// vec3 b = vec3( 0 ,1 ,0 );
        ///
        /// // result  = vec3( 0, 0, 1)
        /// vec3 result = cross(a, b);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first vector
        /// \param b The second vector
        /// \return The cross product between the two vectors
        ///
        static __forceinline type3 cross(const type3 &a, const type3 &b)
        {
            return type3((a.y * b.z - a.z * b.y),
                         (a.z * b.x - a.x * b.z),
                         (a.x * b.y - a.y * b.x));
        }

        /// \brief Computes the reflected vector 'a' related to a normal N
        ///
        /// The reflection of a vector is another vector with the same length, but the direction is
        /// modified by the surface normal (after hit the surface).
        ///
        /// <pre>
        ///          \  |
        ///           \ |w
        ///           a\\|a
        /// Normal <----|l
        ///            /|l
        ///           / |
        ///          /  |
        /// reflected   |
        ///
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a = vec3( 1 , -1 ,0 );
        /// vec3 normal = vec3( 0 ,1 ,0 );
        ///
        /// // result  = vec3( 1, 1, 0)
        /// vec3 reflected = reflect(a, normal);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The incident vector
        /// \param N The normal of a surface (unit vector)
        /// \return The reflected vector 'a' considering the normal N
        ///
        static __forceinline type3 reflect(const type3 &a, const type3 &N) noexcept
        {
            return (a - N * ((_type)2 * self_type::dot(a, N)));
        }

        /// \brief snell law refraction, vector implementation
        ///
        /// from input ray, normal, ni and nr calculate the refracted vector
        /// ni = source index of refraction (iOr)
        /// nr = target index of refraction (iOr)
        ///
        /// The function can lead to return false when occurs the total internal reflection. <br />
        /// This case may occurrs when you exit a ray from a more dense environment to a less dense environment.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float ior_air = 1.0f;
        /// float ior_water = 1.33f;
        /// vec3 rayDirection = normalize( vec3( 1 , -1 ,0 ) );
        /// vec3 normal = vec3( 0 ,1 ,0 );
        ///
        /// vec3 refracted;
        ///
        /// // compute the refracted ray that comes from air to the water surface
        /// if ( refract(rayDirection, normal, ior_air, ior_water, &refracted) ){
        ///     // the refracted has the value of the direction in the second environment
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rayDir Incident ray direction
        /// \param normal The normal of a surface (unit vector)
        /// \param ni source index of refraction
        /// \param nr target index of refraction
        /// \param vOut output vector
        /// \return true if vector is calculated, false if this is a total internal reflection case
        static __forceinline bool refract_old(const type3 &rayDir, const type3 &normal, const _type &ni, const _type &nr, type3 *vOut)
        {

            type3 L = self_type::normalize(-rayDir);

            _type ni_nr = ni / nr;
            _type cos_i = self_type::dot(normal, L);

            _type cos_r = (_type)1 - ni_nr * ni_nr * ((_type)1 - cos_i * cos_i);

            if (cos_r <= 0)
                return false;

            cos_r = OP<_type>::sqrt(cos_r);
            type3 T = (ni_nr * cos_i - cos_r) * normal - ni_nr * L;
            *vOut = self_type::normalize(T);
            return true;
        }

        static __forceinline type3 refract(const type3 &rayDir, const type3 &normal, const _type &ni, const _type &nr)
        {

            type3 L = self_type::normalize(-rayDir);

            _type ni_nr = ni / nr;
            _type cos_i = self_type::dot(normal, L);

            _type cos_r = (_type)1 - ni_nr * ni_nr * ((_type)1 - cos_i * cos_i);

            // if (cos_r <= 0)
            //     return false;
            _type select = OP<_type>::step(cos_r, 0);

            cos_r = OP<_type>::sqrt(cos_r);
            type3 T = (ni_nr * cos_i - cos_r) * normal - ni_nr * L;
            T = self_type::normalize(T);

            type3 rlct = self_type::reflect(rayDir, normal);

            return self_type::lerp(T, rlct, select);
        }

        /// \brief Computes the squared length of a vector
        ///
        /// The squared length of a vector 'a' is:
        ///
        /// |a|^2
        ///
        /// It is cheaper to compute this value than the length of 'a'.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 input;
        ///
        /// float result = sqrLength(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The Vector
        /// \return The squared length
        ///
        static __forceinline _type sqrLength(const type3 &a) noexcept
        {
            return self_type::dot(a, a);
        }

        /// \brief Computes the length of a vector
        ///
        /// The length of a vector 'a' is:
        ///
        /// |a|
        ///
        /// This computation uses the sqrtf, and it consumes a lot of cicles to compute.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 input;
        ///
        /// float result = length(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The Vector
        /// \return The length
        ///
        static __forceinline _type length(const type3 &a) noexcept
        {
            return OP<_type>::sqrt(self_type::dot(a, a));
        }

        /// \brief Computes the squared distance between two vectors
        ///
        /// The squared distance is the euclidian distance, without the square root:
        ///
        /// |b-a|^2
        ///
        /// It is cheaper to compute this value than the distance from 'a' to 'b'.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, b;
        ///
        /// float result = sqrDistance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The First vector
        /// \param b The Second vector
        /// \return The squared distance between a and b
        ///
        static __forceinline _type sqrDistance(const type3 &a, const type3 &b) noexcept
        {
            type3 ab = b - a;
            return self_type::dot(ab, ab);
        }

        /// \brief Computes the distance between two vectors
        ///
        /// The squared distance is the euclidian distance:
        ///
        /// |b-a|
        ///
        /// This computation uses the sqrtf, and it consumes a lot of cicles to compute.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, b;
        ///
        /// float result = distance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The First vector
        /// \param b The Second vector
        /// \return The distance between a and b
        ///
        static __forceinline _type distance(const type3 &a, const type3 &b) noexcept
        {
            type3 ab = b - a;
            return OP<_type>::sqrt(self_type::dot(ab, ab));
        }

        /// \brief Computes the projection of a vector over a unit vector
        ///
        /// The projection result is a vector parallel to the unitV
        ///
        /// <pre>
        /// ex.:
        ///         /                 |
        ///     a  /      | unitV     | vOut
        ///       o       o           o
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, unitV;
        ///
        /// unitV = normalize( unitV );
        ///
        /// vec3 vOout = parallelComponent( a, unitV );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The vector to decompose
        /// \param unitV The base vector (must be unit)
        /// \return The projection of 'a' over unitV
        ///
        static __forceinline type3 parallelComponent(const type3 &a, const type3 &unitV) noexcept
        {
            return unitV * (self_type::dot(a, unitV)); // dot(a,unitV) É a projeção de a em unitV
        }

        /// \brief Computes a vector perpendicular to the projection of a vector over a unit vector
        ///
        /// The vector perpendicular to the projection result is a vector normal to the unitV
        ///
        /// <pre>
        /// ex.:
        ///         /
        ///     a  /      | unitV
        ///       o       o           o-- vOut
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, unitV;
        ///
        /// unitV = normalize( unitV );
        ///
        /// vec3 vOout = perpendicularComponent( a, unitV );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The vector to decompose
        /// \param unitV The base vector (must be unit)
        /// \return The normal to the projection of 'a' over unitV
        ///
        static __forceinline type3 perpendicularComponent(const type3 &a, const type3 &unitV) noexcept
        {
            return a - unitV * (self_type::dot(a, unitV)); // unitV*(dot(a,unitV)) É o componenete paralelo
        }

        /// \brief Computes both: a vector perpendicular and a parallel to the projection of a vector over a unit vector
        ///
        /// The vector perpendicular to the projection result is a vector normal to the unitV
        ///
        /// <pre>
        /// ex.:
        ///         /
        ///     a  /      | unitV
        ///       o       o           o-- perpendicular
        /// </pre>
        ///
        /// The projection result is a vector parallel to the unitV
        ///
        /// <pre>
        /// ex.:
        ///         /                 |
        ///     a  /      | unitV     | parallel
        ///       o       o           o
        /// </pre>
        ///
        /// This function do a vector decomposition in two other vectors according the unitV.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, unitV;
        ///
        /// unitV = normalize( unitV );
        ///
        /// vec3 perpendicular, parallel;
        ///
        /// vecDecomp( a, unitV, &perpendicular, &parallel );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The vector to decompose
        /// \param unitV The base vector (must be unit)
        /// \param perpendicular It is a return parameter, thats will hold the computed perpendicular vector
        /// \param parallel It is a return parameter, thats will hold the computed parallel vector
        ///
        static __forceinline void vecDecomp(const type3 &a, const type3 &unitV,
                                          type3 *perpendicular, type3 *parallel) noexcept
        {
            *parallel = unitV * (self_type::dot(a, unitV));
            *perpendicular = a - *parallel;
        }

        /// \brief Test a point and clips its values according the euclidian distance from another point
        ///
        /// The quadratic clamp can be used to make limits like circular limits or spherical limits.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 center = vec3( 0, 0, 0 );
        /// float maxRadius = 10.0f;
        ///
        /// vec3 point = vec3( 100, 0, 0 );
        ///
        /// // result = vec3 ( 10, 0, 0 )
        /// vec3 result = quadraticClamp( center, point, maxRadius );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param point The point to clip
        /// \param center The center to compute the euclidian distance
        /// \param maxRadius The max distance that the point can be from the center
        /// \return The point if it is below the distance or a point in the same line of the point, but with distance from center equals to maxRadius
        ///
        static __forceinline type3 quadraticClamp(const type3 &center, const type3 &point, const _type maxRadius) noexcept
        {
            using type_info = FloatTypeInfo<_type>;

            type3 direction = point - center;

            _type length = self_type::length(direction);
            _type length_avoid_div_zero = OP<_type>::maximum(length, type_info::min);

            // normalize direction
            direction *= (_type)1 / length_avoid_div_zero;

            _type select_result = OP<_type>::step(maxRadius, length);

            return self_type::lerp(point, center + direction * maxRadius, select_result);

            // if (length > maxRadius)
            //     return center + direction * (maxRadius / length);
            // return point;
        }

        /// \brief Return the greater value from the parameter
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 input;
        ///
        /// float max = maximum( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Set of values to test
        /// \return The greater value from the parameter
        ///
        static __forceinline _type maximum(const type3 &a) noexcept
        {
            return OP<_type>::maximum(OP<_type>::maximum(a.x, a.y), a.z);
        }

        /// \brief Component-wise maximum value from two vectors
        ///
        ///  Return the maximum value considering each component of the vector.
        ///
        /// result: vec3( maximum(a.x,b.x), maximum(a.y,b.y), maximum(a.z,b.z) )
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, b;
        ///
        /// vec3 result = maximum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A vector
        /// \param b A vector
        /// \return The maximum value for each vector component
        ///
        static __forceinline type3 maximum(const type3 &a, const type3 &b) noexcept
        {
            return type3(
                OP<_type>::maximum(a.x, b.x),
                OP<_type>::maximum(a.y, b.y),
                OP<_type>::maximum(a.z, b.z));
        }

        /// \brief Return the smaller value from the parameter
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a;
        ///
        /// float result = minimum( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Set of values to test
        /// \return The smaller value from the parameter
        ///
        static __forceinline _type minimum(const type3 &a) noexcept
        {
            return OP<_type>::minimum(OP<_type>::minimum(a.x, a.y), a.z);
        }

        /// \brief Component-wise minimum value from two vectors
        ///
        ///  Return the minimum value considering each component of the vector.
        ///
        /// result: vec3( minimum(a.x,b.x), minimum(a.y,b.y), minimum(a.z,b.z) )
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, b;
        ///
        /// vec3 result = minimum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A vector
        /// \param b A vector
        /// \return The minimum value for each vector component
        ///
        static __forceinline type3 minimum(const type3 &a, const type3 &b) noexcept
        {
            return type3(
                OP<_type>::minimum(a.x, b.x),
                OP<_type>::minimum(a.y, b.y),
                OP<_type>::minimum(a.z, b.z));
        }

        /// \brief Compute the absolute value of a vector (magnitude)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 input = vec3( -10, 20, -30 );
        ///
        /// // result = vec3( 10, 20, 30 )
        /// vec3 result = absv( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A vector
        /// \return vec3( |a.x|, |a.y|, |a.z| )
        ///
        static __forceinline type3 abs(const type3 &a) noexcept
        {
            return type3(
                OP<_type>::abs(a.x),
                OP<_type>::abs(a.y),
                OP<_type>::abs(a.z));
        }

        /// \brief Computes the linear interpolation
        ///
        /// When the fator is between 0 and 1, it returns the convex relation (linear interpolation) between a and b.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a = vec3( 0.0f );
        /// vec3 b = vec3( 100.0f );
        ///
        /// // result = vec3( 75.0f, 75.0f, 75.0f )
        /// vec3 result = lerp( a, b, 0.75f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Origin Vector
        /// \param b Target Vector
        /// \param factor The amount (%) to leave the Origin to the Target.
        /// \return The interpolation result
        ///
        static __forceinline type3 lerp(const type3 &a, const type3 &b, const _type &factor) noexcept
        {
            //  return a+(b-a)*fator;
            return a * ((_type)1 - factor) + (b * factor);
        }

        /// \brief Compute the spherical linear interpolation between 2 vec3
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a = vec3( 1, 0, 0 );
        /// vec3 b = vec3( 0, 10, 0 );
        ///
        /// // 75% spherical interpolation from a to b
        /// vec3 result = slerp(a, b, 0.75f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The source vector
        /// \param b The target vector
        /// \param lerp The amount of interpolation
        /// \return The spherical interpolation of the source and target vectors
        ///
        static __forceinline type3 slerp(const type3 &a, const type3 &b, const _type &_lerp) noexcept
        {
            _type lerp = OP<_type>::clamp(_lerp, (_type)0, (_type)1);

            _type _cos = OP<_type>::clamp(
                self_type::dot(self_type::normalize(a), self_type::normalize(b)),
                (_type)-1, (_type)1);
            _type angle_rad = OP<_type>::acos(_cos);

            // sin is always positive
            //_type _sin = OP<_type>::sin(angle_rad);
            _type _sin = OP<_type>::sqrt((_type)1 - _cos * _cos);

            // if (OP<_type>::abs(_sin) < EPSILON<_type>::high_precision)
            //     return self_type::lerp(a, b, lerp);

            // _type _sin_abs = OP<_type>::abs(_sin);
            // _type aux = OP<_type>::maximum(_sin_abs, FloatTypeInfo<_type>::min );
            // _sin = OP<_type>::copy_sign( aux, _sin );

            _type select = OP<_type>::step(_sin, EPSILON<_type>::high_precision);
            _sin = OP<_type>::maximum(_sin, FloatTypeInfo<_type>::min);
            _type _1_over_sin = (_type)1 / _sin;
            _type a_factor = OP<_type>::sin(((_type)1 - lerp) * angle_rad) * _1_over_sin;
            _type b_factor = OP<_type>::sin(lerp * angle_rad) * _1_over_sin;

            // select a_factor and b_factor
            //_type select = OP<float>::step(_sin_abs, EPSILON<_type>::high_precision);
            a_factor = OP<_type>::lerp(a_factor, (_type)1 - lerp, select);
            b_factor = OP<_type>::lerp(b_factor, lerp, select);

            return a * a_factor + b * b_factor;
        }

        /// \brief Computes the result of the interpolation based on the baricentric coordinate (uv) considering 3 points
        ///
        /// It is possible to discover the value of 'u' and 'v' by using the triangle area formula.
        ///
        /// After that it is possible to use this function to interpolate normals, colors, etc... based on the baricentric coorginate uv
        ///
        /// Note: If the uv were calculated in euclidian space of a triangle, then interpolation of colors, normals or coordinates are not affected by the perspective projection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// // point inside the triangle
        /// vec3 p;
        ///
        /// // triangle vertex
        /// vec3 a, b, c;
        ///
        /// vec3 crossvec = cross(b-a, c-a);
        /// vec3 cross_unit = normalize( crossvec );
        /// float signed_triangle_area = dot( crossvec, cross_unit ) * 0.5f;
        ///
        /// crossvec = cross(c-a, c-p);
        ///
        /// float u = ( dot( crossvec, cross_unit )  * 0.5f ) / signed_triangle_area;
        ///
        /// crossvec = cross(b-a, p-b);
        ///
        /// float v = ( dot( crossvec, cross_unit )  * 0.5f ) / signed_triangle_area;
        ///
        /// // now the color we want to interpolate
        /// vec3 colorA, colorB, colorC;
        ///
        /// vec3 colorResult = barylerp(u, v, colorA, colorB, colorC);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param u The u component of a baricentric coord
        /// \param v The v component of a baricentric coord
        /// \param v0 The first vector to interpolate
        /// \param v1 The second vector to interpolate
        /// \param v2 The third vector to interpolate
        /// \return A vector interpolated based on uv considering the 3 vectors of the parameter
        ///
        static __forceinline type3 barylerp(const _type &u, const _type &v, const type3 &v0, const type3 &v1, const type3 &v2) noexcept
        {
            // return v0*(1-uv[0]-uv[1])+v1*uv[0]+v2*uv[1];
            return v0 * ((_type)1 - u - v) + v1 * u + v2 * v;
        }

        /// \brief Computes the result of the bilinear interpolation over a square patch with 4 points
        ///
        /// The bilinear interpolation is usefull to compute colors between pixels in a image.
        ///
        /// This implementation considers that the square formed by the four points is a square.
        ///
        /// If you try to interpolate values of a non square area, you will have a result, but it might be weird.
        ///
        /// <pre>
        /// dx - [0..1]
        /// dy - [0..1]
        ///
        ///  D-f(0,1) ---*----- C-f(1,1)
        ///     |        |         |
        ///     |        |         |
        /// .   *--------P---------*   P = (dx,dy)
        ///     |        |         |
        ///     |        |         |
        ///  A-f(0,0) ---*----- B-f(1,0)
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 dataA = vec3(0,0,0);
        /// vec3 dataB = vec3(1,0,0);
        /// vec3 dataC = vec3(1,1,0);
        /// vec3 dataD = vec3(0,1,0);
        ///
        /// // result = vec3( 0.5f, 0.5f, 0.0f )
        /// vec3 result = blerp(dataA,dataB,dataC,dataD,0.5f,0.5f);
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param A The lower-left vector
        /// \param B The lower-right vector
        /// \param C The upper-right vector
        /// \param D The upper-left vector
        /// \param dx The x axis interpolation factor
        /// \param dy The y axis interpolation factor
        /// \return A vector interpolated based on dxdy considering the 4 vectors of the parameter
        ///
        static __forceinline type3 blerp(const type3 &A, const type3 &B, const type3 &C, const type3 &D,
                                       const _type &dx, const _type &dy) noexcept
        {
            _type omdx = (_type)1 - dx,
                  omdy = (_type)1 - dy;
            return (omdx * omdy) * A + (omdx * dy) * D + (dx * omdy) * B + (dx * dy) * C;
        }

        /// \brief Computes the result of the spline interpolation using the CatmullRom aproach
        ///
        /// The spline is a curve based in four points. The CatmullRom aproach makes the curve walk through the control points.
        ///
        /// It can be used to make smooth curves in paths.
        ///
        /// The values interpolated will be between the 2nd and 3rd control points.
        ///
        /// <pre>
        /// t - [0..1]
        ///
        /// q(t) = 0.5 * (1.0f,t,t2,t3)  *
        ///
        /// [  0  2  0  0 ]  [P0]
        /// [ -1  0  1  0 ]* [P1]
        /// [  2 -5  4 -1 ]  [P2]
        /// [ -1  3 -3  1 ]  [P3]
        ///
        /// q(t) = 0.5 *((2 * P1) +
        ///              (-P0 + P2) * t +
        ///              (2*P0 - 5*P1 + 4*P2 - P3) * t2 +
        ///              (-P0 + 3*P1- 3*P2 + P3) * t3)
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 p0,p1,p2,p3;
        ///
        /// //the result is inside the range of p1 (0%) to p2 (100%)
        /// vec3 result = splineCatmullRom(p0,p1,p2,p3,0.75f);
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param P0 The 1st control point
        /// \param P1 The 2nd control point
        /// \param P2 The 3rd control point
        /// \param P3 The 4th control point
        /// \param t The interpolation value
        /// \return A vector interpolated based on t considering the 4 control points
        ///
        static __forceinline type3 splineCatmullRom(const type3 &P0, const type3 &P1, const type3 &P2, const type3 &P3, const _type &t) noexcept
        {
            _type tt = t * t;
            _type ttt = tt * t;
            return (_type)0.5 * (((_type)2 * P1) +
                                 (P2 - P0) * t +
                                 ((_type)2 * P0 - (_type)5 * P1 + (_type)4 * P2 - P3) * (tt) +
                                 ((_type)3 * P1 - P0 - (_type)3 * P2 + P3) * (ttt));
        }

        /// \brief Move from current to target, considering the max variation
        ///
        /// This function could be used as a constant motion interpolation<br />
        /// between two values considering the delta time and max speed variation.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// PlatformTime timer;
        /// float moveSpeed;
        /// vec3 current;
        /// vec3 target;
        ///
        /// {
        ///     timer.update();
        ///     ...
        ///     // current will be modified to be the target,
        ///     // but the delta time and move speed will make
        ///     // this transition smoother.
        ///     current = move( current, target, time.deltaTime * moveSpeed );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param current The current state
        /// \param target The target state
        /// \param maxDistanceVariation The max amount the current can be modified to reach target
        /// \return the lerp from current to target according max variation
        ///
        static __forceinline type3 move(const type3 &current, const type3 &target, const _type &maxDistanceVariation) noexcept
        {
            _type deltaDistance = self_type::distance(current, target);

            deltaDistance = OP<_type>::maximum(deltaDistance, maxDistanceVariation);
            // avoid division by zero
            // deltaDistance = self_type::maximum(deltaDistance, EPSILON<float>::high_precision);
            deltaDistance = OP<_type>::maximum(deltaDistance, FloatTypeInfo<_type>::min);
            return self_type::lerp(current, target, maxDistanceVariation / deltaDistance);

            // if (deltaDistance < maxDistanceVariation + EPSILON<_type>::high_precision)
            //     return target;
            // return lerp(current, target, maxDistanceVariation / deltaDistance);
        }

        /// \brief Move from current to target, considering the max variation in angle.
        ///
        /// This function could be used as a constant angular motion interpolation<br />
        /// between two values considering the delta time and max angle speed variation.
        ///
        /// Uses the spherical linear interpolation function.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// PlatformTime timer;
        /// float moveSpeedAngle;
        /// vec3 current;
        /// vec3 target;
        ///
        /// {
        ///     timer.update();
        ///     ...
        ///     // current will be modified to be the target,
        ///     // but the delta time and move speed will make
        ///     // this transition smoother.
        ///     current = moveSlerp( current, target, time.deltaTime * moveSpeedAngle );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param currentParam The current state
        /// \param targetParam The target state
        /// \param maxAngleVariation The max amount the current can be modified to reach target
        /// \return the slerp from current to target according max variation
        ///
        static __forceinline type3 moveSlerp(const type3 &currentParam, const type3 &targetParam, const _type &maxAngleVariation) noexcept
        {
            type3 current = currentParam;
            type3 target = targetParam;
            // const float EPSILON = 1e-6f;

            _type lengthCurrent = OP<type3>::length(current);
            _type lengthTarget = OP<type3>::length(target);

            // //trying to interpolate from zero vector
            // if (lengthCurrent < EPSILON2)
            //     return targetParam;
            // //trying to interpolate to zero vector
            // if (lengthTarget < EPSILON2)
            //     return currentParam;

            // avoid division by 0
            lengthCurrent = OP<_type>::maximum(lengthCurrent, FloatTypeInfo<_type>::min);
            lengthTarget = OP<_type>::maximum(lengthTarget, FloatTypeInfo<_type>::min);

            current *= ((_type)1 / lengthCurrent);
            target *= ((_type)1 / lengthTarget);

            // float deltaAngle = angleBetween(current, target);
            _type deltaAngle = OP<_type>::acos(OP<_type>::clamp(OP<type3>::dot(current, target), (_type)-1, (_type)1));
            // if (deltaAngle < maxAngleVariation + EPSILON)
            //     return target;
            deltaAngle = OP<_type>::maximum(deltaAngle, maxAngleVariation);
            deltaAngle = OP<_type>::maximum(deltaAngle, FloatTypeInfo<_type>::min);

            // 180º case interpolation -- force one orientation based on eulerAngle
            // if (deltaAngle >= CONSTANT<_type>::PI - EPSILON<_type>::high_precision)
            // if (deltaAngle >= CONSTANT<quatT>::_180_move_slerp_case_deltaAngle)
            // {
            //     current = CONSTANT<quatT>::_180_move_slerp_case() * current;
            //     deltaAngle = CONSTANT<quatT>::_180_move_slerp_case_deltaAngle;//OP<type3>::angleBetween(current, target);
            // }
            // branch-less implementation
            _type select_delta_angle = OP<_type>::step(CONSTANT<quatT>::_180_move_slerp_case_deltaAngle, deltaAngle);
            current = OP<type3>::lerp(current, CONSTANT<quatT>::_180_move_slerp_case() * current, select_delta_angle);
            deltaAngle = OP<_type>::lerp(deltaAngle, CONSTANT<quatT>::_180_move_slerp_case_deltaAngle, select_delta_angle);

            _type lrpFactor = maxAngleVariation / deltaAngle;
            type3 result = OP<type3>::slerp(current, target, lrpFactor);

            result = OP<type3>::normalize(result) * OP<_type>::lerp(lengthCurrent, lengthTarget, lrpFactor);

            return result;
        }

        /// \brief The sign of each component. ( v >= 0 ) ? 1 : -1
        ///
        static __forceinline type3 sign(const type3 &v) noexcept
        {
            return type3(
                OP<_type>::sign(v.x),
                OP<_type>::sign(v.y),
                OP<_type>::sign(v.z));
        }

        /// \brief The floor of each component.
        ///
        static __forceinline type3 floor(const type3 &v) noexcept
        {
            return type3(
                OP<_type>::floor(v.x),
                OP<_type>::floor(v.y),
                OP<_type>::floor(v.z));
        }

        /// \brief The ceil of each component.
        ///
        static __forceinline type3 ceil(const type3 &v) noexcept
        {
            return type3(
                OP<_type>::ceil(v.x),
                OP<_type>::ceil(v.y),
                OP<_type>::ceil(v.z));
        }

        /// \brief Round each component.
        ///
        static __forceinline type3 round(const type3 &v) noexcept
        {
            return type3(
                OP<_type>::round(v.x),
                OP<_type>::round(v.y),
                OP<_type>::round(v.z));
        }

        /// \brief fmod each component.
        ///
        static __forceinline type3 fmod(const type3 &a, const type3 &b) noexcept
        {
            return type3(
                OP<_type>::fmod(a.x, b.x),
                OP<_type>::fmod(a.y, b.y),
                OP<_type>::fmod(a.z, b.z));
        }

        /// \brief Step function on each component. ( v >= threshould ) ? 1 : 0
        ///
        static __forceinline type3 step(const type3 &threshould, const type3 &v) noexcept
        {
            return type3(
                OP<_type>::step(threshould.x, v.x),
                OP<_type>::step(threshould.y, v.y),
                OP<_type>::step(threshould.z, v.z));
        }

        static __forceinline type3 smoothstep(const type3 &edge0, const type3 &edge1, const type3 &x) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            type3 dir = edge1 - edge0;

            _type length_dir = self_type::maximum(self_type::length(dir), type_info::min);

            type3 value = x - edge0;
            value *= (_type)1 / length_dir;

            type3 t = self_type::clamp(value, type3((_type)0), type3((_type)1));
            return t * t * ((_type)3 - (_type)2 * t);
        }
    };

}