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
	void moveRelative(Vector3f &direction);

	void moveX(float dx);
	void moveY(float dy);
	void moveZ(float dx);

	void rotate(float yaw, float pitch, float roll);
	void rotate(float yaw, float pitch, float roll, const Vector3f &centerOfRotation, float distance);
	void rotateSmoothly(float yaw, float pitch, float roll);
	void rotateSmoothly(float yaw, float pitch, float roll, const Vector3f &centerOfRotation, float distance);

	void pitchReflection(const float distance);
	void calcLightTransformation(Vector3f &direction);
	void calcLightTransformation2(Vector3f &direction);

	void setUpLightTransformation(float distance);
	void setUpLightTransformation(std::vector<Vector2f>& bounds);
	
	void calcLightTransformation(Vector3f &direction, float near, float far, Matrix4f& viewMatrix, Matrix4f& perspectiveMatrix);

	const float getFar() const;
	const float getNear() const;
	const float getFovXDeg() const;
	const float getFovXRad() const;
	const float getAspect() const;
	const float getPitchAngle() const;
	const float getYawAngle() const;
	const float getRollAngle() const;

	const float getLeftOrthographic() const;
	const float getRightOrthographic() const;
	const float getBottomOrthographic() const;
	const float getTopOrthographic() const;
	const float getNearOrthographic() const;
	const float getFarOrthographic() const;

	const Matrix4f& getViewMatrix() const;
	const Matrix4f& getInvViewMatrix() const;
	const Matrix4f  getInvViewMatrixNew() const;
	const Matrix4f& getPerspectiveMatrix() const;
	const Matrix4f& getInvPerspectiveMatrix() const;
	const Matrix4f  getInvPerspectiveMatrixNew() const;
	const Matrix4f  getRotationMatrix(const Vector3f &position = Vector3f(0.0f, 0.0f, 0.0f)) const;

	const Matrix4f& getOrthographicMatrix() const;
	const Matrix4f  getInvOrthographicMatrixNew() const;
	const Vector3f& getPosition() const;
	const float getPositionX() const;
	const float getPositionY() const;
	const float getPositionZ() const;
	const Vector3f& getCamX() const;
	const Vector3f& getCamY() const;
	const Vector3f& getCamZ() const;
	const Vector3f& getViewDirection() const;
	const float getViewDirectionX() const;
	const float getViewDirectionY() const;
	const float getViewDirectionZ() const;
	const Vector3f getViewSpaceDirection(const Vector3f &direction);

	void setPosition(float x, float y, float z);
	void setPosition(const Vector3f &position);
	void setPositionX(float x);
	void setPositionY(float y);
	void setPositionZ(float z);
	void setMovingSpeed(float movingSpeed);

	void setAcceleration(const Vector3f &acceleration);
	void setVelocity(const Vector3f &velocity);
	void updatePosition(const Vector3f &direction, float m_dt);
	void setRotationSpeed(float rotationSpeed);

	Matrix4f lightView;
	Matrix4f lightPerspective;

	std::vector<Matrix4f> lightViews;
	std::vector<Matrix4f> lightPerspectives;
	std::vector<Vector2f> m_bounds;
	short m_numberCascades = 0;
	float* m_cascadeEndClipSpace;

protected:

    void rotateFirstPerson(float yaw, float pitch);
	void rotateFirstPerson(float yaw, float pitch, const Vector3f &centerOfRotation, float distance);
	//void rotateFirstPersonYP(float yaw, float pitch, const Vector3f &centerOfRotation, float distance);
	void updateViewMatrix(bool orthogonalizeAxes);
	void updateViewMatrix(const Vector3f &eye, const Vector3f &target, const Vector3f &up);

    Vector3f WORLD_XAXIS;
	Vector3f WORLD_YAXIS;
	Vector3f WORLD_ZAXIS;

    float			m_fovx;
    float			m_znear;
    float			m_zfar;
	float			m_aspectRatio;
	float			m_accumPitchDegrees;
	float			m_rotationSpeed;
	float			m_movingSpeed;

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

	Matrix4f		m_persMatrix;
	Matrix4f		m_invPersMatrix;
	Matrix4f		m_orthMatrix;
};
#endif // __cameraH__