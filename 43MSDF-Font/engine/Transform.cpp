#include "Transform.h"

Transform::Transform() {
	T.identity();
	invT.identity();
}

Transform::Transform(const Matrix4f& m) {
	fromMatrix(m);
}

Transform::Transform(Transform const& rhs) {
	T = rhs.T;
	invT = rhs.invT;
}

Transform::Transform(Transform&& rhs) {
	T = rhs.T;
	invT = rhs.invT;
}

Transform& Transform::operator=(const Transform& rhs) {
	T = rhs.T;
	invT = rhs.invT;
	return *this;
}

void Transform::reset() {
	T.identity();
	invT.identity();
}

Transform::~Transform() {

}

void Transform::setRotPos(const Vector3f& axis, float degrees, float dx, float dy, float dz) {
	T.identity();
	invT.identity();

	if (degrees != 0.0f && !(axis[0] == 0.0f && axis[1] == 0.0f && axis[2] == 0.0f)) {
		Matrix4f rotMtx;
		rotMtx.rotate(axis, degrees);
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
}

void Transform::setRotPosScale(const Vector3f& axis, float degrees, float dx, float dy, float dz, float sx, float sy, float sz) {
	T.identity();
	invT.identity();

	if (degrees != 0.0f && !(axis[0] == 0.0f && axis[1] == 0.0f && axis[2] == 0.0f)) {

		Matrix4f rotMtx;
		rotMtx.rotate(axis, degrees);

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

	
	T[0][0] = T[0][0] * sx;  T[1][0] = T[1][0] * sy; T[2][0] = T[2][0] * sz;
	T[0][1] = T[0][1] * sx;  T[1][1] = T[1][1] * sy; T[2][1] = T[2][1] * sz;
	T[0][2] = T[0][2] * sx;  T[1][2] = T[1][2] * sy; T[2][2] = T[2][2] * sz;

	//invT[0][0] = invT[0][0] * (1.0 / sx); invT[0][1] = invT[0][1] * (1.0 / sy); invT[0][2] = invT[0][2] * (1.0 / sz);
	//invT[1][0] = invT[1][0] * (1.0 / sx); invT[1][1] = invT[1][1] * (1.0 / sy); invT[1][2] = invT[1][2] * (1.0 / sz);
	//invT[2][0] = invT[2][0] * (1.0 / sx); invT[2][1] = invT[2][1] * (1.0 / sy); invT[2][2] = invT[2][2] * (1.0 / sz);
	//invT[3][0] = invT[3][0] * (1.0 / sx); invT[3][1] = invT[3][1] * (1.0 / sy); invT[3][2] = invT[3][2] * (1.0 / sz);
}

void Transform::rotate(const Vector3f& axis, float degrees) {
	Matrix4f rotMtx;
	rotMtx.rotate(axis, degrees);

	//T = (translate * rotMtx * invTranslate) * T
	rotMtx[3][0] = T[3][0] * (1.0f - rotMtx[0][0]) - T[3][1] * rotMtx[1][0] - T[3][2] * rotMtx[2][0];
	rotMtx[3][1] = T[3][1] * (1.0f - rotMtx[1][1]) - T[3][0] * rotMtx[0][1] - T[3][2] * rotMtx[2][1];
	rotMtx[3][2] = T[3][2] * (1.0f - rotMtx[2][2]) - T[3][0] * rotMtx[0][2] - T[3][1] * rotMtx[1][2];
	rotMtx[3][3] = 1.0f;

	T = rotMtx * T;

	//T = (translate * invRotMtx * invTranslate) * T
	//Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], 0.0f,
	//	rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], 0.0f,
	//	rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);

	//invRotMtx[3][0] = T[3][0] * (1.0f - invRotMtx[0][0]) - T[3][1] * invRotMtx[1][0] - T[3][2] * invRotMtx[2][0];
	//invRotMtx[3][1] = T[3][1] * (1.0f - invRotMtx[1][1]) - T[3][0] * invRotMtx[0][1] - T[3][2] * invRotMtx[2][1];
	//invRotMtx[3][2] = T[3][2] * (1.0f - invRotMtx[2][2]) - T[3][0] * invRotMtx[0][2] - T[3][1] * invRotMtx[1][2];
	//invRotMtx[3][3] = 1.0f;

	//invT = invT * invRotMtx;
}

void Transform::rotate(const Quaternion& quat) {
	Matrix4f rotMtx = quat.toMatrix4f();

	//T = (translate * rotMtx * invTranslate) * T
	rotMtx[3][0] = T[3][0] * (1.0f - rotMtx[0][0]) - T[3][1] * rotMtx[1][0] - T[3][2] * rotMtx[2][0];
	rotMtx[3][1] = T[3][1] * (1.0f - rotMtx[1][1]) - T[3][0] * rotMtx[0][1] - T[3][2] * rotMtx[2][1];
	rotMtx[3][2] = T[3][2] * (1.0f - rotMtx[2][2]) - T[3][0] * rotMtx[0][2] - T[3][1] * rotMtx[1][2];
	rotMtx[3][3] = 1.0f;

	T = rotMtx * T;
	
	//T = (translate * invRotMtx * invTranslate) * T
	//Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], 0.0f,
	//	rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], 0.0f,
	//	rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);

	//invRotMtx[3][0] = T[3][0] * (1.0f - invRotMtx[0][0]) - T[3][1] * invRotMtx[1][0] - T[3][2] * invRotMtx[2][0];
	//invRotMtx[3][1] = T[3][1] * (1.0f - invRotMtx[1][1]) - T[3][0] * invRotMtx[0][1] - T[3][2] * invRotMtx[2][1];
	//invRotMtx[3][2] = T[3][2] * (1.0f - invRotMtx[2][2]) - T[3][0] * invRotMtx[0][2] - T[3][1] * invRotMtx[1][2];
	//invRotMtx[3][3] = 1.0f;

	//invT = invT * invRotMtx;
}

void Transform::rotate(float pitch, float yaw, float roll) {
	Matrix4f rotMtx;
	rotMtx.rotate(pitch, yaw, roll);

	//T = (translate * rotMtx * invTranslate) * T
	rotMtx[3][0] = T[3][0] * (1.0f - rotMtx[0][0]) - T[3][1] * rotMtx[1][0] - T[3][2] * rotMtx[2][0];
	rotMtx[3][1] = T[3][1] * (1.0f - rotMtx[1][1]) - T[3][0] * rotMtx[0][1] - T[3][2] * rotMtx[2][1];
	rotMtx[3][2] = T[3][2] * (1.0f - rotMtx[2][2]) - T[3][0] * rotMtx[0][2] - T[3][1] * rotMtx[1][2];
	rotMtx[3][3] = 1.0f;

	T = rotMtx * T;

	//T = (translate * invRotMtx * invTranslate) * T
	//Matrix4f invRotMtx = Matrix4f(rotMtx[0][0], rotMtx[1][0], rotMtx[2][0], 0.0f,
	//	rotMtx[0][1], rotMtx[1][1], rotMtx[2][1], 0.0f,
	//	rotMtx[0][2], rotMtx[1][2], rotMtx[2][2], 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);

	//invRotMtx[3][0] = T[3][0] * (1.0f - invRotMtx[0][0]) - T[3][1] * invRotMtx[1][0] - T[3][2] * invRotMtx[2][0];
	//invRotMtx[3][1] = T[3][1] * (1.0f - invRotMtx[1][1]) - T[3][0] * invRotMtx[0][1] - T[3][2] * invRotMtx[2][1];
	//invRotMtx[3][2] = T[3][2] * (1.0f - invRotMtx[2][2]) - T[3][0] * invRotMtx[0][2] - T[3][1] * invRotMtx[1][2];
	//invRotMtx[3][3] = 1.0f;

	//invT = invT * invRotMtx;
}

void Transform::rotate(const Vector3f& axis, float degrees, const Vector3f& centerOfRotation) {
	Matrix4f rotMtx;
	rotMtx.rotate(axis, degrees, centerOfRotation);
	T = rotMtx * T;

	//rotMtx.transpose3();	
	//float tmp1 = rotMtx[3][0]; float tmp2 = rotMtx[3][1]; float tmp3 = rotMtx[3][2];
	//rotMtx[3][0] = -(tmp1 * rotMtx[0][0] + tmp2 * rotMtx[1][0] + tmp3 * rotMtx[2][0]);
	//rotMtx[3][1] = -(tmp1 * rotMtx[0][1] + tmp2 * rotMtx[1][1] + tmp3 * rotMtx[2][1]);
	//rotMtx[3][2] = -(tmp1 * rotMtx[0][2] + tmp2 * rotMtx[1][2] + tmp3 * rotMtx[2][2]);
	//rotMtx[0][3] = 0.0f; rotMtx[1][3] = 0.0f; rotMtx[2][3] = 0.0f; rotMtx[3][3] = 1.0f;

	//invT = invT * rotMtx;
}

void Transform::rotate(const Quaternion& quat, const Vector3f& centerOfRotation) {
	Matrix4f rotMtx = quat.toMatrix4f(centerOfRotation);
	T = rotMtx * T;

	//rotMtx.transpose3();	
	//float tmp1 = rotMtx[3][0]; float tmp2 = rotMtx[3][1]; float tmp3 = rotMtx[3][2];
	//rotMtx[3][0] = -(tmp1 * rotMtx[0][0] + tmp2 * rotMtx[1][0] + tmp3 * rotMtx[2][0]);
	//rotMtx[3][1] = -(tmp1 * rotMtx[0][1] + tmp2 * rotMtx[1][1] + tmp3 * rotMtx[2][1]);
	//rotMtx[3][2] = -(tmp1 * rotMtx[0][2] + tmp2 * rotMtx[1][2] + tmp3 * rotMtx[2][2]);
	//rotMtx[0][3] = 0.0f; rotMtx[1][3] = 0.0f; rotMtx[2][3] = 0.0f; rotMtx[3][3] = 1.0f;

	//invT = invT * rotMtx;
}

void Transform::rotate(float pitch, float yaw, float roll, const Vector3f& centerOfRotation) {
	Matrix4f rotMtx;
	rotMtx.rotate(pitch, yaw, roll, centerOfRotation);
	T = rotMtx * T;

	//rotMtx.transpose3();	
	//float tmp1 = rotMtx[3][0]; float tmp2 = rotMtx[3][1]; float tmp3 = rotMtx[3][2];
	//rotMtx[3][0] = -(tmp1 * rotMtx[0][0] + tmp2 * rotMtx[1][0] + tmp3 * rotMtx[2][0]);
	//rotMtx[3][1] = -(tmp1 * rotMtx[0][1] + tmp2 * rotMtx[1][1] + tmp3 * rotMtx[2][1]);
	//rotMtx[3][2] = -(tmp1 * rotMtx[0][2] + tmp2 * rotMtx[1][2] + tmp3 * rotMtx[2][2]);
	//rotMtx[0][3] = 0.0f; rotMtx[1][3] = 0.0f; rotMtx[2][3] = 0.0f; rotMtx[3][3] = 1.0f;

	//invT = invT * rotMtx;
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

void Transform::translate(const Vector3f& trans) {
	T[0][0] = T[0][0] + T[0][3] * trans[0]; T[0][1] = T[0][1] + T[0][3] * trans[2]; T[0][2] = T[0][2] + T[0][3] * trans[1];
	T[1][0] = T[1][0] + T[1][3] * trans[0]; T[1][1] = T[1][1] + T[1][3] * trans[2]; T[1][2] = T[1][2] + T[1][3] * trans[1];
	T[2][0] = T[2][0] + T[2][3] * trans[0]; T[2][1] = T[2][1] + T[2][3] * trans[2]; T[2][2] = T[2][2] + T[2][3] * trans[1];
	T[3][0] = T[3][0] + trans[0]; T[3][1] = T[3][1] + trans[1]; T[3][2] = T[3][2] + trans[2];

	//invT[3][0] = invT[3][0] - trans[0]*invT[0][0] - trans[2]*invT[2][0] - trans[1]*invT[1][0];
	//invT[3][1] = invT[3][1] - trans[0]*invT[0][1] - trans[2]*invT[2][1] - trans[1]*invT[1][1];
	//invT[3][2] = invT[3][2] - trans[0]*invT[0][2] - trans[2]*invT[2][2] - trans[1]*invT[1][2];
}

void Transform::scale(float s) {
	scale(s, s, s);
}

void Transform::scale(float a, float b, float c) {
	
	T[0][0] = T[0][0] * a;  T[1][0] = T[1][0] * b; T[2][0] = T[2][0] * c;
	T[0][1] = T[0][1] * a;  T[1][1] = T[1][1] * b; T[2][1] = T[2][1] * c;
	T[0][2] = T[0][2] * a;  T[1][2] = T[1][2] * b; T[2][2] = T[2][2] * c;

	//if (a == 0) a = 1.0;
	//if (b == 0) b = 1.0;
	//if (c == 0) c = 1.0;

	//invT[0][0] = invT[0][0] * (1.0 / a); invT[0][1] = invT[0][1] * (1.0 / b); invT[0][2] = invT[0][2] * (1.0 / c);
	//invT[1][0] = invT[1][0] * (1.0 / a); invT[1][1] = invT[1][1] * (1.0 / b); invT[1][2] = invT[1][2] * (1.0 / c);
	//invT[2][0] = invT[2][0] * (1.0 / a); invT[2][1] = invT[2][1] * (1.0 / b); invT[2][2] = invT[2][2] * (1.0 / c);
	//invT[3][0] = invT[3][0] * (1.0 / a); invT[3][1] = invT[3][1] * (1.0 / b); invT[3][2] = invT[3][2] * (1.0 / c);
}

void Transform::scale(const Vector3f& scale) {
	T[0][0] = T[0][0] * scale[0];  T[1][0] = T[1][0] * scale[1]; T[2][0] = T[2][0] * scale[2];
	T[0][1] = T[0][1] * scale[0];  T[1][1] = T[1][1] * scale[1]; T[2][1] = T[2][1] * scale[2];
	T[0][2] = T[0][2] * scale[0];  T[1][2] = T[1][2] * scale[1]; T[2][2] = T[2][2] * scale[2];

	//if (scale[0] == 0) scale[0] = 1.0;
	//if (scale[1] == 0) scale[1] = 1.0;
	//if (scale[2] == 0) scale[2] = 1.0;

	//invT[0][0] = invT[0][0] * (1.0 / scale[0]); invT[0][1] = invT[0][1] * (1.0 / scale[1]); invT[0][2] = invT[0][2] * (1.0 / scale[2]);
	//invT[1][0] = invT[1][0] * (1.0 / scale[0]); invT[1][1] = invT[1][1] * (1.0 / scale[1]); invT[1][2] = invT[1][2] * (1.0 / scale[2]);
	//invT[2][0] = invT[2][0] * (1.0 / scale[0]); invT[2][1] = invT[2][1] * (1.0 / scale[1]); invT[2][2] = invT[2][2] * (1.0 / scale[2]);
	//invT[3][0] = invT[3][0] * (1.0 / scale[0]); invT[3][1] = invT[3][1] * (1.0 / scale[1]); invT[3][2] = invT[3][2] * (1.0 / scale[2]);
}

void Transform::scale(float s, const Vector3f& centerOfScale) {
	scale(s, s, s, centerOfScale);
}

void Transform::scale(float a, float b, float c, const Vector3f& centerOfScale) {
	//Matrix4f scale;
	//scale.scale(a, b, c, centerOfScale);
	//T =  T * scale;
	//T.print();

	T[3][0] = T[0][0] * centerOfScale[0] * (1.0f - a) + T[1][0] * centerOfScale[1] * (1.0f - b) + T[2][0] * centerOfScale[2] * (1.0f - c) + T[3][0];
	T[3][1] = T[0][1] * centerOfScale[0] * (1.0f - a) + T[1][1] * centerOfScale[1] * (1.0f - b) + T[2][1] * centerOfScale[2] * (1.0f - c) + T[3][1];
	T[3][2] = T[0][2] * centerOfScale[0] * (1.0f - a) + T[1][2] * centerOfScale[1] * (1.0f - b) + T[2][2] * centerOfScale[2] * (1.0f - c) + T[3][2];

	T[0][0] = T[0][0] * a;  T[1][0] = T[1][0] * b; T[2][0] = T[2][0] * c;
	T[0][1] = T[0][1] * a;  T[1][1] = T[1][1] * b; T[2][1] = T[2][1] * c;
	T[0][2] = T[0][2] * a;  T[1][2] = T[1][2] * b; T[2][2] = T[2][2] * c;

	//if (a == 0) a = 1.0;
	//if (b == 0) b = 1.0;
	//if (c == 0) c = 1.0;

	//invT[3][0] = invT[0][0] * centerOfScale[0] * (1.0f - a) + invT[1][0] * centerOfScale[1] * (1.0f - b) + invT[2][0] * centerOfScale[2] * (1.0f - c) + invT[3][0];
	//invT[3][1] = invT[0][1] * centerOfScale[0] * (1.0f - a) + invT[1][1] * centerOfScale[1] * (1.0f - b) + invT[2][1] * centerOfScale[2] * (1.0f - c) + invT[3][1];
	//invT[3][2] = invT[0][2] * centerOfScale[0] * (1.0f - a) + invT[1][2] * centerOfScale[1] * (1.0f - b) + invT[2][2] * centerOfScale[2] * (1.0f - c) + invT[3][2];
	/**invT[3][3] = invT[0][3] * centerOfScale[0] * (1.0f - a) + invT[1][3] * centerOfScale[1] * (1.0f - b) + invT[2][3] * centerOfScale[2] * (1.0f - c) + invT[3][3];*/

	//invT[0][0] = invT[0][0] * (1.0 / a); invT[0][1] = invT[0][1] * (1.0 / b); invT[0][2] = invT[0][2] * (1.0 / c);
	//invT[1][0] = invT[1][0] * (1.0 / a); invT[1][1] = invT[1][1] * (1.0 / b); invT[1][2] = invT[1][2] * (1.0 / c);
	//invT[2][0] = invT[2][0] * (1.0 / a); invT[2][1] = invT[2][1] * (1.0 / b); invT[2][2] = invT[2][2] * (1.0 / c);
	//invT[3][0] = invT[3][0] * (1.0 / a); invT[3][1] = invT[3][1] * (1.0 / b); invT[3][2] = invT[3][2] * (1.0 / c);
}

void Transform::scale(const Vector3f& scale, const Vector3f& centerOfScale) {
	T[3][0] = T[0][0] * centerOfScale[0] * (1.0f - scale[0]) + T[1][0] * centerOfScale[1] * (1.0f - scale[1]) + T[2][0] * centerOfScale[2] * (1.0f - scale[2]) + T[3][0];
	T[3][1] = T[0][1] * centerOfScale[0] * (1.0f - scale[0]) + T[1][1] * centerOfScale[1] * (1.0f - scale[1]) + T[2][1] * centerOfScale[2] * (1.0f - scale[2]) + T[3][1];
	T[3][2] = T[0][2] * centerOfScale[0] * (1.0f - scale[0]) + T[1][2] * centerOfScale[1] * (1.0f - scale[1]) + T[2][2] * centerOfScale[2] * (1.0f - scale[2]) + T[3][2];

	T[0][0] = T[0][0] * scale[0];  T[1][0] = T[1][0] * scale[1]; T[2][0] = T[2][0] * scale[2];
	T[0][1] = T[0][1] * scale[0];  T[1][1] = T[1][1] * scale[1]; T[2][1] = T[2][1] * scale[2];
	T[0][2] = T[0][2] * scale[0];  T[1][2] = T[1][2] * scale[1]; T[2][2] = T[2][2] * scale[2];

	//if (scale[0] == 0) scale[0] = 1.0;
	//if (scale[1] == 0) scale[1] = 1.0;
	//if (scale[2] == 0) scale[2] = 1.0;

	//invT[3][0] = invT[0][0] * centerOfScale[0] * (1.0f - scale[0]) + invT[1][0] * centerOfScale[1] * (1.0f - scale[1]) + invT[2][0] * centerOfScale[2] * (1.0f - scale[2]) + invT[3][0];
	//invT[3][1] = invT[0][1] * centerOfScale[0] * (1.0f - scale[0]) + invT[1][1] * centerOfScale[1] * (1.0f - scale[1]) + invT[2][1] * centerOfScale[2] * (1.0f - scale[2]) + invT[3][1];
	//invT[3][2] = invT[0][2] * centerOfScale[0] * (1.0f - scale[0]) + invT[1][2] * centerOfScale[1] * (1.0f - scale[1]) + invT[2][2] * centerOfScale[2] * (1.0f - scale[2]) + invT[3][2];
	/**invT[3][3] = invT[0][3] * centerOfScale[0] * (1.0f - scale[0]) + invT[1][3] * centerOfScale[1] * (1.0f - scale[1]) + invT[2][3] * centerOfScale[2] * (1.0f - scale[2]) + invT[3][3];*/

	//invT[0][0] = invT[0][0] * (1.0 / scale[0]); invT[0][1] = invT[0][1] * (1.0 / scale[1]); invT[0][2] = invT[0][2] * (1.0 / scale[2]);
	//invT[1][0] = invT[1][0] * (1.0 / scale[0]); invT[1][1] = invT[1][1] * (1.0 / scale[1]); invT[1][2] = invT[1][2] * (1.0 / scale[2]);
	//invT[2][0] = invT[2][0] * (1.0 / scale[0]); invT[2][1] = invT[2][1] * (1.0 / scale[1]); invT[2][2] = invT[2][2] * (1.0 / scale[2]);
	//invT[3][0] = invT[3][0] * (1.0 / scale[0]); invT[3][1] = invT[3][1] * (1.0 / scale[1]); invT[3][2] = invT[3][2] * (1.0 / scale[2]);
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

void Transform::setPosition(float x, float y, float z) {
	T[3][0] = x; T[3][1] = y; T[3][2] = z;
}

void Transform::setPosition(const Vector3f& position) {
	T[3][0] = position[0]; T[3][1] = position[1]; T[3][2] = position[2];
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
	rot[3][0] = 0.0f;				   rot[3][1] = 0.0f;				  rot[3][2] = 0.0f;					 rot[3][3] = 1.0f;

	orientation.fromMatrix(rot);
}

void Transform::fromMatrix(const Matrix4f& m) {

	T[0][0] = m[0][0]; T[0][1] = m[0][1]; T[0][2] = m[0][2]; T[0][3] = m[0][3];
	T[1][0] = m[1][0]; T[1][1] = m[1][1]; T[1][2] = m[1][2]; T[1][3] = m[1][3];
	T[2][0] = m[2][0]; T[2][1] = m[2][1]; T[2][2] = m[2][2]; T[2][3] = m[2][3];
	T[3][0] = m[3][0]; T[3][1] = m[3][1]; T[3][2] = m[3][2]; T[3][3] = m[3][3];
	
	//float sx = Vector3f(T[0][0], T[1][0], T[2][0]).length();
	//float sy = Vector3f(T[0][1], T[1][1], T[2][1]).length();
	//float sz = Vector3f(T[0][2], T[1][2], T[2][2]).length();

	//float sxx = sx * sx;
	//float sxy = sx * sy;
	//float sxz = sx * sz;

	//float syy = sy * sy;
	//float syz = sy * sz;

	//float szz = sz * sz;

	//invT[0][0] = T[0][0] * (1.0f / sxx); invT[1][0] = T[0][1] * (1.0f / sxy); invT[2][0] = T[0][2] * (1.0f / sxz); invT[3][0] = -(T[3][0] * T[0][0] * (1.0f / sxx) + T[3][1] * T[0][1] * (1.0f / sxy) + T[3][2] * T[0][2] * (1.0f / sxz));
	//invT[0][1] = T[1][0] * (1.0f / sxy); invT[1][1] = T[1][1] * (1.0f / syy); invT[2][1] = T[1][2] * (1.0f / syz); invT[3][1] = -(T[3][0] * T[1][0] * (1.0f / sxy) + T[3][1] * T[1][1] * (1.0f / syy) + T[3][2] * T[1][2] * (1.0f / syz));
	//invT[0][2] = T[2][0] * (1.0f / sxz); invT[1][2] = T[2][1] * (1.0f / syz); invT[2][2] = T[2][2] * (1.0f / szz); invT[3][2] = -(T[3][0] * T[2][0] * (1.0f / sxz) + T[3][1] * T[2][1] * (1.0f / syz) + T[3][2] * T[2][2] * (1.0f / szz));
	//invT[0][3] = 0.0f;					 invT[1][3] = 0.0f;					  invT[2][3] = 0.0f;				   invT[3][3] = 1.0f;
	
}

