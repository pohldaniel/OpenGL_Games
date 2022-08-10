#ifndef _MODELMATRIX_H
#define _MODELMATRIX_H

#include "Vector.h"

class Transform{

public:

	Transform();
	Transform(const Matrix4f &m);
	~Transform();

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);
	void setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz);
	void setRotPosScale(const Vector3f &axis, float degrees, float dx, float dy, float dz, float a, float b, float c);

	void setRotXYZPos(const Vector3f &axisX, float degreesX, 
					  const Vector3f &axisY, float degreesY,
					  const Vector3f &axisZ, float degreesZ,
					  float dx, float dy, float dz);

	void fromMatrix(const Matrix4f &m);
	void fromMatrix(const Matrix4f &m, const Vector3f& startPosition);

	const Vector3f& getPosition() const;	
	void getScale(Vector3f& scale);
	void getPosition(Vector3f& position);	
	Matrix4f& getOrientation();
	void getOrientation(Matrix4f& orientation);

	const Quaternion& getOrientation() const;

private:
	Vector3f position;
	Vector3f startPosition;

	Quaternion orientation;

	bool pos;

	Matrix4f T;
	Matrix4f invT;
};


#endif