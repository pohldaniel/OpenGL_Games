#include "Camera.h"

const glm::mat4 Camera::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

Camera::Camera(){
	
	WORLD_XAXIS = glm::vec3(1.0f, 0.0f, 0.0f);
	WORLD_YAXIS = glm::vec3(0.0f, 1.0f, 0.0f);
	WORLD_ZAXIS = glm::vec3(0.0f, 0.0f, 1.0f);

	m_accumPitchDegrees = 0.0f;
	m_accumYawDegrees = 0.0f;
	m_rotationSpeed = 1.0f;
	m_movingSpeed = 1.0f;
	m_offsetDistance = 0.0f;

    m_xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    m_yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    m_zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
    m_viewDir = glm::vec3(0.0f, 0.0f, -1.0f);

	m_eye = glm::vec3(0.0f, 0.0f, 0.0f) ;
    m_persMatrix = glm::mat4(1.0f);
	m_invPersMatrix = glm::mat4(1.0f);
	m_orthMatrix = glm::mat4(1.0f);
	m_invOrthMatrix = glm::mat4(1.0f);

	orthogonalize();
	updateViewMatrix();
}

Camera::Camera(const glm::vec3 &eye, const glm::vec3& target, const glm::vec3& up) {

	WORLD_XAXIS = glm::vec3(1.0f, 0.0f, 0.0f);
	WORLD_YAXIS = glm::vec3(0.0f, 1.0f, 0.0f);
	WORLD_ZAXIS = glm::vec3(0.0f, 0.0f, 1.0f);

	m_accumPitchDegrees = 0.0f;
	m_accumYawDegrees = 0.0f;
	m_rotationSpeed = 1.0f;
	m_movingSpeed = 1.0f;
	m_offsetDistance = 0.0f;

	m_persMatrix = glm::mat4(1.0f);
	m_invPersMatrix = glm::mat4(1.0f);
	m_orthMatrix = glm::mat4(1.0f);
	m_invOrthMatrix = glm::mat4(1.0f);

	lookAt(eye, target, up);
}

Camera::~Camera() {}

void Camera::perspective(float fovx, float aspect, float znear, float zfar){
    m_persMatrix = glm::perspective(fovx, aspect, znear, zfar);

    float e = tanf(PI_ON_180 * fovx * 0.5f);
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
	m_invPersMatrix[2][2] = 0.0;
	m_invPersMatrix[2][3] = (znear - zfar) / (2 * zfar * znear);

	m_invPersMatrix[3][0] = 0.0f;
	m_invPersMatrix[3][1] = 0.0f;
	m_invPersMatrix[3][2] = -1.0f;
	m_invPersMatrix[3][3] = (znear + zfar) / (2 * zfar * znear);
}

void Camera::orthographic(float left, float right, float bottom, float top, float znear, float zfar){
    m_orthMatrix = glm::ortho(left, right, bottom, top, znear, zfar);
	
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

void Camera::lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up){
	m_eye = eye;
	m_target = target;
	m_offsetDistance = glm::length((m_target - m_eye));

	m_zAxis = glm::normalize(m_eye - target);
	m_xAxis = glm::normalize(glm::cross(up, m_zAxis));
	m_yAxis = glm::normalize(glm::cross(m_zAxis, m_xAxis));

	WORLD_YAXIS =  glm::normalize(glm::cross(m_zAxis, m_xAxis));

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

	m_viewMatrix[3][0] = -glm::dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -glm::dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -glm::dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	// Extract the pitch angle from the view matrix.
	m_accumPitchDegrees = -asinf(m_viewMatrix[2][1]) * _180_ON_PI;

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

void Camera::updateViewMatrix() {

	m_viewMatrix[3][0] = -glm::dot(m_xAxis, m_eye);
	m_viewMatrix[3][1] = -glm::dot(m_yAxis, m_eye);
	m_viewMatrix[3][2] = -glm::dot(m_zAxis, m_eye);
	m_viewMatrix[3][3] = 1.0f;

	m_invViewMatrix[3][0] = m_eye[0];
	m_invViewMatrix[3][1] = m_eye[1];
	m_invViewMatrix[3][2] = m_eye[2];
	m_invViewMatrix[3][3] = 1.0f;
}

void Camera::rotate(float yaw, float pitch) {
	rotateFirstPerson(yaw * m_rotationSpeed, pitch * m_rotationSpeed);
	orthogonalize();
	updateViewMatrix();
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
	
	glm::mat4 rotMtx;

	// Rotate camera's existing x and z axes about the world y axis.
	if (yaw != 0.0f){
		rotMtx = glm::rotate(IDENTITY, yaw * PI_ON_180, WORLD_YAXIS);
		m_xAxis = glm::vec4(m_xAxis, 0.0f) * rotMtx;
		m_zAxis = glm::vec4(m_zAxis, 0.0f) * rotMtx;
	}

	// Rotate camera's existing y and z axes about its existing x axis.
	if (pitch != 0.0f) {
		rotMtx = glm::rotate(IDENTITY, pitch * PI_ON_180, m_xAxis);
		m_yAxis = glm::vec4(m_yAxis, 0.0f) * rotMtx;
		m_zAxis = glm::vec4(m_zAxis, 0.0f) * rotMtx;
	}
}

void Camera::orthogonalize() {

    m_zAxis = glm::normalize(m_zAxis);
    m_yAxis = glm::normalize(glm::cross(m_zAxis, m_xAxis));
	m_xAxis = glm::normalize(glm::cross(m_yAxis, m_zAxis));

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

void Camera::move(const glm::vec3& direction) {
	m_eye += m_xAxis * direction[0] * m_movingSpeed;
	m_eye += WORLD_YAXIS * direction[1] * m_movingSpeed;
	m_eye += m_viewDir * direction[2] * m_movingSpeed;
	updateViewMatrix();
}

void Camera::setPosition(float x, float y, float z){
    m_eye = glm::vec3(x, y, z);
    updateViewMatrix();
}

void Camera::setPosition(const glm::vec3& position){
    m_eye = position;
    updateViewMatrix();
}

void Camera::setRotationSpeed(float rotationSpeed){
	m_rotationSpeed = rotationSpeed;
}

void Camera::setMovingSpeed(float movingSpeed){
	m_movingSpeed = movingSpeed;
}

const glm::mat4& Camera::getPerspectiveMatrix() const{
	return m_persMatrix;
}

const glm::mat4& Camera::getInvPerspectiveMatrix() const{
	return  m_invPersMatrix;
}

const glm::mat4& Camera::getOrthographicMatrix() const{
	return m_orthMatrix;
}

const glm::mat4& Camera::getInvOrthographicMatrix() const {
	return m_invOrthMatrix;
}

const glm::mat4& Camera::getViewMatrix() const{
	return m_viewMatrix;
}

const glm::mat4& Camera::getInvViewMatrix() const{
	return m_invViewMatrix;
}