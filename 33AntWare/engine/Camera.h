#ifndef __cameraH__
#define __cameraH__

#include <vector>
#include "Vector.h"

class Camera {

public:

	Camera();
	Camera(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	~Camera();

	void perspective(float fovx, float aspect, float znear, float zfar);
	void orthographic(float left, float right, float bottom, float top, float znear, float zfar);
	void lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	void lookAt(const Vector3f &pos, float pitch, float yaw);
	void lookAt(const Vector3f &pos, float pitch, float yaw, float roll);
	void follow(const Matrix4f& targetMat, const Vector3f& targetVelocity, const float dt);

	void move(float dx, float dy, float dz);
	void move(const Vector3f &direction);
	void moveRelative(const Vector3f &direction);
	void moveX(float dx);
	void moveY(float dy);
	void moveZ(float dx);

	virtual void rotate(float yaw, float pitch);
	void rotate(float yaw, float pitch, const Vector3f &target);

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
	const float getOffsetDistance() const;

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
	const Matrix4f& getOrthographicMatrix() const;
	const Matrix4f& getInvOrthographicMatrix() const;
	const Matrix4f  getInvOrthographicMatrixNew() const;

	const Matrix4f  getRotationMatrix(const Vector3f &position = Vector3f(0.0f, 0.0f, 0.0f)) const;

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
	const float getDistance() const;
	const float getDistanceSq() const;

	void setPosition(float x, float y, float z);
	void setPosition(const Vector3f &position);
	void setPositionX(float x);
	void setPositionY(float y);
	void setPositionZ(float z);
	void setRotation(float pitch, float yaw, float roll = 0.0f);

	virtual void setTarget(const Vector3f& target);
	void setMovingSpeed(float movingSpeed);
	void setOffsetDistance(float offsetDistance);
	void setRotationSpeed(float rotationSpeed);

	Matrix4f lightView;
	Matrix4f lightPerspective;

	std::vector<Matrix4f> lightViews;
	std::vector<Matrix4f> lightPerspectives;
	std::vector<Vector2f> m_bounds;
	short m_numberCascades = 0;
	float* m_cascadeEndClipSpace;

	float currentSettleTime = 0.0f;

protected:

    void rotateFirstPerson(float yaw, float pitch);
	void updateViewMatrix();
	void orthogonalize();

    Vector3f WORLD_XAXIS;
	Vector3f WORLD_YAXIS;
	Vector3f WORLD_ZAXIS;

	float			m_accumPitchDegrees;
	float			m_rotationSpeed;
	float			m_movingSpeed;
	float			m_offsetDistance;

	Vector3f		m_eye;
	Vector3f		m_xAxis;
    Vector3f		m_yAxis;
    Vector3f		m_zAxis;
	Vector3f		m_viewDir;
	Vector3f		m_target;

	Matrix4f		m_viewMatrix;
	Matrix4f		m_invViewMatrix;

	Matrix4f		m_persMatrix;
	Matrix4f		m_invPersMatrix;
	Matrix4f		m_orthMatrix;
	Matrix4f		m_invOrthMatrix;
};

class ThirdPersonCamera : public Camera {


public:

	ThirdPersonCamera();
	~ThirdPersonCamera();

	using Camera::rotate;
	void rotate(float yaw, float pitch) override;
	void update(float elapsedTimeSec);

	bool springSystemIsEnabled() const;
	void enableSpringSystem(bool enableSpringSystem);
	float getDampingConstant() const;
	float getSpringConstant() const;

	const Vector3f& getTarget() const;
	const Vector3f& getVelocity() const;

	void setSpringConstant(float springConstant);
	void setTarget(const Vector3f& target) override;

private:

	void updateViewMatrix(float elapsedTimeSec);
	void updateViewMatrixDump(float elapsedTimeSec);

	bool m_enableSpringSystem;
	float m_springConstant;
	float m_dampingConstant;
	Vector3f m_velocity;

	static const float DEFAULT_SPRING_CONSTANT;
	static const float DEFAULT_DAMPING_CONSTANT;
};

#endif // __cameraH__