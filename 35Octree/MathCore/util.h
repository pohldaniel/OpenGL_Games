#pragma once

#include "math_common.h"

#include "int.h"
#include "float.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#include "quat.h"

#include "mat2.h"
#include "mat3.h"
#include "mat4.h"

#include "cvt.h"

namespace MathCore
{

    namespace Util
    {

        template <typename _type,
                  typename std::enable_if<
                      std::is_floating_point<_type>::value,
                      bool>::type = true>
        static __forceinline void eulerEquivalent(const _type &_roll, const _type &_pitch, const _type &_yaw,
                                                _type *roll, _type *pitch, _type *yaw)
        {

            *roll = _roll + CONSTANT<_type>::PI;
            *pitch = CONSTANT<_type>::PI - _pitch;
            *yaw = _yaw + CONSTANT<_type>::PI;

            *roll = OP<_type>::fmod(*roll, (_type)2 * CONSTANT<_type>::PI);
            *pitch = OP<_type>::fmod(*pitch, (_type)2 * CONSTANT<_type>::PI);
            *yaw = OP<_type>::fmod(*yaw, (_type)2 * CONSTANT<_type>::PI);
        }

        template <typename _type, typename _simd,
                  typename std::enable_if<
                      std::is_floating_point<_type>::value,
                      bool>::type = true>
        static __forceinline bool unproject(const vec3<_type, _simd> &_pointInWindow,
                                          const mat4<_type, _simd> &modelViewMatrix,
                                          const mat4<_type, _simd> &projectionMatrix,
                                          int viewportX, int viewportY, int viewportW, int viewportH,
                                          vec3<_type, _simd> *worldPtn) noexcept
        {
            vec3<_type, _simd> pointInWindow = _pointInWindow;
            mat4<_type, _simd> modelViewProjection_inverse = projectionMatrix * modelViewMatrix; // pre_multiplyed ogl Like
            modelViewProjection_inverse = modelViewProjection_inverse.inverse();
            // if (modelViewProjection_inverse.array[0] == std::numeric_limits<_type>::quiet_NaN())
            //     return false;
            /* Map x and y from window coordinates */
            pointInWindow.x = (pointInWindow.x - (_type)viewportX) / (_type)viewportW;
            pointInWindow.y = (pointInWindow.y - (_type)viewportY) / (_type)viewportH;
            /* Map to range -1 to 1 */
            pointInWindow = pointInWindow * 2.0f - 1.0f;
            vec4<_type, _simd> position_homogeneos = modelViewProjection_inverse * vec4<_type, _simd>(pointInWindow, 1);
            
            position_homogeneos.w = OP<_type>::maximum(position_homogeneos.w,FloatTypeInfo<_type>::min);
            // if (position_homogeneos.w < EPSILON<_type>::high_precision)
            //     return false;
            
            *worldPtn = CVT<vec4<_type, _simd>>::toVec3_PerspDiv(position_homogeneos);
            return true;
        }

        template <typename _type, typename _simd,
                  typename std::enable_if<
                      std::is_floating_point<_type>::value,
                      bool>::type = true>
        static __forceinline bool project(const vec3<_type, _simd> &worldPtn,
                                        const mat4<_type, _simd> &modelViewMatrix,
                                        const mat4<_type, _simd> &projectionMatrix,
                                        int viewportX, int viewportY, int viewportW, int viewportH,
                                        vec3<_type, _simd> *pointInWindow) noexcept
        {
            mat4<_type, _simd> modelViewProjection = projectionMatrix * modelViewMatrix; // pre_multiplyed ogl Like
            vec4<_type, _simd> position_homogeneos = modelViewProjection * vec4<_type, _simd>(worldPtn, 1);
            position_homogeneos.w = OP<_type>::maximum(position_homogeneos.w,FloatTypeInfo<_type>::min);
            // if (position_homogeneos.w < EPSILON<_type>::high_precision)
            //     return false;
            vec3<_type, _simd> result = CVT<vec4<_type, _simd>>::toVec3_PerspDiv(position_homogeneos);
            // Map x, y and z to range 0-1
            result = result * 0.5f + 0.5f;
            // Map x,y to viewport
            result.x = result.x * float(viewportW) + float(viewportX);
            result.y = result.y * float(viewportH) + float(viewportY);
            *pointInWindow = result;
            return true;
        }


        template <typename _type, typename _simd,
                  typename std::enable_if<
                      std::is_floating_point<_type>::value,
                      bool>::type = true>
        static __forceinline void projectOnAxis(const vec3<_type, _simd> *points, int count, const vec3<_type, _simd> &axis, _type *outMin, _type *outMax) noexcept
        {
            _type min = FloatTypeInfo<_type>::max;
            _type max = -FloatTypeInfo<_type>::max;
            for (int i = 0; i < count; i++)
            {
                const vec3<_type, _simd> &p = points[i];
                _type val = OP<vec3<_type, _simd>>::dot(axis, p);
                min = OP<_type>::minimum(min, val);
                max = OP<_type>::maximum(max, val);
            }
            *outMin = min;
            *outMax = max;
        }


    }

}