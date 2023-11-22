#include <iostream>
#include "Transform.h"

Transform::Transform() {
	pos = false;
	T.identity();
	invT.identity();

	startPosition = Vector3f(0.0, 0.0, 0.0);
}

void Transform::reset() {
	pos = false;
	T.identity();
	invT.identity();

	startPosition = Vector3f(0.0, 0.0, 0.0);
}

Transform::Transform(const Matrix4f &m) {
	fromMatrix(m);
}

Transform::~Transform() {

}

void Transform::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz) {
	T.identity();
	invT.identity();

	if (degrees != 0.0f && !(axis[0] == 0.0f && axis[1] == 0.0f && axis[2] == 0.0f)) {
		Matrix4f rotMtx;
		rotMtx.invRotate(axis, degrees);
		T = rotMtx * T;

		//Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
		//	rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
		//	rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
		//	rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

		//invT = invT * invRotMtx;
	}

	//T = Translate * T
	T[0][0] = T[0][0] + T[0][3] * dx; T[0][1] = T[0][1] + T[0][3] * dz; T[0][2] = T[0][2] + T[0][3] * dy;
	T[1][0] = T[1][0] + T[1][3] * dx; T[1][1] = T[1][1] + T[1][3] * dz; T[1][2] = T[1][2] + T[1][3] * dy;
	T[2][0] = T[2][0] + T[2][3] * dx; T[2][1] = T[2][1] + T[2][3] * dz; T[2][2] = T[2][2] + T[2][3] * dy;
	T[3][0] = T[3][0] + dx; T[3][1] = T[3][1] + dy; T[3][2] = T[3][2] + dz;

	//T^-1 = T^-1 * Translate^-1 
	//invT[3][0] = invT[3][0] - dx*invT[0][0] - dz*invT[2][0] - dy*invT[1][0];
	//invT[3][1] = invT[3][1] - dx*invT[0][1] - dz*invT[2][1] - dy*invT[1][1];
	//invT[3][2] = invT[3][2] - dx*invT[0][2] - dz*invT[2][2] - dy*invT[1][2];

	startPosition = Vector3f(dx, dy, dz);

	if (!startPosition.zero()) {
		pos = true;
	}
}

void Transform::setRotPosScale(const Vector3f &axis, float degrees, float dx, float dy, float dz, float x, float y, float z) {
	T.identity();
	invT.identity();

	if (degrees != 0.0f && !(axis[0] == 0.0f && axis[1] == 0.0f && axis[2] == 0.0f)) {

		Matrix4f rotMtx;
		rotMtx.invRotate(axis, degrees);

		T = rotMtx * T;

		//Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
		//	rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
		//	rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
		//	rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

		//invT = invT * invRotMtx;
	}
	//T = Translate * T
	T[0][0] = T[0][0] + T[0][3] * dx; T[0][1] = T[0][1] + T[0][3] * dz; T[0][2] = T[0][2] + T[0][3] * dy;
	T[1][0] = T[1][0] + T[1][3] * dx; T[1][1] = T[1][1] + T[1][3] * dz; T[1][2] = T[1][2] + T[1][3] * dy;
	T[2][0] = T[2][0] + T[2][3] * dx; T[2][1] = T[2][1] + T[2][3] * dz; T[2][2] = T[2][2] + T[2][3] * dy;
	T[3][0] = T[3][0] + dx; T[3][1] = T[3][1] + dy; T[3][2] = T[3][2] + dz;

	//T^-1 = T^-1 * Translate^-1 
	//invT[3][0] = invT[3][0] - dx*invT[0][0] - dz*invT[2][0] - dy*invT[1][0];
	//invT[3][1] = invT[3][1] - dx*invT[0][1] - dz*invT[2][1] - dy*invT[1][1];
	//invT[3][2] = invT[3][2] - dx*invT[0][2] - dz*invT[2][2] - dy*invT[1][2];

	startPosition = Vector3f(dx, dy, dz);

	if (!startPosition.zero()) {
		pos = true;
	}

	T[0][0] = T[0][0] * x;  T[1][0] = T[1][0] * y; T[2][0] = T[2][0] * z;
	T[0][1] = T[0][1] * x;  T[1][1] = T[1][1] * y; T[2][1] = T[2][1] * z;
	T[0][2] = T[0][2] * x;  T[1][2] = T[1][2] * y; T[2][2] = T[2][2] * z;

	//invT[0][0] = invT[0][0] * (1.0 / x); invT[0][1] = invT[0][1] * (1.0 / y); invT[0][2] = invT[0][2] * (1.0 / z);
	//invT[1][0] = invT[1][0] * (1.0 / x); invT[1][1] = invT[1][1] * (1.0 / y); invT[1][2] = invT[1][2] * (1.0 / z);
	//invT[2][0] = invT[2][0] * (1.0 / x); invT[2][1] = invT[2][1] * (1.0 / y); invT[2][2] = invT[2][2] * (1.0 / z);
	//invT[3][0] = invT[3][0] * (1.0 / x); invT[3][1] = invT[3][1] * (1.0 / y); invT[3][2] = invT[3][2] * (1.0 / z);
}

void Transform::rotate(const Vector3f &axis, float degrees) {

	Matrix4f rotMtx;
	rotMtx.invRotate(axis, degrees);

	//Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
	//	rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
	//	rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
	//	rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);
	
	if (!pos) {
		T = rotMtx * T;
		//invT = invT * invRotMtx;
	}else {
		//T = (translate * invRotMtx * invTranslate) * T
		float tmp1 = rotMtx[3][0], tmp2 = rotMtx[3][1], tmp3 = rotMtx[3][2], tmp4 = rotMtx[3][3];
		rotMtx[3][0] = startPosition[0] * (tmp4 - rotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * rotMtx[1][0] - startPosition[2] * rotMtx[2][0];
		rotMtx[3][1] = startPosition[1] * (tmp4 - rotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[0][1] - startPosition[2] * rotMtx[2][1];
		rotMtx[3][2] = startPosition[2] * (tmp4 - rotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[0][2] - startPosition[1] * rotMtx[1][2];
		rotMtx[3][3] = startPosition[0] * (tmp1 - rotMtx[0][3]) + startPosition[1] * (tmp2 - rotMtx[1][3]) + startPosition[2] * (tmp3 - rotMtx[2][3]) + tmp4;

		T = rotMtx * T;

		//tmp1 = invRotMtx[3][0], tmp2 = invRotMtx[3][1], tmp3 = invRotMtx[3][2], tmp4 = invRotMtx[3][3];
		//invRotMtx[3][0] = startPosition[0] * (tmp4 - invRotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * invRotMtx[1][0] - startPosition[2] * invRotMtx[2][0];
		//invRotMtx[3][1] = startPosition[1] * (tmp4 - invRotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * invRotMtx[0][1] - startPosition[2] * invRotMtx[2][1];
		//invRotMtx[3][2] = startPosition[2] * (tmp4 - invRotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * invRotMtx[0][2] - startPosition[1] * invRotMtx[1][2];
		//invRotMtx[3][3] = startPosition[0] * (tmp1 - invRotMtx[0][3]) + startPosition[1] * (tmp2 - invRotMtx[1][3]) + startPosition[2] * (tmp3 - invRotMtx[2][3]) + tmp4;
		//invT = invT * invRotMtx;
	}
}


void Transform::translate(float dx, float dy, float dz) {
	T[0][0] = T[0][0] + T[0][3] * dx; T[0][1] = T[0][1] + T[0][3] * dz; T[0][2] = T[0][2] + T[0][3] * dy;
	T[1][0] = T[1][0] + T[1][3] * dx; T[1][1] = T[1][1] + T[1][3] * dz; T[1][2] = T[1][2] + T[1][3] * dy;
	T[2][0] = T[2][0] + T[2][3] * dx; T[2][1] = T[2][1] + T[2][3] * dz; T[2][2] = T[2][2] + T[2][3] * dy;
	T[3][0] = T[3][0] + dx; T[3][1] = T[3][1] + dy; T[3][2] = T[3][2] + dz;

	//invT[3][0] = invT[3][0] - dx*invT[0][0] - dz*invT[2][0] - dy*invT[1][0];
	//invT[3][1] = invT[3][1] - dx*invT[0][1] - dz*invT[2][1] - dy*invT[1][1];
	//invT[3][2] = invT[3][2] - dx*invT[0][2] - dz*invT[2][2] - dy*invT[1][2];
}

void Transform::scale(float a, float b, float c) {

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	T[0][0] = T[0][0] * a;  T[1][0] = T[1][0] * b; T[2][0] = T[2][0] * c;
	T[0][1] = T[0][1] * a;  T[1][1] = T[1][1] * b; T[2][1] = T[2][1] * c;
	T[0][2] = T[0][2] * a;  T[1][2] = T[1][2] * b; T[2][2] = T[2][2] * c;

	//invT[0][0] = invT[0][0] * (1.0 / a); invT[0][1] = invT[0][1] * (1.0 / b); invT[0][2] = invT[0][2] * (1.0 / c);
	//invT[1][0] = invT[1][0] * (1.0 / a); invT[1][1] = invT[1][1] * (1.0 / b); invT[1][2] = invT[1][2] * (1.0 / c);
	//invT[2][0] = invT[2][0] * (1.0 / a); invT[2][1] = invT[2][1] * (1.0 / b); invT[2][2] = invT[2][2] * (1.0 / c);
	//invT[3][0] = invT[3][0] * (1.0 / a); invT[3][1] = invT[3][1] * (1.0 / b); invT[3][2] = invT[3][2] * (1.0 / c);
}

const Matrix4f& Transform::getTransformationMatrix() const {
	return T;
}

//this just works with uniform scaling
const Matrix4f& Transform::getInvTransformationMatrix() {
	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	float sxx = sx * sx;
	float sxy = sx * sy;
	float sxz = sx * sz;

	float syy = sy * sy;
	float syz = sy * sz;

	float szz = sz * sz;

	invT[0][0] = T[0][0] * (1.0f / sxx); invT[1][0] = T[0][1] * (1.0f / sxy); invT[2][0] = T[0][2] * (1.0f / sxz); invT[3][0] = -(T[3][0] * T[0][0] * (1.0f / sxx) + T[3][1] * T[0][1] * (1.0f / sxy) + T[3][2] * T[0][2] * (1.0f / sxz));
	invT[0][1] = T[1][0] * (1.0f / sxy); invT[1][1] = T[1][1] * (1.0f / syy); invT[2][1] = T[1][2] * (1.0f / syz); invT[3][1] = -(T[3][0] * T[1][0] * (1.0f / sxy) + T[3][1] * T[1][1] * (1.0f / syy) + T[3][2] * T[1][2] * (1.0f / syz));
	invT[0][2] = T[2][0] * (1.0f / sxz); invT[1][2] = T[2][1] * (1.0f / syz); invT[2][2] = T[2][2] * (1.0f / szz); invT[3][2] = -(T[3][0] * T[2][0] * (1.0f / sxz) + T[3][1] * T[2][1] * (1.0f / syz) + T[3][2] * T[2][2] * (1.0f / szz));
	invT[0][3] = 0.0f;					 invT[1][3] = 0.0f;					  invT[2][3] = 0.0f;				   invT[3][3] = 1.0f;

	return invT;
}

void Transform::getScale(Vector3f& scale) {
	scale = Vector3f(Vector3f(T[0][0], T[0][1], T[0][2]).length(), Vector3f(T[1][0], T[1][1], T[1][2]).length(), Vector3f(T[2][0], T[2][1], T[2][2]).length());
}

void Transform::getPosition(Vector3f& position) {
	position = Vector3f(T[3][0], T[3][1], T[3][2]);
}

void Transform::getOrientation(Matrix4f& orientation) {
	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	orientation[0][0] = T[0][0] * (1.0f / sx); orientation[0][1] = T[0][1] * (1.0f / sx); orientation[0][2] = T[0][2] * (1.0f / sx); orientation[0][3] = 0.0f;
	orientation[1][0] = T[1][0] * (1.0f / sy); orientation[1][1] = T[1][1] * (1.0f / sy); orientation[1][2] = T[1][2] * (1.0f / sy); orientation[1][3] = 0.0f;
	orientation[2][0] = T[2][0] * (1.0f / sz); orientation[2][1] = T[2][1] * (1.0f / sz); orientation[2][2] = T[2][2] * (1.0f / sz); orientation[2][3] = 0.0f;
	orientation[3][0] = 0.0f;				   orientation[3][1] = 0.0f;				  orientation[3][2] = 0.0f;				     orientation[3][3] = 1.0f;
}

void Transform::getOrientation(Quaternion& orientation) {
	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	Matrix4f rot;
	rot[0][0] = T[0][0] * (1.0f / sx); rot[0][1] = T[0][1] * (1.0f / sx); rot[0][2] = T[0][2] * (1.0f / sx); rot[0][3] = 0.0f;
	rot[1][0] = T[1][0] * (1.0f / sy); rot[1][1] = T[1][1] * (1.0f / sy); rot[1][2] = T[1][2] * (1.0f / sy); rot[1][3] = 0.0f;
	rot[2][0] = T[2][0] * (1.0f / sz); rot[2][1] = T[2][1] * (1.0f / sz); rot[2][2] = T[2][2] * (1.0f / sz); rot[2][3] = 0.0f;
	rot[3][0] = 0.0f;				  rot[3][1] = 0.0f; rot[3][2] = 0.0f; rot[3][3] = 1.0f;

	orientation.fromMatrix(rot);
}

void Transform::fromMatrix(const Matrix4f &m) {
	T[0][0] = m[0][0]; T[0][1] = m[0][1]; T[0][2] = m[0][2]; T[0][3] = m[0][3];
	T[1][0] = m[1][0]; T[1][1] = m[1][1]; T[1][2] = m[1][2]; T[1][3] = m[1][3];
	T[2][0] = m[2][0]; T[2][1] = m[2][1]; T[2][2] = m[2][2]; T[2][3] = m[2][3];
	T[3][0] = m[3][0]; T[3][1] = m[3][1]; T[3][2] = m[3][2]; T[3][3] = m[3][3];

	pos = false;
}

void Transform::fromMatrix(const Matrix4f &m, const Vector3f& _startPosition) {
	T[0][0] = m[0][0]; T[0][1] = m[0][1]; T[0][2] = m[0][2]; T[0][3] = m[0][3];
	T[1][0] = m[1][0]; T[1][1] = m[1][1]; T[1][2] = m[1][2]; T[1][3] = m[1][3];
	T[2][0] = m[2][0]; T[2][1] = m[2][1]; T[2][2] = m[2][2]; T[2][3] = m[2][3];
	T[3][0] = m[3][0]; T[3][1] = m[3][1]; T[3][2] = m[3][2]; T[3][3] = m[3][3];
	startPosition = _startPosition;
	pos = !startPosition.zero();
}