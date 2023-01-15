#ifndef __cameraH__
#define __cameraH__

#include <vector>
#include "Vector.h"

class Camera{

public:

	Camera();
	Camera(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	~Camera();

	void perspective(float fovx, float aspect, float znear, float zfar);
	void orthographic(float left, float right, float bottom, float top, float znear, float zfar);
	void lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	void move(float dx, float dy, float dz);
	void move(Vector3f &direction);
	void rotate(float yaw, float pitch, float roll);
	void rotateSmoothly(float yaw, float pitch, float roll);
	void pitchReflection(const float distance);
	void calcLightTransformation(Vector3f &direction);
	void calcLightTransformation2(Vector3f &direction);

	void setUpLightTransformation(float distance);
	void setUpLightTransformation(std::vector<Vector2f>& bounds);
	
	void calcLightTransformation(Vector3f &direction, float near, float far, Matrix4f& viewMatrix, Matrix4f& projectionMatrix);

	const float getFar() const;
	const float getNear() const;
	const float getFovXDeg() const;
	const float getFovXRad() const;

	const Matrix4f &getViewMatrix() const;
	const Matrix4f &getInvViewMatrix() const;
	const Matrix4f &getProjectionMatrix() const;
	const Matrix4f &getInvProjectionMatrix() const;
	const Matrix4f &getOrthographicMatrix() const;
	const Vector3f &getPosition() const;
	const Vector3f &getCamX() const;
	const Vector3f &getCamY() const;
	const Vector3f &getCamZ() const;
	const Vector3f &getViewDirection() const;

	void setPosition(float x, float y, float z);
	void setPosition(const Vector3f &position);

	void setAcceleration(const Vector3f &acceleration);
	void setVelocity(const Vector3f &velocity);
	void updatePosition(const Vector3f &direction, float m_dt);
	void setRotationSpeed(float rotationSpeed);

	Matrix4f lightView;
	Matrix4f lightProjection;

	std::vector<Matrix4f> lightViews;
	std::vector<Matrix4f> lightProjections;
	std::vector<Vector2f> m_bounds;
	short m_numberCascades = 0;
	float* m_cascadeEndClipSpace;
protected:

    void rotateFirstPerson(float yaw, float pitch);
	void updateViewMatrix(bool orthogonalizeAxes);
	void updateViewMatrix(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	void updateVelocity(const Vector3f &direction, float elapsedTimeSec);

    Vector3f WORLD_XAXIS;
	Vector3f WORLD_YAXIS;
	Vector3f WORLD_ZAXIS;

    float			m_fovx;
    float			m_znear;
    float			m_zfar;
	float			m_aspectRatio;
	float			m_accumPitchDegrees;
	float			m_rotationSpeed;

	Vector3f		m_eye;
	Vector3f		m_xAxis;
    Vector3f		m_yAxis;
    Vector3f		m_zAxis;
	Vector3f		m_viewDir;
	Vector3f		m_currentVelocity;
	Vector3f		m_acceleration;
	Vector3f		m_velocity;

	Matrix4f		m_viewMatrix;
	Matrix4f		m_invViewMatrix;

	Matrix4f		m_projMatrix;
	Matrix4f		m_invProjMatrix;
	Matrix4f		m_orthMatrix;
};
#endif // __cameraH__