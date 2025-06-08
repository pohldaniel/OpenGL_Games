#include <cmath>
#include <iostream>

#include "Camera.h"

Camera::Camera(){

	WORLD_XAXIS = Vector3f(1.0f, 0.0f, 0.0f);
	WORLD_YAXIS = Vector3f(0.0f, 1.0f, 0.0f);
	WORLD_ZAXIS = Vector3f(0.0f, 0.0f, 1.0f);

	m_accumPitchDegrees = 0.0f;
	m_accumYawDegrees = 0.0f;
	m_rotationSpeed = 1.0f;
	m_movingSpeed = 1.0f;
	m_offsetDistance = 0.0f;
	m_currentSettleTime = 0.0f;

    m_xAxis.set(1.0f, 0.0f, 0.0f);
    m_yAxis.set(0.0f, 1.0f, 0.0f);
    m_zAxis.set(0.0f, 0.0f, 1.0f);
    m_viewDir.set(0.0f, 0.0f, -1.0f);

	m_eye = Vector3f(0.0f, 0.0f, 0.0f) ;
    m_persMatrix.identity();
	m_invPersMatrix.identity();
	m_orthMatrix.identity();
	m_invOrthMatrix.identity();

	orthogonalize();
	updateViewMatrix();
}

Camera::Camera(Camera const& rhs) {

	WORLD_XAXIS = rhs.WORLD_XAXIS;
	WORLD_YAXIS = rhs.WORLD_YAXIS;
	WORLD_ZAXIS = rhs.WORLD_ZAXIS;

	m_accumPitchDegrees = rhs.m_accumPitchDegrees;
	m_accumYawDegrees = rhs.m_accumYawDegrees;
	m_rotationSpeed = rhs.m_rotationSpeed;
	m_movingSpeed = rhs.m_movingSpeed;
	m_offsetDistance = rhs.m_offsetDistance;

	m_eye = rhs.m_eye;
	m_xAxis = rhs.m_xAxis;
	m_yAxis = rhs.m_yAxis;
	m_zAxis = rhs.m_zAxis;
	m_viewDir = rhs.m_viewDir;
	m_target = rhs.m_target;
	m_currentSettleTime = rhs.m_currentSettleTime;

	m_viewMatrix = rhs.m_viewMatrix;
	m_invViewMatrix = rhs.m_invViewMatrix;

	m_persMatrix = rhs.m_persMatrix;
	m_invPersMatrix = rhs.m_invPersMatrix;
	m_orthMatrix = rhs.m_orthMatrix;
	m_invOrthMatrix = rhs.m_invOrthMatrix;
}

Camera::Camera(Camera&& rhs) {
	WORLD_XAXIS = rhs.WORLD_XAXIS;
	WORLD_YAXIS = rhs.WORLD_YAXIS;
	WORLD_ZAXIS = rhs.WORLD_ZAXIS;

	m_accumPitchDegrees = rhs.m_accumPitchDegrees;
	m_accumYawDegrees = rhs.m_accumYawDegrees;
	m_rotationSpeed = rhs.m_rotationSpeed;
	m_movingSpeed = rhs.m_movingSpeed;
	m_offsetDistance = rhs.m_offsetDistance;
	m_currentSettleTime = rhs.m_currentSettleTime;

	m_eye = rhs.m_eye;
	m_xAxis = rhs.m_xAxis;
	m_yAxis = rhs.m_yAxis;
	m_zAxis = rhs.m_zAxis;
	m_viewDir = rhs.m_viewDir;
	m_target = rhs.m_target;

	m_viewMatrix = rhs.m_viewMatrix;
	m_invViewMatrix = rhs.m_invViewMatrix;

	m_persMatrix = rhs.m_persMatrix;
	m_invPersMatrix = rhs.m_invPersMatrix;
	m_orthMatrix = rhs.m_orthMatrix;
	m_invOrthMatrix = rhs.m_invOrthMatrix;
}

Camera& Camera::operator=(const Camera& rhs) {
	WORLD_XAXIS = rhs.WORLD_XAXIS;
	WORLD_YAXIS = rhs.WORLD_YAXIS;
	WORLD_ZAXIS = rhs.WORLD_ZAXIS;

	m_accumPitchDegrees = rhs.m_accumPitchDegrees;
	m_accumYawDegrees = rhs.m_accumYawDegrees;
	m_rotationSpeed = rhs.m_rotationSpeed;
	m_movingSpeed = rhs.m_movingSpeed;
	m_offsetDistance = rhs.m_offsetDistance;
	m_currentSettleTime = rhs.m_currentSettleTime;

	m_eye = rhs.m_eye;
	m_xAxis = rhs.m_xAxis;
	m_yAxis = rhs.m_yAxis;
	m_zAxis = rhs.m_zAxis;
	m_viewDir = rhs.m_viewDir;
	m_target = rhs.m_target;

	m_viewMatrix = rhs.m_viewMatrix;
	m_invViewMatrix = rhs.m_invViewMatrix;

	m_persMatrix = rhs.m_persMatrix;
	m_invPersMatrix = rhs.m_invPersMatrix;
	m_orthMatrix = rhs.m_orthMatrix;
	m_invOrthMatrix = rhs.m_invOrthMatrix;
	return *this;
}

Camera::Camera(const Vector3f &eye, const Vector3f &target, const Vector3f &up) {

	WORLD_XAXIS = Vector3f(1.0f, 0.0f, 0.0f);
	WORLD_YAXIS = Vector3f(0.0f, 1.0f, 0.0f);
	WORLD_ZAXIS = Vector3f(0.0f, 0.0f, 1.0f);

	m_accumPitchDegrees = 0.0f;
	m_accumYawDegrees = 0.0f;
	m_rotationSpeed = 0.1f;
	m_movingSpeed = 1.0f;
	m_offsetDistance = 0.0f;
	m_currentSettleTime = 0.0f;

	m_persMatrix.identity();
	m_invPersMatrix.identity();
	m_orthMatrix.identity();
	m_invOrthMatrix.identity();

	lookAt(eye, target, up);
}

Camera::~Camera() {}

void Camera::orthogonalize() {

	Vector3f::Normalize(m_zAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	m_xAxis = Vector3f::Cross(m_yAxis, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_viewDir = -m_zAxis;

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

void Camera::updateViewMatrix() {

	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	m_invViewMatrix[3][0] = m_eye[0];
	m_invViewMatrix[3][1] = m_eye[1];
	m_invViewMatrix[3][2] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void Camera::perspective(float fovx, float aspect, float znear, float zfar){
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

	m_invPersMatrix[0][0] = e * aspect;
	m_invPersMatrix[0][1] = 0.0f;
	m_invPersMatrix[0][2] = 0.0f;
	m_invPersMatrix[0][3] = 0.0f;

	m_invPersMatrix[1][0] = 0.0f;
	m_invPersMatrix[1][1] = e;
	m_invPersMatrix[1][2] = 0.0f;
	m_invPersMatrix[1][3] = 0.0f;

	m_invPersMatrix[2][0] = 0.0f;
	m_invPersMatrix[2][1] = 0.0f;
	m_invPersMatrix[2][2] = 0.0f;
	m_invPersMatrix[2][3] = (znear - zfar) / (2.0f * zfar * znear);

	m_invPersMatrix[3][0] = 0.0f;
	m_invPersMatrix[3][1] = 0.0f;
	m_invPersMatrix[3][2] = -1.0f;
	m_invPersMatrix[3][3] = (znear + zfar) / (2.0f * zfar * znear);
}

void Camera::orthographic(float left, float right, float bottom, float top, float znear, float zfar){

	m_orthMatrix[0][0] = 2 / (right - left);
	m_orthMatrix[0][1] = 0.0f;
	m_orthMatrix[0][2] = 0.0f;
	m_orthMatrix[0][3] = 0.0f;

	m_orthMatrix[1][0] = 0.0f;
	m_orthMatrix[1][1] = 2 / (top - bottom);
	m_orthMatrix[1][2] = 0.0f;
	m_orthMatrix[1][3] = 0.0f;

	m_orthMatrix[2][0] = 0.0f;
	m_orthMatrix[2][1] = 0.0f;
	m_orthMatrix[2][2] = 2 / (znear - zfar);
	m_orthMatrix[2][3] = 0.0f;

	m_orthMatrix[3][0] = (right + left) / (left - right);
	m_orthMatrix[3][1] = (top + bottom) / (bottom - top);
	m_orthMatrix[3][2] = (zfar + znear) / (znear - zfar);
	m_orthMatrix[3][3] = 1.0f;

	m_invOrthMatrix[0][0] = (right - left) * 0.5f;
	m_invOrthMatrix[0][1] = 0.0f;
	m_invOrthMatrix[0][2] = 0.0f;
	m_invOrthMatrix[0][3] = 0.0f;

	m_invOrthMatrix[1][0] = 0.0f;
	m_invOrthMatrix[1][1] = (top - bottom) * 0.5f;
	m_invOrthMatrix[1][2] = 0.0f;
	m_invOrthMatrix[1][3] = 0.0f;

	m_invOrthMatrix[2][0] = 0.0f;
	m_invOrthMatrix[2][1] = 0.0f;
	m_invOrthMatrix[2][2] = (znear - zfar) * 0.5f;
	m_invOrthMatrix[2][3] = 0.0f;

	m_invOrthMatrix[3][0] = (right + left) * 0.5f;
	m_invOrthMatrix[3][1] = (top + bottom) * 0.5f;
	m_invOrthMatrix[3][2] = -(zfar + znear) * 0.5f;
	m_invOrthMatrix[3][3] = 1.0f;
}

void Camera::lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up){
	m_eye = eye;
	m_target = target;
	m_offsetDistance = (m_target - m_eye).length();

	m_zAxis = m_eye - target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	//WORLD_YAXIS = m_yAxis;

	m_viewDir = -m_zAxis;

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

	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	// Extract the pitch angle from the view matrix.
	m_accumPitchDegrees = -asinf(m_viewMatrix[2][1]) * _180_ON_PI;
	//m_accumPitchDegrees = atan2f(m_zAxis[1], sqrtf(m_zAxis[0] * m_zAxis[0] + m_zAxis[2] * m_zAxis[2])) * _180_ON_PI;
	m_accumYawDegrees = atan2f(m_zAxis[2], m_zAxis[0]) * _180_ON_PI - 90.0f;

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

	m_invViewMatrix[3][0] = m_eye[0];
	m_invViewMatrix[3][1] = m_eye[1];
	m_invViewMatrix[3][2] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void Camera::lookAt(const Vector3f& pos, float pitch, float yaw) {
	m_eye = pos;
	m_accumPitchDegrees = 0.0f;

	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);

	float sinY = sinf(yaw);
	float sinP = sinf(pitch);

	//m_xAxis[0] = cosY; m_xAxis[1] = 0.0f; m_xAxis[2] = -sinY;
	//m_yAxis[0] = sinY * sinP; m_yAxis[1] = cosP; m_yAxis[2] = cosY * sinP;
	//m_zAxis[0] = sinY * cosP; m_zAxis[1] = -sinP; m_zAxis[2] = cosP * cosY;

	//roll = 180 <-> up = z-Axe 
	m_xAxis[0] = -cosY; m_xAxis[1] = 0.0f; m_xAxis[2] = -sinY;
	m_yAxis[0] = -sinY * sinP; m_yAxis[1] = -cosP; m_yAxis[2] = cosY * sinP;
	m_zAxis[0] = -sinY * cosP; m_zAxis[1] = sinP; m_zAxis[2] = cosP * cosY;

	//WORLD_YAXIS = m_yAxis;

	m_viewDir = -m_zAxis;

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

	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	//m_accumPitchDegrees = -asinf(m_viewMatrix[2][1]) * _180_ON_PI;
	//m_accumPitchDegrees = atan2f(m_zAxis[1], sqrtf(m_zAxis[0] * m_zAxis[0] + m_zAxis[2] * m_zAxis[2])) * _180_ON_PI;
	m_accumYawDegrees = atan2f(m_zAxis[2], m_zAxis[0]) * _180_ON_PI - 90.0f;

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

	m_invViewMatrix[3][0] = m_eye[0];
	m_invViewMatrix[3][1] = m_eye[1];
	m_invViewMatrix[3][2] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void Camera::lookAt(const Vector3f &pos, float pitch, float yaw, float roll) {
	m_eye = pos;
	m_accumPitchDegrees = pitch;

	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = -cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	m_xAxis[0] = cosR * cosY - sinR * sinP * sinY; m_xAxis[1] = -sinR * cosP; m_xAxis[2] = cosR * sinY + sinR * sinP * cosY;
	m_yAxis[0] = sinR * cosY + cosR * sinP * sinY; m_yAxis[1] = cosR * cosP; m_yAxis[2] = sinR * sinY - cosR * sinP * cosY;
	m_zAxis[0] = -cosP * sinY; m_zAxis[1] = sinP; m_zAxis[2] = cosP * cosY;

	//WORLD_YAXIS = m_yAxis;
	m_viewDir = -m_zAxis;

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

	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	m_accumYawDegrees = atan2f(m_zAxis[2], m_zAxis[0]) * _180_ON_PI - 90.0f;

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

	m_invViewMatrix[3][0] = m_eye[0];
	m_invViewMatrix[3][1] = m_eye[1];
	m_invViewMatrix[3][2] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void Camera::lookAt(const Vector3f &pos, const Vector3f &target, float pitch, float yaw, float roll) {
	m_target = target;
	m_accumPitchDegrees = pitch;

	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = -cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	m_xAxis[0] = cosR * cosY - sinR * sinP * sinY; m_xAxis[1] = -sinR * cosP; m_xAxis[2] = cosR * sinY + sinR * sinP * cosY;
	m_yAxis[0] = sinR * cosY + cosR * sinP * sinY; m_yAxis[1] = cosR * cosP; m_yAxis[2] = sinR * sinY - cosR * sinP * cosY;
	m_zAxis[0] = -cosP * sinY; m_zAxis[1] = sinP; m_zAxis[2] = cosP * cosY;

	//WORLD_YAXIS = m_yAxis;
	m_viewDir = -m_zAxis;

	m_eye = m_target - m_offsetDistance * m_viewDir;

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

	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	m_accumYawDegrees = atan2f(m_zAxis[2], m_zAxis[0]) * _180_ON_PI - 90.0f;

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

	m_invViewMatrix[3][0] = m_eye[0];
	m_invViewMatrix[3][1] = m_eye[1];
	m_invViewMatrix[3][2] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}


void Camera::pitchReflection(const float distance) {
	m_viewMatrix[1][1] = -m_viewMatrix[1][1];
	m_viewMatrix[3][1] = 2 * distance + m_viewMatrix[3][1];
}

void Camera::move(float dx, float dy, float dz){
	Vector3f eye = m_eye;
	eye += m_xAxis * dx * m_movingSpeed;
	eye += WORLD_YAXIS * dy * m_movingSpeed;
	eye += m_viewDir * dz * m_movingSpeed;
	setPosition(eye);
}

void Camera::move(const Vector3f &direction) {
	Vector3f eye = m_eye;
	eye += m_xAxis * direction[0] * m_movingSpeed;
	eye += WORLD_YAXIS * direction[1] * m_movingSpeed;
	eye += m_viewDir * direction[2] * m_movingSpeed;
	setPosition(eye);
}

void Camera::moveRelative(const Vector3f &direction) {

	m_eye[0] += (m_xAxis[0] * direction[0] + m_yAxis[0] * direction[1] + m_viewDir[0] * direction[2]);
	m_eye[1] += (m_xAxis[1] * direction[0] + m_yAxis[1] * direction[1] + m_viewDir[1] * direction[2]);
	m_eye[2] += (m_xAxis[2] * direction[0] + m_yAxis[2] * direction[1] + m_viewDir[2] * direction[2]);

	updateViewMatrix();
}

const Vector3f Camera::getViewSpaceDirection(const Vector3f &direction) {

	return Vector3f(m_xAxis[0] * direction[0] + m_yAxis[0] * direction[1] + m_viewDir[0] * direction[2],
					m_xAxis[1] * direction[0] + m_yAxis[1] * direction[1] + m_viewDir[1] * direction[2],
					m_xAxis[2] * direction[0] + m_yAxis[2] * direction[1] + m_viewDir[2] * direction[2]);
}

const float Camera::getDistance() const {
	return (m_target - m_eye).length();
}

const float Camera::getDistanceSq() const {
	return (m_target - m_eye).lengthSq();
}

/*const float Camera::getYawDeg() const {
	return acosf(m_viewMatrix[2][2]) *  _180_ON_PI;
}*/

void Camera::moveX(float dx) {
	m_eye[0] = (dx * m_movingSpeed) + m_eye[0];
	m_viewMatrix[3][0] = -(m_xAxis[0] * (m_eye[0] -m_viewDir[0] * m_offsetDistance) + m_xAxis[1] * m_eye[1] + m_xAxis[2] * m_eye[2]);
	m_invViewMatrix[3][0] = m_eye[0] - m_viewDir[0] * m_offsetDistance;
}

void Camera::moveY(float dy) {
	m_eye[1] = (dy * m_movingSpeed) + m_eye[1];
	m_viewMatrix[3][1] = -(m_yAxis[0] * m_eye[0] + m_yAxis[1] * (m_eye[1] - m_viewDir[1] * m_offsetDistance) + m_yAxis[2] * m_eye[2]);
	m_invViewMatrix[3][1] = m_eye[1] - m_viewDir[1] * m_offsetDistance;
}

void Camera::moveZ(float dz) {
	m_eye[2] = (dz * m_movingSpeed) + m_eye[2];
	m_viewMatrix[3][2] = -(m_zAxis[0] * m_eye[0] + m_zAxis[1] * m_eye[1] + m_zAxis[2] * (m_eye[2] - m_viewDir[2] * m_offsetDistance));
	m_invViewMatrix[3][2] = m_eye[2] - m_viewDir[2] * m_offsetDistance;
}

void Camera::rotate(float yaw, float pitch) {
	rotateFirstPerson(yaw * m_rotationSpeed, pitch * m_rotationSpeed);
	orthogonalize();
	updateViewMatrix();
}

void Camera::rotate(float yaw, float pitch, const Vector3f &target) {
	rotateFirstPerson(yaw * m_rotationSpeed, pitch * m_rotationSpeed);
	orthogonalize();
	m_eye = target - m_offsetDistance * m_viewDir;
	updateViewMatrix();
}

void Camera::updateTarget() {
	m_target = m_eye + m_offsetDistance * m_viewDir;
}

void Camera::rotateFirstPerson(float yaw, float pitch){

	m_accumPitchDegrees += pitch;
	m_accumYawDegrees += yaw;
	
	if (m_accumPitchDegrees > 90.0f){
		pitch = 90.0f - (m_accumPitchDegrees - pitch);
		m_accumPitchDegrees = 90.0f;
	}

	if (m_accumPitchDegrees < -90.0f){
		pitch = -90.0f - (m_accumPitchDegrees - pitch);
		m_accumPitchDegrees = -90.0f;
	}
	
	Matrix4f rotMtx;

	// Rotate camera's existing x and z axes about the world y axis.
	if (yaw != 0.0f){
		rotMtx.rotate(WORLD_YAXIS, yaw);
		m_xAxis = rotMtx * m_xAxis;
		m_zAxis = rotMtx * m_zAxis;
	}

	// Rotate camera's existing y and z axes about its existing x axis.
	if (pitch != 0.0f){
		rotMtx.rotate(m_xAxis, pitch);
		m_yAxis = rotMtx * m_yAxis;
		m_zAxis = rotMtx * m_zAxis;
	}
}

const float Camera::getFar() const {
	return m_persMatrix[3][2] / (m_persMatrix[2][2] + 1);
}

const float Camera::getNear() const {
	return m_persMatrix[3][2] / (m_persMatrix[2][2] - 1);
}

const float Camera::getFovXDeg() const {
	return 2.0f * atanf(1.0f / m_persMatrix[1][1]) * _180_ON_PI;
}

const float Camera::getFovXRad() const {
	return 2.0f * atanf(1.0f / m_persMatrix[1][1]);
}

const float Camera::getScaleFactor() const{
	//return 1.0f / m_persMatrix[1][1];
	return m_invPersMatrix[1][1];
}

const float Camera::getAspect(bool ortho) const {
	//return m_persMatrix[1][1] / m_persMatrix[0][0];
	return ortho ? m_orthMatrix[0][0] / m_orthMatrix[1][1] : m_invPersMatrix[0][0] / m_invPersMatrix[1][1];
}

const float Camera::getLeftOrthographic() const {
	return -(1.0f / m_orthMatrix[0][0]) * (1.0f + m_orthMatrix[3][0]);
}

const float Camera::getRightOrthographic() const {
	return (1.0f / m_orthMatrix[0][0]) * (1.0f - m_orthMatrix[3][0]);
}

const float Camera::getBottomOrthographic() const {
	return -(1.0f / m_orthMatrix[1][1]) * (1.0f + m_orthMatrix[3][1]);
}

const float Camera::getTopOrthographic() const {
	return (1.0f / m_orthMatrix[1][1]) * (1.0f - m_orthMatrix[3][1]);
}

const float Camera::getNearOrthographic() const {
	return (1.0f / m_orthMatrix[2][2]) * (1.0f - m_orthMatrix[3][2]);
}

const float Camera::getFarOrthographic() const {
	return -(1.0f / m_orthMatrix[2][2]) * (1.0f + m_orthMatrix[3][2]);
}

const float Camera::getPitchDegrees() const {
	return m_accumPitchDegrees;
	//return -asinf(m_viewMatrix[2][1]) * _180_ON_PI;
}

const float Camera::getYawDegrees() const {
	return m_accumYawDegrees;
	//return atan2f(m_zAxis[2], m_zAxis[0]) * _180_ON_PI - 90.0f;
}

const float Camera::getOffsetDistance() const {
	return m_offsetDistance;
}

const float Camera::getRotationSpeed() const {
	return m_rotationSpeed;
}

void Camera::calcLightTransformation(Vector3f &direction) {
	
	/*float _near = getNear();
	float _far = getFar();

	Vector2f boundXFar;
	boundXFar[0] = tanf(0.5 * getFovXRad()) * _far * m_aspectRatio;
	boundXFar[1] = -boundXFar[0];

	Vector2f boundYFar;
	boundYFar[0] = tanf(0.5 * getFovXRad()) * _far;
	boundYFar[1] = -boundYFar[0];

	Vector2f boundXNear;
	boundXNear[0] = tanf(0.5 * getFovXRad()) * _near * m_aspectRatio;
	boundXNear[1] = -boundXNear[0] ;

	Vector2f boundYNear;
	boundYNear[0] = tanf(0.5 * getFovXRad()) * _near;
	boundYNear[1] = -boundYNear[0] ;	

	//viewspace
	Vector3f _nearTopLeft = Vector3f(boundXNear[1], boundYNear[0], -_near);
	Vector3f _nearTopRight = Vector3f(boundXNear[0], boundYNear[0], -_near);
	Vector3f _nearBottomLeft = Vector3f(boundXNear[1], boundYNear[1], -_near);
	Vector3f _nearBottomRight = Vector3f(boundXNear[0], boundYNear[1], -_near);

	Vector3f _farTopLeft = Vector3f(boundXFar[1], boundYFar[0], -_far);
	Vector3f _farTopRight = Vector3f(boundXFar[0], boundYFar[0], -_far);
	Vector3f _farBottomLeft = Vector3f(boundXFar[1], boundYFar[1], -_far);
	Vector3f _farBottomRight = Vector3f(boundXFar[0], boundYFar[1], -_far);

	//viewspace -> worldspace
	_nearTopLeft = m_invViewMatrix *  Vector4f(_nearTopLeft);
	_nearTopRight = m_invViewMatrix *  Vector4f(_nearTopRight);
	_nearBottomLeft = m_invViewMatrix *  Vector4f(_nearBottomLeft);
	_nearBottomRight = m_invViewMatrix *   Vector4f(_nearBottomRight);

	_farTopLeft = m_invViewMatrix *   Vector4f(_farTopLeft);
	_farTopRight = m_invViewMatrix *  Vector4f(_farTopRight);
	_farBottomLeft = m_invViewMatrix *  Vector4f(_farBottomLeft);
	_farBottomRight = m_invViewMatrix *   Vector4f(_farBottomRight);*/

	float near = getNear();
	float far = getFar();

	float heightNear = 2.0f * tanf(0.5f * getFovXRad()) * near;
	float widthNear = heightNear * getAspect();
	float heightFar = 2.0f * tanf(0.5f * getFovXRad()) * far;
	float widthFar = heightFar  * getAspect();


	//worldSpace
	Vector3f centerNear = (m_eye - m_viewDir * m_offsetDistance) + m_viewDir * near;
	Vector3f centerFar = (m_eye - m_viewDir * m_offsetDistance) + m_viewDir * far;
	Vector3f center = (m_eye - m_viewDir * m_offsetDistance) + m_viewDir * ((far + near) * 0.5f);
	
	Vector3f nearTopLeft = centerNear + m_yAxis * (heightNear * 0.5f) - m_xAxis * (widthNear * 0.5f);
	Vector3f nearTopRight = centerNear + m_yAxis * (heightNear * 0.5f) + m_xAxis * (widthNear * 0.5f);
	Vector3f nearBottomLeft = centerNear - m_yAxis * (heightNear * 0.5f) - m_xAxis * (widthNear * 0.5f);
	Vector3f nearBottomRight = centerNear - m_yAxis * (heightNear * 0.5f) + m_xAxis * (widthNear * 0.5f);

	Vector3f farTopLeft = centerFar + m_yAxis * (heightFar * 0.5f) - m_xAxis * (widthFar * 0.5f);
	Vector3f farTopRight = centerFar + m_yAxis * (heightFar * 0.5f) + m_xAxis * (widthFar * 0.5f);
	Vector3f farBottomLeft = centerFar - m_yAxis * (heightFar * 0.5f) - m_xAxis * (widthFar * 0.5f);
	Vector3f farBottomRight = centerFar - m_yAxis * (heightFar * 0.5f) + m_xAxis * (widthFar * 0.5f);

	//Set up vieMatrix
	/*Vector3f::Normalize(direction);
	Vector3f xaxis = Vector3f::Cross(Vector3f(0.0f, 1.0f, 0.0f), direction);
	Vector3f::Normalize(xaxis);
	
	Vector3f yaxis = Vector3f::Cross(direction, xaxis);
	Vector3f::Normalize(yaxis);

	lightView[0][0] = xaxis[0];
	lightView[1][0] = yaxis[0];
	lightView[2][0] = direction[0];
	lightView[3][0] = 0.0f;

	lightView[0][1] = xaxis[1];
	lightView[1][1] = yaxis[1];
	lightView[2][1] = direction[1];
	lightView[3][1] = 0.0f;

	lightView[0][2] = xaxis[2];
	lightView[1][2] = yaxis[2];
	lightView[2][2] = direction[2];
	lightView[3][2] = 0.0f;

	lightView[0][3] = 0.0f;
	lightView[1][3] = 0.0f;
	lightView[2][3] = 0.0f;
	lightView[3][3] = 1.0f;
	
	center = lightView * center;

	lightView[0][3] = -center[0];
	lightView[1][3] = -center[1];
	lightView[2][3] = -center[2];*/

	Vector3f::Normalize(direction);
	lightView.lookAt(center + direction, center, Vector3f(0.0f, 1.0f, 0.0f));

	Vector3f transforms[] = {
		Vector4f(nearTopLeft) * lightView ,
		Vector4f(nearTopRight) * lightView ,
		Vector4f(nearBottomLeft) * lightView ,
		Vector4f(nearBottomRight) * lightView ,

		Vector4f(farTopLeft) * lightView ,
		Vector4f(farTopRight) * lightView ,
		Vector4f(farBottomLeft) * lightView ,
		Vector4f(farBottomRight) * lightView
	};

	float minX, maxX, minY, maxY, minZ, maxZ;
	for (unsigned short i = 0; i < 8; i++) {
		if (i == 0) {
			minX = transforms[i][0];
			maxX = transforms[i][0];
			minY = transforms[i][1];
			maxY = transforms[i][1];
			minZ = transforms[i][2];
			maxZ = transforms[i][2];
		}

		minX = std::min(minX, transforms[i][0]);
		maxX = std::max(maxX, transforms[i][0]);
		minY = std::min(minY, transforms[i][1]);
		maxY = std::max(maxY, transforms[i][1]);
		minZ = std::min(minZ, transforms[i][2]);
		maxZ = std::max(maxZ, transforms[i][2]);
	}

	lightPerspective.orthographic(minX, maxX, minY, maxY, minZ, maxZ);
}

void Camera::calcLightTransformation(Vector3f &direction, float near, float far, Matrix4f& viewMatrix, Matrix4f& projectionMatrix) {
	float heightNear = 2.0f * tanf(0.5f * getFovXRad()) * near;
	float widthNear = heightNear * getAspect();
	float heightFar = 2.0f * tanf(0.5f * getFovXRad()) * far;
	float widthFar = heightFar  * getAspect();

	//worldSpace
	Vector3f centerNear = (m_eye - m_viewDir * m_offsetDistance) + m_viewDir * near;
	Vector3f centerFar = (m_eye - m_viewDir * m_offsetDistance) + m_viewDir * far;
	Vector3f center = (m_eye - m_viewDir * m_offsetDistance) + m_viewDir * ((far + near) * 0.5f);
	
	Vector3f nearTopLeft = centerNear + m_yAxis * (heightNear * 0.5f) - m_xAxis * (widthNear * 0.5f);
	Vector3f nearTopRight = centerNear + m_yAxis * (heightNear * 0.5f) + m_xAxis * (widthNear * 0.5f);
	Vector3f nearBottomLeft = centerNear - m_yAxis * (heightNear * 0.5f) - m_xAxis * (widthNear * 0.5f);
	Vector3f nearBottomRight = centerNear - m_yAxis * (heightNear * 0.5f) + m_xAxis * (widthNear * 0.5f);

	Vector3f farTopLeft = centerFar + m_yAxis * (heightFar * 0.5f) - m_xAxis * (widthFar * 0.5f);
	Vector3f farTopRight = centerFar + m_yAxis * (heightFar * 0.5f) + m_xAxis * (widthFar * 0.5f);
	Vector3f farBottomLeft = centerFar - m_yAxis * (heightFar * 0.5f) - m_xAxis * (widthFar * 0.5f);
	Vector3f farBottomRight = centerFar - m_yAxis * (heightFar * 0.5f) + m_xAxis * (widthFar * 0.5f);

	Vector3f::Normalize(direction);
	viewMatrix.lookAt(center + direction, center, Vector3f(0.0f, 1.0f, 0.0f));

	Vector3f transforms[] = {
		Vector4f(nearTopLeft) * viewMatrix ,
		Vector4f(nearTopRight) * viewMatrix ,
		Vector4f(nearBottomLeft) * viewMatrix ,
		Vector4f(nearBottomRight) * viewMatrix ,

		Vector4f(farTopLeft) * viewMatrix ,
		Vector4f(farTopRight) * viewMatrix ,
		Vector4f(farBottomLeft) * viewMatrix ,
		Vector4f(farBottomRight) * viewMatrix
	};

	float minX, maxX, minY, maxY, minZ, maxZ;
	for (unsigned short i = 0; i < 8; i++) {
		if (i == 0) {
			minX = transforms[i][0];
			maxX = transforms[i][0];
			minY = transforms[i][1];
			maxY = transforms[i][1];
			minZ = transforms[i][2];
			maxZ = transforms[i][2];
		}

		minX = std::min(minX, transforms[i][0]);
		maxX = std::max(maxX, transforms[i][0]);
		minY = std::min(minY, transforms[i][1]);
		maxY = std::max(maxY, transforms[i][1]);
		minZ = std::min(minZ, transforms[i][2]);
		maxZ = std::max(maxZ, transforms[i][2]);
	}

	constexpr float zMult = 10.0f;
	minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
	maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

	projectionMatrix.orthographic(minX, maxX, minY, maxY, minZ, maxZ);
}

void Camera::calcLightTransformation2(Vector3f &direction) {
	for (unsigned short i = 0; i < m_numberCascades; i++) {	
		calcLightTransformation(direction, m_bounds[i][0], m_bounds[i][1], lightViews[i], lightPerspectives[i]);
	}
}

void Camera::setUpLightTransformation(float distance) {
	float near = getNear();
	float far = getFar();

	while (far > 0){
		far = far - distance;		
		m_numberCascades++;			
	}
	
	far = getFar();

	lightViews.resize(m_numberCascades);
	lightPerspectives.resize(m_numberCascades);
	
	float _near = getNear();
	float _far = distance >= far ? far : distance;

	
	m_cascadeEndClipSpace = new float[m_numberCascades];

	for (unsigned short numberCascades = 0; numberCascades < m_numberCascades; numberCascades++) {		
		m_bounds.push_back(Vector2f(_near, _far));

		Vector4f vClip = Vector4f(0.0f, 0.0f, -_far, 1.0f) ^ m_persMatrix;
		
		//clipSpace
		m_cascadeEndClipSpace[numberCascades] = vClip[2];

		//screenSpace
		//m_cascadeEndClipSpace[numberCascades] = vClip[2] / vClip[3];

		_near = _far;
		_far = _far + distance;

		if (_far > far) {
			_far = far;	
			
		}	
	}	
}

void Camera::setUpLightTransformation(std::vector<Vector2f>& bounds) {
	m_bounds = bounds;
	m_numberCascades = static_cast<short>(bounds.size());
	lightViews.resize(m_numberCascades);
	lightPerspectives.resize(m_numberCascades);
	m_cascadeEndClipSpace = new float[m_numberCascades];

	for (unsigned short numberCascades = 0; numberCascades < m_numberCascades; numberCascades++) {
		//Vector4f vClip = m_projMatrix ^ Vector4f(0.0f, 0.0f, -bounds[numberCascades][1], 1.0f);

		Vector4f vClip =  Vector4f(0.0f, 0.0f, -bounds[numberCascades][1], 1.0f) ^ m_persMatrix;

		//clipSpace
		m_cascadeEndClipSpace[numberCascades] = vClip[2];
	}
}

void Camera::setPosition(float x, float y, float z){
	m_eye = Vector3f(x, y, z);
	updateViewMatrix();
}

void Camera::setPosition(const Vector3f &position){
	m_eye = position;
	updateViewMatrix();
}

void Camera::setTarget(const Vector3f& target) {
	m_target = target;
	m_eye = m_target - m_offsetDistance * m_viewDir;
	updateViewMatrix();
}

void Camera::setPositionX(float x) {
	m_eye[0] = x ;
	m_viewMatrix[3][0] = -(m_xAxis[0] * m_eye[0] + m_xAxis[1] * m_eye[1] + m_xAxis[2] * m_eye[2]);
	m_invViewMatrix[3][0] = m_eye[0];
}

void Camera::setPositionY(float y) {
	m_eye[1] = y;
	m_viewMatrix[3][1] = -(m_yAxis[0] * m_eye[0] + m_yAxis[1] * m_eye[1] + m_yAxis[2] * m_eye[2]);
	m_invViewMatrix[3][1] = m_eye[1];
}

void Camera::setPositionZ(float z) {
	m_eye[2] = z;
	m_viewMatrix[3][2] = -(m_zAxis[0] * m_eye[0] + m_zAxis[1] * m_eye[1] + m_zAxis[2] * m_eye[2]);
	m_invViewMatrix[3][2] = m_eye[2];
}

void Camera::setAspect(float aspect) {

	float e = 1.0f / m_persMatrix[1][1];
	float xScale = (1.0f / (e * aspect));
	float yScale = (1.0f / e);

	m_persMatrix[0][0] = xScale;
	m_persMatrix[1][1] = yScale;

	m_invPersMatrix[0][0] = e * aspect;
	m_invPersMatrix[1][1] = e;
}

void Camera::setRotation(float pitch, float yaw, float roll) {
	
	m_accumPitchDegrees = 0.0f;
	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	m_xAxis[0] = cosR * cosY - sinR * sinP * sinY; m_xAxis[1] = -sinR * cosP; m_xAxis[2] = cosR * sinY + sinR * sinP * cosY;
	m_yAxis[0] = sinR * cosY + cosR * sinP * sinY; m_yAxis[1] = cosR * cosP; m_yAxis[2] = sinR * sinY - cosR * sinP * cosY;
	m_zAxis[0] = -cosP * sinY; m_zAxis[1] = sinP; m_zAxis[2] = cosP * cosY;

	//WORLD_YAXIS = m_yAxis;
	m_viewDir = -m_zAxis;

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

	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

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

	m_invViewMatrix[3][0] = m_eye[0];
	m_invViewMatrix[3][1] = m_eye[1];
	m_invViewMatrix[3][2] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void Camera::setDirection(const Vector3f &direction, const Vector3f &up) {
	m_viewDir = direction;
	m_zAxis = -direction;
	m_yAxis = up;

	m_xAxis = Vector3f::Cross(m_yAxis, m_zAxis);
	Vector3f::Normalize(m_xAxis);

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

void Camera::setRotationSpeed(float rotationSpeed){
	m_rotationSpeed = rotationSpeed;
}

void Camera::setMovingSpeed(float movingSpeed) {
	m_movingSpeed = movingSpeed;
}

const Vector3f &Camera::getPosition() const{
	return m_eye;
}

const float Camera::getPositionX() const {
	return m_eye[0];
}

const float Camera::getPositionY() const {
	return m_eye[1];
}

const float Camera::getPositionZ() const {
	return m_eye[2];
}

const Vector3f &Camera::getCamX() const{
	return m_xAxis;
}

const Vector3f &Camera::getCamY() const{
	return m_yAxis;
}

const Vector3f &Camera::getCamZ() const {
	return m_zAxis;
}

const Vector3f &Camera::getViewDirection() const {
	return m_viewDir;
}

const float Camera::getViewDirectionX() const {
	return m_viewDir[0];
}

const float Camera::getViewDirectionY() const {
	return m_viewDir[1];
}

const float Camera::getViewDirectionZ() const {
	return m_viewDir[2];
}

const Matrix4f &Camera::getPerspectiveMatrix() const{
	return m_persMatrix;
}

const Matrix4f &Camera::getInvPerspectiveMatrix() const{
	return  m_invPersMatrix;
}

const Matrix4f &Camera::getOrthographicMatrix() const{
	return m_orthMatrix;
}

const Matrix4f& Camera::getInvOrthographicMatrix() const {
	return m_invOrthMatrix;
}

const Matrix4f &Camera::getViewMatrix() const{
	return m_viewMatrix;
}

const Matrix4f &Camera::getInvViewMatrix() const{
	return m_invViewMatrix;
}

const Matrix4f Camera::getInvViewMatrixNew() const {
	return Matrix4f(m_viewMatrix[0][0], m_viewMatrix[1][0], m_viewMatrix[2][0], 0.0f,
					m_viewMatrix[0][1], m_viewMatrix[1][1], m_viewMatrix[2][1], 0.0f,
					m_viewMatrix[0][2], m_viewMatrix[1][2], m_viewMatrix[2][2], 0.0f,
				   -(m_viewMatrix[3][0] * m_viewMatrix[0][0] + m_viewMatrix[3][1] * m_viewMatrix[0][1] + m_viewMatrix[3][2] * m_viewMatrix[0][2]),
				   -(m_viewMatrix[3][0] * m_viewMatrix[1][0] + m_viewMatrix[3][1] * m_viewMatrix[1][1] + m_viewMatrix[3][2] * m_viewMatrix[1][2]),
				   -(m_viewMatrix[3][0] * m_viewMatrix[2][0] + m_viewMatrix[3][1] * m_viewMatrix[2][1] + m_viewMatrix[3][2] * m_viewMatrix[2][2]), 1.0);
}

const Matrix4f Camera::getInvOrthographicMatrixNew() const {
	float left = -(1.0f / m_orthMatrix[0][0]) * (1.0f + m_orthMatrix[3][0]);
	float right = (1.0f / m_orthMatrix[0][0]) * (1.0f - m_orthMatrix[3][0]);
	float bottom = -(1.0f / m_orthMatrix[1][1]) * (1.0f + m_orthMatrix[3][1]);
	float top = (1.0f / m_orthMatrix[1][1]) * (1.0f - m_orthMatrix[3][1]);
	float near = (1.0f / m_orthMatrix[2][2]) * (1.0f - m_orthMatrix[3][2]);
	float far = -(1.0f / m_orthMatrix[2][2]) * (1.0f + m_orthMatrix[3][2]);
	
	return Matrix4f((right - left) * 0.5f, 0.0f, 0.0f, 0.0f,
					 0.0f, (top - bottom) * 0.5f, 0.0f, 0.0f,
					 0.0f, 0.0f, (near - far) * 0.5f, 0.0f,
					 (right + left) * 0.5f, (top + bottom) * 0.5f, -(far + near) * 0.5f, 1.0f);
}

const Matrix4f Camera::getInvPerspectiveMatrixNew() const {
	float e = 1.0f / m_persMatrix[1][1];
	float aspect = m_persMatrix[1][1] / m_persMatrix[0][0];
	float near = m_persMatrix[3][2] / (m_persMatrix[2][2] - 1);
	float far = m_persMatrix[3][2] / (m_persMatrix[2][2] + 1);
	return Matrix4f(e * aspect, 0.0f, 0.0f, 0.0f,
					0.0f, e, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, (near - far) / (2 * far * near),
					0.0f, 0.0f, -1.0f , (near + far) / (2 * far * near));	
}

const Matrix4f Camera::getRotationMatrix(const Vector3f &position) const {
	
	return Matrix4f(m_viewMatrix[0][0], m_viewMatrix[1][0], m_viewMatrix[2][0], 0.0f,
					m_viewMatrix[0][1], m_viewMatrix[1][1], m_viewMatrix[2][1], 0.0f,
					m_viewMatrix[0][2], m_viewMatrix[1][2], m_viewMatrix[2][2], 0.0f,
					position[0], position[1], position[2], 1.0);
}

void Camera::setOffsetDistance(float offsetDistance) {
	m_offsetDistance = offsetDistance;
	m_eye = m_target - m_offsetDistance * m_viewDir;
	updateViewMatrix();
}

void Camera::follow(const Matrix4f& targetMat, const Vector3f& targetVelocity, const float dt){
	
	const Vector3f lookAtPos(0.0f, 1.0f, 0.0f);
	const Vector3f lookFromPos(0.0f, 1.0f, 1.0f);  // 0, 3, -2
	float viewDistance = m_offsetDistance;
	const float settleTime = 2.0f;

	Vector3f targetPos;
	targetPos.set(targetMat[3][0], targetMat[3][1], targetMat[3][2]);
	Vector3f targetRot;
	targetRot.set(targetMat[2][0], targetMat[2][1], targetMat[2][2]);

	static Vector3f lastBaseLookFromPos = targetPos - targetRot * 20.0f;
	static float facing = 1.0f;
	//static float currentSettleTime2 = 0.f;

	// camera intro
	if (m_currentSettleTime < settleTime) {
		Vector3f defaultPos = targetPos - targetRot * 20.0f;
		if (m_currentSettleTime > settleTime - 1.0f)
			lastBaseLookFromPos += (settleTime - m_currentSettleTime) * (defaultPos - lastBaseLookFromPos);
		else
			lastBaseLookFromPos = defaultPos;

		viewDistance += (settleTime - m_currentSettleTime) * (settleTime - m_currentSettleTime);
		m_currentSettleTime += dt;
	}

	float nudge = Vector3f::Length(targetPos, lastBaseLookFromPos);
	if (nudge < viewDistance) {

		nudge = viewDistance - nudge;
		lastBaseLookFromPos -= targetRot * nudge * 1.1f;
		lastBaseLookFromPos[1] += nudge * 0.2f;
	}

	lastBaseLookFromPos -= targetVelocity * (dt);

	// Nudge the camera so that is a bit more likely to look in the direction
	// that the car is facing. This helps most when maneuvering at low speed.
	Vector3f facingNudge = targetRot * 0.2f * facing;
	facingNudge[1] = 0.0f;
	lastBaseLookFromPos -= facingNudge;

	// Smoothly choose whether the car is facing forwards or backwards for the above code.
	float dot = Vector3f::Dot(targetPos - lastBaseLookFromPos, targetRot);
	facing = dot * 0.1f;
	if (facing >  1.0f) facing = 1.0f;  // ClampPosNeg
	else if (facing < -1.0f) facing = -1.0f;

	// Now we make the camera follow the new object position.
	Vector3f lookDirection = targetPos - lastBaseLookFromPos;
	float mag = lookDirection.length();
	if (mag > 0.0f)
		lookDirection /= mag;
	else
		lookDirection = Vector3f(0.0f, 0.0f, 1.0f);   // It would be much better to use the last valid value

	Vector3f currentLookFromPos = targetPos - lookDirection * viewDistance;

	lastBaseLookFromPos = currentLookFromPos;

	Vector3f currentLookAtPos;
	if (std::fabsf(lookDirection[1]) <= 0.99f) {
		Matrix4f rot = Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 0.0f), lookDirection, Vector3f(0.0f, 1.0f, 0.0f));
		currentLookFromPos += rot * lookFromPos;
		currentLookAtPos = lookDirection;	
		currentLookFromPos[1] += 2.0f;
		lookAt(currentLookFromPos, currentLookAtPos + currentLookFromPos, Vector3f(0.0f, 1.0f, 0.0f));			
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
const float ThirdPersonCamera::DEFAULT_SPRING_CONSTANT = 16.0f;
const float ThirdPersonCamera::DEFAULT_DAMPING_CONSTANT = 8.0f;

ThirdPersonCamera::ThirdPersonCamera() : Camera() {
	
	m_enableSpringSystem = true;
	m_springConstant = DEFAULT_SPRING_CONSTANT;
	m_dampingConstant = DEFAULT_DAMPING_CONSTANT;
	m_target.set(0.0f, 0.0f, 0.0f);
	m_velocity.set(0.0f, 0.0f, 0.0f);
}

ThirdPersonCamera::~ThirdPersonCamera() {

}

float ThirdPersonCamera::getDampingConstant() const {
	return m_dampingConstant;
}

float ThirdPersonCamera::getSpringConstant() const {
	return m_springConstant;
}

bool ThirdPersonCamera::springSystemIsEnabled() const {
	return m_enableSpringSystem;
}

const Vector3f& ThirdPersonCamera::getTarget() const {
	return m_target;
}

const Vector3f& ThirdPersonCamera::getVelocity() const {
	return m_velocity;
}

void ThirdPersonCamera::rotate(float yaw, float pitch) {
	rotateFirstPerson(yaw * m_rotationSpeed, pitch* m_rotationSpeed);
	orthogonalize();	
}

void ThirdPersonCamera::setSpringConstant(float springConstant) {
	// We're using a critically damped spring system where the damping ratio
	// is equal to one.
	//
	// damping ratio = m_dampingConstant / (2.0f * sqrtf(m_springConstant))
	m_springConstant = springConstant;
	m_dampingConstant = 2.0f * sqrtf(springConstant);
}

void ThirdPersonCamera::update(float elapsedTimeSec) {

	if (m_enableSpringSystem) {
		//updateViewMatrix(elapsedTimeSec);
		updateViewMatrixDump(elapsedTimeSec);
	}else {
		m_eye = m_target - m_offsetDistance * m_viewDir;
		Camera::updateViewMatrix();
	}
}

void ThirdPersonCamera::updateViewMatrix(float elapsedTimeSec) {
	
	Vector3f idealPosition = m_target + m_zAxis * m_offsetDistance;
	Vector3f displacement = m_eye - idealPosition;
	Vector3f springAcceleration = (-m_springConstant * displacement) - (m_dampingConstant * m_velocity);

	m_velocity += springAcceleration * elapsedTimeSec;
	m_eye += m_velocity * elapsedTimeSec;

	
	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;
}

void ThirdPersonCamera::updateViewMatrixDump(float elapsedTimeSec) {

	Vector3f displacement = m_eye - m_target;
	Vector3f springAcceleration = (-m_springConstant * displacement) - (m_dampingConstant * m_velocity);

	m_velocity += springAcceleration * elapsedTimeSec;
	m_eye += m_velocity * elapsedTimeSec;

	m_viewMatrix[3][0] = -Vector3f::Dot(m_xAxis, m_eye - m_offsetDistance * m_viewDir);
	m_viewMatrix[3][1] = -Vector3f::Dot(m_yAxis, m_eye - m_offsetDistance * m_viewDir);
	m_viewMatrix[3][2] = -Vector3f::Dot(m_zAxis, m_eye - m_offsetDistance * m_viewDir);
	m_viewMatrix[3][3] = 1.0f;
}

void ThirdPersonCamera::enableSpringSystem(bool enableSpringSystem){
	m_enableSpringSystem = enableSpringSystem;
}

void ThirdPersonCamera::setTarget(const Vector3f& target) {
	m_target = target;
}

///////////////////////////////////////////////////////////////////////////////////////
IsometricCamera::IsometricCamera(unsigned int width, unsigned int height) : Camera(), m_startAngle(-0.628f){
	m_height = 30.0f;
	m_distance = m_height * M_SQRT3;
	//distance = 50.0f;
	m_angle = m_startAngle;

	m_target[0] = 0.0f;
	m_target[1] = 0.0f;
	m_target[2] = 0.0f;

	m_initx = -780.0f;
	m_initz = 780.0f;

	m_breadth = 2000.0f;
	m_length = 2000.0f;

	m_hres = static_cast<float>(width);
	m_vres = static_cast<float>(height);

	
	m_eye[0] = m_distance * sinf(m_angle) + m_initx;
	m_eye[1] = m_height;
	m_eye[2] = m_distance * cosf(m_angle) + m_initz;

	m_yfactor = -sqrt(1 + (m_distance * m_distance) / (m_height * m_height));

	m_speed = 5.0f;

	move();
}

IsometricCamera::~IsometricCamera() {

}

void IsometricCamera::move() {
	m_eye[0] = m_distance * sin(m_angle) + m_initx;
	m_eye[2] = m_distance * cos(m_angle) + m_initz;

	m_target[0] = m_initx;
	m_target[2] = m_initz;

	lookAt(m_eye, m_target, Vector3f(0.0f, 1.0f, 0.0f));
}

void IsometricCamera::moveUp() {
	float tmpx, tmpy;
	convertCoordinates((m_hres / 2.0), (m_vres / 2.0), tmpx, tmpy);
	if (tmpy > m_breadth / 2)
		return;

	m_initx += sin(m_angle) * m_speed;
	m_initz += cos(m_angle) * m_speed;
	move();
}

void IsometricCamera::moveDown() {
	float tmpx, tmpy;
	convertCoordinates((m_hres / 2.0), (m_vres / 2.0), tmpx, tmpy);
	if (tmpy < -m_breadth / 2)
		return;

	m_initx -= sinf(m_angle) * m_speed;
	m_initz -= cosf(m_angle) * m_speed;
	move();
}

void IsometricCamera::moveLeft() {
	float tmpx, tmpy;
	convertCoordinates((m_hres / 2.0), (m_vres / 2.0), tmpx, tmpy);
	if (tmpx > m_length / 2)
		return;

	m_initx += cosf(m_angle) * m_speed;
	m_initz -= sinf(m_angle) * m_speed;
	move();
}

void IsometricCamera::moveRight() {
	float tmpx, tmpy;
	convertCoordinates((m_hres / 2.0), (m_vres / 2.0), tmpx, tmpy);
	if (tmpx < -m_length / 2)
		return;

	m_initx -= cos(m_angle) * m_speed;
	m_initz += sin(m_angle) * m_speed;
	move();
}

void IsometricCamera::rotate(float angle) {
	m_angle = angle;
	m_eye[0] = m_distance * sinf(angle) + m_initx;
	m_eye[2] = m_distance * cosf(angle) + m_initz;

	m_target[0] = m_initx;
	m_target[2] = m_initz;

	lookAt(m_eye, m_target, Vector3f(0.0f, 1.0f, 0.0f));
}

void IsometricCamera::setHeight(float height) {
	m_height = height;

	m_eye[0] = m_distance * sinf(m_angle) + m_initx;
	m_eye[1] = m_height;
	m_eye[2] = m_distance * cosf(m_angle) + m_initz;

	m_yfactor = -sqrtf(1 + (m_distance * m_distance) / (m_height * m_height));
	lookAt(m_eye, m_target, Vector3f(0.0f, 1.0f, 0.0f));
}

void IsometricCamera::resize(unsigned int width, unsigned int height) {
	m_hres = static_cast<float>(width);
	m_vres = static_cast<float>(height);
}

int IsometricCamera::convertCoordinates(unsigned int x, unsigned int y, float &x3, float &y3) {
	float newx = x - (m_hres / 2.0);
	float newy = m_yfactor * ((m_vres / 2.0) - y);

	x3 = newx * cosf(m_angle) + newy * sinf(m_angle);
	y3 = newy * cosf(m_angle) - newx * sinf(m_angle);

	x3 += m_eye[0];
	y3 += m_eye[2];

	return 0;
}

float IsometricCamera::getAngle() {
	return m_angle;
}

void IsometricCamera::scrollOver(float x, float y){
	m_initx = x;
	m_initz = y;
	move();
}