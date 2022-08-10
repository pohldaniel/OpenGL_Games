#include <iostream>
#include "Transform.h"

Transform::Transform(){
	pos = false;
	T.identity();
	invT.identity();

	startPosition = Vector3f(0.0, 0.0, 0.0);
}

Transform::Transform(const Matrix4f &m) {
	fromMatrix(m);
}

Transform::~Transform(){

}

void Transform::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz){

	Matrix4f rotMtx;
	Matrix4f invRotMtx;

	if (degrees != 0 && !(axis[0] == 0.0 && axis[1] == 0.0 && axis[2] == 0.0)){

		rotMtx.rotate(axis, degrees);

		invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
							 rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
							 rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
							 rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

		T = T ^ rotMtx;
		invT = invRotMtx ^ invT;
	}

	orientation.fromAxisAngle(axis, degrees);

	//T = Translate * T
	T[0][0] = T[0][0] + T[3][0] * dx; T[1][0] = T[1][0] + T[3][0] * dz; T[2][0] = T[2][0] + T[3][0] * dy;
	T[0][1] = T[0][1] + T[3][1] * dx; T[1][1] = T[1][1] + T[3][1] * dz; T[2][1] = T[2][1] + T[3][1] * dy;
	T[0][2] = T[0][2] + T[3][2] * dx; T[1][2] = T[1][2] + T[3][2] * dz; T[2][2] = T[2][2] + T[3][2] * dy;
	T[0][3] = T[0][3] + dx; T[1][3] = T[1][3] + dy; T[2][3] = T[2][3] + dz;

	//T^-1 = T^-1 * Translate^-1
	invT[0][3] = invT[0][3] - dx*invT[0][0] - dz*invT[0][2] - dy*invT[0][1];
	invT[1][3] = invT[1][3] - dx*invT[1][0] - dz*invT[1][2] - dy*invT[1][1];
	invT[2][3] = invT[2][3] - dx*invT[2][0] - dz*invT[2][2] - dy*invT[2][1];


	startPosition = Vector3f(dx, dy, dz);
	position = Vector3f(dx, dy, dz);
	

	if (!startPosition.zero()){
		pos = true;
	}
}

void Transform::setRotPosScale(const Vector3f &axis, float degrees, float dx, float dy, float dz, float a, float b, float c) {
	Matrix4f rotMtx;
	Matrix4f invRotMtx;

	if (degrees != 0 && !(axis[0] == 0.0 && axis[1] == 0.0 && axis[2] == 0.0)) {

		rotMtx.rotate(axis, degrees);

		invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
			rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
			rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
			rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

		T = T ^ rotMtx;
		invT = invRotMtx ^ invT;

		
	}

	orientation.fromAxisAngle(axis, degrees);
	
	//T = Translate * T
	T[0][0] = T[0][0] + T[3][0] * dx; T[1][0] = T[1][0] + T[3][0] * dz; T[2][0] = T[2][0] + T[3][0] * dy;
	T[0][1] = T[0][1] + T[3][1] * dx; T[1][1] = T[1][1] + T[3][1] * dz; T[2][1] = T[2][1] + T[3][1] * dy;
	T[0][2] = T[0][2] + T[3][2] * dx; T[1][2] = T[1][2] + T[3][2] * dz; T[2][2] = T[2][2] + T[3][2] * dy;
	T[0][3] = T[0][3] + dx; T[1][3] = T[1][3] + dy; T[2][3] = T[2][3] + dz;

	//T^-1 = T^-1 * Translate^-1
	invT[0][3] = invT[0][3] - dx*invT[0][0] - dz*invT[0][2] - dy*invT[0][1];
	invT[1][3] = invT[1][3] - dx*invT[1][0] - dz*invT[1][2] - dy*invT[1][1];
	invT[2][3] = invT[2][3] - dx*invT[2][0] - dz*invT[2][2] - dy*invT[2][1];


	startPosition = Vector3f(dx, dy, dz);
	position = Vector3f(dx, dy, dz);

	T[0][0] = T[0][0] * a;  T[0][1] = T[0][1] * b; T[0][2] = T[0][2] * c;
	T[1][0] = T[1][0] * a;  T[1][1] = T[1][1] * b; T[1][2] = T[1][2] * c;
	T[2][0] = T[2][0] * a;  T[2][1] = T[2][1] * b; T[2][2] = T[2][2] * c;


	invT[0][0] = invT[0][0] * (1.0 / a); invT[1][0] = invT[1][0] * (1.0 / b); invT[2][0] = invT[2][0] * (1.0 / c);
	invT[0][1] = invT[0][1] * (1.0 / a); invT[1][1] = invT[1][1] * (1.0 / b); invT[2][1] = invT[2][1] * (1.0 / c);
	invT[0][2] = invT[0][2] * (1.0 / a); invT[1][2] = invT[1][2] * (1.0 / b); invT[2][2] = invT[2][2] * (1.0 / c);
	invT[0][3] = invT[0][3] * (1.0 / a); invT[1][3] = invT[1][3] * (1.0 / b); invT[2][3] = invT[2][3] * (1.0 / c);

	if (!startPosition.zero()) {
		pos = true;
	}
}

void Transform::setRotXYZPos(const Vector3f &axisX, float degreesX,
	const Vector3f &axisY, float degreesY,
	const Vector3f &axisZ, float degreesZ,
	float dx, float dy, float dz){

	Matrix4f rotMtx;
	Matrix4f invRotMtx;

	if (degreesX != 0 && !(axisX[0] == 0.0 && axisX[1] == 0.0 && axisX[2] == 0.0)){
	
		rotMtx.rotate(axisX, degreesX);
		invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
							 rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
							 rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
							 rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

		T = T ^ rotMtx;
		invT = invRotMtx ^ invT;

	}

	if (degreesY != 0 && !(axisY[0] == 0.0 && axisY[1] == 0.0 && axisY[2] == 0.0)){
	
		rotMtx.rotate(axisY, degreesY);
		invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
							 rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
							 rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
							 rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);
		T = T ^ rotMtx;
		invT = invRotMtx ^ invT;

		

	}

	if (degreesZ != 0 && !(axisZ[0] == 0.0 && axisZ[1] == 0.0 && axisZ[2] == 0.0)){
		
		rotMtx.rotate(axisZ, degreesZ);
		invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
							 rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
							 rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
							 rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

		T = T ^ rotMtx;
		invT = invRotMtx ^ invT;
		
	}
	
	orientation.fromMatrix(T);

	//T = Translate * T
	T[0][0] = T[0][0] + T[3][0] * dx; T[1][0] = T[1][0] + T[3][0] * dz; T[2][0] = T[2][0] + T[3][0] * dy;
	T[0][1] = T[0][1] + T[3][1] * dx; T[1][1] = T[1][1] + T[3][1] * dz; T[2][1] = T[2][1] + T[3][1] * dy;
	T[0][2] = T[0][2] + T[3][2] * dx; T[1][2] = T[1][2] + T[3][2] * dz; T[2][2] = T[2][2] + T[3][2] * dy;
	T[0][3] = T[0][3] + dx; T[1][3] = T[1][3] + dy; T[2][3] = T[2][3] + dz;

	//T^-1 = T^-1 * Translate^-1
	invT[0][3] = invT[0][3] - dx*invT[0][0] - dz*invT[0][2] - dy*invT[0][1];
	invT[1][3] = invT[1][3] - dx*invT[1][0] - dz*invT[1][2] - dy*invT[1][1];
	invT[2][3] = invT[2][3] - dx*invT[2][0] - dz*invT[2][2] - dy*invT[2][1];

	startPosition = Vector3f(dx, dy, dz);
	pos = true;


	startPosition = Vector3f(dx, dy, dz);
	position = Vector3f(dx, dy, dz);

	if (!startPosition.zero()){
		pos = true;
	}
}

void Transform::rotate(const Vector3f &axis, float degrees){

	Matrix4f rotMtx;
	rotMtx.rotate(axis, degrees);

	Quaternion rotQuat;
	rotQuat.fromAxisAngle(axis, degrees);

	orientation = orientation * rotQuat;

	Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
		rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
		rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
		rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

	if (!pos){
		T = T ^ rotMtx;
		invT = invRotMtx ^ invT;
		
	}else{
		//T = (translate * invRotMtx * invTranslate) * T
		float tmp1 = invRotMtx[0][3], tmp2 = invRotMtx[1][3], tmp3 = invRotMtx[2][3], tmp4 = invRotMtx[3][3];
		invRotMtx[0][3] = startPosition[0] * (tmp4 - invRotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * invRotMtx[0][1] - startPosition[2] * invRotMtx[0][2];
		invRotMtx[1][3] = startPosition[1] * (tmp4 - invRotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * invRotMtx[1][0] - startPosition[2] * invRotMtx[1][2];
		invRotMtx[2][3] = startPosition[2] * (tmp4 - invRotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * invRotMtx[2][0] - startPosition[1] * invRotMtx[2][1];
		invRotMtx[3][3] = startPosition[0] * (tmp1 - invRotMtx[3][0]) + startPosition[1] * (tmp2 - invRotMtx[3][1]) + startPosition[2] * (tmp3 - invRotMtx[3][2]) + tmp4;
		invT = invT ^ invRotMtx;

		//invT = invT * (translate * rotMtx * invTranslate)
		tmp1 = rotMtx[0][3], tmp2 = rotMtx[1][3], tmp3 = rotMtx[2][3], tmp4 = rotMtx[3][3];
		rotMtx[0][3] = startPosition[0] * (tmp4 - rotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * rotMtx[0][1] - startPosition[2] * rotMtx[0][2];
		rotMtx[1][3] = startPosition[1] * (tmp4 - rotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[1][0] - startPosition[2] * rotMtx[1][2];
		rotMtx[2][3] = startPosition[2] * (tmp4 - rotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[2][0] - startPosition[1] * rotMtx[2][1];
		rotMtx[3][3] = startPosition[0] * (tmp1 - rotMtx[3][0]) + startPosition[1] * (tmp2 - rotMtx[3][1]) + startPosition[2] * (tmp3 - rotMtx[3][2]) + tmp4;
		T = rotMtx ^ T;	
	}
}


void Transform::translate(float dx, float dy, float dz){

	position = position + Vector3f(dx, dy, dz);

	T[0][3] = T[0][3] + dx*T[0][0] + dz*T[0][2] + dy*T[0][1];
	T[1][3] = T[1][3] + dx*T[1][0] + dz*T[1][2] + dy*T[1][1];
	T[2][3] = T[2][3] + dx*T[2][0] + dz*T[2][2] + dy*T[2][1];

	invT[0][0] = invT[0][0] - invT[3][0] * dx; invT[1][0] = invT[1][0] - invT[3][0] * dz; invT[2][0] = invT[2][0] - invT[3][0] * dy;
	invT[0][1] = invT[0][1] - invT[3][1] * dx; invT[1][1] = invT[1][1] - invT[3][1] * dz; invT[2][1] = invT[2][1] - invT[3][1] * dy;
	invT[0][2] = invT[0][2] - invT[3][2] * dx; invT[1][2] = invT[1][2] - invT[3][2] * dz; invT[2][2] = invT[2][2] - invT[3][2] * dy;
	invT[0][3] = invT[0][3] - dx; invT[1][3] = invT[1][3] - dy; invT[2][3] = invT[2][3] - dz;

}

void Transform::scale(float a, float b, float c){

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	T[0][0] = T[0][0] * a;  T[0][1] = T[0][1] * b; T[0][2] = T[0][2] * c;
	T[1][0] = T[1][0] * a;  T[1][1] = T[1][1] * b; T[1][2] = T[1][2] * c;
	T[2][0] = T[2][0] * a;  T[2][1] = T[2][1] * b; T[2][2] = T[2][2] * c;


	invT[0][0] = invT[0][0] * (1.0 / a); invT[1][0] = invT[1][0] * (1.0 / b); invT[2][0] = invT[2][0] * (1.0 / c);
	invT[0][1] = invT[0][1] * (1.0 / a); invT[1][1] = invT[1][1] * (1.0 / b); invT[2][1] = invT[2][1] * (1.0 / c);
	invT[0][2] = invT[0][2] * (1.0 / a); invT[1][2] = invT[1][2] * (1.0 / b); invT[2][2] = invT[2][2] * (1.0 / c);
	invT[0][3] = invT[0][3] * (1.0 / a); invT[1][3] = invT[1][3] * (1.0 / b); invT[2][3] = invT[2][3] * (1.0 / c);
}

const Matrix4f& Transform::getTransformationMatrix() const{
	return T;
}

const Matrix4f& Transform::getInvTransformationMatrix() const{
	return invT;
}

void Transform::getScale(Vector3f& scale) {
	scale = Vector3f(Vector3f(T[0][0], T[1][0], T[2][0]).length(), Vector3f(T[0][1], T[1][1], T[2][1]).length(), Vector3f(T[0][2], T[1][2], T[2][2]).length());
}

void Transform::getPosition(Vector3f& position) {
	position = Vector3f(T[0][3], T[1][3], T[2][3]);
}

const Vector3f& Transform::getPosition() const {
	return position;
}

const Quaternion& Transform::getOrientation() const {
	return orientation;
}

void Transform::getOrientation(Matrix4f& orientation) {
	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2],T[1][2], T[2][2]).length();

	orientation[0][0] = T[0][0] * (1.0 / sx); orientation[0][1] = T[0][1] * (1.0 / sx); orientation[0][2] = T[0][2] * (1.0 / sx); orientation[3][0] = 0.0;
	orientation[1][0] = T[1][0] * (1.0 / sy); orientation[1][1] = T[1][1] * (1.0 / sy); orientation[1][2] = T[1][2] * (1.0 / sy); orientation[3][1] = 0.0;
	orientation[2][0] = T[2][0] * (1.0 / sz); orientation[2][1] = T[2][1] * (1.0 / sz); orientation[2][2] = T[2][2] * (1.0 / sz); orientation[3][2] = 0.0;
	orientation[0][3] = 0.0;				  orientation[1][3] = 0.0;				    orientation[2][3] = 0.0;				  orientation[3][3] = 1.0;
}

Matrix4f& Transform::getOrientation() {
	return orientation.toMatrix4f();
}

void Transform::fromMatrix(const Matrix4f &m) {
	T[0][0] = m[0][0]; T[0][1] = m[0][1]; T[0][2] = m[0][2]; T[0][3] = m[0][3];
	T[1][0] = m[1][0]; T[1][1] = m[1][1]; T[1][2] = m[1][2]; T[1][3] = m[1][3];
	T[2][0] = m[2][0]; T[2][1] = m[2][1]; T[2][2] = m[2][2]; T[2][3] = m[2][3];
	T[3][0] = m[3][0]; T[3][1] = m[3][1]; T[3][2] = m[3][2]; T[3][3] = m[3][3];

	position = Vector3f(T[0][3], T[1][3], T[2][3]);

	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	Matrix4f rot;
	rot[0][0] = T[0][0] * (1.0 / sx); rot[0][1] = T[0][1] * (1.0 / sx); rot[0][2] = T[0][2] * (1.0 / sx); rot[0][3] = 0.0f;
	rot[1][0] = T[1][0] * (1.0 / sy); rot[1][1] = T[1][1] * (1.0 / sy); rot[1][2] = T[1][2] * (1.0 / sy); rot[1][3] = 0.0f;
	rot[2][0] = T[2][0] * (1.0 / sz); rot[2][1] = T[2][1] * (1.0 / sz); rot[2][2] = T[2][2] * (1.0 / sz); rot[2][3] = 0.0f;
	rot[3][0] = 0.0; rot[3][1] = 0.0; rot[3][2] = 0.0; rot[3][3] = 1.0f;

	orientation.fromMatrix(rot);
	startPosition[0] = 0.0f; startPosition[1] = 0.0f; startPosition[2] = 0.0f;

	invT[0][0] = rot[0][0] * (1.0 / sx); invT[0][1] = rot[1][0] * (1.0 / sx); invT[0][2] = rot[2][0] * (1.0 / sx); invT[0][3] = -(1.0 / sx) * (T[0][3] * rot[0][0] + T[1][3] * rot[1][0] + T[2][3] * rot[2][0]);
	invT[1][0] = rot[0][1] * (1.0 / sy); invT[1][1] = rot[1][1] * (1.0 / sy); invT[1][2] = rot[2][1] * (1.0 / sy); invT[1][3] = -(1.0 / sy) * (T[0][3] * rot[0][1] + T[1][3] * rot[1][1] + T[2][3] * rot[2][1]);
	invT[2][0] = rot[0][2] * (1.0 / sz); invT[2][1] = rot[1][2] * (1.0 / sz); invT[2][2] = rot[2][2] * (1.0 / sz); invT[2][3] = -(1.0 / sz) * (T[0][3] * rot[0][2] + T[1][3] * rot[1][2] + T[2][3] * rot[2][2]);
	invT[3][0] = 0.0f; invT[3][1] = 0.0f; invT[3][2] = 0.0f; invT[3][3] = 1.0f;

	pos = false;
}

void Transform::fromMatrix(const Matrix4f &m, const Vector3f& _startPosition) {
	T[0][0] = m[0][0]; T[0][1] = m[0][1]; T[0][2] = m[0][2]; T[0][3] = m[0][3];
	T[1][0] = m[1][0]; T[1][1] = m[1][1]; T[1][2] = m[1][2]; T[1][3] = m[1][3];
	T[2][0] = m[2][0]; T[2][1] = m[2][1]; T[2][2] = m[2][2]; T[2][3] = m[2][3];
	T[3][0] = m[3][0]; T[3][1] = m[3][1]; T[3][2] = m[3][2]; T[3][3] = m[3][3];

	position = Vector3f(T[0][3], T[1][3], T[2][3]);

	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	Matrix4f rot;
	rot[0][0] = T[0][0] * (1.0 / sx); rot[0][1] = T[0][1] * (1.0 / sx); rot[0][2] = T[0][2] * (1.0 / sx); rot[0][3] = 0.0f;
	rot[1][0] = T[1][0] * (1.0 / sy); rot[1][1] = T[1][1] * (1.0 / sy); rot[1][2] = T[1][2] * (1.0 / sy); rot[1][3] = 0.0f;
	rot[2][0] = T[2][0] * (1.0 / sz); rot[2][1] = T[2][1] * (1.0 / sz); rot[2][2] = T[2][2] * (1.0 / sz); rot[2][3] = 0.0f;
	rot[3][0] = 0.0; rot[3][1] = 0.0; rot[3][2] = 0.0; rot[3][3] = 1.0f;

	orientation.fromMatrix(rot);
	startPosition = _startPosition;

	invT[0][0] = rot[0][0] * (1.0 / sx); invT[0][1] = rot[1][0] * (1.0 / sx); invT[0][2] = rot[2][0] * (1.0 / sx); invT[0][3] = -(1.0 / sx) * (T[0][3] * rot[0][0] + T[1][3] * rot[1][0] + T[2][3] * rot[2][0]);
	invT[1][0] = rot[0][1] * (1.0 / sy); invT[1][1] = rot[1][1] * (1.0 / sy); invT[1][2] = rot[2][1] * (1.0 / sy); invT[1][3] = -(1.0 / sy) * (T[0][3] * rot[0][1] + T[1][3] * rot[1][1] + T[2][3] * rot[2][1]);
	invT[2][0] = rot[0][2] * (1.0 / sz); invT[2][1] = rot[1][2] * (1.0 / sz); invT[2][2] = rot[2][2] * (1.0 / sz); invT[2][3] = -(1.0 / sz) * (T[0][3] * rot[0][2] + T[1][3] * rot[1][2] + T[2][3] * rot[2][2]);
	invT[3][0] = 0.0f; invT[3][1] = 0.0f; invT[3][2] = 0.0f; invT[3][3] = 1.0f;


	pos = !startPosition.zero();	
}