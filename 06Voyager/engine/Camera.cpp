#include <cmath>
#include <iostream>
#include <limits>

#include "camera.h"

Camera::Camera(){
	
	WORLD_XAXIS = Vector3f(1.0f, 0.0f, 0.0f);
	WORLD_YAXIS = Vector3f(0.0f, 1.0f, 0.0f);
	WORLD_ZAXIS = Vector3f(0.0f, 0.0f, 1.0f);

	m_fovx = 45.0f;
	m_znear = 0.1f;
	m_zfar = 1000.0f;
    m_aspectRatio = 0.0f;
	m_accumPitchDegrees = 0.0f;

    m_eye.set(0.0f, 0.0f, 0.0f);
    m_xAxis.set(1.0f, 0.0f, 0.0f);
    m_yAxis.set(0.0f, 1.0f, 0.0f);
    m_zAxis.set(0.0f, 0.0f, 1.0f);
    m_viewDir.set(0.0f, 0.0f, -1.0f);
    
    m_projMatrix.identity();
	m_orthMatrix.identity();
	
	updateViewMatrix(false);

	m_acceleration.set(0.0f, 0.0f, 0.0f);
	m_currentVelocity.set(0.0f, 0.0f, 0.0f);
	m_velocity.set(0.0f, 0.0f, 0.0f);
}

Camera::Camera(const Vector3f &eye, const Vector3f &target, const Vector3f &up) {
	m_accumPitchDegrees = 0.0f;
	m_eye = eye;
	m_projMatrix.identity();
	m_orthMatrix.identity();
	updateViewMatrix(eye, target, up);
}

Camera::~Camera() {}

void Camera::updateViewMatrix(bool orthogonalizeAxes){
	// Regenerate the camera's local axes to orthogonalize them.
	if (orthogonalizeAxes){

		Vector3f::Normalize(m_zAxis);

		m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
		Vector3f::Normalize(m_yAxis);

		m_xAxis = Vector3f::Cross(m_yAxis, m_zAxis);
		Vector3f::Normalize(m_xAxis);

		m_viewDir = -m_zAxis;
	}

	// Reconstruct the view matrix.
	m_viewMatrix[0][0] = m_xAxis[0];
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
	m_viewMatrix[3][2] = 0.0f;

	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	m_viewMatrixTranspose[0][0] = m_xAxis[0];
	m_viewMatrixTranspose[0][1] = m_yAxis[0];
	m_viewMatrixTranspose[0][2] = m_zAxis[0];
	m_viewMatrixTranspose[0][3] = 0.0f;

	m_viewMatrixTranspose[1][0] = m_xAxis[1];
	m_viewMatrixTranspose[1][1] = m_yAxis[1];
	m_viewMatrixTranspose[1][2] = m_zAxis[1];
	m_viewMatrixTranspose[1][3] = 0.0f;

	m_viewMatrixTranspose[2][0] = m_xAxis[2];
	m_viewMatrixTranspose[2][1] = m_yAxis[2];
	m_viewMatrixTranspose[2][2] = m_zAxis[2];
	m_viewMatrixTranspose[2][3] = 0.0f;

	m_viewMatrixTranspose[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrixTranspose[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrixTranspose[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrixTranspose[3][3] = 1.0f;

	m_invViewMatrix[0][0] = m_xAxis[0];
	m_invViewMatrix[1][0] = m_xAxis[1];
	m_invViewMatrix[2][0] = m_xAxis[2];
	m_invViewMatrix[3][0] = 0.0f;

	m_invViewMatrix[0][1] = m_yAxis[0];
	m_invViewMatrix[1][1] = m_yAxis[1];
	m_invViewMatrix[2][1] = m_yAxis[2];
	m_invViewMatrix[3][1] = 0.0f;

	m_invViewMatrix[0][2] = m_zAxis[0];
	m_invViewMatrix[1][2] = m_zAxis[1];
	m_invViewMatrix[2][2] = m_zAxis[2];
	m_invViewMatrix[3][2] = 0.0f;

	m_invViewMatrix[0][3] = m_eye[0];
	m_invViewMatrix[1][3] = m_eye[1];
	m_invViewMatrix[2][3] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void Camera::updateViewMatrix(const Vector3f &eye, const Vector3f &target, const Vector3f &up){

	m_zAxis = eye - target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	WORLD_YAXIS = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(WORLD_YAXIS);

	m_viewDir = -m_zAxis;

	m_viewMatrix[0][0] = m_xAxis[0];
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
	m_viewMatrix[3][2] = 0.0f;

	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, eye);
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, eye);
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, eye);
	m_viewMatrix[3][3] = 1.0f;

	m_viewMatrixTranspose[0][0] = m_xAxis[0];
	m_viewMatrixTranspose[0][1] = m_yAxis[0];
	m_viewMatrixTranspose[0][2] = m_zAxis[0];
	m_viewMatrixTranspose[0][3] = 0.0f;

	m_viewMatrixTranspose[1][0] = m_xAxis[1];
	m_viewMatrixTranspose[1][1] = m_yAxis[1];
	m_viewMatrixTranspose[1][2] = m_zAxis[1];
	m_viewMatrixTranspose[1][3] = 0.0f;

	m_viewMatrixTranspose[2][0] = m_xAxis[2];
	m_viewMatrixTranspose[2][1] = m_yAxis[2];
	m_viewMatrixTranspose[2][2] = m_zAxis[2];
	m_viewMatrixTranspose[2][3] = 0.0f;

	m_viewMatrixTranspose[3][0] = -Vector3f::Dot(m_xAxis, m_eye);
	m_viewMatrixTranspose[3][1] = -Vector3f::Dot(m_yAxis, m_eye);
	m_viewMatrixTranspose[3][2] = -Vector3f::Dot(m_zAxis, m_eye);
	m_viewMatrixTranspose[3][3] = 1.0f;

	// Extract the pitch angle from the view matrix.
	m_accumPitchDegrees = -asinf(m_viewMatrix[1][2])*180.f / PI;

	Matrix4f invView;
	invView.invLookAt(eye, target, up);
	m_invViewMatrix = invView;
}

void Camera::perspective(float fovx, float aspect, float znear, float zfar){
	// Construct a projection matrix based on the horizontal field of view
	// 'fovx' rather than the more traditional vertical field of view 'fovy'.

	float e = 1.0f /tanf(PI*fovx / 360.0f);
	float xScale = e / aspect;
	float yScale = e;

	m_projMatrix[0][0] = xScale;
	m_projMatrix[1][0] = 0.0f;
	m_projMatrix[2][0] = 0.0f;
	m_projMatrix[3][0] = 0.0f;

	m_projMatrix[0][1] = 0.0f;
	m_projMatrix[1][1] = yScale;
	m_projMatrix[2][1] = 0.0f;
	m_projMatrix[3][1] = 0.0f;

	m_projMatrix[0][2] = 0.0f;
	m_projMatrix[1][2] = 0.0f;
	m_projMatrix[2][2] = (zfar + znear) / (znear - zfar);
	m_projMatrix[3][2] = -1.0f;

	m_projMatrix[0][3] = 0.0f;
	m_projMatrix[1][3] = 0.0f;
	m_projMatrix[2][3] = (2.0f * zfar * znear) / (znear - zfar);
	m_projMatrix[3][3] = 0.0f;

	m_fovx = fovx;
	m_aspectRatio = aspect;
	m_znear = znear;
	m_zfar = zfar;	
}

void Camera::orthographic(float left, float right, float bottom, float top, float znear, float zfar){

	m_orthMatrix[0][0] = 2 / (right - left);
	m_orthMatrix[1][0] = 0.0f;
	m_orthMatrix[2][0] = 0.0f;
	m_orthMatrix[3][0] = 0.0f;

	m_orthMatrix[0][1] = 0.0f;
	m_orthMatrix[1][1] = 2 / (top - bottom);
	m_orthMatrix[2][1] = 0.0f;
	m_orthMatrix[3][1] = 0.0f;

	m_orthMatrix[0][2] = 0.0f;
	m_orthMatrix[1][2] = 0.0f;
	m_orthMatrix[2][2] = 2 / (znear - zfar);
	m_orthMatrix[3][2] = 0.0f;

	m_orthMatrix[0][3] = (right + left) / (left - right);
	m_orthMatrix[1][3] = (top + bottom) / (bottom - top);
	m_orthMatrix[2][3] = (zfar + znear) / (znear - zfar);
	m_orthMatrix[3][3] = 1.0f;

	m_znear = znear;
	m_zfar = zfar;
}

void Camera::lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up){
	m_eye = eye;

	m_zAxis = eye - target;
	Vector3f::Normalize(m_zAxis);

	m_xAxis = Vector3f::Cross(up, m_zAxis);
	Vector3f::Normalize(m_xAxis);

	m_yAxis = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(m_yAxis);

	WORLD_YAXIS = Vector3f::Cross(m_zAxis, m_xAxis);
	Vector3f::Normalize(WORLD_YAXIS);

	m_viewDir = -m_zAxis;

	m_viewMatrix[0][0] = m_xAxis[0];
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
	m_viewMatrix[3][2] = 0.0f;

	m_viewMatrix[0][3] = -Vector3f::Dot(m_xAxis, eye);
	m_viewMatrix[1][3] = -Vector3f::Dot(m_yAxis, eye);
	m_viewMatrix[2][3] = -Vector3f::Dot(m_zAxis, eye);
	m_viewMatrix[3][3] = 1.0f;

	// Extract the pitch angle from the view matrix.
	m_accumPitchDegrees = -asinf(m_viewMatrix[1][2])*180.f / PI;

	Matrix4f invView;
	invView.invLookAt(eye, target, up);
	m_invViewMatrix = invView;
}

void Camera::pitchReflection(const float distance) {
	m_viewMatrix[1][1] = -m_viewMatrix[1][1];
	m_viewMatrix[1][3] = 2 * distance + m_viewMatrix[1][3];
}

void Camera::move(float dx, float dy, float dz){
	Vector3f eye = m_eye;
	eye += m_xAxis * dx;
	eye += WORLD_YAXIS * dy;
	eye += m_viewDir * dz;
	setPosition(eye);
}

void Camera::updatePosition(const Vector3f &direction, float m_dt) {

	if (m_currentVelocity.lengthSq() != 0.0f) {
		Vector3f displacement = (m_currentVelocity * m_dt) + (0.5f * m_acceleration * m_dt * m_dt);
		move(displacement[0], displacement[1], displacement[2]);
	}

	if (direction[0] != 0.0f){
		// Camera is moving along the x axis.
		// Linearly accelerate up to the camera's max speed.

		m_currentVelocity[0] += direction[0] * m_acceleration[0] * m_dt;

		if (m_currentVelocity[0] > m_velocity[0])
			m_currentVelocity[0] = m_velocity[0];
		else if (m_currentVelocity[0] < -m_velocity[0])
			m_currentVelocity[0] = -m_velocity[0];
	}else{
		// Camera is no longer moving along the x axis.
		// Linearly decelerate back to stationary state.

		if (m_currentVelocity[0] > 0.0f){
			if ((m_currentVelocity[0] -= m_acceleration[0] * m_dt) < 0.0f)
				m_currentVelocity[0] = 0.0f;

		}else{

			if ((m_currentVelocity[0] += m_acceleration[0] * m_dt) > 0.0f)
				m_currentVelocity[0] = 0.0f;
		}
	}

	if (direction[1] != 0.0f){
		// Camera is moving along the y axis.
		// Linearly accelerate up to the camera's max speed.
		m_currentVelocity[1] += direction[1] * m_acceleration[1] * m_dt;

		if (m_currentVelocity[1] > m_velocity[1])
			m_currentVelocity[1] = m_velocity[1];
		else if (m_currentVelocity[1] < -m_velocity[1])
			m_currentVelocity[1] = -m_velocity[1];
	}else{
		// Camera is no longer moving along the y axis.
		// Linearly decelerate back to stationary state.

		if (m_currentVelocity[1] > 0.0f){
			if ((m_currentVelocity[1] -= m_acceleration[1] * m_dt) < 0.0f)
				m_currentVelocity[1] = 0.0f;

		}else{
			if ((m_currentVelocity[1] += m_acceleration[1] * m_dt) > 0.0f)
				m_currentVelocity[1] = 0.0f;
		}
	}

	if (direction[2] != 0.0f){
		// Camera is moving along the z axis.
		// Linearly accelerate up to the camera's max speed.

		m_currentVelocity[2] += direction[2] * m_acceleration[2] * m_dt;

		if (m_currentVelocity[2] > m_velocity[2])
			m_currentVelocity[2] = m_velocity[2];
		else if (m_currentVelocity[2] < -m_velocity[2])
			m_currentVelocity[2] = -m_velocity[2];
	}else{
		// Camera is no longer moving along the z axis.
		// Linearly decelerate back to stationary state.

		if (m_currentVelocity[2] > 0.0f){
			if ((m_currentVelocity[2] -= m_acceleration[2] * m_dt) < 0.0f)
				m_currentVelocity[2] = 0.0f;

		}else{
			if ((m_currentVelocity[2] += m_acceleration[2] * m_dt) > 0.0f)
				m_currentVelocity[2] = 0.0f;
		}
	}
}

void Camera::rotate(float yaw, float pitch, float roll){
	rotateFirstPerson(yaw, pitch);
	updateViewMatrix(true);
}

void Camera::rotateSmoothly(float yaw, float pitch, float roll) {
	rotateFirstPerson(yaw * m_rotationSpeed, pitch * m_rotationSpeed);
	updateViewMatrix(true);
}

void Camera::rotateFirstPerson(float yaw, float pitch){

	m_accumPitchDegrees += pitch;

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
	return m_projMatrix[2][3] / (m_projMatrix[2][2] + 1);
}

const float Camera::getNear() const {
	return m_projMatrix[2][3] / (m_projMatrix[2][2] - 1);
}

const float Camera::getFovXDeg() const {
	return 2 * atanf(1.0f / m_projMatrix[1][1]) * _180_ON_PI;
}

const float  Camera::getFovXRad() const {
	return 2 * atanf(1.0f / m_projMatrix[1][1]);
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

	float heightNear = 2 * tanf(0.5 * getFovXRad()) * near;
	float widthNear = heightNear * m_aspectRatio;	
	float heightFar = 2 * tanf(0.5 * getFovXRad()) * far;
	float widthFar = heightFar  * m_aspectRatio;


	//worldSpace
	Vector3f centerNear = m_eye + m_viewDir * near;
	Vector3f centerFar = m_eye + m_viewDir * far;
	Vector3f center = m_eye + m_viewDir * ((far + near) * 0.5f);
	
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
		lightView * Vector4f(nearTopLeft) ,
		lightView * Vector4f(nearTopRight) ,
		lightView * Vector4f(nearBottomLeft) ,
		lightView * Vector4f(nearBottomRight) ,

		lightView * Vector4f(farTopLeft) ,
		lightView * Vector4f(farTopRight) ,
		lightView * Vector4f(farBottomLeft) ,
		lightView * Vector4f(farBottomRight)
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

	lightProjection.orthographic(minX, maxX, minY, maxY, minZ, maxZ);
}

void Camera::calcLightTransformation(Vector3f &direction, float near, float far, Matrix4f& viewMatrix, Matrix4f& projectionMatrix) {
	float heightNear = 2 * tanf(0.5 * getFovXRad()) * near;
	float widthNear = heightNear * m_aspectRatio;
	float heightFar = 2 * tanf(0.5 * getFovXRad()) * far;
	float widthFar = heightFar  * m_aspectRatio;

	//worldSpace
	Vector3f centerNear = m_eye + m_viewDir * near;
	Vector3f centerFar = m_eye + m_viewDir * far;
	Vector3f center = m_eye + m_viewDir * ((far + near) * 0.5f);
	
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
		viewMatrix * Vector4f(nearTopLeft) ,
		viewMatrix * Vector4f(nearTopRight) ,
		viewMatrix * Vector4f(nearBottomLeft) ,
		viewMatrix * Vector4f(nearBottomRight) ,

		viewMatrix * Vector4f(farTopLeft) ,
		viewMatrix * Vector4f(farTopRight) ,
		viewMatrix * Vector4f(farBottomLeft) ,
		viewMatrix * Vector4f(farBottomRight)
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
		calcLightTransformation(direction, m_bounds[i][0], m_bounds[i][1], lightViews[i], lightProjections[i]);
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
	lightProjections.resize(m_numberCascades);
	
	float _near = getNear();
	float _far = distance >= far ? far : distance;

	
	m_cascadeEndClipSpace = new float[m_numberCascades];

	for (unsigned short numberCascades = 0; numberCascades < m_numberCascades; numberCascades++) {		
		m_bounds.push_back(Vector2f(_near, _far));

		Vector4f vClip = m_projMatrix ^ Vector4f(0.0f, 0.0f, -_far, 1.0f);

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
	m_numberCascades = bounds.size();
	lightViews.resize(m_numberCascades);
	lightProjections.resize(m_numberCascades);
	m_cascadeEndClipSpace = new float[m_numberCascades];

	for (unsigned short numberCascades = 0; numberCascades < m_numberCascades; numberCascades++) {
		Vector4f vClip = m_projMatrix ^ Vector4f(0.0f, 0.0f, -bounds[numberCascades][1], 1.0f);

		//clipSpace
		m_cascadeEndClipSpace[numberCascades] = vClip[2];
	}
}

void Camera::setPosition(float x, float y, float z){
	m_eye.set(x, y, z);
	updateViewMatrix(false);
}

void Camera::setPosition(const Vector3f &position){
	m_eye = position;
	updateViewMatrix(false);
}

void Camera::setAcceleration(const Vector3f &acceleration) {
	m_acceleration = acceleration;
}

void Camera::setVelocity(const Vector3f &velocity){
	m_velocity = velocity;
}

void Camera::setRotationSpeed(float rotationSpeed){
	m_rotationSpeed = rotationSpeed;
}

const Vector3f &Camera::getPosition() const{
	return m_eye;
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

const Vector3f &Camera::getViewDirection() const{
	return m_viewDir;
}

const Matrix4f &Camera::getProjectionMatrix() const{
	return m_projMatrix;
}

const Matrix4f &Camera::getInvProjectionMatrix() const{
	return  m_invProjMatrix;
}

const Matrix4f &Camera::getOrthographicMatrix() const{
	return m_orthMatrix;
}

const Matrix4f &Camera::getViewMatrix() const{
	return m_viewMatrix;
}

const Matrix4f &Camera::getViewMatrixTranspose() const {
	return m_viewMatrixTranspose;
}

const Matrix4f &Camera::getInvViewMatrix() const{
	return m_invViewMatrix;
}

const Matrix4f &Camera::getProjectionMatrixTranspose() const {
	return m_projMatrixTranspose;
}