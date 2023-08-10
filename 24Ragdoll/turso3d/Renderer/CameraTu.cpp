// For conditions of distribution and use, see copyright notice in License.txt

#include "../Math/Matrix3x4.h"
#include "CameraTu.h"
#include <iostream>

static const float DEFAULT_NEAR_CLIP = 0.1f;
static const float DEFAULT_FAR_CLIP = 1000.0f;
static const float DEFAULT_FOV = 45.0f;
static const float DEFAULT_ORTHO_SIZE = 20.0f;

static const Matrix4 flipMatrix(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

CameraTu::CameraTu() :
    viewMatrix(Matrix3x4::IDENTITY),
    viewMatrixDirty(false),
    orthographic(false),
    flipVertical(false),
    nearClip(DEFAULT_NEAR_CLIP),
    farClip(DEFAULT_FAR_CLIP),
    fov(DEFAULT_FOV),
    orthoSize(DEFAULT_ORTHO_SIZE),
    aspectRatio(1.0f),
    zoom(1.0f),
    lodBias(1.0f),
    viewMask(M_MAX_UNSIGNED),
    reflectionPlane(Plane::UP),
    clipPlane(Plane::UP),
    useReflection(false),
    useClipping(false)
{
    reflectionMatrix = reflectionPlane.ReflectionMatrix();
}

void CameraTu::RegisterObject()
{
    RegisterFactory<CameraTu>();
    RegisterDerivedType<CameraTu, SpatialNode>();
    CopyBaseAttributes<CameraTu, SpatialNode>();

    RegisterAttribute("nearClip", &CameraTu::NearClip, &CameraTu::SetNearClip, DEFAULT_NEAR_CLIP);
    RegisterAttribute("farClip", &CameraTu::FarClip, &CameraTu::SetFarClip, DEFAULT_FAR_CLIP);
    RegisterAttribute("fov", &CameraTu::Fov, &CameraTu::SetFov, DEFAULT_FOV);
    RegisterAttribute("aspectRatio", &CameraTu::AspectRatio, &CameraTu::SetAspectRatio, 1.0f);
    RegisterAttribute("orthographic", &CameraTu::IsOrthographic, &CameraTu::SetOrthographic, false);
    RegisterAttribute("orthoSize", &CameraTu::OrthoSize, &CameraTu::SetOrthoSize, DEFAULT_ORTHO_SIZE);
    RegisterAttribute("zoom", &CameraTu::Zoom, &CameraTu::SetZoom, 1.0f);
    RegisterAttribute("lodBias", &CameraTu::LodBias, &CameraTu::SetLodBias, 1.0f);
    RegisterAttribute("viewMask", &CameraTu::ViewMask, &CameraTu::SetViewMask, M_MAX_UNSIGNED);
    RegisterMixedRefAttribute("reflectionPlane", &CameraTu::ReflectionPlaneAttr, &CameraTu::SetReflectionPlaneAttr, Vector4(0.0f, 1.0f, 0.0f, 0.0f));
    RegisterMixedRefAttribute("clipPlane", &CameraTu::ClipPlaneAttr, &CameraTu::SetClipPlaneAttr, Vector4(0.0f, 1.0f, 0.0f, 0.0f));
    RegisterAttribute("useReflection", &CameraTu::UseReflection, &CameraTu::SetUseReflection, false);
    RegisterAttribute("useClipping", &CameraTu::UseClipping, &CameraTu::SetUseClipping, false);
}

float CameraTu::NearClip() const
{
    // Orthographic camera has always near clip at 0 to avoid trouble with shader depth parameters,
    // and unlike in perspective mode there should be no depth buffer precision issue
    return orthographic ? 0.0f : nearClip;
}

FrustumTu CameraTu::WorldFrustum() const
{
    FrustumTu ret;

	if (!orthographic) {
		//ret.Define(fov, aspectRatio, zoom, NearClip(), farClip, EffectiveWorldTransform());
		ret.Define(ProjectionMatrix(), ViewMatrix());
	}else
        ret.DefineOrtho(orthoSize, aspectRatio, zoom, NearClip(), farClip, EffectiveWorldTransform());

    return ret;
}

FrustumTu CameraTu::WorldSplitFrustum(float nearClip_, float farClip_) const
{
    FrustumTu ret;

    nearClip_ = Max(nearClip_, NearClip());
    farClip_ = Min(farClip_, farClip);

    if (!orthographic)
        ret.Define(fov, aspectRatio, zoom, nearClip_, farClip_, EffectiveWorldTransform());
    else
        ret.DefineOrtho(orthoSize, aspectRatio, zoom, nearClip_, farClip_, EffectiveWorldTransform());

    return ret;
}

FrustumTu CameraTu::ViewSpaceFrustum() const
{
    FrustumTu ret;

    if (!orthographic)
        ret.Define(fov, aspectRatio, zoom, NearClip(), farClip);
    else
        ret.DefineOrtho(orthoSize, aspectRatio, zoom, NearClip(), farClip);

    return ret;
}

FrustumTu CameraTu::ViewSpaceSplitFrustum(float nearClip_, float farClip_) const
{
    FrustumTu ret;

    nearClip_ = Max(nearClip_, NearClip());
    farClip_ = Min(farClip_, farClip);
    if (farClip_ < nearClip_)
        farClip_ = nearClip_;

    if (!orthographic)
        ret.Define(fov, aspectRatio, zoom, nearClip_, farClip_);
    else
        ret.DefineOrtho(orthoSize, aspectRatio, zoom, nearClip_, farClip_);

    return ret;
}

Matrix4 CameraTu::ProjectionMatrix(bool apiSpecific) const
{
    Matrix4 ret(Matrix4::ZERO);

    bool openGLFormat = apiSpecific;

    if (!orthographic)
    {
		float e = tanf(PI_ON_180 * fov * 0.5f);
		float xScale = (1.0f / (e * aspectRatio));
		float yScale = (1.0f / e);

		ret.m00 = xScale;
		ret.m01 = 0.0f;
		ret.m02 = 0.0f;
		ret.m03 = 0.0f;

		ret.m10 = 0.0f;
		ret.m11 = yScale;
		ret.m12 = 0.0f;
		ret.m13 = 0.0f;

		ret.m20 = 0.0f;
		ret.m21 = 0.0f;
		ret.m22 = (farClip + nearClip) / (nearClip - farClip);
		ret.m23 = -1.0f;

		ret.m30 = 0.0f;
		ret.m31 = 0.0f;
		ret.m32 = (2.0f * farClip * nearClip) / (nearClip - farClip);
		ret.m33 = 0.0f;

    }
    else
    {
        // Disregard near clip, because it does not affect depth precision as with perspective projection
        float h = (1.0f / (orthoSize * 0.5f)) * zoom;
        float w = h / aspectRatio;
        float q = 1.0f / farClip;
        float r = 0.0f;

        ret.m00 = w;
        ret.m03 = 0.0f;
        ret.m11 = h;
        ret.m13 = 0.0f;
        ret.m22 = q;
        ret.m23 = r;
        ret.m33 = 1.0f;
    }

    if (flipVertical)
        ret = flipMatrix * ret;

    /*if (openGLFormat)
    {
        ret.m20 = 2.0f * ret.m20 - ret.m30;
        ret.m21 = 2.0f * ret.m21 - ret.m31;
        ret.m22 = 2.0f * ret.m22 - ret.m32;
        ret.m23 = 2.0f * ret.m23 - ret.m33;
    }*/

    return ret;
}

void CameraTu::FrustumSize(Vector3& near, Vector3& far) const
{
    near.z = NearClip();
    far.z = farClip;

    if (!orthographic)
    {
        float halfViewSize = tanf(fov * M_DEGTORAD * 0.5f) / zoom;
        near.y = near.z * halfViewSize;
        near.x = near.y * aspectRatio;
        far.y = far.z * halfViewSize;
        far.x = far.y * aspectRatio;
    }
    else
    {
        float halfViewSize = orthoSize * 0.5f / zoom;
        near.y = far.y = halfViewSize;
        near.x = far.x = near.y * aspectRatio;
    }

    if (flipVertical)
    {
        near.y = -near.y;
        far.y = -far.y;
    }
}

float CameraTu::HalfViewSize() const
{
    if (!orthographic)
        return tanf(fov * M_DEGTORAD * 0.5f) / zoom;
    else
        return orthoSize * 0.5f / zoom;
}

Ray CameraTu::ScreenRay(float x, float y) const
{
    Ray ret;

    // If projection is invalid, just return a ray pointing forward
    if (!IsProjectionValid())
    {
        ret.origin = WorldPosition();
        ret.direction = WorldDirection();
        return ret;
    }

    Matrix4 viewProjInverse = (ProjectionMatrix(false) * ViewMatrix()).Inverse();

    // The parameters range from 0.0 to 1.0. Expand to normalized device coordinates (-1.0 to 1.0) & flip Y axis
    x = 2.0f * x - 1.0f;
    y = 1.0f - 2.0f * y;
    Vector3 near(x, y, 0.0f);
    Vector3 far(x, y, 1.0f);

    ret.origin = viewProjInverse * near;
    ret.direction = ((viewProjInverse * far) - ret.origin).Normalized();
    return ret;
}

Vector2 CameraTu::WorldToScreenPoint(const Vector3& worldPos) const
{
    Vector3 eyeSpacePos = ViewMatrix() * worldPos;
    Vector2 ret;

    if (eyeSpacePos.z > 0.0f)
    {
        Vector3 screenSpacePos = ProjectionMatrix(false) * eyeSpacePos;
        ret.x = screenSpacePos.x;
        ret.y = screenSpacePos.y;
    }
    else
    {
        ret.x = (-eyeSpacePos.x > 0.0f) ? -1.0f : 1.0f;
        ret.y = (-eyeSpacePos.y > 0.0f) ? -1.0f : 1.0f;
    }

    ret.x = (ret.x * 0.5f) + 0.5f;
    ret.y = 1.0f - ((ret.y * 0.5f) + 0.5f);
    return ret;
}

Vector3 CameraTu::ScreenToWorldPoint(const Vector3& screenPos) const
{
    Ray ray = ScreenRay(screenPos.x, screenPos.y);
    return ray.origin + ray.direction * screenPos.z;
}

QuaternionTu CameraTu::FaceCameraRotation(const Vector3& position_, const QuaternionTu& rotation_, FaceCameraMode mode)
{
    switch (mode)
    {
    default:
        return rotation_;

    case FC_ROTATE_XYZ:
        return WorldRotation();

    case FC_ROTATE_Y:
        {
            Vector3 euler = rotation_.EulerAngles();
            euler.y = WorldRotation().EulerAngles().y;
            return QuaternionTu(euler.x, euler.y, euler.z);
        }

    case FC_LOOKAT_XYZ:
        {
			QuaternionTu lookAt;
            lookAt.FromLookRotation(position_ - WorldPosition());
            return lookAt;
        }

    case FC_LOOKAT_Y:
        {
            // Make the Y-only lookat happen on an XZ plane to make sure there are no unwanted transitions or singularities
            Vector3 lookAtVec(position_ - WorldPosition());
            lookAtVec.y = 0.0f;

			QuaternionTu lookAt;
            lookAt.FromLookRotation(lookAtVec);

            Vector3 euler = rotation_.EulerAngles();
            euler.y = lookAt.EulerAngles().y;
            return QuaternionTu(euler.x, euler.y, euler.z);
        }
    }
}

Matrix4 CameraTu::EffectiveWorldTransform() const
{
	//Matrix3x4 transform(WorldPosition(), WorldRotation(), 1.0f);
	//return useReflection ? reflectionMatrix * transform : transform;

	Matrix3x4 transform(WorldPosition(), WorldRotation(), 1.0f);
	Matrix4 trans(transform);
	return useReflection ? Matrix4(reflectionMatrix * transform).Transpose() : trans.Transpose();
}

bool CameraTu::IsProjectionValid() const
{
    return farClip > NearClip();
}

void CameraTu::OnTransformChanged()
{
    SpatialNode::OnTransformChanged();
    viewMatrixDirty = true;

}

void CameraTu::SetReflectionPlaneAttr(const Vector4& value)
{
    SetReflectionPlane(Plane(value));
}

void CameraTu::SetClipPlaneAttr(const Vector4& value)
{
    SetClipPlane(Plane(value));
}

Vector4 CameraTu::ReflectionPlaneAttr() const
{
    return reflectionPlane.ToVector4();
}

Vector4 CameraTu::ClipPlaneAttr() const
{
    return clipPlane.ToVector4();
}