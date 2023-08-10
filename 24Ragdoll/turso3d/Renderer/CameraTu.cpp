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

	WORLD_XAXIS = Vector3f(1.0f, 0.0f, 0.0f);
	WORLD_YAXIS = Vector3f(0.0f, 1.0f, 0.0f);
	WORLD_ZAXIS = Vector3f(0.0f, 0.0f, 1.0f);

	m_accumPitchDegrees = 0.0f;
	m_rotationSpeed = 0.1f;
	m_movingSpeed = 1.0f;
	m_offsetDistance = 0.0f;

	m_xAxis.set(1.0f, 0.0f, 0.0f);
	m_yAxis.set(0.0f, 1.0f, 0.0f);
	m_zAxis.set(0.0f, 0.0f, 1.0f);
	m_viewDir.set(0.0f, 0.0f, -1.0f);

	m_eye = Vector3f(0.0f, 0.0f, 0.0f);
	m_persMatrix.identity();
	m_invPersMatrix.identity();
	m_orthMatrix.identity();
	m_invOrthMatrix.identity();

	orthogonalize();
	updateViewMatrix();
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
		ret.Define(fov, aspectRatio, zoom, NearClip(), farClip, EffectiveWorldTransform(), ProjectionMatrix(), ViewMatrix(), m_persMatrix, m_viewMatrix);
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
        float h = (1.0f / tanf(fov * M_DEGTORAD * 0.5f)) * zoom;
        float w = h / aspectRatio;
        float q = farClip / (farClip - nearClip);
        float r = -q * nearClip;

        ret.m00 = w;
        ret.m02 = 0.0f;
        ret.m11 = h;
        ret.m12 = 0.0f;
		/*ret.m22 = q;
        ret.m23 = r;
        ret.m32 = 1.0f;*/

		ret.m22 = -q;
		ret.m23 = -1.0f;
		ret.m32 = 2.0f * r;

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
    Matrix3x4 transform(WorldPosition(), WorldRotation(), 1.0f);

	Matrix4 trans(transform);

    return useReflection ? (Matrix4(reflectionMatrix) * trans).Transpose() : trans.Transpose();
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

void CameraTu::orthogonalize() {

	Vector3f::Normalize(m_zAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	m_xAxis = Vector3f::Cross(m_yAxis, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_viewDir = -m_zAxis;

/*	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[1][0] = m_yAxis[0];
	m_viewMatrix[2][0] = m_zAxis[0];
	m_viewMatrix[3][0] = 0.0f;

	m_viewMatrix[0][1] = m_xAxis[1];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[2][1] = m_zAxis[1];
	m_viewMatrix[3][1] = 0.0f;

	m_viewMatrix[0][2] = m_xAxis[2];
	m_viewMatrix[1][2] = m_yAxis[2];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[3][2] = 0.0f;*/

	m_viewMatrix[0][0] = m_xAxis[0];
	m_viewMatrix[0][1] = m_yAxis[0];
	m_viewMatrix[0][2] = m_zAxis[0];
	m_viewMatrix[0][3] = 0.0f;

	m_viewMatrix[1][0] = m_xAxis[1];
	m_viewMatrix[1][1] = m_yAxis[1];
	m_viewMatrix[1][2] = m_zAxis[1];
	m_viewMatrix[1][3] = 0.0f;

	m_viewMatrix[2][0] = m_xAxis[2];
	m_viewMatrix[2][1] = m_yAxis[2];
	m_viewMatrix[2][2] = m_zAxis[2];
	m_viewMatrix[2][3] = 0.0f;

	m_invViewMatrix[0][0] = m_xAxis[0];
	m_invViewMatrix[0][1] = m_xAxis[1];
	m_invViewMatrix[0][2] = m_xAxis[2];
	m_invViewMatrix[0][3] = 0.0f;

	m_invViewMatrix[1][0] = m_yAxis[0];
	m_invViewMatrix[1][1] = m_yAxis[1];
	m_invViewMatrix[1][2] = m_yAxis[2];
	m_invViewMatrix[1][3] = 0.0f;

	m_invViewMatrix[2][0] = m_zAxis[0];
	m_invViewMatrix[2][1] = m_zAxis[1];
	m_invViewMatrix[2][2] = m_zAxis[2];
	m_invViewMatrix[2][3] = 0.0f;
}

void CameraTu::updateViewMatrix() {

	/*m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;*/

	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	m_invViewMatrix[3][0] = m_eye[0];
	m_invViewMatrix[3][1] = m_eye[1];
	m_invViewMatrix[3][2] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void CameraTu::rotateFirstPerson(float yaw, float pitch) {

	m_accumPitchDegrees += pitch;

	if (m_accumPitchDegrees > 90.0f) {
		pitch = 90.0f - (m_accumPitchDegrees - pitch);
		m_accumPitchDegrees = 90.0f;
	}

	if (m_accumPitchDegrees < -90.0f) {
		pitch = -90.0f - (m_accumPitchDegrees - pitch);
		m_accumPitchDegrees = -90.0f;
	}

	Matrix4f rotMtx;

	// Rotate camera's existing x and z axes about the world y axis.
	if (yaw != 0.0f) {
		rotMtx.rotate(WORLD_YAXIS, yaw);
		m_xAxis = rotMtx * m_xAxis;
		m_zAxis = rotMtx * m_zAxis;
	}

	// Rotate camera's existing y and z axes about its existing x axis.
	if (pitch != 0.0f) {
		rotMtx.rotate(m_xAxis, pitch);
		m_yAxis = rotMtx * m_yAxis;
		m_zAxis = rotMtx * m_zAxis;
	}
}

void CameraTu::rotate(float yaw, float pitch) {
	rotateFirstPerson(yaw * m_rotationSpeed, pitch * m_rotationSpeed);
	orthogonalize();
	updateViewMatrix();
}

void CameraTu::rotate(float yaw, float pitch, const Vector3f &target) {
	rotateFirstPerson(yaw * m_rotationSpeed, pitch * m_rotationSpeed);
	orthogonalize();
	m_eye = target - m_offsetDistance * m_viewDir;
	updateViewMatrix();
}

void CameraTu::move(float dx, float dy, float dz) {
	Vector3f eye = m_eye;
	eye += m_xAxis * dx * m_movingSpeed;
	eye += WORLD_YAXIS * dy * m_movingSpeed;
	eye += m_viewDir * dz * m_movingSpeed;
	setPosition(eye);
}

void CameraTu::move(const Vector3f &direction) {
	Vector3f eye = m_eye;
	eye += m_xAxis * direction[0] * m_movingSpeed;
	eye += WORLD_YAXIS * direction[1] * m_movingSpeed;
	eye += m_viewDir * direction[2] * m_movingSpeed;
	setPosition(eye);
}

void CameraTu::setPosition(float x, float y, float z) {
	m_eye = Vector3f(x, y, z);
	updateViewMatrix();
}

void CameraTu::setPosition(const Vector3f &position) {
	m_eye = position;
	updateViewMatrix();
}

void CameraTu::perspective(float fovx, float aspect, float znear, float zfar) {
	// Construct a projection matrix based on the horizontal field of view
	// 'fovx' rather than the more traditional vertical field of view 'fovy'.
	float e = tanf(PI_ON_180 * fovx * 0.5f);
	float xScale = (1.0f / (e * aspect));
	float yScale = (1.0f / e);

	m_persMatrix[0][0] = xScale;
	m_persMatrix[0][1] = 0.0f;
	m_persMatrix[0][2] = 0.0f;
	m_persMatrix[0][3] = 0.0f;

	m_persMatrix[1][0] = 0.0f;
	m_persMatrix[1][1] = yScale;
	m_persMatrix[1][2] = 0.0f;
	m_persMatrix[1][3] = 0.0f;

	m_persMatrix[2][0] = 0.0f;
	m_persMatrix[2][1] = 0.0f;
	m_persMatrix[2][2] = (zfar + znear) / (znear - zfar);
	m_persMatrix[2][3] = -1.0f;

	m_persMatrix[3][0] = 0.0f;
	m_persMatrix[3][1] = 0.0f;
	m_persMatrix[3][2] = (2.0f * zfar * znear) / (znear - zfar);
	m_persMatrix[3][3] = 0.0f;
}

const Matrix4& CameraTu::ViewMatrix() const { 
	if (viewMatrixDirty) { 
		viewMatrix = EffectiveWorldTransform().Inverse(); 
		viewMatrixDirty = false; 
	} 
	return viewMatrix;
}