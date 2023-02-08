#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "Vector.h"

class Transform {

public:

	Transform();
	Transform(const Matrix4f& m);
	~Transform();

	const Matrix4f& getTransformationMatrix() const;
	const Matrix4f& getInvTransformationMatrix();

	void setRotPos(const Vector3f& axis, float degrees, float dx, float dy, float dz);
	void setRotPosScale(const Vector3f& axis, float degrees, float dx, float dy, float dz, float x = 1.0f, float y = 1.0f, float z = 1.0f);


	void setPosition(const Vector3f& position);

	void translate(float dx, float dy, float dz);

	void rotate(const Vector3f& axis, float degrees, const Vector3f& centerOfRotation);
	void rotate(Quaternion& quat, const Vector3f& centerOfRotation);
	void rotate(const Vector3f& axis, float degrees);
	void rotate(Quaternion& quat);

	void scale(float s);
	void scale(float a, float b, float c);
	void scale(float s, const Vector3f& centerOfScale);
	void scale(float a, float b, float c, const Vector3f& centerOfScale);

	void reset();

	void fromMatrix(const Matrix4f& m);
	void getPosition(Vector3f& position);
	void getOrientation(Matrix4f& orientation);
	void getOrientation(Quaternion& orientation);
	void getScale(Vector3f& scale);

private:

	Matrix4f T;
	Matrix4f invT;
};
#endif