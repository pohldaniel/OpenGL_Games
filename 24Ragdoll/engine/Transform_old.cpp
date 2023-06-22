#include <iostream>
#include "Transform_old.h"

TransformOld::TransformOld(){
	pos = false;
	T.identity();
	invT.identity();

	startPosition = Vector3f(0.0f, 0.0f, 0.0f);
}

TransformOld::TransformOld(const Matrix4f &m) {
	fromMatrix(m);
}

TransformOld::~TransformOld(){

}

void TransformOld::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz){

	Matrix4f rotMtx;
	Matrix4f invRotMtx;

	if (degrees != 0.0f && !(axis[0] == 0.0f && axis[1] == 0.0f && axis[2] == 0.0f)){

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

void TransformOld::setRotPosScale(const Vector3f &axis, float degrees, float dx, float dy, float dz, float a, float b, float c) {
	Matrix4f rotMtx;
	Matrix4f invRotMtx;

	if (degrees != 0.0f && !(axis[0] == 0.0f && axis[1] == 0.0f && axis[2] == 0.0f)) {

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


	invT[0][0] = invT[0][0] * (1.0f / a); invT[1][0] = invT[1][0] * (1.0f / b); invT[2][0] = invT[2][0] * (1.0f / c);
	invT[0][1] = invT[0][1] * (1.0f / a); invT[1][1] = invT[1][1] * (1.0f / b); invT[2][1] = invT[2][1] * (1.0f / c);
	invT[0][2] = invT[0][2] * (1.0f / a); invT[1][2] = invT[1][2] * (1.0f / b); invT[2][2] = invT[2][2] * (1.0f / c);
	invT[0][3] = invT[0][3] * (1.0f / a); invT[1][3] = invT[1][3] * (1.0f / b); invT[2][3] = invT[2][3] * (1.0f / c);

	if (!startPosition.zero()) {
		pos = true;
	}
}

void TransformOld::setRotXYZPos(const Vector3f &axisX, float degreesX,
	const Vector3f &axisY, float degreesY,
	const Vector3f &axisZ, float degreesZ,
	float dx, float dy, float dz){

	Matrix4f rotMtx;
	Matrix4f invRotMtx;

	if (degreesX != 0.0f && !(axisX[0] == 0.0f && axisX[1] == 0.0f && axisX[2] == 0.0f)){
	
		rotMtx.rotate(axisX, degreesX);
		invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
							 rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
							 rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
							 rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);

		T = T ^ rotMtx;
		invT = invRotMtx ^ invT;

	}

	if (degreesY != 0.0f && !(axisY[0] == 0.0f && axisY[1] == 0.0f && axisY[2] == 0.0f)){
	
		rotMtx.rotate(axisY, degreesY);
		invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], rotMtx[3][0],
							 rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], rotMtx[3][1],
							 rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], rotMtx[3][2],
							 rotMtx[0][3], rotMtx[1][3], rotMtx[2][3], rotMtx[3][3]);
		T = T ^ rotMtx;
		invT = invRotMtx ^ invT;

		

	}

	if (degreesZ != 0.0f && !(axisZ[0] == 0.0f && axisZ[1] == 0.0f && axisZ[2] == 0.0f)){
		
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

void TransformOld::rotate(const Vector3f &axis, float degrees){

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
		//invT = invT * (translate * rotMtx * invTranslate)
		float tmp1 = invRotMtx[0][3], tmp2 = invRotMtx[1][3], tmp3 = invRotMtx[2][3], tmp4 = invRotMtx[3][3];
		invRotMtx[0][3] = startPosition[0] * (tmp4 - invRotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * invRotMtx[0][1] - startPosition[2] * invRotMtx[0][2];
		invRotMtx[1][3] = startPosition[1] * (tmp4 - invRotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * invRotMtx[1][0] - startPosition[2] * invRotMtx[1][2];
		invRotMtx[2][3] = startPosition[2] * (tmp4 - invRotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * invRotMtx[2][0] - startPosition[1] * invRotMtx[2][1];
		invRotMtx[3][3] = startPosition[0] * (tmp1 - invRotMtx[3][0]) + startPosition[1] * (tmp2 - invRotMtx[3][1]) + startPosition[2] * (tmp3 - invRotMtx[3][2]) + tmp4;
		invT = invT ^ invRotMtx;


		//T = (translate * invRotMtx * invTranslate) * T
		tmp1 = rotMtx[0][3], tmp2 = rotMtx[1][3], tmp3 = rotMtx[2][3], tmp4 = rotMtx[3][3];
		rotMtx[0][3] = startPosition[0] * (tmp4 - rotMtx[0][0]) + tmp1 + tmp2 + tmp3 - startPosition[1] * rotMtx[0][1] - startPosition[2] * rotMtx[0][2];
		rotMtx[1][3] = startPosition[1] * (tmp4 - rotMtx[1][1]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[1][0] - startPosition[2] * rotMtx[1][2];
		rotMtx[2][3] = startPosition[2] * (tmp4 - rotMtx[2][2]) + tmp1 + tmp2 + tmp3 - startPosition[0] * rotMtx[2][0] - startPosition[1] * rotMtx[2][1];
		rotMtx[3][3] = startPosition[0] * (tmp1 - rotMtx[3][0]) + startPosition[1] * (tmp2 - rotMtx[3][1]) + startPosition[2] * (tmp3 - rotMtx[3][2]) + tmp4;
		T = rotMtx ^ T;	
	}
}


void TransformOld::translate(float dx, float dy, float dz){

	position = position + Vector3f(dx, dy, dz);

	T[0][3] = T[0][3] + dx*T[0][0] + dz*T[0][2] + dy*T[0][1];
	T[1][3] = T[1][3] + dx*T[1][0] + dz*T[1][2] + dy*T[1][1];
	T[2][3] = T[2][3] + dx*T[2][0] + dz*T[2][2] + dy*T[2][1];

	invT[0][0] = invT[0][0] - invT[3][0] * dx; invT[1][0] = invT[1][0] - invT[3][0] * dz; invT[2][0] = invT[2][0] - invT[3][0] * dy;
	invT[0][1] = invT[0][1] - invT[3][1] * dx; invT[1][1] = invT[1][1] - invT[3][1] * dz; invT[2][1] = invT[2][1] - invT[3][1] * dy;
	invT[0][2] = invT[0][2] - invT[3][2] * dx; invT[1][2] = invT[1][2] - invT[3][2] * dz; invT[2][2] = invT[2][2] - invT[3][2] * dy;
	invT[0][3] = invT[0][3] - dx; invT[1][3] = invT[1][3] - dy; invT[2][3] = invT[2][3] - dz;

}

void TransformOld::scale(float a, float b, float c){

	if (a == 0.0f) a = 1.0f;
	if (b == 0.0f) b = 1.0f;
	if (c == 0.0f) c = 1.0f;

	T[0][0] = T[0][0] * a;  T[0][1] = T[0][1] * b; T[0][2] = T[0][2] * c;
	T[1][0] = T[1][0] * a;  T[1][1] = T[1][1] * b; T[1][2] = T[1][2] * c;
	T[2][0] = T[2][0] * a;  T[2][1] = T[2][1] * b; T[2][2] = T[2][2] * c;


	invT[0][0] = invT[0][0] * (1.0f / a); invT[1][0] = invT[1][0] * (1.0f / b); invT[2][0] = invT[2][0] * (1.0f / c);
	invT[0][1] = invT[0][1] * (1.0f / a); invT[1][1] = invT[1][1] * (1.0f / b); invT[2][1] = invT[2][1] * (1.0f / c);
	invT[0][2] = invT[0][2] * (1.0f / a); invT[1][2] = invT[1][2] * (1.0f / b); invT[2][2] = invT[2][2] * (1.0f / c);
	invT[0][3] = invT[0][3] * (1.0f / a); invT[1][3] = invT[1][3] * (1.0f / b); invT[2][3] = invT[2][3] * (1.0f / c);
}

const Matrix4f& TransformOld::getTransformationMatrix() const{
	return T;
}

const Matrix4f& TransformOld::getInvTransformationMatrix() const{
	return invT;
}

const Matrix4f& TransformOld::GetInvTransformationMatrix() {
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

void TransformOld::getScale(Vector3f& scale) {
	scale = Vector3f(Vector3f(T[0][0], T[1][0], T[2][0]).length(), Vector3f(T[0][1], T[1][1], T[2][1]).length(), Vector3f(T[0][2], T[1][2], T[2][2]).length());
}

void TransformOld::getPosition(Vector3f& position) {
	position = Vector3f(T[0][3], T[1][3], T[2][3]);
}

void TransformOld::getOrientation(Matrix4f& orientation) {
	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	orientation[0][0] = T[0][0] * (1.0f / sx); orientation[0][1] = T[0][1] * (1.0f / sx); orientation[0][2] = T[0][2] * (1.0f / sx); orientation[0][3] = 0.0f;
	orientation[1][0] = T[1][0] * (1.0f / sy); orientation[1][1] = T[1][1] * (1.0f / sy); orientation[1][2] = T[1][2] * (1.0f / sy); orientation[1][3] = 0.0f;
	orientation[2][0] = T[2][0] * (1.0f / sz); orientation[2][1] = T[2][1] * (1.0f / sz); orientation[2][2] = T[2][2] * (1.0f / sz); orientation[2][3] = 0.0f;
	orientation[3][0] = 0.0f;				   orientation[3][1] = 0.0f;				  orientation[3][2] = 0.0f;				     orientation[3][3] = 1.0f;
}

const Vector3f& TransformOld::getPosition() const {
	return position;
}

const Quaternion& TransformOld::getOrientation() const {
	return orientation;
}

Matrix4f& TransformOld::getOrientation() {
	return orientation.toMatrix4f();
}

void TransformOld::fromMatrix(const Matrix4f &m) {
	T[0][0] = m[0][0]; T[0][1] = m[0][1]; T[0][2] = m[0][2]; T[0][3] = m[0][3];
	T[1][0] = m[1][0]; T[1][1] = m[1][1]; T[1][2] = m[1][2]; T[1][3] = m[1][3];
	T[2][0] = m[2][0]; T[2][1] = m[2][1]; T[2][2] = m[2][2]; T[2][3] = m[2][3];
	T[3][0] = m[3][0]; T[3][1] = m[3][1]; T[3][2] = m[3][2]; T[3][3] = m[3][3];

	position = Vector3f(T[0][3], T[1][3], T[2][3]);

	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	Matrix4f rot;
	rot[0][0] = T[0][0] * (1.0f / sx); rot[0][1] = T[0][1] * (1.0f / sx); rot[0][2] = T[0][2] * (1.0f / sx); rot[0][3] = 0.0f;
	rot[1][0] = T[1][0] * (1.0f / sy); rot[1][1] = T[1][1] * (1.0f / sy); rot[1][2] = T[1][2] * (1.0f / sy); rot[1][3] = 0.0f;
	rot[2][0] = T[2][0] * (1.0f / sz); rot[2][1] = T[2][1] * (1.0f / sz); rot[2][2] = T[2][2] * (1.0f / sz); rot[2][3] = 0.0f;
	rot[3][0] = 0.0f; rot[3][1] = 0.0f; rot[3][2] = 0.0; rot[3][3] = 1.0f;

	orientation.fromMatrix(rot);
	startPosition[0] = 0.0f; startPosition[1] = 0.0f; startPosition[2] = 0.0f;

	invT[0][0] = rot[0][0] * (1.0f / sx); invT[0][1] = rot[1][0] * (1.0f / sx); invT[0][2] = rot[2][0] * (1.0f / sx); invT[0][3] = -(1.0f / sx) * (T[0][3] * rot[0][0] + T[1][3] * rot[1][0] + T[2][3] * rot[2][0]);
	invT[1][0] = rot[0][1] * (1.0f / sy); invT[1][1] = rot[1][1] * (1.0f / sy); invT[1][2] = rot[2][1] * (1.0f / sy); invT[1][3] = -(1.0f / sy) * (T[0][3] * rot[0][1] + T[1][3] * rot[1][1] + T[2][3] * rot[2][1]);
	invT[2][0] = rot[0][2] * (1.0f / sz); invT[2][1] = rot[1][2] * (1.0f / sz); invT[2][2] = rot[2][2] * (1.0f / sz); invT[2][3] = -(1.0f / sz) * (T[0][3] * rot[0][2] + T[1][3] * rot[1][2] + T[2][3] * rot[2][2]);
	invT[3][0] = 0.0f; invT[3][1] = 0.0f; invT[3][2] = 0.0f; invT[3][3] = 1.0f;

	pos = false;
}

void TransformOld::fromMatrix(const Matrix4f &m, const Vector3f& _startPosition) {
	T[0][0] = m[0][0]; T[0][1] = m[0][1]; T[0][2] = m[0][2]; T[0][3] = m[0][3];
	T[1][0] = m[1][0]; T[1][1] = m[1][1]; T[1][2] = m[1][2]; T[1][3] = m[1][3];
	T[2][0] = m[2][0]; T[2][1] = m[2][1]; T[2][2] = m[2][2]; T[2][3] = m[2][3];
	T[3][0] = m[3][0]; T[3][1] = m[3][1]; T[3][2] = m[3][2]; T[3][3] = m[3][3];

	position = Vector3f(T[0][3], T[1][3], T[2][3]);

	float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	Matrix4f rot;
	rot[0][0] = T[0][0] * (1.0f / sx); rot[0][1] = T[0][1] * (1.0f / sx); rot[0][2] = T[0][2] * (1.0f / sx); rot[0][3] = 0.0f;
	rot[1][0] = T[1][0] * (1.0f / sy); rot[1][1] = T[1][1] * (1.0f / sy); rot[1][2] = T[1][2] * (1.0f / sy); rot[1][3] = 0.0f;
	rot[2][0] = T[2][0] * (1.0f / sz); rot[2][1] = T[2][1] * (1.0f / sz); rot[2][2] = T[2][2] * (1.0f / sz); rot[2][3] = 0.0f;
	rot[3][0] = 0.0f; rot[3][1] = 0.0f; rot[3][2] = 0.0f; rot[3][3] = 1.0f;

	orientation.fromMatrix(rot);
	startPosition = _startPosition;

	invT[0][0] = rot[0][0] * (1.0f / sx); invT[0][1] = rot[1][0] * (1.0f / sx); invT[0][2] = rot[2][0] * (1.0f / sx); invT[0][3] = -(1.0f / sx) * (T[0][3] * rot[0][0] + T[1][3] * rot[1][0] + T[2][3] * rot[2][0]);
	invT[1][0] = rot[0][1] * (1.0f / sy); invT[1][1] = rot[1][1] * (1.0f / sy); invT[1][2] = rot[2][1] * (1.0f / sy); invT[1][3] = -(1.0f / sy) * (T[0][3] * rot[0][1] + T[1][3] * rot[1][1] + T[2][3] * rot[2][1]);
	invT[2][0] = rot[0][2] * (1.0f / sz); invT[2][1] = rot[1][2] * (1.0f / sz); invT[2][2] = rot[2][2] * (1.0f / sz); invT[2][3] = -(1.0f / sz) * (T[0][3] * rot[0][2] + T[1][3] * rot[1][2] + T[2][3] * rot[2][2]);
	invT[3][0] = 0.0f; invT[3][1] = 0.0f; invT[3][2] = 0.0f; invT[3][3] = 1.0f;


	pos = !startPosition.zero();	
}