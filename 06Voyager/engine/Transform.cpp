#include <iostream>
#include "Transform.h"

Transform::Transform() {
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

void Transform::rotate(const Vector3f &axis, float degrees) {

	Matrix4f rotMtx;
	rotMtx.rotate(axis, degrees);

	Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
		rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
		rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
		rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

	if (!pos) {
		T = T ^ rotMtx;
	}else {
		//T = (translate * invRotMtx * invTranslate) * T
		float  tmp1 = rotMtx[0][3], tmp2 = rotMtx[1][3], tmp3 = rotMtx[2][3], tmp4 = rotMtx[3][3];
		rotMtx[0][3] = startPosition[0] * (tmp4 - rotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * rotMtx[0][1] - startPosition[2] * rotMtx[0][2];
		rotMtx[1][3] = startPosition[1] * (tmp4 - rotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[1][0] - startPosition[2] * rotMtx[1][2];
		rotMtx[2][3] = startPosition[2] * (tmp4 - rotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[2][0] - startPosition[1] * rotMtx[2][1];
		rotMtx[3][3] = startPosition[0] * (tmp1 - rotMtx[3][0]) + startPosition[1] * (tmp2 - rotMtx[3][1]) + startPosition[2] * (tmp3 - rotMtx[3][2]) + tmp4;
		T = rotMtx ^ T;
	}
}


void Transform::translate(float dx, float dy, float dz) {
	T[0][3] = T[0][3] + dx*T[0][0] + dz*T[0][2] + dy*T[0][1];
	T[1][3] = T[1][3] + dx*T[1][0] + dz*T[1][2] + dy*T[1][1];
	T[2][3] = T[2][3] + dx*T[2][0] + dz*T[2][2] + dy*T[2][1];
}

void Transform::scale(float a, float b, float c) {

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	T[0][0] = T[0][0] * a;  T[0][1] = T[0][1] * b; T[0][2] = T[0][2] * c;
	T[1][0] = T[1][0] * a;  T[1][1] = T[1][1] * b; T[1][2] = T[1][2] * c;
	T[2][0] = T[2][0] * a;  T[2][1] = T[2][1] * b; T[2][2] = T[2][2] * c;
}

const Matrix4f& Transform::getTransformationMatrix() const {
	return T;
}

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

	invT[0][0] = T[0][0] * (1.0f / sxx); invT[0][1] = T[1][0] * (1.0f / sxy); invT[0][2] = T[2][0] * (1.0f / sxz); invT[0][3] = -(T[0][3] * T[0][0] * (1.0f / sxx) + T[1][3] * T[1][0] * (1.0f / sxy) + T[2][3] * T[2][0] * (1.0f / sxz));
	invT[1][0] = T[0][1] * (1.0f / sxy); invT[1][1] = T[1][1] * (1.0f / syy); invT[1][2] = T[2][1] * (1.0f / syz); invT[1][3] = -(T[0][3] * T[0][1] * (1.0f / sxy) + T[1][3] * T[1][1] * (1.0f / syy) + T[2][3] * T[2][1] * (1.0f / syz));
	invT[2][0] = T[0][2] * (1.0f / sxz); invT[2][1] = T[1][2] * (1.0f / syz); invT[2][2] = T[2][2] * (1.0f / szz); invT[2][3] = -(T[0][3] * T[0][2] * (1.0f / sxz) + T[1][3] * T[1][2] * (1.0f / syz) + T[2][3] * T[2][2] * (1.0f / szz));
	invT[3][0] = 0.0f; invT[3][1] = 0.0f; invT[3][2] = 0.0f; invT[3][3] = 1.0f;

	return invT;
}

void Transform::getScale(Vector3f& scale) {
	scale = Vector3f(Vector3f(T[0][0], T[1][0], T[2][0]).length(), Vector3f(T[0][1], T[1][1], T[2][1]).length(), Vector3f(T[0][2], T[1][2], T[2][2]).length());
}

void Transform::getPosition(Vector3f& position) {
	position = Vector3f(T[0][3], T[1][3], T[2][3]);
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

	pos = !startPosition.zero();
}