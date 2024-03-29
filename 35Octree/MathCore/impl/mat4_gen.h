#pragma once

#include "mat4_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd>
    struct GEN<mat4<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_simd, SIMD_TYPE::NONE>::value>::type>
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
            return typeMat4(
                1, 0, 0, _x_,
                0, 1, 0, _y_,
                0, 0, 1, _z_,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 translateHomogeneous(const typeVec2 &_v_) noexcept
        {
            return typeMat4(
                1, 0, 0, _v_.x,
                0, 1, 0, _v_.y,
                0, 0, 1, 0,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 translateHomogeneous(const typeVec3 &_v_) noexcept
        {
            return typeMat4(
                1, 0, 0, _v_.x,
                0, 1, 0, _v_.y,
                0, 0, 1, _v_.z,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 translateHomogeneous(const typeVec4 &_v_) noexcept
        {
            return typeMat4(
                1, 0, 0, _v_.x,
                0, 1, 0, _v_.y,
                0, 0, 1, _v_.z,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 scaleHomogeneous(const _type &_x_, const _type &_y_, const _type &_z_ = (_type)1) noexcept
        {
            return typeMat4(
                _x_, 0, 0, 0,
                0, _y_, 0, 0,
                0, 0, _z_, 0,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 scaleHomogeneous(const typeVec2 &_v_) noexcept
        {
            return typeMat4(
                _v_.x, 0, 0, 0,
                0, _v_.y, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 scaleHomogeneous(const typeVec3 &_v_) noexcept
        {
            return typeMat4(
                _v_.x, 0, 0, 0,
                0, _v_.y, 0, 0,
                0, 0, _v_.z, 0,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 scaleHomogeneous(const typeVec4 &_v_) noexcept
        {
            return typeMat4(
                _v_.x, 0, 0, 0,
                0, _v_.y, 0, 0,
                0, 0, _v_.z, 0,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 xRotateHomogeneous(const _type &_phi_) noexcept
        {
            _type c = OP<_type>::cos(_phi_);
            _type s = OP<_type>::sin(_phi_);
            return typeMat4(
                1, 0, 0, 0,
                0, c, -s, 0,
                0, s, c, 0,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 yRotateHomogeneous(const _type &_theta_) noexcept
        {
            _type c = OP<_type>::cos(_theta_);
            _type s = OP<_type>::sin(_theta_);
            return typeMat4(
                c, 0, s, 0,
                0, 1, 0, 0,
                -s, 0, c, 0,
                0, 0, 0, 1);
        }

        static __forceinline typeMat4 zRotateHomogeneous(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
            return typeMat4(
                c, -s, 0, 0,
                s, c, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1);
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
            return // scale(0.002f,0.002f,0.002f)*
                typeMat4(x.x, y.x, z.x, position.x,
                         x.y, y.y, z.y, position.y,
                         x.z, y.z, z.z, position.z,
                         0, 0, 0, 1);
        }

        static __forceinline typeMat4 modelLookAtLH(const typeVec3 &front, const typeVec3 &up, const typeVec3 &position) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);
            return // scale(0.002f,0.002f,0.002f)*
                typeMat4(x.x, y.x, z.x, position.x,
                         x.y, y.y, z.y, position.y,
                         x.z, y.z, z.z, position.z,
                         0, 0, 0, 1);
        }

        static __forceinline typeMat4 modelLookAtRH(const typeVec4 &front, const typeVec4 &up, const typeVec4 &position) noexcept
        {
            typeVec4 lookTo = front;
            typeVec4 x, y, z;
            z = OP<typeVec4>::normalize(lookTo) * -1;
            x = OP<typeVec4>::normalize(OP<typeVec4>::cross(up, z));
            y = OP<typeVec4>::cross(z, x);
            return // scale(0.002f,0.002f,0.002f)*
                typeMat4(x.x, y.x, z.x, position.x,
                         x.y, y.y, z.y, position.y,
                         x.z, y.z, z.z, position.z,
                         0, 0, 0, 1);
        }

        static __forceinline typeMat4 modelLookAtLH(const typeVec4 &front, const typeVec4 &up, const typeVec4 &position) noexcept
        {
            typeVec4 lookTo = front;
            typeVec4 x, y, z;
            z = OP<typeVec4>::normalize(lookTo);
            x = OP<typeVec4>::normalize(OP<typeVec4>::cross(up, z));
            y = OP<typeVec4>::cross(z, x);
            return // scale(0.002f,0.002f,0.002f)*
                typeMat4(x.x, y.x, z.x, position.x,
                         x.y, y.y, z.y, position.y,
                         x.z, y.z, z.z, position.z,
                         0, 0, 0, 1);
        }

        static __forceinline typeMat4 lookAtRotationRH(const typeVec2 &_front, const typeVec2 &position) noexcept
        {
            typeVec2 front = OP<typeVec2>::normalize(_front);
            typeVec2 side = OP<typeVec2>::cross_z_up(front);

            typeVec4 x, y, z, w;
            w = typeVec4(position.x, position.y, 0, 1);
            z = typeVec4(0, 0, 1, 0);
            x = typeVec4(front.x, front.y, 0, 0) * -1;
            y = typeVec4(side.x, side.y, 0, 0);

            return typeMat4(x, y, z, w);
        }

        static __forceinline typeMat4 lookAtRotationLH(const typeVec3 &_front, const typeVec2 &position) noexcept
        {
            typeVec2 front = OP<typeVec2>::normalize(_front);
            typeVec2 side = OP<typeVec2>::cross_z_up(front);

            typeVec4 x, y, z, w;
            w = typeVec4(position.x, position.y, 0, 1);
            z = typeVec4(0, 0, 1, 0);
            x = typeVec4(front.x, front.y, 0, 0);
            y = typeVec4(side.x, side.y, 0, 0);

            return typeMat4(x, y, z, w);
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

            _type x2 = q.x * q.x;
            _type y2 = q.y * q.y;
            _type z2 = q.z * q.z;
            _type xy = q.x * q.y;
            _type xz = q.x * q.z;
            _type yz = q.y * q.z;
            _type wx = q.w * q.x;
            _type wy = q.w * q.y;
            _type wz = q.w * q.z;

            // This calculation would be a lot more complicated for non-unit length quaternions
            // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
            //   OpenGL
            return typeMat4((_type)1 - (_type)2 * (y2 + z2), (_type)2 * (xy - wz), (_type)2 * (xz + wy), 0,
                            (_type)2 * (xy + wz), (_type)1 - (_type)2 * (x2 + z2), (_type)2 * (yz - wx), 0,
                            (_type)2 * (xz - wy), (_type)2 * (yz + wx), (_type)1 - (_type)2 * (x2 + y2), 0,
                            0, 0, 0, 1);
        }

        static __forceinline typeMat4 fromMat2(const typeMat2 &v) noexcept
        {
            return typeMat4(
                typeVec4(v.a1, v.a2, 0, 0),
                typeVec4(v.b1, v.b2, 0, 0),
                typeVec4(0, 0, (_type)1, 0),
                typeVec4(0, 0, 0, (_type)1));
        }

        static __forceinline typeMat4 fromMat3(const typeMat3 &v) noexcept
        {
            return typeMat4(
                typeVec4(v[0], 0),
                typeVec4(v[1], 0),
                typeVec4(v[2], 0),
                typeVec4(0, 0, 0, (_type)1));
        }
    };
}