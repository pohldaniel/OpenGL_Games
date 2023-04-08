#include "CameraVo.h"

CameraVo::CameraVo() :
	m_cameraPos(glm::vec3(0.0f, 0.0f, 1.0f)),
	m_cameraForward(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_cameraUpVector(glm::vec3(0.0f, 1.0f, 0.0f)),
	m_cameraSpeed(20.0f),
	m_fieldOfView(80.0f),
	m_cameraSensitivity(7.0f)
{
	m_cameraRight = glm::normalize(glm::cross(m_cameraUpVector, -m_cameraForward));
}

CameraVo::~CameraVo()
{
}

void CameraVo::InitCameraPerspective(float fov, float aspectRatio, float near, float far)
{
	m_fieldOfView = fov;
	m_projection = glm::perspective(glm::radians(m_fieldOfView), aspectRatio, near, far);
}

void CameraVo::InitCameraOrthographic(float left, float right, float bottom, float up, float near, float far)
{
	m_projection = glm::ortho(left, right, bottom, up, near, far);
}

void CameraVo::MoveForward(float dt)
{
	m_cameraPos += (m_cameraSpeed * dt) * m_cameraForward;
}

void CameraVo::MoveBackward(float dt)
{
	m_cameraPos -= (m_cameraSpeed * dt) * m_cameraForward;
}

void CameraVo::StrafeLeft(float dt)
{
	glm::vec3 StrafteDirection = glm::cross(m_cameraForward, m_cameraUpVector);
	m_cameraPos -= (m_cameraSpeed * dt) * StrafteDirection;
}

void CameraVo::StrafeRight(float dt)
{
	glm::vec3 StrafteDirection = glm::cross(m_cameraForward, m_cameraUpVector);
	m_cameraPos += (m_cameraSpeed * dt) * StrafteDirection;
}

void CameraVo::Rise(float dt)
{
	m_cameraPos += (m_cameraSpeed * dt) * m_cameraUpVector;
}

void CameraVo::Fall(float dt)
{
	m_cameraPos -= (m_cameraSpeed * dt) * m_cameraUpVector;
}

void CameraVo::MouseUpdate(const glm::vec2& newMousePos, float dt)
{
	glm::vec2 MouseDelta = newMousePos - m_oldMousePos;

	if (glm::length(MouseDelta) > 50.0f)
	{
		m_oldMousePos = newMousePos;
		return;
	}

	glm::vec3 RotateAround = glm::cross(m_cameraForward, m_cameraUpVector);

	m_cameraForward = glm::mat3(glm::rotate(glm::radians(-MouseDelta.x) * m_cameraSensitivity * dt, m_cameraUpVector)) * m_cameraForward;
	m_cameraForward = glm::mat3(glm::rotate(glm::radians(-MouseDelta.y) * m_cameraSensitivity * dt, RotateAround)) * m_cameraForward;

	m_oldMousePos = newMousePos;
}

void CameraVo::UpdateLookAt()
{
	m_view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraForward, m_cameraUpVector);
}