#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat4_gen_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "mat4_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd>
    struct GEN<mat4<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_type, float>::value &&
                   (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                    std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type>
    {
    private:
        using typeMat4 = mat4<_type, _simd>;
        using typeMat2 = mat2<_type, _simd>;
        using typeMat3 = mat3<_type, _simd>;
        using typeVec4 = vec4<_type, _simd>;
        using typeVec3 = vec3<_type, _simd>;
        using typeVec2 = vec2<_type, _simd>;
        using quatT = quat<_type, _simd>;
        using self_type = GEN<typeMat4>;

    public:
        static __forceinline typeMat4 translateHomogeneous(const _type &_x_, const _type &_y_, const _type &_z_ = 0) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat4(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _vec4_0010_sse,
                            _mm_setr_ps(_x_, _y_, _z_, 1));
#elif defined(ITK_NEON)
            return typeMat4(_neon_1000,
                            _neon_0100,
                            _neon_0010,
                            (float32x4_t){_x_, _y_, _z_, 1});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 translateHomogeneous(const typeVec2 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 _t = _mm_setr_ps(_v_.x, _v_.y, 0.0f, 1.0f);
            return typeMat4(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _vec4_0010_sse,
                            _t);
#else
            return typeMat4(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _vec4_0010_sse,
                            _mm_blend_ps(_v_.array_sse, _vec4_0001_sse, 0xC));
#endif
#elif defined(ITK_NEON)
            return typeMat4(_neon_1000,
                            _neon_0100,
                            _neon_0010,
                            (float32x4_t){_v_.x, _v_.y, 0, 1});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 translateHomogeneous(const typeVec3 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 _t = _v_.array_sse;
            _mm_f32_(_t,3) = 1.0f;
            return typeMat4(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _vec4_0010_sse,
                            _t);
#else
            return typeMat4(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _vec4_0010_sse,
                            _mm_blend_ps(_v_.array_sse, _vec4_0001_sse, 0x8));
#endif
#elif defined(ITK_NEON)
            return typeMat4(_neon_1000,
                            _neon_0100,
                            _neon_0010,
                            (float32x4_t){_v_.x, _v_.y, _v_.z, 1});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 translateHomogeneous(const typeVec4 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 _t = _v_.array_sse;
            _mm_f32_(_t,3) = 1.0f;
            return typeMat4(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _vec4_0010_sse,
                            _t);
#else
            return typeMat4(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _vec4_0010_sse,
                            _mm_blend_ps(_v_.array_sse, _vec4_0001_sse, 0x8));
#endif
#elif defined(ITK_NEON)
            return typeMat4(_neon_1000,
                            _neon_0100,
                            _neon_0010,
                            (float32x4_t){_v_.x, _v_.y, _v_.z, 1});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 scaleHomogeneous(const _type &_x_, const _type &_y_, const _type &_z_ = (_type)1) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat4(
                _mm_setr_ps(_x_, 0, 0, 0),
                _mm_setr_ps(0, _y_, 0, 0),
                _mm_setr_ps(0, 0, _z_, 0),
                _vec4_0001_sse);
#elif defined(ITK_NEON)
            return typeMat4((float32x4_t){_x_, 0, 0, 0},
                            (float32x4_t){0, _y_, 0, 0},
                            (float32x4_t){0, 0, _z_, 0},
                            _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 scaleHomogeneous(const typeVec2 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat4(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _vec4_0010_sse,
                _vec4_0001_sse);
#elif defined(ITK_NEON)
            return typeMat4((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            _neon_0010,
                            _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 scaleHomogeneous(const typeVec3 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat4(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _mm_setr_ps(0, 0, _v_.z, 0),
                _vec4_0001_sse);
#elif defined(ITK_NEON)
            return typeMat4((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            (float32x4_t){0, 0, _v_.z, 0},
                            _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 scaleHomogeneous(const typeVec4 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat4(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _mm_setr_ps(0, 0, _v_.z, 0),
                _vec4_0001_sse);
#elif defined(ITK_NEON)
            return typeMat4((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            (float32x4_t){0, 0, _v_.z, 0},
                            _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 xRotateHomogeneous(const _type &_phi_) noexcept
        {
            _type c = OP<_type>::cos(_phi_);
            _type s = OP<_type>::sin(_phi_);
#if defined(ITK_SSE2)
            return typeMat4(
                _vec4_1000_sse,
                _mm_setr_ps(0, c, s, 0),
                _mm_setr_ps(0, -s, c, 0),
                _vec4_0001_sse);
#elif defined(ITK_NEON)
            return typeMat4(_neon_1000,
                            (float32x4_t){0, c, s, 0},
                            (float32x4_t){0, -s, c, 0},
                            _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 yRotateHomogeneous(const _type &_theta_) noexcept
        {
            _type c = OP<_type>::cos(_theta_);
            _type s = OP<_type>::sin(_theta_);
#if defined(ITK_SSE2)
            return typeMat4(
                _mm_setr_ps(c, 0, -s, 0),
                _vec4_0100_sse,
                _mm_setr_ps(s, 0, c, 0),
                _vec4_0001_sse);
#elif defined(ITK_NEON)
            return typeMat4((float32x4_t){c, 0, -s, 0},
                            _neon_0100,
                            (float32x4_t){s, 0, c, 0},
                            _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 zRotateHomogeneous(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
#if defined(ITK_SSE2)
            return typeMat4(
                _mm_setr_ps(c, s, 0, 0),
                _mm_setr_ps(-s, c, 0, 0),
                _vec4_0010_sse,
                _vec4_0001_sse);
#elif defined(ITK_NEON)
            return typeMat4((float32x4_t){c, s, 0, 0},
                            (float32x4_t){-s, c, 0, 0},
                            _neon_0010,
                            _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 fromEulerHomogeneous(const _type &roll, const _type &pitch, const _type &yaw) noexcept
        {
            return self_type::zRotateHomogeneous(yaw) * self_type::yRotateHomogeneous(pitch) * self_type::xRotateHomogeneous(roll);
        }

        static __forceinline typeMat4 rotateHomogeneous(const _type &_ang_, const _type &_x, const _type &_y, const _type &_z) noexcept
        {
            _type x = _x;
            _type y = _y;
            _type z = _z;

            using type_info = FloatTypeInfo<_type>;
            // depois tem como otimizar esta rotação
            _type length_inv = x * x + y * y + z * z;
            length_inv = OP<_type>::sqrt(length_inv);
            length_inv = OP<_type>::maximum(length_inv, type_info::min);
            // ARIBEIRO_ABORT(length_inv == 0, "division by zero\n");
            length_inv = (_type)1 / length_inv;

            x *= length_inv;
            y *= length_inv;
            z *= length_inv;

            _type c = OP<_type>::cos(_ang_);
            _type s = OP<_type>::sin(_ang_);

            _type _1_m_c = (_type)1 - c;
            // original -- rotacao em sentido anti-horario
            return typeMat4(x * x * _1_m_c + c, x * y * _1_m_c - z * s, x * z * _1_m_c + y * s, 0,
                            y * x * _1_m_c + z * s, y * y * _1_m_c + c, y * z * _1_m_c - x * s, 0,
                            x * z * _1_m_c - y * s, y * z * _1_m_c + x * s, z * z * _1_m_c + c, 0,
                            0, 0, 0, 1);

            // transposto -- rotacao em sentido horario
            //   return  mat4(x*x*(1-c)+c  ,  y*x*(1-c)+z*s  ,  x*z*(1-c)-y*s,   0  ,
            //                x*y*(1-c)-z*s,  y*y*(1-c)+c    ,  y*z*(1-c)+x*s,   0  ,
            //                x*z*(1-c)+y*s,  y*z*(1-c)-x*s  ,  z*z*(1-c)+c  ,   0  ,
            //                    0        ,        0        ,      0        ,   1  );
        }

        static __forceinline typeMat4 rotateHomogeneous(const _type &_ang_, const typeVec2 &axis) noexcept
        {
            return self_type::rotateHomogeneous(_ang_, axis.x, axis.y, 0);
        }

        static __forceinline typeMat4 rotateHomogeneous(const _type &_ang_, const typeVec3 &axis) noexcept
        {
            return self_type::rotateHomogeneous(_ang_, axis.x, axis.y, axis.z);
        }

        static __forceinline typeMat4 rotateHomogeneous(const _type &_ang_, const typeVec4 &axis) noexcept
        {
            return self_type::rotateHomogeneous(_ang_, axis.x, axis.y, axis.z);
        }

        /// \brief Creates a projection matrix (Right Handed)
        ///
        /// <pre>
        /// f=cotangent(FieldOfView/2)
        /// matriz:
        ///
        /// f/aspect  0      0                           0
        /// 0         f      0                           0
        /// 0         0    (zfar+znear)/(znear-zfar)    (2*zfar*znear)/(znear-zfar)
        /// 0         0     -1                           0
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float FovY = 60.0f;
        /// float aspectX = screenWidth / screenHeight;
        /// float near = 0.001f;
        /// float far = 1000.0f;
        ///
        /// mat4 projection_matrix = projection_perspective_rh_negative_one(FovY,aspectX,near,far);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param FovY Angle in degrees
        /// \param aspectX The aspect of X related to the Height (ex.: Width/Height)
        /// \param near_ Near plane
        /// \param far_ Far plane
        /// \return A 4x4 matrix
        ///
        static __forceinline typeMat4 projection_perspective_rh_negative_one(const _type &FovY, const _type &aspectX, const _type &near_, const _type &far_) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            //   f=cotangent(FieldOfView/2)
            // matriz:
            //
            // f/aspect  0      0                           0
            // 0         f      0                           0
            // 0         0    (zfar+znear)/(znear-zfar)    (2*zfar*znear)/(znear-zfar)
            // 0         0     -1                           0

            _type focus = OP<_type>::tan(OP<_type>::deg_2_rad(FovY) * (_type)0.5);
            focus = OP<_type>::maximum(focus, type_info::min);
            focus = (_type)1 / focus;

            _type _1_o_nmf = (_type)1 / (near_ - far_);

            return typeMat4(focus / aspectX, 0, 0, 0,
                            0, focus, 0, 0,
                            0, 0, (near_ + far_) * _1_o_nmf, ((_type)2 * near_ * far_) * _1_o_nmf,
                            0, 0, -1, 0);
        }

        /// \brief Creates a projection matrix (Right Handed)
        ///
        /// The unit of the focal length is the same as specified by the width and height.
        ///
        /// ex.: Considering millimeters (mm) in a focal length of 35mm in a CCD area of 50x30 mm.<br />
        /// It is possible to use this function to configure the projection:
        ///
        /// projection_perspective_rh_negative_one(35,50,30,0.001,100.0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float focalLength = 35.0f;
        /// float width = screenWidth;
        /// float height = screenHeight;
        /// float near = 0.001f;
        /// float far = 1000.0f;
        ///
        /// mat4 projection_matrix = projection_perspective_rh_negative_one(focalLength,width,height,near,far);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param focalLength The focal length
        /// \param w Width
        /// \param h Height
        /// \param near_ Near plane
        /// \param far_ Far plane
        /// \return A 4x4 matrix
        ///
        static __forceinline typeMat4 projection_perspective_rh_negative_one(const _type &focalLength, const _type &w, const _type &h, const _type &near_, const _type &far_) noexcept
        {
            _type fovY = (_type)2 * OP<_type>::atan((h * (_type)0.5) / focalLength);
            fovY = OP<_type>::rad_2_deg(fovY);
            _type aspectX = w / h;
            return self_type::projection_perspective_rh_negative_one(fovY, aspectX, near_, far_);
        }

        static __forceinline typeMat4 projection_perspective_rh_zero_one(const _type &FovY, const _type &aspectX, const _type &near_, const _type &far_) noexcept
        {

            using type_info = FloatTypeInfo<_type>;
            _type focus = OP<_type>::tan(OP<_type>::deg_2_rad(FovY) * (_type)0.5);
            focus = OP<_type>::maximum(focus, type_info::min);
            focus = (_type)1 / focus;

            _type _1_o_nmf = (_type)1 / (near_ - far_);

            return typeMat4(
                focus / aspectX, 0, 0, 0,
                0, focus, 0, 0,
                0, 0, far_ * _1_o_nmf, (far_ * near_) * _1_o_nmf,
                0, 0, -1, 0);
        }

        static __forceinline typeMat4 projection_perspective_rh_zero_one(const _type &focalLength, const _type &w, const _type &h, const _type &near_, const _type &far_) noexcept
        {
            _type fovY = (_type)2 * OP<_type>::atan((h * (_type)0.5) / focalLength);
            fovY = OP<_type>::rad_2_deg(fovY);
            _type aspectX = w / h;
            return projection_perspective_rh_zero_one(fovY, aspectX, near_, far_);
        }

        /// \brief Creates a projection matrix (Left Handed)
        ///
        /// <pre>
        /// f=cotangent(FieldOfView/2)
        /// matriz:
        ///
        /// f/aspect  0      0                           0
        /// 0         f      0                           0
        /// 0         0   -(zfar+znear)/(znear-zfar)    (2*zfar*znear)/(znear-zfar)
        /// 0         0      1                           0
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float FovY = 60.0f;
        /// float aspectX = screenWidth / screenHeight;
        /// float near = 0.001f;
        /// float far = 1000.0f;
        ///
        /// mat4 projection_matrix = projection_perspective_lh_negative_one(FovY,aspectX,near,far);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param FovY Angle in degrees
        /// \param aspectX The aspect of X related to the Height (ex.: Width/Height)
        /// \param near_ Near plane
        /// \param far_ Far plane
        /// \return A 4x4 matrix
        ///
        static __forceinline typeMat4 projection_perspective_lh_negative_one(const _type &FovY, const _type &aspectX, const _type &near_, const _type &far_) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            _type focus = OP<_type>::tan(OP<_type>::deg_2_rad(FovY) * (_type)0.5);
            focus = OP<_type>::maximum(focus, type_info::min);
            focus = (_type)1 / focus;

            _type _1_o_nmf = (_type)1 / (near_ - far_);

            return typeMat4(focus / aspectX, 0, 0, 0,
                            0, focus, 0, 0,
                            0, 0, -(near_ + far_) * _1_o_nmf, ((_type)2 * near_ * far_) * _1_o_nmf,
                            0, 0, 1, 0);
        }

        /// \brief Creates a projection matrix (Left Handed)
        ///
        /// The unit of the focal length is the same as specified by the width and height.
        ///
        /// ex.: Considering millimeters (mm) in a focal length of 35mm in a CCD area of 50x30 mm.<br />
        /// It is possible to use this function to configure the projection:
        ///
        /// projection_perspective_lh_negative_one(35,50,30,0.001,100.0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float focalLength = 35.0f;
        /// float width = screenWidth;
        /// float height = screenHeight;
        /// float near = 0.001f;
        /// float far = 1000.0f;
        ///
        /// mat4 projection_matrix = projection_perspective_lh_negative_one(focalLength,width,height,near,far);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param focalLength The focal length
        /// \param w Width
        /// \param h Height
        /// \param near_ Near plane
        /// \param far_ Far plane
        /// \return A 4x4 matrix
        ///
        static __forceinline typeMat4 projection_perspective_lh_negative_one(const _type &focalLength, const _type &w, const _type &h, const _type &near_, const _type &far_) noexcept
        {
            _type fovY = (_type)2 * OP<_type>::atan((h * (_type)0.5) / focalLength);
            fovY = OP<_type>::rad_2_deg(fovY);
            _type aspectX = w / h;
            return projection_perspective_lh_negative_one(fovY, aspectX, near_, far_);
        }

        static __forceinline typeMat4 projection_perspective_lh_zero_one(const _type &FovY, const _type &aspectX, const _type &near_, const _type &far_) noexcept
        {

            using type_info = FloatTypeInfo<_type>;
            _type focus = OP<_type>::tan(OP<_type>::deg_2_rad(FovY) * (_type)0.5);
            focus = OP<_type>::maximum(focus, type_info::min);
            focus = (_type)1 / focus;

            _type _1_o_nmf = (_type)1 / (near_ - far_);

            return typeMat4(
                focus / aspectX, 0, 0, 0,
                0, focus, 0, 0,
                0, 0, -far_ * _1_o_nmf, (far_ * near_) * _1_o_nmf,
                0, 0, 1, 0);
        }

        static __forceinline typeMat4 projection_perspective_lh_zero_one(const _type &focalLength, const _type &w, const _type &h, const _type &near_, const _type &far_) noexcept
        {
            _type fovY = (_type)2 * OP<_type>::atan((h * (_type)0.5) / focalLength);
            fovY = OP<_type>::rad_2_deg(fovY);
            _type aspectX = w / h;
            return projection_perspective_lh_zero_one(fovY, aspectX, near_, far_);
        }

        /// \brief Creates a projection matrix from the frustum definition (Right Handed)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float Left = -1.0f, Right = 1.0f;
        /// float Bottom = -1.0f, Top = 1.0f;
        /// float Near = 0.001f, Far = 1000.0f;
        ///
        /// mat4 projection_matrix = projection_frustum_rh_negative_one(Left,Right,Bottom,Top,Near,Far);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param Left The left limit of the projection plane
        /// \param Right The right limit of the projection plane
        /// \param Bottom The bottom limit of the projection plane
        /// \param Top The top limit of the projection plane
        /// \param Near Near plane
        /// \param Far Far plane
        /// \return A 4x4 matrix
        ///
        static __forceinline typeMat4 projection_frustum_rh_negative_one(const _type &Left, const _type &Right, const _type &Bottom, const _type &Top, const _type &Near, const _type &Far) noexcept
        {
            _type _2_t_near = (_type)2 * Near;

            _type _1_o_rml = (_type)1 / (Right - Left);
            _type _1_o_tmb = (_type)1 / (Top - Bottom);
            _type _1_o_fmn = (_type)1 / (Far - Near);

            return typeMat4(_2_t_near * _1_o_rml, 0, (Right + Left) * _1_o_rml, 0,
                            0, _2_t_near * _1_o_tmb, (Top + Bottom) * _1_o_tmb, 0,
                            0, 0, (-(Far + Near)) * _1_o_fmn, ((_type)-2 * Far * Near) * _1_o_fmn,
                            0, 0, -1, 0);
        }

        static __forceinline typeMat4 projection_frustum_rh_zero_one(const _type &Left, const _type &Right, const _type &Bottom, const _type &Top, const _type &Near, const _type &Far) noexcept
        {
            _type _1_o_rml = (_type)1 / (Right - Left);
            _type _1_o_tmb = (_type)1 / (Top - Bottom);
            _type _1_o_fmn = (_type)1 / (Far - Near);

            return typeMat4(
                ((_type)2 * Near) * _1_o_rml, 0, (Right + Left) * _1_o_rml, 0,
                0, ((_type)2 * Near) * _1_o_tmb, (Top + Bottom) * _1_o_tmb, 0,
                0, 0, -Far * _1_o_fmn, -(Far * Near) * _1_o_fmn,
                0, 0, -1, 0);
        }

        /// \brief Creates a projection matrix from the frustum definition (Left Handed)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float Left = -1.0f, Right = 1.0f;
        /// float Bottom = -1.0f, Top = 1.0f;
        /// float Near = 0.001f, Far = 1000.0f;
        ///
        /// mat4 projection_matrix = projection_frustum_lh_negative_one(Left,Right,Bottom,Top,Near,Far);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param Left The left limit of the projection plane
        /// \param Right The right limit of the projection plane
        /// \param Bottom The bottom limit of the projection plane
        /// \param Top The top limit of the projection plane
        /// \param Near Near plane
        /// \param Far Far plane
        /// \return A 4x4 matrix
        ///
        static __forceinline typeMat4 projection_frustum_lh_negative_one(const _type &Left, const _type &Right, const _type &Bottom, const _type &Top, const _type &Near, const _type &Far) noexcept
        {
            _type _2_t_near = (_type)2 * Near;

            _type _1_o_rml = (_type)1 / (Right - Left);
            _type _1_o_tmb = (_type)1 / (Top - Bottom);
            _type _1_o_fmn = (_type)1 / (Far - Near);

            return typeMat4(_2_t_near * _1_o_rml, 0, (Right + Left) * _1_o_rml, 0,
                            0, _2_t_near * _1_o_tmb, (Top + Bottom) * _1_o_tmb, 0,
                            0, 0, (Far + Near) * _1_o_fmn, ((_type)-2 * Far * Near) * _1_o_fmn,
                            0, 0, 1, 0);
        }

        static __forceinline typeMat4 projection_frustum_lh_zero_one(const _type &Left, const _type &Right, const _type &Bottom, const _type &Top, const _type &Near, const _type &Far) noexcept
        {
            _type _1_o_rml = (_type)1 / (Right - Left);
            _type _1_o_tmb = (_type)1 / (Top - Bottom);
            _type _1_o_fmn = (_type)1 / (Far - Near);

            return typeMat4(
                ((_type)2 * Near) * _1_o_rml, 0, (Right + Left) * _1_o_rml, 0,
                0, ((_type)2 * Near) * _1_o_tmb, (Top + Bottom) * _1_o_tmb, 0,
                0, 0, Far * _1_o_fmn, -(Far * Near) * _1_o_fmn,
                0, 0, 1, 0);
        }

        /// \brief Creates a projection matrix from the orthographic definition (Right Handed)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float Left = -screenWidth/2.0f, Right = screenWidth/2.0f;
        /// float Bottom = -screenHeight/2.0f, Top = screenHeight/2.0f;
        /// float Near = -1000.0f, Far = 1000.0f;
        ///
        /// mat4 projection_matrix = projection_ortho_rh_negative_one(Left,Right,Bottom,Top,Near,Far);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param Left The left limit of the projection plane
        /// \param Right The right limit of the projection plane
        /// \param Bottom The bottom limit of the projection plane
        /// \param Top The top limit of the projection plane
        /// \param Near Near plane
        /// \param Far Far plane
        /// \return A 4x4 matrix
        ///
        static __forceinline typeMat4 projection_ortho_rh_negative_one(const _type &Left, const _type &Right, const _type &Bottom, const _type &Top, const _type &Near, const _type &Far) noexcept
        {
            _type _1_o_rml = (_type)1 / (Right - Left);
            _type _1_o_tmb = (_type)1 / (Top - Bottom);
            _type _1_o_fmn = (_type)1 / (Far - Near);

            return typeMat4((_type)2 * _1_o_rml, 0, 0, -(Right + Left) * _1_o_rml,
                            0, (_type)2 * _1_o_tmb, 0, -(Top + Bottom) * _1_o_tmb,
                            0, 0, (_type)-2 * _1_o_fmn, -(Far + Near) * _1_o_fmn,
                            0, 0, 0, 1);
        }

        static __forceinline typeMat4 projection_ortho_rh_zero_one(const _type &Left, const _type &Right, const _type &Bottom, const _type &Top, const _type &Near, const _type &Far) noexcept
        {
            _type _1_o_rml = (_type)1 / (Right - Left);
            _type _1_o_tmb = (_type)1 / (Top - Bottom);
            _type _1_o_fmn = (_type)1 / (Far - Near);

            return typeMat4(
                (_type)2 * _1_o_rml, 0, 0, -(Right + Left) * _1_o_rml,
                0, (_type)2 * _1_o_tmb, 0, -(Top + Bottom) * _1_o_tmb,
                0, 0, -_1_o_fmn, -Near * _1_o_fmn,
                0, 0, 0, 1);
        }

        /// \brief Creates a projection matrix from the orthographic definition (Left Handed)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float Left = -screenWidth/2.0f, Right = screenWidth/2.0f;
        /// float Bottom = -screenHeight/2.0f, Top = screenHeight/2.0f;
        /// float Near = -1000.0f, Far = 1000.0f;
        ///
        /// mat4 projection_matrix = projection_ortho_lh_negative_one(Left,Right,Bottom,Top,Near,Far);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param Left The left limit of the projection plane
        /// \param Right The right limit of the projection plane
        /// \param Bottom The bottom limit of the projection plane
        /// \param Top The top limit of the projection plane
        /// \param Near Near plane
        /// \param Far Far plane
        /// \return A 4x4 matrix
        ///
        static __forceinline typeMat4 projection_ortho_lh_negative_one(const _type &Left, const _type &Right, const _type &Bottom, const _type &Top, const _type &Near, const _type &Far) noexcept
        {
            _type _1_o_rml = (_type)1 / (Right - Left);
            _type _1_o_tmb = (_type)1 / (Top - Bottom);
            _type _1_o_fmn = (_type)1 / (Far - Near);

            return typeMat4(
                (_type)2 * _1_o_rml, 0, 0, -(Right + Left) * _1_o_rml,
                0, (_type)2 * _1_o_tmb, 0, -(Top + Bottom) * _1_o_tmb,
                0, 0, (_type)2 * _1_o_fmn, -(Far + Near) * _1_o_fmn,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 projection_ortho_lh_zero_one(const _type &Left, const _type &Right, const _type &Bottom, const _type &Top, const _type &Near, const _type &Far) noexcept
        {
            _type _1_o_rml = (_type)1 / (Right - Left);
            _type _1_o_tmb = (_type)1 / (Top - Bottom);
            _type _1_o_fmn = (_type)1 / (Far - Near);

            return typeMat4(
                (_type)2 * _1_o_rml, 0, 0, -(Right + Left) * _1_o_rml,
                0, (_type)2 * _1_o_tmb, 0, -(Top + Bottom) * _1_o_tmb,
                0, 0, _1_o_fmn, -Near * _1_o_fmn,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 cameraLookAtRH(const typeVec3 &front, const typeVec3 &up, const typeVec3 &position) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;

            z = OP<typeVec3>::normalize(lookTo) * -1;
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);

            return // scale(0.002f,0.002f,0.002f)*
                typeMat4(x.x, x.y, x.z, -OP<typeVec3>::dot(x, position),
                         y.x, y.y, y.z, -OP<typeVec3>::dot(y, position),
                         z.x, z.y, z.z, -OP<typeVec3>::dot(z, position),
                         0, 0, 0, 1);
        }

        static __forceinline typeMat4 cameraLookAtLH(const typeVec3 &front, const typeVec3 &up, const typeVec3 &position) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;

            z = OP<typeVec3>::normalize(lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);

            return // scale(0.002f,0.002f,0.002f)*
                typeMat4(x.x, x.y, x.z, -OP<typeVec3>::dot(x, position),
                         y.x, y.y, y.z, -OP<typeVec3>::dot(y, position),
                         z.x, z.y, z.z, -OP<typeVec3>::dot(z, position),
                         0, 0, 0, 1);
        }

        static __forceinline typeMat4 cameraLookAtRH(const typeVec4 &front, const typeVec4 &up, const typeVec4 &position) noexcept
        {
            typeVec4 lookTo = front;
            typeVec4 x, y, z;
            z = OP<typeVec4>::normalize(lookTo) * -1;
            x = OP<typeVec4>::normalize(OP<typeVec4>::cross(up, z));
            y = OP<typeVec4>::cross(z, x);

            return // scale(0.002f,0.002f,0.002f)*
                typeMat4(x.x, x.y, x.z, -OP<typeVec4>::dot(x, position),
                         y.x, y.y, y.z, -OP<typeVec4>::dot(y, position),
                         z.x, z.y, z.z, -OP<typeVec4>::dot(z, position),
                         0, 0, 0, 1);
        }

        static __forceinline typeMat4 cameraLookAtLH(const typeVec4 &front, const typeVec4 &up, const typeVec4 &position) noexcept
        {
            typeVec4 lookTo = front;
            typeVec4 x, y, z;
            z = OP<typeVec4>::normalize(lookTo);
            x = OP<typeVec4>::normalize(OP<typeVec4>::cross(up, z));
            y = OP<typeVec4>::cross(z, x);

            return // scale(0.002f,0.002f,0.002f)*
                typeMat4(x.x, x.y, x.z, -OP<typeVec4>::dot(x, position),
                         y.x, y.y, y.z, -OP<typeVec4>::dot(y, position),
                         z.x, z.y, z.z, -OP<typeVec4>::dot(z, position),
                         0, 0, 0, 1);
        }

        static __forceinline typeMat4 modelLookAtRH(const typeVec3 &front, const typeVec3 &up, const typeVec3 &position) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(lookTo) * -1;
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat4(typeVec4(x, 0),
                            typeVec4(y, 0),
                            typeVec4(z, 0),
                            typeVec4(position, 1));
        }

        static __forceinline typeMat4 modelLookAtLH(const typeVec3 &front, const typeVec3 &up, const typeVec3 &position) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat4(typeVec4(x, 0),
                            typeVec4(y, 0),
                            typeVec4(z, 0),
                            typeVec4(position, 1));
        }

        static __forceinline typeMat4 modelLookAtRH(const typeVec4 &front, const typeVec4 &up, const typeVec4 &position) noexcept
        {
            typeVec4 lookTo = front;
            typeVec4 x, y, z;
            z = OP<typeVec4>::normalize(lookTo) * -1;
            x = OP<typeVec4>::normalize(OP<typeVec4>::cross(up, z));
            y = OP<typeVec4>::cross(z, x);
            return typeMat4(x, y, z,
                            position);
        }

        static __forceinline typeMat4 modelLookAtLH(const typeVec4 &front, const typeVec4 &up, const typeVec4 &position) noexcept
        {
            typeVec4 lookTo = front;
            typeVec4 x, y, z;
            z = OP<typeVec4>::normalize(lookTo);
            x = OP<typeVec4>::normalize(OP<typeVec4>::cross(up, z));
            y = OP<typeVec4>::cross(z, x);
            return typeMat4(x, y, z,
                            position);
        }

        static __forceinline typeMat4 lookAtRotationRH(const typeVec2 &_front, const typeVec2 &position) noexcept
        {
            typeVec2 front = OP<typeVec2>::normalize(_front);
            typeVec2 side = OP<typeVec2>::cross_z_up(front);

            typeVec4 x, y, w; // z,
            x = typeVec4(front.x, front.y, 0, 0) * -1;
            y = typeVec4(side.x, side.y, 0, 0);
            // z = typeVec4(0, 0, 1, 0);
            w = typeVec4(position.x, position.y, 0, 1);
#if defined(ITK_SSE2)
            return typeMat4(x, y, _vec4_0010_sse, w);
#elif defined(ITK_NEON)
            return typeMat4(x, y, _neon_0010, w);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 lookAtRotationLH(const typeVec3 &_front, const typeVec2 &position) noexcept
        {
            typeVec2 front = OP<typeVec2>::normalize(_front);
            typeVec2 side = OP<typeVec2>::cross_z_up(front);

            typeVec4 x, y, w; // z,
            x = typeVec4(front.x, front.y, 0, 0);
            y = typeVec4(side.x, side.y, 0, 0);
            // z = typeVec4(0, 0, 1, 0);
            w = typeVec4(position.x, position.y, 0, 1);

#if defined(ITK_SSE2)
            return typeMat4(x, y, _vec4_0010_sse, w);
#elif defined(ITK_NEON)
            return typeMat4(x, y, _neon_0010, w);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            
        }

        /// \brief Construct a 4x4 transformation matrix from a quaternion
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// quat rotation = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
        ///
        /// mat4 result = fromQuat( rotation );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param q The rotation quaternion
        /// \return The mat4
        ///
        static __forceinline typeMat4 fromQuat(const quatT &q) noexcept
        {
#if defined(ITK_SSE2)
            __m128 elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 0, 1));
            __m128 elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 2, 1, 1));
            __m128 op0 = _mm_mul_ps(elm0, elm1);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 3, 3, 2));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 2, 2));
            __m128 op1 = _mm_mul_ps(elm0, elm1);
            const __m128 mask01 = _mm_setr_ps(1.0f, 1.0f, -1.0f, 0.0f);
            op1 = _mm_mul_ps(mask01, op1);

            __m128 m0_ = _mm_add_ps(op0, op1);
            const __m128 mask02 = _mm_setr_ps(-2.0f, 2.0f, 2.0f, 0.0f);
            m0_ = _mm_mul_ps(mask02, m0_);
            // m0_[0] += 1.0f;
            m0_ = _mm_add_ps(m0_, _vec4_1000_sse);

            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 0, 0));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 2, 0, 1));
            op0 = _mm_mul_ps(elm0, elm1);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 3, 2, 3));
            __m128 elm1_s = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 2, 2));
            op1 = _mm_mul_ps(elm0, elm1_s);
            const __m128 mask03 = _mm_setr_ps(-1.0f, 1.0f, 1.0f, 0.0f);
            op1 = _mm_mul_ps(mask03, op1);

            __m128 m1_ = _mm_add_ps(op0, op1);
            const __m128 mask04 = _mm_setr_ps(2.0f, -2.0f, 2.0f, 0.0f);
            m1_ = _mm_mul_ps(mask04, m1_);
            // m1_[1] += 1.0f;
            m1_ = _mm_add_ps(m1_, _vec4_0100_sse);

            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 1, 0));
            // elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 2, 2));
            op0 = _mm_mul_ps(elm0, elm1_s);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 3, 3));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 0, 1));
            op1 = _mm_mul_ps(elm0, elm1);
            const __m128 mask05 = _mm_setr_ps(1.0f, -1.0f, 1.0f, 0.0f);
            op1 = _mm_mul_ps(mask05, op1);

            __m128 m2_ = _mm_add_ps(op0, op1);
            const __m128 mask06 = _mm_setr_ps(2.0f, 2.0f, -2.0f, 0.0f);
            m2_ = _mm_mul_ps(mask06, m2_);
            // m2_[2] += 1.0f;
            m2_ = _mm_add_ps(m2_, _vec4_0010_sse);

            //__m128 m3_ = (__m128){0,0,0,1.0f};

            return typeMat4(m0_, m1_, m2_, _vec4_0001_sse);
#elif defined(ITK_NEON)
            float32x4_t x2y2z2 = vmulq_f32(q.array_neon, q.array_neon);

            float32x4_t op0 = vshuffle_0100(q.array_neon);
            float32x4_t op1 = vshuffle_0221(q.array_neon);

            float32x4_t xy_xz_yz = vmulq_f32(op0, op1);
            float32x4_t wx_wy_wz = vmulq_f32(vshuffle_3333(q.array_neon), q.array_neon);

            const float32x4_t _2 = vset1(2.0f);

            float32x4_t m0 =
                vmulq_f32(_2, (float32x4_t){
                                  (x2y2z2[1] + x2y2z2[2]),
                                  (xy_xz_yz[0] + wx_wy_wz[2]),
                                  (xy_xz_yz[1] - wx_wy_wz[1]),
                                  0});

            m0[0] = 1.0f - m0[0];

            float32x4_t m1 =
                vmulq_f32(_2, (float32x4_t){
                                  (xy_xz_yz[0] - wx_wy_wz[2]),
                                  (x2y2z2[0] + x2y2z2[2]),
                                  (xy_xz_yz[2] + wx_wy_wz[0]),
                                  0});
            m1[1] = 1.0f - m1[1];

            float32x4_t m2 =
                vmulq_f32(_2, (float32x4_t){
                                  (xy_xz_yz[1] + wx_wy_wz[1]),
                                  (xy_xz_yz[2] - wx_wy_wz[0]),
                                  (x2y2z2[0] + x2y2z2[1]),
                                  0});

            m2[2] = 1.0f - m2[2];

            // const float32x4_t m3 = (float32x4_t){0,0,0,1.0f};

            return typeMat4(m0, m1, m2, _neon_0001); // m3
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 fromMat2(const typeMat2 &v) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return typeMat4(
                _mm_setr_ps(v.a1, v.a2, 0, 0),
                _mm_setr_ps(v.b1, v.b2, 0, 0),
                _vec4_0010_sse,
                _vec4_0001_sse);
#else
            return typeMat4(
                _mm_blend_ps(
                    _mm_shuffle_ps(v.array_sse, v.array_sse, _MM_SHUFFLE(1, 0, 1, 0)),
                    _vec4_zero_sse, 0xC),
                _mm_blend_ps(
                    _mm_shuffle_ps(v.array_sse, v.array_sse, _MM_SHUFFLE(3, 2, 3, 2)),
                    _vec4_zero_sse, 0xC),
                _vec4_0010_sse,
                _vec4_0001_sse);
#endif
#elif defined(ITK_NEON)
            return typeMat4(
                (float32x4_t){v.a1, v.a2, 0, 0},
                (float32x4_t){v.b1, v.b2, 0, 0},
                _neon_0010,
                _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static __forceinline typeMat4 fromMat3(const typeMat3 &m) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat4(
                _mm_and_ps(m.array_sse[0], _vec3_valid_bits_sse),
                _mm_and_ps(m.array_sse[1], _vec3_valid_bits_sse),
                _mm_and_ps(m.array_sse[2], _vec3_valid_bits_sse),
                _vec4_0001_sse);
#elif defined(ITK_NEON)
            return typeMat4(
                typeVec4(m[0], 0).array_neon,
                typeVec4(m[1], 0).array_neon,
                typeVec4(m[2], 0).array_neon,
                _neon_0001);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
    };
}