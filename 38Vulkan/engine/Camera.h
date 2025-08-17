#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define _180_ON_PI  57.295779513082320877f
#define PI_ON_180  0.0174532925199432957f

class Camera {

public:

	Camera();
	Camera(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up);
	~Camera();

    void perspective(float fovx, float aspect, float znear, float zfar);
	void orthographic(float left, float right, float bottom, float top, float znear, float zfar);
	void lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up);
	void move(const glm::vec3& direction);


	virtual void rotate(float yaw, float pitch);

    void setPosition(float x, float y, float z);
    void setPosition(const glm::vec3& position);
    void setRotationSpeed(float rotationSpeed);
	void setMovingSpeed(float movingSpeed);

    const glm::mat4& getViewMatrix() const;
	const glm::mat4& getInvViewMatrix() const;
	const glm::mat4& getPerspectiveMatrix() const;
	const glm::mat4& getInvPerspectiveMatrix() const;
	const glm::mat4& getOrthographicMatrix() const;
	const glm::mat4& getInvOrthographicMatrix() const;

	static const glm::mat4 IDENTITY;

protected:

   void updateViewMatrix();

private:

    void rotateFirstPerson(float yaw, float pitch);
    void orthogonalize();

    glm::vec3 WORLD_XAXIS;
	glm::vec3 WORLD_YAXIS;
	glm::vec3 WORLD_ZAXIS;

	float			m_accumPitchDegrees;
	float			m_accumYawDegrees;
	float			m_rotationSpeed;
	float			m_movingSpeed;
	float			m_offsetDistance;

	glm::vec3		m_eye;
	glm::vec3		m_xAxis;
    glm::vec3		m_yAxis;
    glm::vec3		m_zAxis;
	glm::vec3		m_viewDir;
	glm::vec3		m_target;

	glm::mat4		m_viewMatrix;
	glm::mat4		m_invViewMatrix;
	glm::mat4		m_persMatrix;
	glm::mat4		m_invPersMatrix;
	glm::mat4		m_orthMatrix;
	glm::mat4		m_invOrthMatrix;
};