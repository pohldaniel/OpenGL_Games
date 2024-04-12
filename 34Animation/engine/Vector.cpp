#include <iostream>
#include "vector.h"

const Matrix4f Matrix4f::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

const Matrix4f Matrix4f::BIAS(0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f);

const Matrix4f Matrix4f::SIGN(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

Matrix4f::Matrix4f() {
	identity();
}

Matrix4f::Matrix4f(const Matrix4f& rhs) {
	mtx[0][0] = rhs[0][0]; mtx[0][1] = rhs[0][1]; mtx[0][2] = rhs[0][2]; mtx[0][3] = rhs[0][3];
	mtx[1][0] = rhs[1][0]; mtx[1][1] = rhs[1][1]; mtx[1][2] = rhs[1][2]; mtx[1][3] = rhs[1][3];
	mtx[2][0] = rhs[2][0]; mtx[2][1] = rhs[2][1]; mtx[2][2] = rhs[2][2]; mtx[2][3] = rhs[2][3];
	mtx[3][0] = rhs[3][0]; mtx[3][1] = rhs[3][1]; mtx[3][2] = rhs[3][2]; mtx[3][3] = rhs[3][3];
}

Matrix4f::Matrix4f(Matrix4f&& rhs) {
	mtx[0][0] = rhs[0][0]; mtx[0][1] = rhs[0][1]; mtx[0][2] = rhs[0][2]; mtx[0][3] = rhs[0][3];
	mtx[1][0] = rhs[1][0]; mtx[1][1] = rhs[1][1]; mtx[1][2] = rhs[1][2]; mtx[1][3] = rhs[1][3];
	mtx[2][0] = rhs[2][0]; mtx[2][1] = rhs[2][1]; mtx[2][2] = rhs[2][2]; mtx[2][3] = rhs[2][3];
	mtx[3][0] = rhs[3][0]; mtx[3][1] = rhs[3][1]; mtx[3][2] = rhs[3][2]; mtx[3][3] = rhs[3][3];
}

void Matrix4f::invRotate(const Vector3f &axis, float degrees) {

	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (y * x) * (1.0f - c) - (z * s);
	mtx[0][2] = (z * x) * (1.0f - c) + (y * s);
	mtx[0][3] = 0.0f;

	mtx[1][0] = (x * y) * (1.0f - c) + (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (z * y) * (1.0f - c) - (x * s);
	mtx[1][3] = 0.0f;

	mtx[2][0] = (x * z) * (1.0f - c) - (y * s);
	mtx[2][1] = (y * z) * (1.0f - c) + (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invRotate(const Vector3f &axis, float degrees, const Vector3f &centerOfRotation) {
	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (y * x) * (1.0f - c) - (z * s);
	mtx[0][2] = (z * x) * (1.0f - c) + (y * s);
	mtx[0][3] = 0.0f;

	mtx[1][0] = (x * y) * (1.0f - c) + (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (z * y) * (1.0f - c) - (x * s);
	mtx[1][3] = 0.0f;

	mtx[2][0] = (x * z) * (1.0f - c) - (y * s);
	mtx[2][1] = (y * z) * (1.0f - c) + (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[2][3] = 0.0f;

	mtx[3][0] = centerOfRotation[0] * (1.0f - mtx[0][0]) - centerOfRotation[1] * mtx[1][0] - centerOfRotation[2] * mtx[2][0];
	mtx[3][1] = centerOfRotation[1] * (1.0f - mtx[1][1]) - centerOfRotation[0] * mtx[0][1] - centerOfRotation[2] * mtx[2][1];
	mtx[3][2] = centerOfRotation[2] * (1.0f - mtx[2][2]) - centerOfRotation[0] * mtx[0][2] - centerOfRotation[1] * mtx[1][2];
	mtx[3][3] = 1.0f;
}

void Matrix4f::rotate(const Vector3f &axis, float degrees) {

	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (x * y) * (1.0f - c) + (z * s);
	mtx[0][2] = (x * z) * (1.0f - c) - (y * s);
	mtx[0][3] = 0.0f;

	mtx[1][0] = (y * x) * (1.0f - c) - (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (y * z) * (1.0f - c) + (x * s);
	mtx[1][3] = 0.0f;

	mtx[2][0] = (z * x) * (1.0f - c) + (y * s);
	mtx[2][1] = (z * y) * (1.0f - c) - (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::rotate(const Quaternion &orientation) {

	float x2 = orientation[0] + orientation[0];
	float y2 = orientation[1] + orientation[1];
	float z2 = orientation[2] + orientation[2];
	float xx = orientation[0] * x2;
	float xy = orientation[0] * y2;
	float xz = orientation[0] * z2;
	float yy = orientation[1] * y2;
	float yz = orientation[1] * z2;
	float zz = orientation[2] * z2;
	float wx = orientation[3] * x2;
	float wy = orientation[3] * y2;
	float wz = orientation[3] * z2;

	mtx[0][0] = 1.0f - (yy + zz);
	mtx[0][1] = xy + wz;
	mtx[0][2] = xz - wy;
	mtx[0][3] = 0.0f;

	mtx[1][0] = xy - wz;
	mtx[1][1] = 1.0f - (xx + zz);
	mtx[1][2] = yz + wx;
	mtx[1][3] = 0.0f;

	mtx[2][0] = xz + wy;
	mtx[2][1] = yz - wx;
	mtx[2][2] = 1.0f - (xx + yy);
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::rotate(float pitch, float yaw, float roll) {
	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	mtx[0][0] = cosR * cosY - sinR * sinP * sinY;
	mtx[0][1] = sinR * cosY + cosR * sinP * sinY;
	mtx[0][2] = -cosP * sinY;
	mtx[0][3] = 0.0f;

	mtx[1][0] = -sinR * cosP;
	mtx[1][1] = cosR * cosP;
	mtx[1][2] = sinP;
	mtx[1][3] = 0.0f;

	mtx[2][0] = cosR * sinY + sinR * sinP * cosY;
	mtx[2][1] = sinR * sinY - cosR * sinP * cosY;
	mtx[2][2] = cosP * cosY;
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::rotate(const Vector3f &axis, float degrees, const Vector3f &centerOfRotation) {

	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (x * y) * (1.0f - c) + (z * s);
	mtx[0][2] = (x * z) * (1.0f - c) - (y * s);
	mtx[0][3] = 0.0f;

	mtx[1][0] = (y * x) * (1.0f - c) - (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (y * z) * (1.0f - c) + (x * s);
	mtx[1][3] = 0.0f;

	mtx[2][0] = (z * x) * (1.0f - c) + (y * s);
	mtx[2][1] = (z * y) * (1.0f - c) - (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[2][3] = 0.0f;

	mtx[3][0] = centerOfRotation[0] * (1.0f - mtx[0][0]) - centerOfRotation[1] * mtx[1][0] - centerOfRotation[2] * mtx[2][0];
	mtx[3][1] = centerOfRotation[1] * (1.0f - mtx[1][1]) - centerOfRotation[0] * mtx[0][1] - centerOfRotation[2] * mtx[2][1];
	mtx[3][2] = centerOfRotation[2] * (1.0f - mtx[2][2]) - centerOfRotation[0] * mtx[0][2] - centerOfRotation[1] * mtx[1][2];
	mtx[3][3] = 1.0f;
}

void Matrix4f::rotate(const Quaternion &orientation, const Vector3f &centerOfRotation) {
	float x2 = orientation[0] + orientation[0];
	float y2 = orientation[1] + orientation[1];
	float z2 = orientation[2] + orientation[2];
	float xx = orientation[0] * x2;
	float xy = orientation[0] * y2;
	float xz = orientation[0] * z2;
	float yy = orientation[1] * y2;
	float yz = orientation[1] * z2;
	float zz = orientation[2] * z2;
	float wx = orientation[3] * x2;
	float wy = orientation[3] * y2;
	float wz = orientation[3] * z2;

	mtx[0][0] = 1.0f - (yy + zz);
	mtx[0][1] = xy + wz;
	mtx[0][2] = xz - wy;
	mtx[0][3] = 0.0f;

	mtx[1][0] = xy - wz;
	mtx[1][1] = 1.0f - (xx + zz);
	mtx[1][2] = yz + wx;
	mtx[1][3] = 0.0f;

	mtx[2][0] = xz + wy;
	mtx[2][1] = yz - wx;
	mtx[2][2] = 1.0f - (xx + yy);
	mtx[2][3] = 0.0f;

	mtx[3][0] = centerOfRotation[0] * (1.0f - mtx[0][0]) - centerOfRotation[1] * mtx[1][0] - centerOfRotation[2] * mtx[2][0];
	mtx[3][1] = centerOfRotation[1] * (1.0f - mtx[1][1]) - centerOfRotation[0] * mtx[0][1] - centerOfRotation[2] * mtx[2][1];
	mtx[3][2] = centerOfRotation[2] * (1.0f - mtx[2][2]) - centerOfRotation[0] * mtx[0][2] - centerOfRotation[1] * mtx[1][2];
	mtx[3][3] = 1.0f;
}

void Matrix4f::rotate(float pitch, float yaw, float roll, const Vector3f &centerOfRotation) {
	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	mtx[0][0] = cosR * cosY - sinR * sinP * sinY;
	mtx[0][1] = sinR * cosY + cosR * sinP * sinY;
	mtx[0][2] = -cosP * sinY;
	mtx[0][3] = 0.0f;

	mtx[1][0] = -sinR * cosP;
	mtx[1][1] = cosR * cosP;
	mtx[1][2] = sinP;
	mtx[1][3] = 0.0f;

	mtx[2][0] = cosR * sinY + sinR * sinP * cosY;
	mtx[2][1] = sinR * sinY - cosR * sinP * cosY;
	mtx[2][2] = cosP * cosY;
	mtx[2][3] = 0.0f;

	mtx[3][0] = centerOfRotation[0] * (1.0f - mtx[0][0]) - centerOfRotation[1] * mtx[1][0] - centerOfRotation[2] * mtx[2][0];
	mtx[3][1] = centerOfRotation[1] * (1.0f - mtx[1][1]) - centerOfRotation[0] * mtx[0][1] - centerOfRotation[2] * mtx[2][1];
	mtx[3][2] = centerOfRotation[2] * (1.0f - mtx[2][2]) - centerOfRotation[0] * mtx[0][2] - centerOfRotation[1] * mtx[1][2];
	mtx[3][3] = 1.0f;
}

void Matrix4f::invRotate(const Quaternion &orientation) {
	float x2 = orientation[0] + orientation[0];
	float y2 = orientation[1] + orientation[1];
	float z2 = orientation[2] + orientation[2];
	float xx = orientation[0] * x2;
	float xy = orientation[0] * y2;
	float xz = orientation[0] * z2;
	float yy = orientation[1] * y2;
	float yz = orientation[1] * z2;
	float zz = orientation[2] * z2;
	float wx = orientation[3] * x2;
	float wy = orientation[3] * y2;
	float wz = orientation[3] * z2;

	mtx[0][0] = 1.0f - (yy + zz);
	mtx[0][1] = xy - wz;
	mtx[0][2] = xz + wy;
	mtx[0][3] = 0.0f;

	mtx[1][0] = xy + wz;
	mtx[1][1] = 1.0f - (xx + zz);
	mtx[1][2] = yz - wx;
	mtx[1][3] = 0.0f;

	mtx[2][0] = xz - wy;
	mtx[2][1] = yz + wx;
	mtx[2][2] = 1.0f - (xx + yy);
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invRotate(const Quaternion &orientation, const Vector3f &centerOfRotation) {
	float x2 = orientation[0] + orientation[0];
	float y2 = orientation[1] + orientation[1];
	float z2 = orientation[2] + orientation[2];
	float xx = orientation[0] * x2;
	float xy = orientation[0] * y2;
	float xz = orientation[0] * z2;
	float yy = orientation[1] * y2;
	float yz = orientation[1] * z2;
	float zz = orientation[2] * z2;
	float wx = orientation[3] * x2;
	float wy = orientation[3] * y2;
	float wz = orientation[3] * z2;

	mtx[0][0] = 1.0f - (yy + zz);
	mtx[0][1] = xy - wz;
	mtx[0][2] = xz + wy;
	mtx[0][3] = 0.0f;

	mtx[1][0] = xy + wz;
	mtx[1][1] = 1.0f - (xx + zz);
	mtx[1][2] = yz - wx;
	mtx[1][3] = 0.0f;

	mtx[2][0] = xz - wy;
	mtx[2][1] = yz + wx;
	mtx[2][2] = 1.0f - (xx + yy);
	mtx[2][3] = 0.0f;

	mtx[3][0] = centerOfRotation[0] * (1.0f - mtx[0][0]) - centerOfRotation[1] * mtx[1][0] - centerOfRotation[2] * mtx[2][0];
	mtx[3][1] = centerOfRotation[1] * (1.0f - mtx[1][1]) - centerOfRotation[0] * mtx[0][1] - centerOfRotation[2] * mtx[2][1];
	mtx[3][2] = centerOfRotation[2] * (1.0f - mtx[2][2]) - centerOfRotation[0] * mtx[0][2] - centerOfRotation[1] * mtx[1][2];
	mtx[3][3] = 1.0f;
}

void Matrix4f::invRotate(float pitch, float yaw, float roll) {
	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	mtx[0][0] = cosR * cosY - sinR * sinP * sinY;
	mtx[1][0] = -sinR * cosP;
	mtx[2][0] = cosR * sinY + sinR * sinP * cosY;
	mtx[3][0] = 0.0f;

	mtx[0][1] = sinR * cosY + cosR * sinP * sinY;
	mtx[1][1] = cosR * cosP;
	mtx[2][1] = sinR * sinY - cosR * sinP * cosY;
	mtx[3][1] = 0.0f;

	mtx[0][2] = -cosP * sinY;
	mtx[1][2] = sinP;
	mtx[2][2] = cosP * cosY;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invRotate(float pitch, float yaw, float roll, const Vector3f &centerOfRotation) {
	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	mtx[0][0] = cosR * cosY - sinR * sinP * sinY;
	mtx[1][0] = -sinR * cosP;
	mtx[2][0] = cosR * sinY + sinR * sinP * cosY;
	mtx[3][0] = 0.0f;

	mtx[0][1] = sinR * cosY + cosR * sinP * sinY;
	mtx[1][1] = cosR * cosP;
	mtx[2][1] = sinR * sinY - cosR * sinP * cosY;
	mtx[3][1] = 0.0f;

	mtx[0][2] = -cosP * sinY;
	mtx[1][2] = sinP;
	mtx[2][2] = cosP * cosY;
	mtx[3][2] = 0.0f;

	mtx[3][0] = centerOfRotation[0] * (1.0f - mtx[0][0]) - centerOfRotation[1] * mtx[1][0] - centerOfRotation[2] * mtx[2][0];
	mtx[3][1] = centerOfRotation[1] * (1.0f - mtx[1][1]) - centerOfRotation[0] * mtx[0][1] - centerOfRotation[2] * mtx[2][1];
	mtx[3][2] = centerOfRotation[2] * (1.0f - mtx[2][2]) - centerOfRotation[0] * mtx[0][2] - centerOfRotation[1] * mtx[1][2];
	mtx[3][3] = 1.0f;
}

void Matrix4f::translate(float dx, float dy, float dz) {

	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = dx;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = dy;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = dz;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::translate(const Vector3f &trans) {

	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = trans[0];

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = trans[1];

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = trans[2];

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invTranslate(float dx, float dy, float dz) {

	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = -dx;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = -dy;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = -dz;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invTranslate(const Vector3f &trans) {

	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = -trans[0];

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = -trans[1];

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = -trans[2];

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::scale(float a, float b, float c) {
	//if (a == 0) a = 1.0;
	//if (b == 0) b = 1.0;
	//if (c == 0) c = 1.0;

	mtx[0][0] = 1.0f * a;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f * b;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f * c;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::scale(float a, float b, float c, const Vector3f &centerOfScale) {
	//if (a == 0) a = 1.0;
	//if (b == 0) b = 1.0;
	//if (c == 0) c = 1.0;

	mtx[0][0] = 1.0f * a;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = centerOfScale[0] * (1.0f - a);

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f * b;
	mtx[2][1] = 0.0f;
	mtx[3][1] = centerOfScale[1] * (1.0f - b);

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f * c;
	mtx[3][2] = centerOfScale[2] * (1.0f - c);
}

void Matrix4f::scale(const Vector3f &scale) {
	mtx[0][0] = 1.0f * scale[0];
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f * scale[1];
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f * scale[2];
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::scale(const Vector3f &scale, const Vector3f &centerOfScale) {
	mtx[0][0] = 1.0f * scale[0];
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = centerOfScale[0] * (1.0f - scale[0]);

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f * scale[1];
	mtx[2][1] = 0.0f;
	mtx[3][1] = centerOfScale[1] * (1.0f - scale[1]);

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f * scale[2];
	mtx[3][2] = centerOfScale[2] * (1.0f - scale[2]);
}

void Matrix4f::invScale(float a, float b, float c) {

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	mtx[0][0] = 1.0f / a;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f / b;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f / c;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invScale(float a, float b, float c, const Vector3f &centerOfScale) {

	if (a == 0) a = 1.0;
	if (b == 0) b = 1.0;
	if (c == 0) c = 1.0;

	mtx[0][0] = 1.0f / a;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = centerOfScale[0] * (1.0f - a);

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f / b;
	mtx[2][1] = 0.0f;
	mtx[3][1] = centerOfScale[1] * (1.0f - b);

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f / c;
	mtx[3][2] = centerOfScale[2] * (1.0f - c);

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invScale(const Vector3f &scale) {
	mtx[0][0] = 1.0f / scale[0];
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f / scale[1];
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f / scale[2];
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

void Matrix4f::invScale(const Vector3f &scale, const Vector3f &centerOfScale) {
	mtx[0][0] = 1.0f / scale[0];
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = centerOfScale[0] * (1.0f - scale[0]);

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f / scale[1];
	mtx[2][1] = 0.0f;
	mtx[3][1] = centerOfScale[1] * (1.0f - scale[1]);

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f / scale[2];
	mtx[3][2] = centerOfScale[2] * (1.0f - scale[2]);

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;
}

Vector3f Matrix4f::getTranslation() const {
	return Vector3f(mtx[3][0], mtx[3][1], mtx[3][2]);
}

Vector3f Matrix4f::getScale() const {
	return Vector3f(
		sqrtf(mtx[0][0] * mtx[0][0] + mtx[0][1] * mtx[0][1] + mtx[0][2] * mtx[0][2]),
		sqrtf(mtx[1][0] * mtx[1][0] + mtx[1][1] * mtx[1][1] + mtx[1][2] * mtx[1][2]),
		sqrtf(mtx[2][0] * mtx[2][0] + mtx[2][1] * mtx[2][1] + mtx[2][2] * mtx[2][2])
	);
}

Matrix4f Matrix4f::getRotation() const {
	float scaleX = 1.0f / sqrtf(mtx[0][0] * mtx[0][0] + mtx[0][1] * mtx[0][1] + mtx[0][2] * mtx[0][2]);
	float scaleY = 1.0f / sqrtf(mtx[1][0] * mtx[1][0] + mtx[1][1] * mtx[1][1] + mtx[1][2] * mtx[1][2]);
	float scaleZ = 1.0f / sqrtf(mtx[2][0] * mtx[2][0] + mtx[2][1] * mtx[2][1] + mtx[2][2] * mtx[2][2]);

	return Matrix4f(mtx[0][0] * scaleX, mtx[1][0] * scaleX, mtx[2][0] * scaleX, 0.0f,
		            mtx[0][1] * scaleY, mtx[1][1] * scaleY, mtx[2][1] * scaleY, 0.0f,
		            mtx[0][2] * scaleZ, mtx[1][2] * scaleZ, mtx[2][2] * scaleZ, 0.0f,
		            0.0f, 0.0f, 0.0f, 1.0f);
}

void Matrix4f::lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up) {

	Vector3f zAxis = eye - target;
	Vector3f::Normalize(zAxis);

	Vector3f xAxis = Vector3f::Cross(up, zAxis);
	Vector3f::Normalize(xAxis);

	Vector3f yAxis = Vector3f::Cross(zAxis, xAxis);
	Vector3f::Normalize(yAxis);

	mtx[0][0] = xAxis[0];
	mtx[0][1] = yAxis[0];
	mtx[0][2] = zAxis[0];
	mtx[0][3] = 0.0f;

	mtx[1][0] = xAxis[1];
	mtx[1][1] = yAxis[1];
	mtx[1][2] = zAxis[1];
	mtx[1][3] = 0.0f;

	mtx[2][0] = xAxis[2];
	mtx[2][1] = yAxis[2];
	mtx[2][2] = zAxis[2];
	mtx[2][3] = 0.0f;

	mtx[3][0] = -Vector3f::Dot(xAxis, eye);
	mtx[3][1] = -Vector3f::Dot(yAxis, eye);
	mtx[3][2] = -Vector3f::Dot(zAxis, eye);
	mtx[3][3] = 1.0f;
}

void Matrix4f::invLookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up) {

	Vector3f zAxis = eye - target;
	Vector3f::Normalize(zAxis);

	Vector3f xAxis = Vector3f::Cross(up, zAxis);
	Vector3f::Normalize(xAxis);

	Vector3f yAxis = Vector3f::Cross(zAxis, xAxis);
	Vector3f::Normalize(yAxis);

	mtx[0][0] = xAxis[0];
	mtx[0][1] = xAxis[1];
	mtx[0][2] = xAxis[2];
	mtx[0][3] = 0.0f;

	mtx[1][0] = yAxis[0];
	mtx[1][1] = yAxis[1];
	mtx[1][2] = yAxis[2];
	mtx[1][3] = 0.0f;


	mtx[2][0] = zAxis[0];
	mtx[2][1] = zAxis[1];
	mtx[2][2] = zAxis[2];
	mtx[2][3] = 0.0f;

	mtx[3][0] = eye[0];
	mtx[3][1] = eye[1];
	mtx[3][2] = eye[2];
	mtx[3][3] = 1.0f;
}

void Matrix4f::perspective(float fovx, float aspect, float znear, float zfar) {

	float e = tanf(PI_ON_180 * fovx * 0.5f);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	mtx[0][0] = xScale;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = yScale;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = (zfar + znear) / (znear - zfar);
	mtx[2][3] = -1.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = (2.0f * zfar * znear) / (znear - zfar);
	mtx[3][3] = 0.0f;
}

void Matrix4f::perspectiveD3D(float fovx, float aspect, float znear, float zfar) {

	float e = tanf(PI_ON_180 * fovx * 0.5f);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	mtx[0][0] = xScale;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = yScale;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = zfar / (znear - zfar);
	mtx[2][3] = -1.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = (zfar * znear) / (znear - zfar);
	mtx[3][3] = 0.0f;
}

void Matrix4f::linearPerspectiveD3D(float fovx, float aspect, float znear, float zfar) {

	float e = tanf(PI_ON_180 * fovx * 0.5f);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	//float Q = zfar / (zfar - znear);
	//float near = ((zfar * znear) / (znear - zfar)) / (zfar / (znear - zfar));
	//float far = -N * Q / (1 - Q);

	mtx[0][0] = xScale;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = yScale;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = 1 / (znear - zfar);
	mtx[2][3] = -1.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = znear / (znear - zfar);
	mtx[3][3] = 0.0f;
}

void Matrix4f::invPerspective(float fovx, float aspect, float znear, float zfar) {

	float e = tanf(PI_ON_180 * fovx * 0.5f);

	mtx[0][0] = e * aspect;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = e;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = 0.0;
	mtx[2][3] = (znear - zfar) / (2 * zfar * znear);

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = -1.0f;
	mtx[3][3] = (znear + zfar) / (2 * zfar * znear);
}

void Matrix4f::orthographic(float left, float right, float bottom, float top, float znear, float zfar) {

	mtx[0][0] = 2 / (right - left);
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = 2 / (top - bottom);
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = 2 / (znear - zfar);
	mtx[2][3] = 0.0f;

	mtx[3][0] = (right + left) / (left - right);
	mtx[3][1] = (top + bottom) / (bottom - top);
	mtx[3][2] = (zfar + znear) / (znear - zfar);
	mtx[3][3] = 1.0f;
}

void Matrix4f::invOrthographic(float left, float right, float bottom, float top, float znear, float zfar) {

	mtx[0][0] = (right - left) * 0.5f;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = (top - bottom) * 0.5f;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = (znear - zfar) * 0.5f;
	mtx[2][3] = 0.0f;

	mtx[3][0] = (right + left) * 0.5f;
	mtx[3][1] = (top + bottom) * 0.5f;
	mtx[3][2] = -(zfar + znear) * 0.5f;
	mtx[3][3] = 1.0f;
}

Matrix4f::Matrix4f(float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44) {

	mtx[0][0] = m11, mtx[0][1] = m12, mtx[0][2] = m13, mtx[0][3] = m14;
	mtx[1][0] = m21, mtx[1][1] = m22, mtx[1][2] = m23, mtx[1][3] = m24;
	mtx[2][0] = m31, mtx[2][1] = m32, mtx[2][2] = m33, mtx[2][3] = m34;
	mtx[3][0] = m41, mtx[3][1] = m42, mtx[3][2] = m43, mtx[3][3] = m44;
}

Matrix4f::Matrix4f(float array[16]) {
	mtx[0][0] = array[0], mtx[0][1] = array[1], mtx[0][2] = array[2], mtx[0][3] = array[3];
	mtx[1][0] = array[4], mtx[1][1] = array[5], mtx[1][2] = array[6], mtx[1][3] = array[7];
	mtx[2][0] = array[8], mtx[2][1] = array[9], mtx[2][2] = array[10], mtx[2][3] = array[11];
	mtx[3][0] = array[12], mtx[3][1] = array[13], mtx[3][2] = array[14], mtx[3][3] = array[15];
}

void Matrix4f::set(float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44) {

	mtx[0][0] = m11, mtx[0][1] = m12, mtx[0][2] = m13, mtx[0][3] = m14;
	mtx[1][0] = m21, mtx[1][1] = m22, mtx[1][2] = m23, mtx[1][3] = m24;
	mtx[2][0] = m31, mtx[2][1] = m32, mtx[2][2] = m33, mtx[2][3] = m34;
	mtx[3][0] = m41, mtx[3][1] = m42, mtx[3][2] = m43, mtx[3][3] = m44;
}

void Matrix4f::set(const Matrix4f &rhs) {
	mtx[0][0] = rhs[0][0], mtx[0][1] = rhs[0][1], mtx[0][2] = rhs[0][2], mtx[0][3] = rhs[0][3];
	mtx[1][0] = rhs[1][0], mtx[1][1] = rhs[1][1], mtx[1][2] = rhs[1][2], mtx[1][3] = rhs[1][3];
	mtx[2][0] = rhs[2][0], mtx[2][1] = rhs[2][1], mtx[2][2] = rhs[2][2], mtx[2][3] = rhs[2][3];
	mtx[3][0] = rhs[3][0], mtx[3][1] = rhs[3][1], mtx[3][2] = rhs[3][2], mtx[3][3] = rhs[3][3];
}

void Matrix4f::set(float array[16]) {
	mtx[0][0] = array[0], mtx[0][1] = array[1], mtx[0][2] = array[2], mtx[0][3] = array[3];
	mtx[1][0] = array[4], mtx[1][1] = array[5], mtx[1][2] = array[6], mtx[1][3] = array[7];
	mtx[2][0] = array[8], mtx[2][1] = array[9], mtx[2][2] = array[10], mtx[2][3] = array[11];
	mtx[3][0] = array[12], mtx[3][1] = array[13], mtx[3][2] = array[14], mtx[3][3] = array[15];
}

void Matrix4f::toHeadPitchRoll(float &pitch, float &yaw, float &roll) const {
	// Extracts the Euler angles from a rotation matrix. The returned
	// angles are in degrees. This method might suffer from numerical
	// imprecision for ill defined rotation matrices.
	//
	// This function only works for rotation matrices constructed using
	// the popular NASA standard airplane convention of heading-pitch-roll 
	// (i.e., RzRxRy).
	//
	// The algorithm used is from:
	//  David Eberly, "Euler Angle Formulas", Geometric Tools web site,
	//  http://www.geometrictools.com/Documentation/EulerAngles.pdf.

	float thetaX = asinf(mtx[1][2]);
	float thetaY = 0.0f;
	float thetaZ = 0.0f;

	if (thetaX < HALF_PI) {

		if (thetaX > -HALF_PI) {

			thetaZ = atan2f(-mtx[1][0], mtx[1][1]);
			thetaY = atan2f(-mtx[0][2], mtx[2][2]);

		}else {

			// Not a unique solution.
			thetaZ = -atan2f(mtx[2][0], mtx[0][0]);
			thetaY = 0.0f;
		}

	}else {

		// Not a unique solution.
		thetaZ = atan2f(mtx[2][0], mtx[0][0]);
		thetaY = 0.0f;
	}

	pitch = thetaX * _180_ON_PI;
	yaw = thetaY * _180_ON_PI;
	roll = thetaZ * _180_ON_PI;
}

Matrix4f Matrix4f::Translate(float dx, float dy, float dz) {
	return Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		dx, dy, dz, 1.0);
}

Matrix4f Matrix4f::Translate(const Vector3f &trans) {
	return Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		trans[0], trans[1], trans[2], 1.0);
}

Matrix4f &Matrix4f::Translate(Matrix4f &mtx, float dx, float dy, float dz) {
	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = dx;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = dy;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = dz;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f Matrix4f::InvTranslate(float dx, float dy, float dz) {
	return Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-dx, -dy, -dz, 1.0);
}

Matrix4f Matrix4f::InvTranslate(const Vector3f &trans) {
	return Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-trans[0], -trans[1], -trans[2], 1.0);
}

Matrix4f &Matrix4f::InvTranslate(Matrix4f &mtx, float dx, float dy, float dz) {
	mtx[0][0] = 1.0f;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = -dx;

	mtx[0][1] = 0.0f;
	mtx[1][1] = 1.0f;
	mtx[2][1] = 0.0f;
	mtx[3][1] = -dy;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = -dz;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f Matrix4f::Scale(float x, float y, float z) {

	//if (x == 0) x = 1.0;
	//if (y == 0) y = 1.0;
	//if (z == 0) z = 1.0;
	return Matrix4f(x, 0.0f, 0.0f, 0.0f,
		0.0f, y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0);
}

Matrix4f Matrix4f::Scale(const Vector3f &scale) {
	return Matrix4f(scale[0], 0.0f, 0.0f, 0.0f,
		0.0f, scale[1], 0.0f, 0.0f,
		0.0f, 0.0f, scale[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0);
}

Matrix4f &Matrix4f::Scale(Matrix4f &mtx, float x, float y, float z) {
	mtx[0][0] = x;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = 0.0f;

	mtx[0][1] = 0.0f;
	mtx[1][1] = y;
	mtx[2][1] = 0.0f;
	mtx[3][1] = 0.0f;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = z;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f Matrix4f::Scale(float x, float y, float z, const Vector3f &centerOfScale) {

	return Matrix4f(1.0f * x, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f * y, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f * z, 0.0f,
		centerOfScale[0] * (1.0f - x),
		centerOfScale[1] * (1.0f - y),
		centerOfScale[2] * (1.0f - z), 1.0f);
}

/*Matrix4f Matrix4f::Scale(const Vector3f& scale, const Vector3f& centerOfScale) {
	return Matrix4f(1.0f * scale[0], 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f * scale[1], 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f * scale[2], 0.0f,
		centerOfScale[0] * (1.0f - scale[0]),
		centerOfScale[1] * (1.0f - scale[1]),
		centerOfScale[2] * (1.0f - scale[2]), 1.0f);
}

Matrix4f Matrix4f::Scale(const Vector3f& scale, const Vector3f& a, const Vector3f& b) {
	
	return Matrix4f(1.0f * scale[0], 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f * scale[1], 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f * scale[2], 0.0f,
                    b[0]  + a[0], b[1]  + a[1], b[2]  + a[2], 1.0f);

}*/

Matrix4f Matrix4f::Scale(const Vector3f& scale, const Vector3f& centerOfScale) {
	return Matrix4f(1.0f * scale[0], 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f * scale[1], 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f * scale[2], 0.0f,
                    centerOfScale[0], centerOfScale[1], centerOfScale[2], 1.0f);
}

Matrix4f Matrix4f::InvRotate(const Vector3f &axis, float degrees) {
	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	return Matrix4f((x * x) * (1.0f - c) + c, (y * x) * (1.0f - c) - (z * s), (z * x) * (1.0f - c) + (y * s), 0.0f,
					(x * y) * (1.0f - c) + (z * s), (y * y) * (1.0f - c) + c, (y * z) * (1.0f - c) + (x * s), 0.0f,
					(x * z) * (1.0f - c) - (y * s), (y * z) * (1.0f - c) + (x * s), (z * z) * (1.0f - c) + c, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0);
}

Matrix4f Matrix4f::InvRotate(const Quaternion &orientation) {
	float x2 = orientation[0] + orientation[0];
	float y2 = orientation[1] + orientation[1];
	float z2 = orientation[2] + orientation[2];
	float xx = orientation[0] * x2;
	float xy = orientation[0] * y2;
	float xz = orientation[0] * z2;
	float yy = orientation[1] * y2;
	float yz = orientation[1] * z2;
	float zz = orientation[2] * z2;
	float wx = orientation[3] * x2;
	float wy = orientation[3] * y2;
	float wz = orientation[3] * z2;

	return Matrix4f(1.0f - (yy + zz), xy - wz, xz + wy, 0.0f,
					xy + wz, 1.0f - (xx + zz), yz - wx, 0.0f,
					xz - wy, yz + wx, 1.0f - (xx + yy), 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4f Matrix4f::InvRotate(float pitch, float yaw, float roll) {
	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	return Matrix4f(cosR * cosY - sinR * sinP * sinY, -sinR * cosP, cosR * sinY + sinR * sinP * cosY, 0.0f,
					sinR * cosY + cosR * sinP * sinY, cosR * cosP, sinR * sinY - cosR * sinP * cosY, 0.0f,
					-cosP * sinY, sinP, cosP * cosY, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4f Matrix4f::InvRotate(const Vector3f &axis, float degrees, const Vector3f &centerOfRotation) {
	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	float mtx00 = (x * x) * (1.0f - c) + c;
	float mtx01 = (y * x) * (1.0f - c) - (z * s);
	float mtx02 = (z * x) * (1.0f - c) + (y * s);

	float mtx10 = (x * y) * (1.0f - c) + (z * s);
	float mtx11 = (y * y) * (1.0f - c) + c;
	float mtx12 = (z * y) * (1.0f - c) - (x * s);

	float mtx20 = (x * z) * (1.0f - c) - (y * s);
	float mtx21 = (y * z) * (1.0f - c) + (x * s);
	float mtx22 = (z * z) * (1.0f - c) + c;

	return Matrix4f(mtx00, mtx01, mtx02, 0.0f,
					mtx10, mtx11, mtx12, 0.0f,
					mtx20, mtx21, mtx22, 0.0f,
					centerOfRotation[0] * (1.0f - mtx00) - centerOfRotation[1] * mtx10 - centerOfRotation[2] * mtx20,
					centerOfRotation[1] * (1.0f - mtx11) - centerOfRotation[0] * mtx01 - centerOfRotation[2] * mtx21,
					centerOfRotation[2] * (1.0f - mtx22) - centerOfRotation[0] * mtx02 - centerOfRotation[1] * mtx12, 1.0);
}

Matrix4f Matrix4f::InvRotate(const Quaternion &orientation, const Vector3f &centerOfRotation) {
	float x2 = orientation[0] + orientation[0];
	float y2 = orientation[1] + orientation[1];
	float z2 = orientation[2] + orientation[2];
	float xx = orientation[0] * x2;
	float xy = orientation[0] * y2;
	float xz = orientation[0] * z2;
	float yy = orientation[1] * y2;
	float yz = orientation[1] * z2;
	float zz = orientation[2] * z2;
	float wx = orientation[3] * x2;
	float wy = orientation[3] * y2;
	float wz = orientation[3] * z2;

	float mtx00 = 1.0f - (yy + zz);
	float mtx01 = xy - wz;
	float mtx02 = xz + wy;

	float mtx10 = xy + wz;
	float mtx11 = 1.0f - (xx + zz);
	float mtx12 = yz - wx;

	float mtx20 = xz - wy;
	float mtx21 = yz + wx;
	float mtx22 = 1.0f - (xx + yy);

	return Matrix4f(mtx00, mtx01, mtx02, 0.0f,
		mtx10, mtx11, mtx12, 0.0f,
		mtx20, mtx21, mtx22, 0.0f,
		centerOfRotation[0] * (1.0f - mtx00) - centerOfRotation[1] * mtx10 - centerOfRotation[2] * mtx20,
		centerOfRotation[1] * (1.0f - mtx11) - centerOfRotation[0] * mtx01 - centerOfRotation[2] * mtx21,
		centerOfRotation[2] * (1.0f - mtx22) - centerOfRotation[0] * mtx02 - centerOfRotation[1] * mtx12, 1.0);
}

Matrix4f Matrix4f::InvRotate(float pitch, float yaw, float roll, const Vector3f &centerOfRotation) {
	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	float mtx00 = cosR * cosY - sinR * sinP * sinY;
	float mtx01 = -sinR * cosP;
	float mtx02 = cosR * sinY + sinR * sinP * cosY;

	float mtx10 = sinR * cosY + cosR * sinP * sinY;
	float mtx11 = cosR * cosP;
	float mtx12 = sinR * sinY - cosR * sinP * cosY;

	float mtx20 = -cosP * sinY;
	float mtx21 = sinP;
	float mtx22 = cosP * cosY;

	return Matrix4f(mtx00, mtx01, mtx02, 0.0f,
					mtx10, mtx11, mtx12, 0.0f,
					mtx20, mtx21, mtx22, 0.0f,
					centerOfRotation[0] * (1.0f - mtx00) - centerOfRotation[1] * mtx10 - centerOfRotation[2] * mtx20,
					centerOfRotation[1] * (1.0f - mtx11) - centerOfRotation[0] * mtx01 - centerOfRotation[2] * mtx21,
					centerOfRotation[2] * (1.0f - mtx22) - centerOfRotation[0] * mtx02 - centerOfRotation[1] * mtx12, 1.0);
}


Matrix4f &Matrix4f::InvRotate(Matrix4f &mtx, const Vector3f &axis, float degrees) {
	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (y * x) * (1.0f - c) - (z * s);
	mtx[0][2] = (z * x) * (1.0f - c) + (y * s);
	mtx[0][3] = 0.0f;

	mtx[1][0] = (x * y) * (1.0f - c) + (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (z * y) * (1.0f - c) - (x * s);
	mtx[1][3] = 0.0f;

	mtx[2][0] = (x * z) * (1.0f - c) - (y * s);
	mtx[2][1] = (y * z) * (1.0f - c) + (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f &Matrix4f::InvRotate(Matrix4f &mtx, const Vector3f &axis, float degrees, const Vector3f &centerOfRotation) {
	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (y * x) * (1.0f - c) - (z * s);
	mtx[0][2] = (z * x) * (1.0f - c) + (y * s);

	mtx[1][0] = (x * y) * (1.0f - c) + (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (z * y) * (1.0f - c) - (x * s);

	mtx[2][0] = (x * z) * (1.0f - c) - (y * s);
	mtx[2][1] = (y * z) * (1.0f - c) + (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;

	mtx[3][0] = centerOfRotation[0] * (1.0f - mtx[0][0]) - centerOfRotation[1] * mtx[1][0] - centerOfRotation[2] * mtx[2][0];
	mtx[3][1] = centerOfRotation[1] * (1.0f - mtx[1][1]) - centerOfRotation[0] * mtx[0][1] - centerOfRotation[2] * mtx[2][1];
	mtx[3][2] = centerOfRotation[2] * (1.0f - mtx[2][2]) - centerOfRotation[0] * mtx[0][2] - centerOfRotation[1] * mtx[1][2];
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f Matrix4f::Rotate(const Vector3f& eulerAngles) {
	float pitch = eulerAngles[0] * PI_ON_180;
	float yaw = eulerAngles[1] * PI_ON_180;
	float roll = eulerAngles[2] * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	return Matrix4f(cosR * cosY - sinR * sinP * sinY, sinR * cosY + cosR * sinP * sinY, -cosP * sinY, 0.0f,
					-sinR * cosP, cosR * cosP, sinP, 0.0f,
					cosR * sinY + sinR * sinP * cosY, sinR * sinY - cosR * sinP * cosY, cosP * cosY, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4f Matrix4f::Rotate(const Vector3f& eulerAngles, const Vector3f& centerOfRotation) {
	float pitch = centerOfRotation[0] * PI_ON_180;
	float yaw = centerOfRotation[1] * PI_ON_180;
	float roll = centerOfRotation[2] * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	float mtx00 = cosR * cosY - sinR * sinP * sinY;
	float mtx01 = sinR * cosY + cosR * sinP * sinY;
	float mtx02 = -cosP * sinY;

	float mtx10 = -sinR * cosP;
	float mtx11 = cosR * cosP;
	float mtx12 = sinP;

	float mtx20 = cosR * sinY + sinR * sinP * cosY;
	float mtx21 = sinR * sinY - cosR * sinP * cosY;
	float mtx22 = cosP * cosY;

	return Matrix4f(mtx00, mtx01, mtx02, 0.0f,
		mtx10, mtx11, mtx12, 0.0f,
		mtx20, mtx21, mtx22, 0.0f,
		centerOfRotation[0] * (1.0f - mtx00) - centerOfRotation[1] * mtx10 - centerOfRotation[2] * mtx20,
		centerOfRotation[1] * (1.0f - mtx11) - centerOfRotation[0] * mtx01 - centerOfRotation[2] * mtx21,
		centerOfRotation[2] * (1.0f - mtx22) - centerOfRotation[0] * mtx02 - centerOfRotation[1] * mtx12, 1.0);
}

Matrix4f Matrix4f::Rotate(const Vector3f &axis, float degrees) {
	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	return Matrix4f((x * x) * (1.0f - c) + c, (x * y) * (1.0f - c) + (z * s), (x * z) * (1.0f - c) - (y * s), 0.0f,
		(y * x) * (1.0f - c) - (z * s), (y * y) * (1.0f - c) + c, (y * z) * (1.0f - c) + (x * s), 0.0f,
		(z * x) * (1.0f - c) + (y * s), (z * y) * (1.0f - c) - (x * s), (z * z) * (1.0f - c) + c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0);
}

Matrix4f Matrix4f::Rotate(const Quaternion &orientation) {
	float x2 = orientation[0] + orientation[0];
	float y2 = orientation[1] + orientation[1];
	float z2 = orientation[2] + orientation[2];
	float xx = orientation[0] * x2;
	float xy = orientation[0] * y2;
	float xz = orientation[0] * z2;
	float yy = orientation[1] * y2;
	float yz = orientation[1] * z2;
	float zz = orientation[2] * z2;
	float wx = orientation[3] * x2;
	float wy = orientation[3] * y2;
	float wz = orientation[3] * z2;

	return Matrix4f(1.0f - (yy + zz), xy + wz, xz - wy, 0.0f,
		xy - wz, 1.0f - (xx + zz), yz + wx, 0.0f,
		xz + wy, yz - wx, 1.0f - (xx + yy), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4f Matrix4f::Rotate(float pitch, float yaw, float roll) {

	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	return Matrix4f(cosR * cosY - sinR * sinP * sinY, sinR * cosY + cosR * sinP * sinY, -cosP * sinY, 0.0f,
		-sinR * cosP, cosR * cosP, sinP, 0.0f,
		cosR * sinY + sinR * sinP * cosY, sinR * sinY - cosR * sinP * cosY, cosP * cosY, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

}

Matrix4f Matrix4f::Rotate(const Vector3f &axis, float degrees, const Vector3f &centerOfRotation) {

	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	float mtx00 = (x * x) * (1.0f - c) + c;
	float mtx01 = (x * y) * (1.0f - c) + (z * s);
	float mtx02 = (x * z) * (1.0f - c) - (y * s);

	float mtx10 = (y * x) * (1.0f - c) - (z * s);
	float mtx11 = (y * y) * (1.0f - c) + c;
	float mtx12 = (y * z) * (1.0f - c) + (x * s);

	float mtx20 = (z * x) * (1.0f - c) + (y * s);
	float mtx21 = (z * y) * (1.0f - c) - (x * s);
	float mtx22 = (z * z) * (1.0f - c) + c;

	return Matrix4f(mtx00, mtx01, mtx02, 0.0f,
		mtx10, mtx11, mtx12, 0.0f,
		mtx20, mtx21, mtx22, 0.0f,
		centerOfRotation[0] * (1.0f - mtx00) - centerOfRotation[1] * mtx10 - centerOfRotation[2] * mtx20,
		centerOfRotation[1] * (1.0f - mtx11) - centerOfRotation[0] * mtx01 - centerOfRotation[2] * mtx21,
		centerOfRotation[2] * (1.0f - mtx22) - centerOfRotation[0] * mtx02 - centerOfRotation[1] * mtx12, 1.0);

}

Matrix4f Matrix4f::Rotate(const Quaternion &orientation, const Vector3f &centerOfRotation) {
	float x2 = orientation[0] + orientation[0];
	float y2 = orientation[1] + orientation[1];
	float z2 = orientation[2] + orientation[2];
	float xx = orientation[0] * x2;
	float xy = orientation[0] * y2;
	float xz = orientation[0] * z2;
	float yy = orientation[1] * y2;
	float yz = orientation[1] * z2;
	float zz = orientation[2] * z2;
	float wx = orientation[3] * x2;
	float wy = orientation[3] * y2;
	float wz = orientation[3] * z2;

	float mtx00 = 1.0f - (yy + zz);
	float mtx01 = xy + wz;
	float mtx02 = xz - wy;

	float mtx10 = xy - wz;
	float mtx11 = 1.0f - (xx + zz);
	float mtx12 = yz + wx;

	float mtx20 = xz + wy;
	float mtx21 = yz - wx;
	float mtx22 = 1.0f - (xx + yy);

	return Matrix4f(mtx00, mtx01, mtx02, 0.0f,
                    mtx10, mtx11, mtx12, 0.0f,
                    mtx20, mtx21, mtx22, 0.0f,
                    centerOfRotation[0] * (1.0f - mtx00) - centerOfRotation[1] * mtx10 - centerOfRotation[2] * mtx20,
                    centerOfRotation[1] * (1.0f - mtx11) - centerOfRotation[0] * mtx01 - centerOfRotation[2] * mtx21,
                    centerOfRotation[2] * (1.0f - mtx22) - centerOfRotation[0] * mtx02 - centerOfRotation[1] * mtx12, 1.0);
}

Matrix4f Matrix4f::Rotate(float pitch, float yaw, float roll, const Vector3f &centerOfRotation) {
	pitch = pitch * PI_ON_180;
	yaw = yaw * PI_ON_180;
	roll = roll * PI_ON_180;

	float cosY = cosf(yaw);
	float cosP = cosf(pitch);
	float cosR = cosf(roll);
	float sinY = sinf(yaw);
	float sinP = sinf(pitch);
	float sinR = sinf(roll);

	float mtx00 = cosR * cosY - sinR * sinP * sinY;
	float mtx01 = sinR * cosY + cosR * sinP * sinY;
	float mtx02 = -cosP * sinY;

	float mtx10 = -sinR * cosP;
	float mtx11 = cosR * cosP;
	float mtx12 = sinP;

	float mtx20 = cosR * sinY + sinR * sinP * cosY;
	float mtx21 = sinR * sinY - cosR * sinP * cosY;
	float mtx22 = cosP * cosY;

	return Matrix4f(mtx00, mtx01, mtx02, 0.0f,
		mtx10, mtx11, mtx12, 0.0f,
		mtx20, mtx21, mtx22, 0.0f,
		centerOfRotation[0] * (1.0f - mtx00) - centerOfRotation[1] * mtx10 - centerOfRotation[2] * mtx20,
		centerOfRotation[1] * (1.0f - mtx11) - centerOfRotation[0] * mtx01 - centerOfRotation[2] * mtx21,
		centerOfRotation[2] * (1.0f - mtx22) - centerOfRotation[0] * mtx02 - centerOfRotation[1] * mtx12, 1.0);
}

Matrix4f& Matrix4f::Rotate(Matrix4f &mtx, const Vector3f &axis, float degrees) {
	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (x * y) * (1.0f - c) + (z * s);
	mtx[0][2] = (x * z) * (1.0f - c) - (y * s);
	mtx[0][3] = 0.0f;

	mtx[1][0] = (y * x) * (1.0f - c) - (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (y * z) * (1.0f - c) + (x * s);
	mtx[1][3] = 0.0f;

	mtx[2][0] = (z * x) * (1.0f - c) + (y * s);
	mtx[2][1] = (z * y) * (1.0f - c) - (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[2][3] = 0.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f& Matrix4f::Rotate(Matrix4f &mtx, const Vector3f &axis, float degrees, const Vector3f &centerOfRotation) {
	float rad = degrees * PI_ON_180;
	float magnitude = axis.length();

	float x = axis[0] * (1.0f / magnitude);
	float y = axis[1] * (1.0f / magnitude);
	float z = axis[2] * (1.0f / magnitude);
	float c = cosf(rad);
	float s = sinf(rad);

	mtx[0][0] = (x * x) * (1.0f - c) + c;
	mtx[0][1] = (x * y) * (1.0f - c) + (z * s);
	mtx[0][2] = (x * z) * (1.0f - c) - (y * s);
	mtx[0][3] = 0.0f;

	mtx[1][0] = (y * x) * (1.0f - c) - (z * s);
	mtx[1][1] = (y * y) * (1.0f - c) + c;
	mtx[1][2] = (y * z) * (1.0f - c) + (x * s);
	mtx[1][3] = 0.0f;

	mtx[2][0] = (z * x) * (1.0f - c) + (y * s);
	mtx[2][1] = (z * y) * (1.0f - c) - (x * s);
	mtx[2][2] = (z * z) * (1.0f - c) + c;
	mtx[2][3] = 0.0f;

	mtx[3][0] = centerOfRotation[0] * (1.0f - mtx[0][0]) - centerOfRotation[1] * mtx[1][0] - centerOfRotation[2] * mtx[2][0];
	mtx[3][1] = centerOfRotation[1] * (1.0f - mtx[1][1]) - centerOfRotation[0] * mtx[0][1] - centerOfRotation[2] * mtx[2][1];
	mtx[3][2] = centerOfRotation[2] * (1.0f - mtx[2][2]) - centerOfRotation[0] * mtx[0][2] - centerOfRotation[1] * mtx[1][2];
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f Matrix4f::GetNormalMatrix(const Matrix4f &modelViewMatrix) {

	Matrix4f normalMatrix;
	float det;
	float invDet;

	det = modelViewMatrix[0][0] * (modelViewMatrix[1][1] * modelViewMatrix[2][2] - modelViewMatrix[2][1] * modelViewMatrix[1][2]) +
		  modelViewMatrix[0][1] * (modelViewMatrix[1][2] * modelViewMatrix[2][0] - modelViewMatrix[2][2] * modelViewMatrix[1][0]) +
		  modelViewMatrix[0][2] * (modelViewMatrix[1][0] * modelViewMatrix[2][1] - modelViewMatrix[1][1] * modelViewMatrix[2][0]);

	invDet = 1.0f / det;

	normalMatrix[0][0] = (modelViewMatrix[1][1] * modelViewMatrix[2][2] - modelViewMatrix[2][1] * modelViewMatrix[1][2]) * invDet;
	normalMatrix[1][0] = (modelViewMatrix[2][1] * modelViewMatrix[0][2] - modelViewMatrix[2][2] * modelViewMatrix[0][1]) * invDet;
	normalMatrix[2][0] = (modelViewMatrix[0][1] * modelViewMatrix[1][2] - modelViewMatrix[1][1] * modelViewMatrix[0][2]) * invDet;
	normalMatrix[3][0] = 0.0f;

	normalMatrix[0][1] = (modelViewMatrix[2][0] * modelViewMatrix[1][2] - modelViewMatrix[1][0] * modelViewMatrix[2][2]) * invDet;
	normalMatrix[1][1] = (modelViewMatrix[0][0] * modelViewMatrix[2][2] - modelViewMatrix[2][0] * modelViewMatrix[0][2]) * invDet;
	normalMatrix[2][1] = (modelViewMatrix[1][0] * modelViewMatrix[0][2] - modelViewMatrix[1][2] * modelViewMatrix[0][0]) * invDet;
	normalMatrix[3][1] = 0.0f;

	normalMatrix[0][2] = (modelViewMatrix[1][0] * modelViewMatrix[2][1] - modelViewMatrix[1][1] * modelViewMatrix[2][0]) * invDet;
	normalMatrix[1][2] = (modelViewMatrix[2][0] * modelViewMatrix[0][1] - modelViewMatrix[0][0] * modelViewMatrix[2][1]) * invDet;
	normalMatrix[2][2] = (modelViewMatrix[0][0] * modelViewMatrix[1][1] - modelViewMatrix[0][1] * modelViewMatrix[1][0]) * invDet;
	normalMatrix[3][2] = 0.0f;

	normalMatrix[0][3] = 0.0f;
	normalMatrix[1][3] = 0.0f;
	normalMatrix[2][3] = 0.0f;
	normalMatrix[3][3] = 1.0f;

	return normalMatrix;
}

Matrix4f &Matrix4f::GetNormalMatrix(Matrix4f &mtx, const Matrix4f &modelViewMatrix) {
	float det;
	float invDet;

	det = modelViewMatrix[0][0] * (modelViewMatrix[1][1] * modelViewMatrix[2][2] - modelViewMatrix[2][1] * modelViewMatrix[1][2]) +
		  modelViewMatrix[0][1] * (modelViewMatrix[1][2] * modelViewMatrix[2][0] - modelViewMatrix[2][2] * modelViewMatrix[1][0]) +
		  modelViewMatrix[0][2] * (modelViewMatrix[1][0] * modelViewMatrix[2][1] - modelViewMatrix[1][1] * modelViewMatrix[2][0]);

	invDet = 1.0f / det;

	mtx[0][0] = (modelViewMatrix[1][1] * modelViewMatrix[2][2] - modelViewMatrix[2][1] * modelViewMatrix[1][2]) * invDet;
	mtx[1][0] = (modelViewMatrix[2][1] * modelViewMatrix[0][2] - modelViewMatrix[2][2] * modelViewMatrix[0][1]) * invDet;
	mtx[2][0] = (modelViewMatrix[0][1] * modelViewMatrix[1][2] - modelViewMatrix[1][1] * modelViewMatrix[0][2]) * invDet;
	mtx[3][0] = 0.0f;

	mtx[0][1] = (modelViewMatrix[2][0] * modelViewMatrix[1][2] - modelViewMatrix[1][0] * modelViewMatrix[2][2]) * invDet;
	mtx[1][1] = (modelViewMatrix[0][0] * modelViewMatrix[2][2] - modelViewMatrix[2][0] * modelViewMatrix[0][2]) * invDet;
	mtx[2][1] = (modelViewMatrix[1][0] * modelViewMatrix[0][2] - modelViewMatrix[1][2] * modelViewMatrix[0][0]) * invDet;
	mtx[3][1] = 0.0f;

	mtx[0][2] = (modelViewMatrix[1][0] * modelViewMatrix[2][1] - modelViewMatrix[1][1] * modelViewMatrix[2][0]) * invDet;
	mtx[1][2] = (modelViewMatrix[2][0] * modelViewMatrix[0][1] - modelViewMatrix[0][0] * modelViewMatrix[2][1]) * invDet;
	mtx[2][2] = (modelViewMatrix[0][0] * modelViewMatrix[1][1] - modelViewMatrix[0][1] * modelViewMatrix[1][0]) * invDet;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f Matrix4f::GetViewPortMatrix(float width, float height) {
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	return Matrix4f(halfWidth, 0.0f, 0.0f, halfWidth,
					0.0f, halfHeight, 0.0f, halfHeight,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4f &Matrix4f::GetViewPortMatrix(Matrix4f &mtx, float width, float height) {
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	mtx[0][0] = halfWidth;
	mtx[1][0] = 0.0f;
	mtx[2][0] = 0.0f;
	mtx[3][0] = halfWidth;

	mtx[0][1] = 0.0f;
	mtx[1][1] = halfHeight;
	mtx[2][1] = 0.0f;
	mtx[3][1] = halfHeight;

	mtx[0][2] = 0.0f;
	mtx[1][2] = 0.0f;
	mtx[2][2] = 1.0f;
	mtx[3][2] = 0.0f;

	mtx[0][3] = 0.0f;
	mtx[1][3] = 0.0f;
	mtx[2][3] = 0.0f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f Matrix4f::Perspective(float fovx, float aspect, float znear, float zfar) {
	Matrix4f perspective;

	float e = tanf(PI_ON_180 * fovx * 0.5f);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	perspective[0][0] = xScale;
	perspective[0][1] = 0.0f;
	perspective[0][2] = 0.0f;
	perspective[0][3] = 0.0f;

	perspective[1][0] = 0.0f;
	perspective[1][1] = yScale;
	perspective[1][2] = 0.0f;
	perspective[1][3] = 0.0f;

	perspective[2][0] = 0.0f;
	perspective[2][1] = 0.0f;
	perspective[2][2] = (zfar + znear) / (znear - zfar);
	perspective[2][3] = -1.0f;

	perspective[3][0] = 0.0f;
	perspective[3][1] = 0.0f;
	perspective[3][2] = (2.0f * zfar * znear) / (znear - zfar);
	perspective[3][3] = 0.0f;

	return perspective;
}

Matrix4f Matrix4f::Perspective(float left, float right, float bottom, float top, float znear, float zfar) {
	Matrix4f perspective;

	float sum_rl, sum_tb, sum_nf, inv_rl, inv_tb, inv_nf, n2;
	sum_rl = (right + left);
	sum_tb = (top + bottom);
	sum_nf = (znear + zfar);
	inv_rl = (1.0f / (right - left));
	inv_tb = (1.0f / (top - bottom));
	inv_nf = (1.0f / (znear - zfar));
	n2 = (znear + znear);

	perspective[0][0] = (n2 * inv_rl);
	perspective[0][1] = 0.0f;
	perspective[0][2] = 0.0f;
	perspective[0][3] = 0.0f;

	perspective[1][0] = 0.0f;
	perspective[1][1] = (n2 * inv_tb);
	perspective[1][2] = 0.0f;
	perspective[1][3] = 0.0f;

	perspective[2][0] = sum_rl * inv_rl;
	perspective[2][1] = sum_tb * inv_tb;
	perspective[2][2] = sum_nf * inv_nf;
	perspective[2][3] = -1.0f;

	perspective[3][0] = 0.0f;
	perspective[3][1] = 0.0f;
	perspective[3][2] = (n2 * inv_nf) * zfar;
	perspective[3][3] = 0.0f;

	return perspective;
}

Matrix4f &Matrix4f::Perspective(Matrix4f &mtx, float fovx, float aspect, float znear, float zfar) {
	float e = tanf(PI_ON_180 * fovx * 0.5f);
	float xScale = 1 / (e * aspect);
	float yScale = 1 / e;

	mtx[0][0] = xScale;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = yScale;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = (zfar + znear) / (znear - zfar);
	mtx[2][3] = -1.0f;

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = (2.0f * zfar * znear) / (znear - zfar);
	mtx[3][3] = 0.0f;

	return mtx;
}

Matrix4f Matrix4f::InvPerspective(float fovx, float aspect, float znear, float zfar) {
	Matrix4f invPerspective;

	float e = tanf(PI_ON_180 * fovx * 0.5f);

	invPerspective[0][0] = e * aspect;
	invPerspective[0][1] = 0.0f;
	invPerspective[0][2] = 0.0f;
	invPerspective[0][3] = 0.0f;

	invPerspective[1][0] = 0.0f;
	invPerspective[1][1] = e;
	invPerspective[1][2] = 0.0f;
	invPerspective[1][3] = 0.0f;

	invPerspective[2][0] = 0.0f;
	invPerspective[2][1] = 0.0f;
	invPerspective[2][2] = 0.0;
	invPerspective[2][3] = (znear - zfar) / (2 * zfar * znear);

	invPerspective[3][0] = 0.0f;
	invPerspective[3][1] = 0.0f;
	invPerspective[3][2] = -1.0f;
	invPerspective[3][3] = (znear + zfar) / (2 * zfar * znear);

	return invPerspective;
}

Matrix4f &Matrix4f::InvPerspective(Matrix4f &mtx, float fovx, float aspect, float znear, float zfar) {
	float e = tanf(PI_ON_180 * fovx * 0.5f);

	mtx[0][0] = e * aspect;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = e;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = 0.0;
	mtx[2][3] = (znear - zfar) / (2 * zfar * znear);

	mtx[3][0] = 0.0f;
	mtx[3][1] = 0.0f;
	mtx[3][2] = -1.0f;
	mtx[3][3] = (znear + zfar) / (2 * zfar * znear);

	return mtx;
}

Matrix4f Matrix4f::InvPerspective(const Matrix4f &pers) {
	/*float e = 1.0f / pers[1][1];
	float aspect = pers[1][1] / pers[0][0];
	float near = pers[3][2] / (pers[2][2] - 1);
	float far = pers[3][2] / (pers[2][2] + 1);

	return Matrix4f(e * aspect, 0.0f, 0.0f, 0.0f,
		0.0f, e, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, (near - far) / (2 * far * near),
		0.0f, 0.0f, -1.0f, (near + far) / (2 * far * near));*/


	return Matrix4f(1.0f / pers[0][0], 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f / pers[1][1], 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f / pers[3][2],
                    0.0f, 0.0f, -1.0f, pers[2][2] / pers[3][2]);
}

Matrix4f Matrix4f::Orthographic(float left, float right, float bottom, float top, float znear, float zfar) {
	Matrix4f ortho;

	ortho[0][0] = 2 / (right - left);
	ortho[0][1] = 0.0f;
	ortho[0][2] = 0.0f;
	ortho[0][3] = 0.0f;

	ortho[1][0] = 0.0f;
	ortho[1][1] = 2 / (top - bottom);
	ortho[1][2] = 0.0f;
	ortho[1][3] = 0.0f;

	ortho[2][0] = 0.0f;
	ortho[2][1] = 0.0f;
	ortho[2][2] = 2 / (znear - zfar);
	ortho[2][3] = 0.0f;

	ortho[3][0] = (right + left) / (left - right);
	ortho[3][1] = (top + bottom) / (bottom - top);
	ortho[3][2] = (zfar + znear) / (znear - zfar);
	ortho[3][3] = 1.0f;

	return ortho;
}

Matrix4f &Matrix4f::Orthographic(Matrix4f &mtx, float left, float right, float bottom, float top, float znear, float zfar) {
	mtx[0][0] = 2 / (right - left);
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = 2 / (top - bottom);
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = 2 / (znear - zfar);
	mtx[2][3] = 0.0f;

	mtx[3][0] = (right + left) / (left - right);
	mtx[3][1] = (top + bottom) / (bottom - top);
	mtx[3][2] = (zfar + znear) / (znear - zfar);
	mtx[3][3] = 1.0f;

	return mtx;
}


Matrix4f Matrix4f::InvOrthographic(float left, float right, float bottom, float top, float znear, float zfar) {
	Matrix4f invOrtho;

	invOrtho[0][0] = (right - left) * 0.5f;
	invOrtho[0][1] = 0.0f;
	invOrtho[0][2] = 0.0f;
	invOrtho[0][3] = 0.0f;

	invOrtho[1][0] = 0.0f;
	invOrtho[1][1] = (top - bottom) * 0.5f;
	invOrtho[1][2] = 0.0f;
	invOrtho[1][3] = 0.0f;

	invOrtho[2][0] = 0.0f;
	invOrtho[2][1] = 0.0f;
	invOrtho[2][2] = (znear - zfar) * 0.5f;
	invOrtho[2][3] = 0.0f;

	invOrtho[3][0] = (right + left) * 0.5f;
	invOrtho[3][1] = (top + bottom) * 0.5f;
	invOrtho[3][2] = -(zfar + znear) * 0.5f;
	invOrtho[3][3] = 1.0f;

	return invOrtho;
}

Matrix4f &Matrix4f::InvOrthographic(Matrix4f &mtx, float left, float right, float bottom, float top, float znear, float zfar) {
	mtx[0][0] = (right - left) * 0.5f;
	mtx[0][1] = 0.0f;
	mtx[0][2] = 0.0f;
	mtx[0][3] = 0.0f;

	mtx[1][0] = 0.0f;
	mtx[1][1] = (top - bottom) * 0.5f;
	mtx[1][2] = 0.0f;
	mtx[1][3] = 0.0f;

	mtx[2][0] = 0.0f;
	mtx[2][1] = 0.0f;
	mtx[2][2] = (znear - zfar) * 0.5f;
	mtx[2][3] = 0.0f;

	mtx[3][0] = (right + left) * 0.5f;
	mtx[3][1] = (top + bottom) * 0.5f;
	mtx[3][2] = -(zfar + znear) * 0.5f;
	mtx[3][3] = 1.0f;

	return mtx;
}

Matrix4f Matrix4f::InvOrthographic(const Matrix4f &ortho) {
	float left = -(1.0f / ortho[0][0]) * (1.0f + ortho[3][0]);
	float right = (1.0f / ortho[0][0]) * (1.0f - ortho[3][0]);
	float bottom = -(1.0f / ortho[1][1]) * (1.0f + ortho[3][1]);
	float top = (1.0f / ortho[1][1]) * (1.0f - ortho[3][1]);
	float near = (1.0f / ortho[2][2]) * (1.0f - ortho[3][2]);
	float far = -(1.0f / ortho[2][2]) * (1.0f + ortho[3][2]);

	return Matrix4f((right - left) * 0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, (top - bottom) * 0.5f, 0.0f, 0.0f,
                    0.0f, 0.0f, (near - far) * 0.5f, 0.0f,
                    (right + left) * 0.5f, (top + bottom) * 0.5f, -(far + near) * 0.5f, 1.0f);
}

Matrix4f Matrix4f::LookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up) {
	Vector3f zAxis = eye - target;
	Vector3f::Normalize(zAxis);

	Vector3f xAxis = Vector3f::Cross(up, zAxis);
	Vector3f::Normalize(xAxis);

	Vector3f yAxis = Vector3f::Cross(zAxis, xAxis);
	Vector3f::Normalize(yAxis);

	return Matrix4f(xAxis[0], yAxis[0], zAxis[0], 0.0f,
					xAxis[1], yAxis[1], zAxis[1], 0.0f,
					xAxis[2], yAxis[2], zAxis[2], 0.0f,
					-Vector3f::Dot(xAxis, eye), -Vector3f::Dot(yAxis, eye), -Vector3f::Dot(zAxis, eye), 1.0f);
}

Matrix4f Matrix4f::InvViewMatrix(const Matrix4f &viewMatrix) {
	return Matrix4f(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0], 0.0f,
		viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1], 0.0f,
		viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2], 0.0f,
		-(viewMatrix[3][0] * viewMatrix[0][0]) - (viewMatrix[3][1] * viewMatrix[0][1]) - (viewMatrix[3][2] * viewMatrix[0][2]),
		-(viewMatrix[3][0] * viewMatrix[1][0]) - (viewMatrix[3][1] * viewMatrix[1][1]) - (viewMatrix[3][2] * viewMatrix[1][2]),
		-(viewMatrix[3][0] * viewMatrix[2][0]) - (viewMatrix[3][1] * viewMatrix[2][1]) - (viewMatrix[3][2] * viewMatrix[2][2]), 1.0);
}

float *Matrix4f::operator[](int row) {
	return mtx[row];
}

const float *Matrix4f::operator[](int row) const {
	return mtx[row];
}

void Matrix4f::identity() {
	mtx[0][0] = 1.0f, mtx[0][1] = 0.0f, mtx[0][2] = 0.0f, mtx[0][3] = 0.0f;
	mtx[1][0] = 0.0f, mtx[1][1] = 1.0f, mtx[1][2] = 0.0f, mtx[1][3] = 0.0f;
	mtx[2][0] = 0.0f, mtx[2][1] = 0.0f, mtx[2][2] = 1.0f, mtx[2][3] = 0.0f;
	mtx[3][0] = 0.0f, mtx[3][1] = 0.0f, mtx[3][2] = 0.0f, mtx[3][3] = 1.0f;
}

Matrix4f &Matrix4f::operator=(const Matrix4f &rhs) {
	mtx[0][0] = rhs[0][0]; mtx[0][1] = rhs[0][1]; mtx[0][2] = rhs[0][2]; mtx[0][3] = rhs[0][3];
	mtx[1][0] = rhs[1][0]; mtx[1][1] = rhs[1][1]; mtx[1][2] = rhs[1][2]; mtx[1][3] = rhs[1][3];
	mtx[2][0] = rhs[2][0]; mtx[2][1] = rhs[2][1]; mtx[2][2] = rhs[2][2]; mtx[2][3] = rhs[2][3];
	mtx[3][0] = rhs[3][0]; mtx[3][1] = rhs[3][1]; mtx[3][2] = rhs[3][2]; mtx[3][3] = rhs[3][3];
	return *this;
}

Matrix4f &Matrix4f::operator+=(const Matrix4f &rhs) {
	// Row 1.
	mtx[0][0] = mtx[0][0] + rhs.mtx[0][0];
	mtx[0][1] = mtx[0][1] + rhs.mtx[0][1];
	mtx[0][2] = mtx[0][2] + rhs.mtx[0][2];
	mtx[0][3] = mtx[0][3] + rhs.mtx[0][3];

	// Row 2.
	mtx[1][0] = mtx[1][0] + rhs.mtx[1][0];
	mtx[1][1] = mtx[1][1] + rhs.mtx[1][1];
	mtx[1][2] = mtx[1][2] + rhs.mtx[1][2];
	mtx[1][3] = mtx[1][3] + rhs.mtx[1][3];

	// Row 3.
	mtx[2][0] = mtx[2][0] + rhs.mtx[2][0];
	mtx[2][1] = mtx[2][1] + rhs.mtx[2][1];
	mtx[2][2] = mtx[2][2] + rhs.mtx[2][2];
	mtx[2][3] = mtx[2][3] + rhs.mtx[2][3];

	// Row 4.
	mtx[3][0] = mtx[3][0] + rhs.mtx[3][0];
	mtx[3][1] = mtx[3][1] + rhs.mtx[3][1];
	mtx[3][2] = mtx[3][2] + rhs.mtx[3][2];
	mtx[3][3] = mtx[3][3] + rhs.mtx[3][3];

	return *this;
}

Matrix4f &Matrix4f::operator*=(const Matrix4f &rhs) {
	Matrix4f tmp;

	// Row 1.
	tmp.mtx[0][0] = (rhs.mtx[0][0] * mtx[0][0]) + (rhs.mtx[0][1] * mtx[1][0]) + (rhs.mtx[0][2] * mtx[2][0]) + (rhs.mtx[0][3] * mtx[3][0]);
	tmp.mtx[0][1] = (rhs.mtx[0][0] * mtx[0][1]) + (rhs.mtx[0][1] * mtx[1][1]) + (rhs.mtx[0][2] * mtx[2][1]) + (rhs.mtx[0][3] * mtx[3][1]);
	tmp.mtx[0][2] = (rhs.mtx[0][0] * mtx[0][2]) + (rhs.mtx[0][1] * mtx[1][2]) + (rhs.mtx[0][2] * mtx[2][2]) + (rhs.mtx[0][3] * mtx[3][2]);
	tmp.mtx[0][3] = (rhs.mtx[0][0] * mtx[0][3]) + (rhs.mtx[0][1] * mtx[1][3]) + (rhs.mtx[0][2] * mtx[2][3]) + (rhs.mtx[0][3] * mtx[3][3]);

	// Row 2.
	tmp.mtx[1][0] = (rhs.mtx[1][0] * mtx[0][0]) + (rhs.mtx[1][1] * mtx[1][0]) + (rhs.mtx[1][2] * mtx[2][0]) + (rhs.mtx[1][3] * mtx[3][0]);
	tmp.mtx[1][1] = (rhs.mtx[1][0] * mtx[0][1]) + (rhs.mtx[1][1] * mtx[1][1]) + (rhs.mtx[1][2] * mtx[2][1]) + (rhs.mtx[1][3] * mtx[3][1]);
	tmp.mtx[1][2] = (rhs.mtx[1][0] * mtx[0][2]) + (rhs.mtx[1][1] * mtx[1][2]) + (rhs.mtx[1][2] * mtx[2][2]) + (rhs.mtx[1][3] * mtx[3][2]);
	tmp.mtx[1][3] = (rhs.mtx[1][0] * mtx[0][3]) + (rhs.mtx[1][1] * mtx[1][3]) + (rhs.mtx[1][2] * mtx[2][3]) + (rhs.mtx[1][3] * mtx[3][3]);

	// Row 3.
	tmp.mtx[2][0] = (rhs.mtx[2][0] * mtx[0][0]) + (rhs.mtx[2][1] * mtx[1][0]) + (rhs.mtx[2][2] * mtx[2][0]) + (rhs.mtx[2][3] * mtx[3][0]);
	tmp.mtx[2][1] = (rhs.mtx[2][0] * mtx[0][1]) + (rhs.mtx[2][1] * mtx[1][1]) + (rhs.mtx[2][2] * mtx[2][1]) + (rhs.mtx[2][3] * mtx[3][1]);
	tmp.mtx[2][2] = (rhs.mtx[2][0] * mtx[0][2]) + (rhs.mtx[2][1] * mtx[1][2]) + (rhs.mtx[2][2] * mtx[2][2]) + (rhs.mtx[2][3] * mtx[3][2]);
	tmp.mtx[2][3] = (rhs.mtx[2][0] * mtx[0][3]) + (rhs.mtx[2][1] * mtx[1][3]) + (rhs.mtx[2][2] * mtx[2][3]) + (rhs.mtx[2][3] * mtx[3][3]);

	// Row 4.
	tmp.mtx[3][0] = (rhs.mtx[3][0] * mtx[0][0]) + (rhs.mtx[3][1] * mtx[1][0]) + (rhs.mtx[3][2] * mtx[2][0]) + (rhs.mtx[3][3] * mtx[3][0]);
	tmp.mtx[3][1] = (rhs.mtx[3][0] * mtx[0][1]) + (rhs.mtx[3][1] * mtx[1][1]) + (rhs.mtx[3][2] * mtx[2][1]) + (rhs.mtx[3][3] * mtx[3][1]);
	tmp.mtx[3][2] = (rhs.mtx[3][0] * mtx[0][2]) + (rhs.mtx[3][1] * mtx[1][2]) + (rhs.mtx[3][2] * mtx[2][2]) + (rhs.mtx[3][3] * mtx[3][2]);
	tmp.mtx[3][3] = (rhs.mtx[3][0] * mtx[0][3]) + (rhs.mtx[3][1] * mtx[1][3]) + (rhs.mtx[3][2] * mtx[2][3]) + (rhs.mtx[3][3] * mtx[3][3]);

	*this = tmp;
	return *this;
}

Matrix4f &Matrix4f::operator^=(const Matrix4f &rhs) {
	Matrix4f tmp;

	// Row 1.
	tmp.mtx[0][0] = (mtx[0][0] * rhs.mtx[0][0]) + (mtx[0][1] * rhs.mtx[1][0]) + (mtx[0][2] * rhs.mtx[2][0]) + (mtx[0][3] * rhs.mtx[3][0]);
	tmp.mtx[0][1] = (mtx[0][0] * rhs.mtx[0][1]) + (mtx[0][1] * rhs.mtx[1][1]) + (mtx[0][2] * rhs.mtx[2][1]) + (mtx[0][3] * rhs.mtx[3][1]);
	tmp.mtx[0][2] = (mtx[0][0] * rhs.mtx[0][2]) + (mtx[0][1] * rhs.mtx[1][2]) + (mtx[0][2] * rhs.mtx[2][2]) + (mtx[0][3] * rhs.mtx[3][2]);
	tmp.mtx[0][3] = (mtx[0][0] * rhs.mtx[0][3]) + (mtx[0][1] * rhs.mtx[1][3]) + (mtx[0][2] * rhs.mtx[2][3]) + (mtx[0][3] * rhs.mtx[3][3]);

	// Row 2.
	tmp.mtx[1][0] = (mtx[1][0] * rhs.mtx[0][0]) + (mtx[1][1] * rhs.mtx[1][0]) + (mtx[1][2] * rhs.mtx[2][0]) + (mtx[1][3] * rhs.mtx[3][0]);
	tmp.mtx[1][1] = (mtx[1][0] * rhs.mtx[0][1]) + (mtx[1][1] * rhs.mtx[1][1]) + (mtx[1][2] * rhs.mtx[2][1]) + (mtx[1][3] * rhs.mtx[3][1]);
	tmp.mtx[1][2] = (mtx[1][0] * rhs.mtx[0][2]) + (mtx[1][1] * rhs.mtx[1][2]) + (mtx[1][2] * rhs.mtx[2][2]) + (mtx[1][3] * rhs.mtx[3][2]);
	tmp.mtx[1][3] = (mtx[1][0] * rhs.mtx[0][3]) + (mtx[1][1] * rhs.mtx[1][3]) + (mtx[1][2] * rhs.mtx[2][3]) + (mtx[1][3] * rhs.mtx[3][3]);

	// Row 3.
	tmp.mtx[2][0] = (mtx[2][0] * rhs.mtx[0][0]) + (mtx[2][1] * rhs.mtx[1][0]) + (mtx[2][2] * rhs.mtx[2][0]) + (mtx[2][3] * rhs.mtx[3][0]);
	tmp.mtx[2][1] = (mtx[2][0] * rhs.mtx[0][1]) + (mtx[2][1] * rhs.mtx[1][1]) + (mtx[2][2] * rhs.mtx[2][1]) + (mtx[2][3] * rhs.mtx[3][1]);
	tmp.mtx[2][2] = (mtx[2][0] * rhs.mtx[0][2]) + (mtx[2][1] * rhs.mtx[1][2]) + (mtx[2][2] * rhs.mtx[2][2]) + (mtx[2][3] * rhs.mtx[3][2]);
	tmp.mtx[2][3] = (mtx[2][0] * rhs.mtx[0][3]) + (mtx[2][1] * rhs.mtx[1][3]) + (mtx[2][2] * rhs.mtx[2][3]) + (mtx[2][3] * rhs.mtx[3][3]);

	// Row 4.
	tmp.mtx[3][0] = (mtx[3][0] * rhs.mtx[0][0]) + (mtx[3][1] * rhs.mtx[1][0]) + (mtx[3][2] * rhs.mtx[2][0]) + (mtx[3][3] * rhs.mtx[3][0]);
	tmp.mtx[3][1] = (mtx[3][0] * rhs.mtx[0][1]) + (mtx[3][1] * rhs.mtx[1][1]) + (mtx[3][2] * rhs.mtx[2][1]) + (mtx[3][3] * rhs.mtx[3][1]);
	tmp.mtx[3][2] = (mtx[3][0] * rhs.mtx[0][2]) + (mtx[3][1] * rhs.mtx[1][2]) + (mtx[3][2] * rhs.mtx[2][2]) + (mtx[3][3] * rhs.mtx[3][2]);
	tmp.mtx[3][3] = (mtx[3][0] * rhs.mtx[0][3]) + (mtx[3][1] * rhs.mtx[1][3]) + (mtx[3][2] * rhs.mtx[2][3]) + (mtx[3][3] * rhs.mtx[3][3]);

	*this = tmp;
	return *this;
}

Matrix4f Matrix4f::operator+(const Matrix4f &rhs) const {
	Matrix4f tmp(*this);
	tmp += rhs;
	return tmp;
}

Matrix4f Matrix4f::operator*(const Matrix4f &rhs) const {
	Matrix4f tmp(*this);
	tmp *= rhs;
	return tmp;
}

Matrix4f Matrix4f::operator^(const Matrix4f &rhs) const {
	Matrix4f tmp(*this);
	tmp ^= rhs;
	return tmp;
}

void Matrix4f::transpose() {
	float tmp = mtx[0][1]; mtx[0][1] = mtx[1][0]; mtx[1][0] = tmp;
	tmp = mtx[0][2]; mtx[0][2] = mtx[2][0]; mtx[2][0] = tmp;
	tmp = mtx[0][3]; mtx[0][3] = mtx[3][0]; mtx[3][0] = tmp;

	tmp = mtx[2][1]; mtx[2][1] = mtx[1][2]; mtx[1][2] = tmp;
	tmp = mtx[3][1]; mtx[3][1] = mtx[1][3]; mtx[1][3] = tmp;

	tmp = mtx[2][3]; mtx[2][3] = mtx[3][2]; mtx[3][2] = tmp;
}

void Matrix4f::transpose3() {
	float tmp = mtx[0][1]; mtx[0][1] = mtx[1][0]; mtx[1][0] = tmp;
	tmp = mtx[0][2]; mtx[0][2] = mtx[2][0]; mtx[2][0] = tmp;

	tmp = mtx[2][1]; mtx[2][1] = mtx[1][2]; mtx[1][2] = tmp;
}

Matrix4f& Matrix4f::Transpose(Matrix4f &m) {
	float tmp = m[0][1]; m[0][1] = m[1][0]; m[1][0] = tmp;
	tmp = m[0][2]; m[0][2] = m[2][0]; m[2][0] = tmp;
	tmp = m[0][3]; m[0][3] = m[3][0]; m[3][0] = tmp;

	tmp = m[2][1]; m[2][1] = m[1][2]; m[1][2] = tmp;
	tmp = m[3][1]; m[3][1] = m[1][3]; m[1][3] = tmp;

	tmp = m[2][3]; m[2][3] = m[3][2]; m[3][2] = tmp;

	return m;
}

Matrix4f Matrix4f::Transpose(const Matrix4f &m) {
	Matrix4f tmp;
	tmp[0][0] = m[0][0]; tmp[0][1] = m[1][0]; tmp[0][2] = m[2][0]; tmp[0][3] = m[3][0];
	tmp[1][0] = m[0][1]; tmp[1][1] = m[1][1]; tmp[1][2] = m[2][1]; tmp[1][3] = m[3][1];
	tmp[2][0] = m[0][2]; tmp[2][1] = m[1][2]; tmp[2][2] = m[2][2]; tmp[2][3] = m[3][2];
	tmp[3][0] = m[0][3]; tmp[3][1] = m[1][3]; tmp[3][2] = m[2][3]; tmp[3][3] = m[3][3];
	return tmp;
}

float Matrix4f::determinant() const {

	return (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1])
		 * (mtx[2][2] * mtx[3][3] - mtx[3][2] * mtx[2][3])
		 - (mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1])
		 * (mtx[1][2] * mtx[3][3] - mtx[3][2] * mtx[1][3])
		 + (mtx[0][0] * mtx[3][1] - mtx[3][0] * mtx[0][1])
		 * (mtx[1][2] * mtx[2][3] - mtx[2][2] * mtx[1][3])
		 + (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1])
		 * (mtx[0][2] * mtx[3][3] - mtx[3][2] * mtx[0][3])
		 - (mtx[1][0] * mtx[3][1] - mtx[3][0] * mtx[1][1])
		 * (mtx[0][2] * mtx[2][3] - mtx[2][2] * mtx[0][3])
		 + (mtx[2][0] * mtx[3][1] - mtx[3][0] * mtx[2][1])
		 * (mtx[0][2] * mtx[1][3] - mtx[1][2] * mtx[0][3]);
}

Matrix4f Matrix4f::inverse() const {
	// This method of computing the inverse of a 4x4 matrix is based
	// on a similar function found in Paul Nettle's matrix template
	// class (http://www.fluidstudios.com).
	//
	// If the inverse doesn't exist for this matrix, then the identity
	// matrix will be returned.

	Matrix4f tmp;
	float d = determinant();

	if (fabsf(d) < 0.0001f) {
		tmp.identity();
	}else {
		d = 1.0f / d;

		tmp.mtx[0][0] = d * (mtx[1][1] * (mtx[2][2] * mtx[3][3] - mtx[3][2] * mtx[2][3]) + mtx[2][1] * (mtx[3][2] * mtx[1][3] - mtx[1][2] * mtx[3][3]) + mtx[3][1] * (mtx[1][2] * mtx[2][3] - mtx[2][2] * mtx[1][3]));
		tmp.mtx[1][0] = d * (mtx[1][2] * (mtx[2][0] * mtx[3][3] - mtx[3][0] * mtx[2][3]) + mtx[2][2] * (mtx[3][0] * mtx[1][3] - mtx[1][0] * mtx[3][3]) + mtx[3][2] * (mtx[1][0] * mtx[2][3] - mtx[2][0] * mtx[1][3]));
		tmp.mtx[2][0] = d * (mtx[1][3] * (mtx[2][0] * mtx[3][1] - mtx[3][0] * mtx[2][1]) + mtx[2][3] * (mtx[3][0] * mtx[1][1] - mtx[1][0] * mtx[3][1]) + mtx[3][3] * (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1]));
		tmp.mtx[3][0] = d * (mtx[1][0] * (mtx[3][1] * mtx[2][2] - mtx[2][1] * mtx[3][2]) + mtx[2][0] * (mtx[1][1] * mtx[3][2] - mtx[3][1] * mtx[1][2]) + mtx[3][0] * (mtx[2][1] * mtx[1][2] - mtx[1][1] * mtx[2][2]));

		tmp.mtx[0][1] = d * (mtx[2][1] * (mtx[0][2] * mtx[3][3] - mtx[3][2] * mtx[0][3]) + mtx[3][1] * (mtx[2][2] * mtx[0][3] - mtx[0][2] * mtx[2][3]) + mtx[0][1] * (mtx[3][2] * mtx[2][3] - mtx[2][2] * mtx[3][3]));
		tmp.mtx[1][1] = d * (mtx[2][2] * (mtx[0][0] * mtx[3][3] - mtx[3][0] * mtx[0][3]) + mtx[3][2] * (mtx[2][0] * mtx[0][3] - mtx[0][0] * mtx[2][3]) + mtx[0][2] * (mtx[3][0] * mtx[2][3] - mtx[2][0] * mtx[3][3]));
		tmp.mtx[2][1] = d * (mtx[2][3] * (mtx[0][0] * mtx[3][1] - mtx[3][0] * mtx[0][1]) + mtx[3][3] * (mtx[2][0] * mtx[0][1] - mtx[0][0] * mtx[2][1]) + mtx[0][3] * (mtx[3][0] * mtx[2][1] - mtx[2][0] * mtx[3][1]));
		tmp.mtx[3][1] = d * (mtx[2][0] * (mtx[3][1] * mtx[0][2] - mtx[0][1] * mtx[3][2]) + mtx[3][0] * (mtx[0][1] * mtx[2][2] - mtx[2][1] * mtx[0][2]) + mtx[0][0] * (mtx[2][1] * mtx[3][2] - mtx[3][1] * mtx[2][2]));

		tmp.mtx[0][2] = d * (mtx[3][1] * (mtx[0][2] * mtx[1][3] - mtx[1][2] * mtx[0][3]) + mtx[0][1] * (mtx[1][2] * mtx[3][3] - mtx[3][2] * mtx[1][3]) + mtx[1][1] * (mtx[3][2] * mtx[0][3] - mtx[0][2] * mtx[3][3]));
		tmp.mtx[1][2] = d * (mtx[3][2] * (mtx[0][0] * mtx[1][3] - mtx[1][0] * mtx[0][3]) + mtx[0][2] * (mtx[1][0] * mtx[3][3] - mtx[3][0] * mtx[1][3]) + mtx[1][2] * (mtx[3][0] * mtx[0][3] - mtx[0][0] * mtx[3][3]));
		tmp.mtx[2][2] = d * (mtx[3][3] * (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1]) + mtx[0][3] * (mtx[1][0] * mtx[3][1] - mtx[3][0] * mtx[1][1]) + mtx[1][3] * (mtx[3][0] * mtx[0][1] - mtx[0][0] * mtx[3][1]));
		tmp.mtx[3][2] = d * (mtx[3][0] * (mtx[1][1] * mtx[0][2] - mtx[0][1] * mtx[1][2]) + mtx[0][0] * (mtx[3][1] * mtx[1][2] - mtx[1][1] * mtx[3][2]) + mtx[1][0] * (mtx[0][1] * mtx[3][2] - mtx[3][1] * mtx[0][2]));

		tmp.mtx[0][3] = d * (mtx[0][1] * (mtx[2][2] * mtx[1][3] - mtx[1][2] * mtx[2][3]) + mtx[1][1] * (mtx[0][2] * mtx[2][3] - mtx[2][2] * mtx[0][3]) + mtx[2][1] * (mtx[1][2] * mtx[0][3] - mtx[0][2] * mtx[1][3]));
		tmp.mtx[1][3] = d * (mtx[0][2] * (mtx[2][0] * mtx[1][3] - mtx[1][0] * mtx[2][3]) + mtx[1][2] * (mtx[0][0] * mtx[2][3] - mtx[2][0] * mtx[0][3]) + mtx[2][2] * (mtx[1][0] * mtx[0][3] - mtx[0][0] * mtx[1][3]));
		tmp.mtx[2][3] = d * (mtx[0][3] * (mtx[2][0] * mtx[1][1] - mtx[1][0] * mtx[2][1]) + mtx[1][3] * (mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1]) + mtx[2][3] * (mtx[1][0] * mtx[0][1] - mtx[0][0] * mtx[1][1]));
		tmp.mtx[3][3] = d * (mtx[0][0] * (mtx[1][1] * mtx[2][2] - mtx[2][1] * mtx[1][2]) + mtx[1][0] * (mtx[2][1] * mtx[0][2] - mtx[0][1] * mtx[2][2]) + mtx[2][0] * (mtx[0][1] * mtx[1][2] - mtx[1][1] * mtx[0][2]));
	}

	return tmp;
}

Matrix4f Matrix4f::Inverse(const Matrix4f &m) {
	Matrix4f tmp;
	float d = m.determinant();

	if (fabsf(d) < 0.0001f) {
		tmp.identity();
	}else {
		d = 1.0f / d;

		tmp.mtx[0][0] = d * (m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) + m[2][1] * (m[3][2] * m[1][3] - m[1][2] * m[3][3]) + m[3][1] * (m[1][2] * m[2][3] - m[2][2] * m[1][3]));
		tmp.mtx[1][0] = d * (m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3]) + m[2][2] * (m[3][0] * m[1][3] - m[1][0] * m[3][3]) + m[3][2] * (m[1][0] * m[2][3] - m[2][0] * m[1][3]));
		tmp.mtx[2][0] = d * (m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]) + m[2][3] * (m[3][0] * m[1][1] - m[1][0] * m[3][1]) + m[3][3] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));
		tmp.mtx[3][0] = d * (m[1][0] * (m[3][1] * m[2][2] - m[2][1] * m[3][2]) + m[2][0] * (m[1][1] * m[3][2] - m[3][1] * m[1][2]) + m[3][0] * (m[2][1] * m[1][2] - m[1][1] * m[2][2]));

		tmp.mtx[0][1] = d * (m[2][1] * (m[0][2] * m[3][3] - m[3][2] * m[0][3]) + m[3][1] * (m[2][2] * m[0][3] - m[0][2] * m[2][3]) + m[0][1] * (m[3][2] * m[2][3] - m[2][2] * m[3][3]));
		tmp.mtx[1][1] = d * (m[2][2] * (m[0][0] * m[3][3] - m[3][0] * m[0][3]) + m[3][2] * (m[2][0] * m[0][3] - m[0][0] * m[2][3]) + m[0][2] * (m[3][0] * m[2][3] - m[2][0] * m[3][3]));
		tmp.mtx[2][1] = d * (m[2][3] * (m[0][0] * m[3][1] - m[3][0] * m[0][1]) + m[3][3] * (m[2][0] * m[0][1] - m[0][0] * m[2][1]) + m[0][3] * (m[3][0] * m[2][1] - m[2][0] * m[3][1]));
		tmp.mtx[3][1] = d * (m[2][0] * (m[3][1] * m[0][2] - m[0][1] * m[3][2]) + m[3][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) + m[0][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));

		tmp.mtx[0][2] = d * (m[3][1] * (m[0][2] * m[1][3] - m[1][2] * m[0][3]) + m[0][1] * (m[1][2] * m[3][3] - m[3][2] * m[1][3]) + m[1][1] * (m[3][2] * m[0][3] - m[0][2] * m[3][3]));
		tmp.mtx[1][2] = d * (m[3][2] * (m[0][0] * m[1][3] - m[1][0] * m[0][3]) + m[0][2] * (m[1][0] * m[3][3] - m[3][0] * m[1][3]) + m[1][2] * (m[3][0] * m[0][3] - m[0][0] * m[3][3]));
		tmp.mtx[2][2] = d * (m[3][3] * (m[0][0] * m[1][1] - m[1][0] * m[0][1]) + m[0][3] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]) + m[1][3] * (m[3][0] * m[0][1] - m[0][0] * m[3][1]));
		tmp.mtx[3][2] = d * (m[3][0] * (m[1][1] * m[0][2] - m[0][1] * m[1][2]) + m[0][0] * (m[3][1] * m[1][2] - m[1][1] * m[3][2]) + m[1][0] * (m[0][1] * m[3][2] - m[3][1] * m[0][2]));

		tmp.mtx[0][3] = d * (m[0][1] * (m[2][2] * m[1][3] - m[1][2] * m[2][3]) + m[1][1] * (m[0][2] * m[2][3] - m[2][2] * m[0][3]) + m[2][1] * (m[1][2] * m[0][3] - m[0][2] * m[1][3]));
		tmp.mtx[1][3] = d * (m[0][2] * (m[2][0] * m[1][3] - m[1][0] * m[2][3]) + m[1][2] * (m[0][0] * m[2][3] - m[2][0] * m[0][3]) + m[2][2] * (m[1][0] * m[0][3] - m[0][0] * m[1][3]));
		tmp.mtx[2][3] = d * (m[0][3] * (m[2][0] * m[1][1] - m[1][0] * m[2][1]) + m[1][3] * (m[0][0] * m[2][1] - m[2][0] * m[0][1]) + m[2][3] * (m[1][0] * m[0][1] - m[0][0] * m[1][1]));
		tmp.mtx[3][3] = d * (m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) + m[1][0] * (m[2][1] * m[0][2] - m[0][1] * m[2][2]) + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));
	}

	return tmp;
}

//friend operator
Matrix4f operator*(float scalar, const Matrix4f &rhs) {
	Matrix4f tmp(rhs);
	tmp[0][0] *= scalar, tmp[0][1] *= scalar, tmp[0][2] *= scalar, tmp[0][3] *= scalar;
	tmp[1][0] *= scalar, tmp[1][1] *= scalar, tmp[1][2] *= scalar, tmp[1][3] *= scalar;
	tmp[2][0] *= scalar, tmp[2][1] *= scalar, tmp[2][2] *= scalar, tmp[2][3] *= scalar;
	tmp[3][0] *= scalar, tmp[3][1] *= scalar, tmp[3][2] *= scalar, tmp[3][3] *= scalar;
	return tmp;
}

//friend operator
Vector3f operator*(const Matrix4f &rhs, const Vector3f &lhs) {
	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[0][1]) + (lhs[2] * rhs.mtx[0][2]),
					(lhs[0] * rhs.mtx[1][0]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[1][2]),
					(lhs[0] * rhs.mtx[2][0]) + (lhs[1] * rhs.mtx[2][1]) + (lhs[2] * rhs.mtx[2][2]));
}

//friend operator
Vector3f operator*(const Vector3f &lhs, const Matrix4f &rhs) {
	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[1][0]) + (lhs[2] * rhs.mtx[2][0]) + rhs.mtx[3][0],
					(lhs[0] * rhs.mtx[0][1]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[2][1]) + rhs.mtx[3][1],
					(lhs[0] * rhs.mtx[0][2]) + (lhs[1] * rhs.mtx[1][2]) + (lhs[2] * rhs.mtx[2][2]) + rhs.mtx[3][2]);
}

//friend operator
Vector3f operator^(const Matrix4f &rhs, const Vector3f &lhs) {
	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[1][0]) + (lhs[2] * rhs.mtx[2][0]) + rhs.mtx[3][0],
					(lhs[0] * rhs.mtx[0][1]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[2][1]) + rhs.mtx[3][1],
					(lhs[0] * rhs.mtx[0][2]) + (lhs[1] * rhs.mtx[1][2]) + (lhs[2] * rhs.mtx[2][2]) + rhs.mtx[3][2]);
}

//friend operator
Vector3f operator^(const Vector3f &lhs, const Matrix4f &rhs) {
	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[0][1]) + (lhs[2] * rhs.mtx[0][2]),
					(lhs[0] * rhs.mtx[1][0]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[1][2]),
					(lhs[0] * rhs.mtx[2][0]) + (lhs[1] * rhs.mtx[2][1]) + (lhs[2] * rhs.mtx[2][2]));
}

//friend operator
Vector3f operator*(const Vector4f &lhs, const Matrix4f &rhs) {
	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[1][0]) + (lhs[2] * rhs.mtx[2][0]) + (lhs[3] * rhs.mtx[3][0]),
					(lhs[0] * rhs.mtx[0][1]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[2][1]) + (lhs[3] * rhs.mtx[3][1]),
					(lhs[0] * rhs.mtx[0][2]) + (lhs[1] * rhs.mtx[1][2]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[3][2]));
}

//friend operator
Vector3f operator*(const Matrix4f &rhs, const Vector4f &lhs) {
	return Vector3f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[0][1]) + (lhs[2] * rhs.mtx[0][2]) + (lhs[3] * rhs.mtx[0][3]),
					(lhs[0] * rhs.mtx[1][0]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[1][2]) + (lhs[3] * rhs.mtx[1][3]),
					(lhs[0] * rhs.mtx[2][0]) + (lhs[1] * rhs.mtx[2][1]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[2][3]));
}

//friend operator
Vector4f operator^(const Matrix4f &rhs, const Vector4f &lhs) {
	return Vector4f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[1][0]) + (lhs[2] * rhs.mtx[2][0]) + (lhs[3] * rhs.mtx[3][0]),
					(lhs[0] * rhs.mtx[0][1]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[2][1]) + (lhs[3] * rhs.mtx[3][1]),
					(lhs[0] * rhs.mtx[0][2]) + (lhs[1] * rhs.mtx[1][2]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[3][2]),
					(lhs[0] * rhs.mtx[0][3]) + (lhs[1] * rhs.mtx[1][3]) + (lhs[2] * rhs.mtx[2][3]) + (lhs[3] * rhs.mtx[3][3])
	);
}

Vector4f operator^(const Vector4f &lhs, const Matrix4f &rhs) {	
	return Vector4f((lhs[0] * rhs.mtx[0][0]) + (lhs[1] * rhs.mtx[0][1]) + (lhs[2] * rhs.mtx[0][2]) + (lhs[3] * rhs.mtx[0][3]),
		(lhs[0] * rhs.mtx[1][0]) + (lhs[1] * rhs.mtx[1][1]) + (lhs[2] * rhs.mtx[1][2]) + (lhs[3] * rhs.mtx[1][3]),
		(lhs[0] * rhs.mtx[2][0]) + (lhs[1] * rhs.mtx[2][1]) + (lhs[2] * rhs.mtx[2][2]) + (lhs[3] * rhs.mtx[2][3]),
		(lhs[0] * rhs.mtx[3][0]) + (lhs[1] * rhs.mtx[3][1]) + (lhs[2] * rhs.mtx[3][2]) + (lhs[3] * rhs.mtx[3][3]));
}

void Matrix4f::print() const {
	std::cout << mtx[0][0] << "  " << mtx[0][1] << "  " << mtx[0][2] << "  " << mtx[0][3] << std::endl;
	std::cout << mtx[1][0] << "  " << mtx[1][1] << "  " << mtx[1][2] << "  " << mtx[1][3] << std::endl;
	std::cout << mtx[2][0] << "  " << mtx[2][1] << "  " << mtx[2][2] << "  " << mtx[2][3] << std::endl;
	std::cout << mtx[3][0] << "  " << mtx[3][1] << "  " << mtx[3][2] << "  " << mtx[3][3] << std::endl;
	std::cout << "-------------------" << std::endl;
}

//////////////////////////////////////////////////////////////////////
Vector2f::Vector2f() {
	vec[0] = 0.0f;
	vec[1] = 0.0f;
}

Vector2f::Vector2f(Vector2f const& rhs) {
	vec[0] = rhs.vec[0];
	vec[1] = rhs.vec[1];
}

Vector2f::Vector2f(Vector2f&& rhs) {
	vec[0] = rhs.vec[0];
	vec[1] = rhs.vec[1];
}

Vector2f::Vector2f(float x_) {
	vec[0] = x_;
	vec[1] = x_;
}

Vector2f::Vector2f(float x_, float y_) {
	vec[0] = x_;
	vec[1] = y_;
}

Vector2f::Vector2f(float array[2]) {
	vec[0] = array[0];
	vec[1] = array[1];
}

void Vector2f::set(float x_, float y_) {
	vec[0] = x_, vec[1] = y_;
}

float Vector2f::length() const {
	return sqrtf((vec[0] * vec[0]) + (vec[1] * vec[1]));
}

float Vector2f::lengthSq() const {
	return (vec[0] * vec[0]) + (vec[1] * vec[1]);
}

bool Vector2f::zero() {
	return vec[0] == 0.0 && vec[1] == 0.0;
}

Vector2f& Vector2f::normalize() {
	float _length = length();
	if (_length == 0.0f) return *this;

	float invMag = 1.0f / _length;
	vec[0] *= invMag, vec[1] *= invMag;
	return *this;
}

bool Vector2f::compare(const Vector2f& rhs, float epsilon) {
	if (std::fabs(vec[0] - rhs[0]) > epsilon)
		return false;

	if (std::fabs(vec[1] - rhs[1]) > epsilon)
		return false;

	return true;
}

float Vector2f::dot(const Vector2f& other) { 
	return  vec[0] * other[0] + vec[1] * other[1]; 
}

float Vector2f::cross(const Vector2f& other) { 
	return vec[0] * other[1] - vec[1] * other[0]; 
}

float Vector2f::angleBetween(const Vector2f& other) { 
	return atan2(cross(other), dot(other)); 
}

float Vector2f::angle() {
	return atan2(vec[1], vec[0]);
}

Vector2f Vector2f::getNegativeReciprocal() {
	return Vector2f(-vec[1], vec[0]); 
}

float &Vector2f::operator[](int index) {
	return vec[index];
}

const float Vector2f::operator[](int index) const {
	return vec[index];
}

const float* Vector2f::getVec()const {
	return vec;
}

Vector2f &Vector2f::operator=(const Vector2f &rhs) {
	vec[0] = rhs.vec[0], vec[1] = rhs.vec[1];
	return *this;
}

Vector2f &Vector2f::operator-=(const Vector2f &rhs) {
	vec[0] -= rhs.vec[0], vec[1] -= rhs.vec[1];
	return *this;
}

Vector2f &Vector2f::operator+=(const Vector2f &rhs) {
	vec[0] += rhs.vec[0], vec[1] += rhs.vec[1];
	return *this;
}

Vector2f &Vector2f::operator*=(float scalar) {
	vec[0] *= scalar, vec[1] *= scalar;
	return *this;
}

Vector2f &Vector2f::operator/=(float scalar) {
	vec[0] /= scalar, vec[1] /= scalar;
	return *this;
}

Vector2f Vector2f::operator+(const Vector2f &rhs) const {
	Vector2f tmp(*this);
	tmp += rhs;
	return tmp;
}

Vector2f Vector2f::operator-(const Vector2f &rhs) const {
	Vector2f tmp(*this);
	tmp -= rhs;
	return tmp;
}

Vector2f Vector2f::operator*(float scalar) const {
	return Vector2f(vec[0] * scalar, vec[1] * scalar);
}

Vector2f Vector2f::operator/(float scalar) const {
	return Vector2f(vec[0] / scalar, vec[1] / scalar);
}

Vector2f operator-(const Vector2f &v) {
	return Vector2f(-v.vec[0], -v.vec[1]);
}

//friend operator
bool operator ==(Vector2f lhs, Vector2f rhs) {
	float epsilon = 0.000001f;
	return fabs(lhs[0] - rhs[0]) <= epsilon && fabs(lhs[1] - rhs[1]) <= epsilon;
}

float Vector2f::Dot(const Vector2f &p, const Vector2f &q) {
	return (p.vec[0] * q.vec[0]) + (p.vec[1] * q.vec[1]);
}

//////////////////////////////////////////////////////////////////////

const Vector3f Vector3f::ZERO(0.0f, 0.0f, 0.0f);
const Vector3f Vector3f::ONE(1.0f, 1.0f, 1.0f);
const Vector3f Vector3f::LEFT(-1.0f, 0.0f, 0.0f);
const Vector3f Vector3f::RIGHT(1.0f, 0.0f, 0.0f);
const Vector3f Vector3f::UP(0.0f, -1.0f, 0.0f);
const Vector3f Vector3f::DOWN(0.0f, 1.0f, 0.0f);
const Vector3f Vector3f::FORWARD(0.0f, 0.0f, 1.0f);
const Vector3f Vector3f::BACK(0.0f, 0.0f, -1.0f);

Vector3f::Vector3f() {
	vec[0] = 0.0f;
	vec[1] = 0.0f;
	vec[2] = 0.0f;
}

Vector3f::Vector3f(float x_) {
	vec[0] = x_;
	vec[1] = x_;
	vec[2] = x_;
}

Vector3f::Vector3f(float x_, float y_, float z_) {
	vec[0] = x_;
	vec[1] = y_;
	vec[2] = z_;
}

Vector3f::Vector3f(float array[3]) {
	vec[0] = array[0];
	vec[1] = array[1];
	vec[2] = array[2];
}

Vector3f::Vector3f(Vector3f const& rhs) {
	vec[0] = rhs.vec[0];
	vec[1] = rhs.vec[1];
	vec[2] = rhs.vec[2];
}

Vector3f::Vector3f(Vector3f&& rhs) {
	vec[0] = rhs.vec[0];
	vec[1] = rhs.vec[1];
	vec[2] = rhs.vec[2];
}

Vector3f::Vector3f(const Vector4f& _vec) {
	vec[0] = _vec[0];
	vec[1] = _vec[1];
	vec[2] = _vec[2];
}

float &Vector3f::operator[](int index) {
	return vec[index];
}

const float Vector3f::operator[](int index) const {
	return vec[index];
}

Vector3f &Vector3f::operator=(const Vector3f& rhs) {
	vec[0] = rhs.vec[0], vec[1] = rhs.vec[1], vec[2] = rhs.vec[2];
	return *this;
}

Vector3f &Vector3f::operator+=(const Vector3f &rhs) {
	vec[0] += rhs.vec[0], vec[1] += rhs.vec[1], vec[2] += rhs.vec[2];
	return *this;
}

Vector3f &Vector3f::operator-=(const Vector3f &rhs) {
	vec[0] -= rhs.vec[0], vec[1] -= rhs.vec[1], vec[2] -= rhs.vec[2];
	return *this;
}

Vector3f &Vector3f::operator*=(const Vector3f &rhs) {
	vec[0] *= rhs.vec[0], vec[1] *= rhs.vec[1], vec[2] *= rhs.vec[2];
	return *this;
}

Vector3f &Vector3f::operator/=(const Vector3f &rhs) {
	vec[0] /= rhs.vec[0], vec[1] /= rhs.vec[1], vec[2] /= rhs.vec[2];
	return *this;
}

Vector3f &Vector3f::operator*=(float scalar) {
	vec[0] *= scalar, vec[1] *= scalar, vec[2] *= scalar;
	return *this;
}

Vector3f &Vector3f::operator/=(float scalar) {
	vec[0] /= scalar, vec[1] /= scalar, vec[2] /= scalar;
	return *this;
}

Vector3f Vector3f::operator+(const Vector3f &rhs) const {
	Vector3f tmp(*this);
	tmp += rhs;
	return tmp;
}

Vector3f Vector3f::operator-(const Vector3f &rhs) const {

	Vector3f tmp(*this);
	tmp -= rhs;
	return tmp;
}

Vector3f Vector3f::operator*(const Vector3f &rhs) const {

	Vector3f tmp(*this);
	tmp *= rhs;
	return tmp;
}

Vector3f Vector3f::operator/(const Vector3f &rhs) const {

	Vector3f tmp(*this);
	tmp /= rhs;
	return tmp;
}

Vector3f Vector3f::operator*(float scalar) const {

	return Vector3f(vec[0] * scalar, vec[1] * scalar, vec[2] * scalar);
}

Vector3f Vector3f::operator/(float scalar) const {

	return Vector3f(vec[0] / scalar, vec[1] / scalar, vec[2] / scalar);
}

bool Vector3f::zero() {

	return vec[0] == 0.0 && vec[1] == 0.0 && vec[2] == 0.0;
}

Vector3f Vector3f::Cross(const Vector3f &p, const Vector3f &q) {

	return Vector3f((p.vec[1] * q.vec[2]) - (p.vec[2] * q.vec[1]),
		(p.vec[2] * q.vec[0]) - (p.vec[0] * q.vec[2]),
		(p.vec[0] * q.vec[1]) - (p.vec[1] * q.vec[0]));
}

Vector3f Vector3f::Inverse(const Vector3f &p) {
	return Vector3f(p[0] != 0 ? 1.0f / p[0] : p[0], p[1] != 0 ? 1.0f / p[1] : p[1], p[2] != 0 ? 1.0f / p[2] : p[2]);
}

float Vector3f::Dot(const Vector3f &p, const Vector3f &q) {
	return (p.vec[0] * q.vec[0]) + (p.vec[1] * q.vec[1]) + (p.vec[2] * q.vec[2]);
}

float Vector3f::NormalDot(const Vector3f &p, const Vector3f &q) {
	return Vector3f::Dot(Normalize(p), Normalize(q));
}

float Vector3f::length() const {
	return sqrtf((vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]));
}

float Vector3f::lengthSq() const {
	return (vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]);
}

Vector3f& Vector3f::Normalize(Vector3f &p) {
	float length = p.length();
	float invMag = length != 0.0f ? 1.0f / length : 1.0f;
	p[0] *= invMag, p[1] *= invMag, p[2] *= invMag;
	return p;
}

Vector3f Vector3f::Normalize(const Vector3f &p) {
	float length = p.length();
	float invMag = length != 0.0f ? 1.0f / length : 1.0f;
	return Vector3f(p[0] * invMag, p[1] * invMag, p[2] * invMag);
}

Vector3f Vector3f::Min(const Vector3f &p, const Vector3f &q) {
	return Vector3f(std::min(p[0], q[0]), std::min(p[1], q[1]), std::min(p[2], q[2]));
}

Vector3f Vector3f::Max(const Vector3f &p, const Vector3f &q) {
	return Vector3f(std::max(p[0], q[0]), std::max(p[1], q[1]), std::max(p[2], q[2]));
}

Vector3f& Vector3f::Clamp(Vector3f &v, float min, float max) {
	v[0] = std::max(min, std::min(v[0], max));
	v[1] = std::max(min, std::min(v[1], max));
	v[2] = std::max(min, std::min(v[2], max));
	return v;
}

Vector3f Vector3f::Fract(const Vector3f &p) {
	return Vector3f(p[0] - (long)p[0], p[1] - (long)p[1], p[2] - (long)p[2]);
}

float Vector3f::Length(const Vector3f &p, const Vector3f &q) {
	float x = q[0] - p[0], y = q[1] - p[1], z = q[2] - p[2];
	return sqrtf((x * x) + (y * y) + (z * z));
}

Vector3f& Vector3f::normalize() {
	float invMag = 1.0f / length();
	vec[0] *= invMag, vec[1] *= invMag, vec[2] *= invMag;
	return *this;
}

void Vector3f::set(float x_, float y_, float z_) {
	vec[0] = x_, vec[1] = y_, vec[2] = z_;
}

void Vector3f::translate(const Vector3f &rhs) {
	vec[0] += rhs[0], vec[1] += rhs[1], vec[2] += rhs[2];
}

void Vector3f::translate(const float dx, const float dy, const float dz) {
	vec[0] += dx, vec[1] += dy, vec[2] += dz;
}

void Vector3f::scale(const Vector3f &rhs) {
	vec[0] *= rhs[0], vec[1] *= rhs[1], vec[2] *= rhs[2];
}

void Vector3f::scale(const float sx, const float sy, const float sz) {
	vec[0] *= sx, vec[1] *= sy, vec[2] *= sz;
}

const float* Vector3f::getVec()const {
	return vec;
}

bool Vector3f::compare(const Vector3f &rhs, float precision) {
	return fabs(lengthSq() - rhs.lengthSq()) < precision * precision;
	//return fabs(vec[0] - rhs[0]) <= precision && fabs(vec[1] - rhs[1]) <= precision && fabs(vec[2] - rhs[2]) <= precision;
}

//friend operator
Vector3f operator-(const Vector3f &v) {
	return Vector3f(-v[0], -v[1], -v[2]);
}

//friend operator
bool operator ==(Vector3f lhs, Vector3f rhs) {
	float epsilon = 0.000001f;
	return fabs(lhs[0] - rhs[0]) <= epsilon && fabs(lhs[1] - rhs[1]) <= epsilon && fabs(lhs[2] - rhs[2]) <= epsilon;
}

bool operator !=(Vector3f lhs, Vector3f rhs) {
	return (lhs[0] != rhs[0]) || (lhs[1] != rhs[1]) || (lhs[2] != rhs[2]);
}

//friend operator
Vector3f operator*(float lhs, const Vector3f &rhs) {
	return Vector3f(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
}
////////////////////////////////////////////////////////////////////////////////////////

Vector4f::Vector4f() {
	vec[0] = 0.0f;
	vec[1] = 0.0f;
	vec[2] = 0.0f;
	vec[3] = 0.0f;
}

Vector4f::Vector4f(float x_, float y_, float z_, float w_) {
	vec[0] = x_;
	vec[1] = y_;
	vec[2] = z_;
	vec[3] = w_;
}

Vector4f::Vector4f(float value) {
	vec[0] = value;
	vec[1] = value;
	vec[2] = value;
	vec[3] = value;
}

Vector4f::Vector4f(const Vector3f &rhs, float w) {
	vec[0] = rhs[0];
	vec[1] = rhs[1];
	vec[2] = rhs[2];
	vec[3] = w;
}

Vector4f::Vector4f(float array[4]) {
	vec[0] = array[0];
	vec[1] = array[1];
	vec[2] = array[2];
	vec[3] = array[3];
}

Vector4f::Vector4f(Vector4f const& rhs) {
	vec[0] = rhs.vec[0];
	vec[1] = rhs.vec[1];
	vec[2] = rhs.vec[2];
	vec[3] = rhs.vec[3];
}

Vector4f::Vector4f(Vector4f&& rhs) {
	vec[0] = rhs.vec[0];
	vec[1] = rhs.vec[1];
	vec[2] = rhs.vec[2];
	vec[3] = rhs.vec[3];
}

void Vector4f::set(float x_, float y_, float z_, float w_) {
	vec[0] = x_;
	vec[1] = y_;
	vec[2] = z_;
	vec[3] = w_;
}

float &Vector4f::operator[](int index) {

	return vec[index];
}

const float Vector4f::operator[](int index) const {
	return vec[index];
}

Vector4f &Vector4f::operator=(const Vector4f &rhs) {
	vec[0] = rhs.vec[0], vec[1] = rhs.vec[1], vec[2] = rhs.vec[2], vec[3] = rhs.vec[3];
	return *this;
}

Vector4f &Vector4f::operator+=(const Vector4f &rhs) {
	vec[0] += rhs.vec[0], vec[1] += rhs.vec[1], vec[2] += rhs.vec[2], vec[3] += rhs.vec[3];
	return *this;
}

Vector4f &Vector4f::operator-=(const Vector4f &rhs) {
	vec[0] -= rhs.vec[0], vec[1] -= rhs.vec[1], vec[2] -= rhs.vec[2], vec[3] -= rhs.vec[3];
	return *this;
}

Vector4f &Vector4f::operator*=(const Vector4f &rhs) {
	vec[0] *= rhs.vec[0], vec[1] *= rhs.vec[1], vec[2] *= rhs.vec[2], vec[3] *= rhs.vec[3];
	return *this;
}

Vector4f &Vector4f::operator/=(const Vector4f &rhs) {
	vec[0] /= rhs.vec[0], vec[1] /= rhs.vec[1], vec[2] /= rhs.vec[2], vec[3] /= rhs.vec[3];
	return *this;
}

Vector4f Vector4f::operator+(const Vector4f &rhs) const {
	Vector4f tmp(*this);
	tmp += rhs;
	return tmp;
}

Vector4f &Vector4f::operator*=(float scalar) {
	vec[0] *= scalar, vec[1] *= scalar, vec[2] *= scalar, vec[3] *= scalar;
	return *this;
}

Vector4f &Vector4f::operator/=(float scalar) {
	vec[0] /= scalar, vec[1] /= scalar, vec[2] /= scalar, vec[3] /= scalar;
	return *this;
}

Vector4f Vector4f::operator-(const Vector4f &rhs) const {

	Vector4f tmp(*this);
	tmp -= rhs;
	return tmp;
}

Vector4f Vector4f::operator*(const Vector4f &rhs) const {

	Vector4f tmp(*this);
	tmp *= rhs;
	return tmp;
}

Vector4f Vector4f::operator/(const Vector4f &rhs) const {

	Vector4f tmp(*this);
	tmp /= rhs;
	return tmp;
}

void Vector4f::normalize3() {
	float invDistance = 1.0f / sqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] = vec[0] * invDistance;
	vec[1] = vec[1] * invDistance;
	vec[2] = vec[2] * invDistance;
	vec[3] = vec[3] * invDistance;
}

unsigned int Vector4f::toUInt() const{
	unsigned int r = Math::Clamp(((unsigned int)(vec[0] * 255.0f)), 0u, 255u);
	unsigned int g = Math::Clamp(((unsigned int)(vec[1] * 255.0f)), 0u, 255u);
	unsigned int b = Math::Clamp(((unsigned int)(vec[2] * 255.0f)), 0u, 255u);
	unsigned int a = Math::Clamp(((unsigned int)(vec[3] * 255.0f)), 0u, 255u);
	return (a << 24) | (b << 16) | (g << 8) | r;
}

float Vector4f::Dot(const Vector4f &p, const Vector4f &q) {

	return (p[0] * q[0]) + (p[1] * q[1]) + (p[2] * q[2]) + (p[3] * q[3]);
}

float Vector4f::Dot(const Vector4f &p, const Vector3f &q) {
	return (p[0] * q[0]) + (p[1] * q[1]) + (p[2] * q[2]);
}

//friend operator
Vector4f operator*(float lhs, const Vector4f &rhs) {
	return Vector4f(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
}

Vector4f operator*(const Vector4f &lhs, float rhs) {
	return Vector4f(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
}

Vector4f operator/(const Vector4f &lhs, float rhs) {
	return Vector4f(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
}

Vector4f operator+(float lhs, const Vector4f &rhs) {
	return Vector4f(lhs + rhs[0], lhs + rhs[1], lhs + rhs[2], lhs + rhs[3]);
}

Vector4f operator+(const Vector4f &lhs, float rhs) {
	return Vector4f(lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs, lhs[3] + rhs);
}

//////////////////////////////////////////////////////////////////////
const Quaternion Quaternion::IDENTITY(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion() {
	quat[0] = 0.0f;
	quat[1] = 0.0f;
	quat[2] = 0.0f;
	quat[3] = 1.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w) {
	quat[0] = x;
	quat[1] = y;
	quat[2] = z;
	quat[3] = w;
}

Quaternion::Quaternion(float array[4]) {
	quat[0] = array[0];
	quat[1] = array[1];
	quat[2] = array[2];
	quat[3] = array[3];
}

Quaternion::Quaternion(float pitch, float roll, float yaw) {
	fromPitchYawRoll(pitch, roll, yaw);
}

Quaternion::Quaternion(Quaternion const& rhs) {
	quat[0] = rhs.quat[0];
	quat[1] = rhs.quat[1];
	quat[2] = rhs.quat[2];
	quat[3] = rhs.quat[3];
}

Quaternion::Quaternion(Quaternion&& rhs) {
	quat[0] = rhs.quat[0];
	quat[1] = rhs.quat[1];
	quat[2] = rhs.quat[2];
	quat[3] = rhs.quat[3];
}

Quaternion::Quaternion(const Vector3f &axis, float degrees) {
	fromAxisAngle(axis, degrees);
}

Quaternion::Quaternion(const Matrix4f &m) {
	fromMatrix(m);
}

float &Quaternion::operator[](int index) {
	return quat[index];
}

const float Quaternion::operator[](int index) const {
	return quat[index];
}

bool Quaternion::operator==(const Quaternion &rhs) const {
	float epsilon = 0.000001f;
	return fabs(quat[0] - rhs[0]) <= epsilon && fabs(quat[1] - rhs[1]) <= epsilon && fabs(quat[2] - rhs[2]) <= epsilon && fabs(quat[3] - rhs[3]) <= epsilon;
}

bool Quaternion::operator!=(const Quaternion &rhs) const {
	return !Quaternion::operator==(rhs);
}

Quaternion &Quaternion::operator=(const Quaternion &rhs) {
	quat[3] = rhs.quat[3], quat[0] = rhs.quat[0], quat[1] = rhs.quat[1], quat[2] = rhs.quat[2];
	return *this;
}

Quaternion &Quaternion::operator+=(const Quaternion &rhs) {
	quat[3] += rhs.quat[3], quat[0] += rhs.quat[0], quat[1] += rhs.quat[1], quat[2] += rhs.quat[2];
	return *this;
}

Quaternion &Quaternion::operator-=(const Quaternion &rhs) {
	quat[3] -= rhs.quat[3], quat[0] -= rhs.quat[0], quat[1] -= rhs.quat[1], quat[2] -= rhs.quat[2];
	return *this;
}

Quaternion &Quaternion::operator*=(const Quaternion &rhs) {

	// Multiply so that rotations are applied in a left to right order.
	/*Quaternion tmp(
		(quat[3] * rhs[0]) + (quat[0] * rhs[3]) - (quat[1] * rhs[2]) + (quat[2] * rhs[1]),
		(quat[3] * rhs[1]) + (quat[0] * rhs[2]) + (quat[1] * rhs[3]) - (quat[2] * rhs[0]),
		(quat[3] * rhs[2]) - (quat[0] * rhs[1]) + (quat[1] * rhs[0]) + (quat[2] * rhs[3]),
		(quat[3] * rhs[3]) - (quat[0] * rhs[0]) - (quat[1] * rhs[1]) - (quat[2] * rhs[2]));*/


	// Multiply so that rotations are applied in a right to left order.
	Quaternion tmp(
	(quat[3] * rhs[0]) + (quat[0] * rhs[3]) + (quat[1] * rhs[2]) - (quat[2] * rhs[1]),
	(quat[3] * rhs[1]) - (quat[0] * rhs[2]) + (quat[1] * rhs[3]) + (quat[2] * rhs[0]),
	(quat[3] * rhs[2]) + (quat[0] * rhs[1]) - (quat[1] * rhs[0]) + (quat[2] * rhs[3]),
	(quat[3] * rhs[3]) - (quat[0] * rhs[0]) - (quat[1] * rhs[1]) - (quat[2] * rhs[2]));
	*this = tmp;
	return *this;
}

Quaternion &Quaternion::operator*=(float scalar) {
	quat[3] *= scalar, quat[0] *= scalar, quat[1] *= scalar, quat[2] *= scalar;
	return *this;
}

Quaternion &Quaternion::operator/=(float scalar) {
	quat[3] /= scalar, quat[0] /= scalar, quat[1] /= scalar, quat[2] /= scalar;
	return *this;
}

Quaternion Quaternion::operator+(const Quaternion &rhs) const {
	Quaternion tmp(*this);
	tmp += rhs;
	return tmp;
}

Quaternion Quaternion::operator-(const Quaternion &rhs) const {
	Quaternion tmp(*this);
	tmp -= rhs;
	return tmp;
}

Quaternion Quaternion::operator*(const Quaternion &rhs) const {
	Quaternion tmp(*this);
	tmp *= rhs;
	return tmp;
}

Quaternion Quaternion::operator*(float scalar) const {
	Quaternion tmp(*this);
	tmp *= scalar;
	return tmp;
}

Quaternion Quaternion::operator/(float scalar) const {
	Quaternion tmp(*this);
	tmp /= scalar;
	return tmp;
}

void Quaternion::identity() {
	quat[0] = quat[1] = quat[2] = 0.0f, quat[3] = 1.0f;
}

float Quaternion::length() const {
	return sqrtf(quat[3] * quat[3] + quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2]);
}

void Quaternion::normalize() {
	float invMag = 1.0f / length();
	quat[3] *= invMag, quat[0] *= invMag, quat[1] *= invMag, quat[2] *= invMag;
}

Quaternion Quaternion::normalize() const {
	float invMag = 1.0f / length();
	return Quaternion(quat[0], quat[1], quat[2], quat[3]) * invMag;
}

void Quaternion::set(float x, float y, float z, float w) {
	quat[0] = x, quat[1] = y, quat[2] = z, quat[3] = w;
}

void Quaternion::set(const Vector3f &axis, float degrees) {
	if (axis.lengthSq() == 0.0f) {
		quat[0] = 0.0f, quat[1] = 0.0f, quat[2] = 0.0f, quat[3] = 1.0f;
	}
	else {
		fromAxisAngle(axis, degrees);
	}
}

void Quaternion::conjugate() {
	quat[0] = -quat[0]; quat[1] = -quat[1]; quat[2] = -quat[2];
}

void Quaternion::inverse() {
	float invMag = 1.0f / length();
	conjugate();
	quat[0] = quat[0] * invMag; quat[1] = quat[1] * invMag; quat[2] = quat[2] * invMag; quat[3] = quat[3] * invMag;
}

void Quaternion::fromAxisAngle(const Vector3f &axis, float degrees) {
	float halfTheta = degrees * PI_ON_180 * 0.5f;
	float s = -sinf(halfTheta);
	quat[3] = cosf(halfTheta), quat[0] = axis[0] * s, quat[1] = axis[1] * s, quat[2] = axis[2] * s;
}

void Quaternion::fromMatrix(const Matrix4f &m) {
	// Creates a quaternion from a rotation matrix. 
	// The algorithm used is from Allan and Mark Watt's "Advanced 
	// Animation and Rendering Techniques" (ACM Press 1992).
	float s = 0.0f;
	float q[4] = { 0.0f };
	float trace = m[0][0] + m[1][1] + m[2][2];

	if (trace > 0.0f) {
		s = sqrtf(trace + 1.0f);
		q[3] = s * 0.5f;
		s = 0.5f / s;
		q[0] = (m[2][1] - m[1][2]) * s;
		q[1] = (m[0][2] - m[2][0]) * s;
		q[2] = (m[1][0] - m[0][1]) * s;
	}else {

		int nxt[3] = { 1, 2, 0 };
		int i = 0, j = 0, k = 0;

		if (m[1][1] > m[0][0])
			i = 1;

		if (m[2][2] > m[i][i])
			i = 2;

		j = nxt[i];
		k = nxt[j];
		s = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

		q[i] = s * 0.5f;
		s = 0.5f / s;
		q[3] = (m[k][j] - m[j][k]) * s;
		q[j] = (m[j][i] + m[i][j]) * s;
		q[k] = (m[k][i] + m[i][k]) * s;
	}

	quat[0] = q[0], quat[1] = q[1], quat[2] = q[2], quat[3] = q[3];
}

//https://math.stackexchange.com/questions/2975109/how-to-convert-euler-angles-to-quaternions-and-get-the-same-euler-angles-back-fr
void Quaternion::fromPitchYawRoll(float pitch, float yaw, float roll) {
	pitch = pitch * HALF_PI_ON_180;
	yaw = yaw * HALF_PI_ON_180;
	roll = roll * HALF_PI_ON_180;

	float cosP = cosf(pitch);
	float sinP = sinf(pitch);
	float cosY = cosf(yaw);
	float sinY = sinf(yaw);
	float cosR = cosf(roll);
	float sinR = sinf(roll);

	quat[0] = sinP * cosY * cosR - cosP * sinY * sinR;
	quat[1] = cosP * sinY * cosR + sinP * cosY * sinR;
	quat[2] = cosP * cosY * sinR - sinP * sinY * cosR;
	quat[3] = cosP * cosY * cosR + sinP * sinY * sinR;
}

void Quaternion::rotate(float pitch, float yaw, float roll) {
	pitch = pitch * HALF_PI_ON_180;
	yaw = yaw * HALF_PI_ON_180;
	roll = roll * HALF_PI_ON_180;

	float cosP = cosf(pitch);
	float sinP = sinf(pitch);
	float cosY = cosf(yaw);
	float sinY = sinf(yaw);
	float cosR = cosf(roll);
	float sinR = sinf(roll);

	float rot0 = sinP * cosY * cosR - cosP * sinY * sinR;
	float rot1 = cosP * sinY * cosR + sinP * cosY * sinR;
	float rot2 = cosP * cosY * sinR - sinP * sinY * cosR;
	float rot3 = cosP * cosY * cosR + sinP * sinY * sinR;

	float q0 = quat[0];
	float q1 = quat[1];
	float q2 = quat[2];
	float q3 = quat[3];

	quat[0] = (q3 * rot0) + (q0 * rot3) + (q1 * rot2) - (q2 * rot1);
	quat[1] = (q3 * rot1) - (q0 * rot2) + (q1 * rot3) + (q2 * rot0);
	quat[2] = (q3 * rot2) + (q0 * rot1) - (q1 * rot0) + (q2 * rot3);
	quat[3] = (q3 * rot3) - (q0 * rot0) - (q1 * rot1) - (q2 * rot2);
}

void Quaternion::rotate(const Vector3f &axis, float degrees) {

	degrees = degrees * HALF_PI_ON_180;
	float s = -sinf(degrees);
	float rot3 = cosf(degrees), rot0 = axis[0] * s, rot1 = axis[1] * s, rot2 = axis[2] * s;


	float q0 = quat[0];
	float q1 = quat[1];
	float q2 = quat[2];
	float q3 = quat[3];

	quat[0] = (q3 * rot0) + (q0 * rot3) + (q1 * rot2) - (q2 * rot1);
	quat[1] = (q3 * rot1) - (q0 * rot2) + (q1 * rot3) + (q2 * rot0);
	quat[2] = (q3 * rot2) + (q0 * rot1) - (q1 * rot0) + (q2 * rot3);
	quat[3] = (q3 * rot3) - (q0 * rot0) - (q1 * rot1) - (q2 * rot2);
}

void Quaternion::toPitchYawRoll(float& pitch, float& yaw, float& roll) const {
	//Matrix4f m = toMatrix4f();
	//m.toHeadPitchRoll(yaw, pitch, roll);
	//yaw = yaw < 0.0f ? yaw + 360.0f : yaw;

	float yy = quat[1] * quat[1];
	float zz = quat[2] * quat[2];
	float ww = quat[2] * quat[2];

	float t5 = 1.0f - 2.0f * (quat[0] * quat[0] + yy);
	float t4 = 2.0f * (quat[0] * quat[2] - quat[3] * quat[1]);

	float t0 = 1.0f - 2.0f * (zz + ww);
	float t1 = 2.0f * (quat[1] * quat[2] + quat[0] * quat[3]);

	float t3 = 1.0f - 2.0f * (zz + yy);
	float t2 = 2.0f * (quat[2] * quat[3] + quat[0] * quat[1]);

	roll = fmod(360.0f - atan2f(t2, t3) * _180_ON_PI, 360.0f);
	pitch = fmod(atan2f(t1, t0) * _180_ON_PI + 180.0f, 360.0f);
	yaw = atan2f(t4, t5) * _180_ON_PI;
	yaw = yaw < 0.0f ? yaw + 360.0f : yaw;
}

float Quaternion::getPitch() const {
	return fmod(atan2f(2.0f * (quat[1] * quat[2] + quat[0] * quat[3]), -2.0f * (quat[2] * quat[2] + quat[3] * quat[3]) + 1.0f) * _180_ON_PI + 180.0f, 360.0f);
}

float Quaternion::getYaw() const {
	//float yaw = atan2f(2.0f * (quat[0] * quat[2] - quat[3] * quat[1]), 1.0f - 2.0f * (quat[0] * quat[0] + quat[1] * quat[1])) * _180_ON_PI;
	//return yaw < 0.0f ? yaw + 360.0f : yaw;
	return 2 * acosf(quat[3]) * _180_ON_PI;
}

float Quaternion::getRoll() const {
	return  fmod(360.0f - atan2f(2.0f * (quat[2] * quat[3] + quat[0] * quat[1]), -2.0f * (quat[1] * quat[1] + quat[2] * quat[2]) + 1.0f) * _180_ON_PI, 360.0f);
}

void Quaternion::toAxisAngle(Vector3f &axis, float &degrees) const {
	// Converts this quaternion to an axis and an angle.
	float sinHalfThetaSq = 1.0f - quat[3] * quat[3];

	// Guard against numerical imprecision and identity quaternions.
	if (sinHalfThetaSq <= 0.0f) {

		axis[0] = 1.0f, axis[1] = axis[2] = 0.0f;
		degrees = 0.0f;

	}else {

		float invSinHalfTheta = 1.0f / sqrtf(sinHalfThetaSq);

		axis[0] = quat[0] * invSinHalfTheta;
		axis[1] = quat[1] * invSinHalfTheta;
		axis[2] = quat[2] * invSinHalfTheta;
		degrees = acosf(quat[2]) * _180_ON_PI * 2.0f;
	}
}

Vector3f Quaternion::getRotationAxis() {
	float sinHalfThetaSq = 1.0f - quat[3] * quat[3];
	if (sinHalfThetaSq <= 0.0f) {
		return Vector3f(1.0f, 0.0f, 0.0f);
	}else {
		float invSinHalfTheta = 1.0f / sqrtf(sinHalfThetaSq);
		return Vector3f(quat[0] * invSinHalfTheta, quat[1] * invSinHalfTheta, quat[2] * invSinHalfTheta);
	}
}

const Matrix4f Quaternion::toMatrix4f() const {
	// Converts this quaternion to a rotation matrix.
	//
	//  | 1 - 2(y^2 + z^2)	2(xy - wz)			2(xz + wy)			0  |
	//  | 2(xy + wz)		1 - 2(x^2 + z^2)	2(yz - wx)			0  |
	//  | 2(xz - wy)		2(yz + wx)			1 - 2(x^2 + y^2)	0  |
	//  | 0					0					0					1  |

	float x2 = quat[0] + quat[0];
	float y2 = quat[1] + quat[1];
	float z2 = quat[2] + quat[2];
	float xx = quat[0] * x2;
	float xy = quat[0] * y2;
	float xz = quat[0] * z2;
	float yy = quat[1] * y2;
	float yz = quat[1] * z2;
	float zz = quat[2] * z2;
	float wx = quat[3] * x2;
	float wy = quat[3] * y2;
	float wz = quat[3] * z2;

	return Matrix4f(1.0f - (yy + zz), xy + wz, xz - wy, 0.0f,
		xy - wz, 1.0f - (xx + zz), yz + wx, 0.0f,
		xz + wy, yz - wx, 1.0f - (xx + yy), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

const Matrix4f Quaternion::toMatrix4f(const Vector3f &centerOfRotation) const {
	
	float x2 = quat[0] + quat[0];
	float y2 = quat[1] + quat[1];
	float z2 = quat[2] + quat[2];
	float xx = quat[0] * x2;
	float xy = quat[0] * y2;
	float xz = quat[0] * z2;
	float yy = quat[1] * y2;
	float yz = quat[1] * z2;
	float zz = quat[2] * z2;
	float wx = quat[3] * x2;
	float wy = quat[3] * y2;
	float wz = quat[3] * z2;

	return Matrix4f(1.0f - (yy + zz), xy + wz, xz - wy, 0.0f,
                    xy - wz, 1.0f - (xx + zz), yz + wx, 0.0f,
                    xz + wy, yz - wx, 1.0f - (xx + yy), 0.0f,
                    centerOfRotation[0] * (yy + zz) - centerOfRotation[1] * (xy - wz) - centerOfRotation[2] * (xz + wy), 
					centerOfRotation[1] * (xx + zz) - centerOfRotation[0] * (xy + wz) - centerOfRotation[2] * (yz - wx), 
					centerOfRotation[2] * (xx + yy) - centerOfRotation[0] * (xz - wy) - centerOfRotation[1] * (yz + wx), 1.0f);
}

void Quaternion::Normalize(Quaternion &q) {
	q.normalize();
}

Quaternion& Quaternion::FromMatrix(Quaternion &quat, const Matrix4f &m) {
	quat.fromMatrix(m);
	return quat;
}

Quaternion& Quaternion::Conjugate(Quaternion &quat) {
	quat.conjugate();
	return quat;
}

Quaternion& Quaternion::Inverse(Quaternion &quat) {
	quat.inverse();
	return quat;
}

//https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
Vector3f Quaternion::Rotate(const Quaternion &quat, const Vector3f &v) {
	Vector3f u(quat[0], quat[1], quat[2]);
	float s = quat[3];
	return 2.0f * Vector3f::Dot(u, v) * u + (s*s - Vector3f::Dot(u, u)) * v + 2.0f * s *  Vector3f::Cross(u, v);

	//Vector3f u(quat[0], quat[1], quat[2]);
	//float s = quat[3];
	//return 2.0f * Vector3f::Dot(u, v) * u + (s*s - Vector3f::Dot(u, u)) * v + 2.0f * s *  Vector3f::Cross(v, u);
}

Vector3f Quaternion::InvRotate(const Quaternion &quat, const Vector3f &v) {
	Vector3f u(quat[0], quat[1], quat[2]);
	float s = quat[3];
	return 2.0f * Vector3f::Dot(u, v) * u + (s*s - Vector3f::Dot(u, u)) * v + 2.0f * s *  Vector3f::Cross(v, u);
}

//friend operator
Quaternion operator*(float lhs, const Quaternion &rhs) {
	return rhs * lhs;
}

//friend operator
bool operator ==(const Quaternion &lhs, const Quaternion &rhs) {
	float epsilon = 0.000001f;
	return fabs(lhs[0] - rhs[0]) <= epsilon && fabs(lhs[1] - rhs[1]) <= epsilon && fabs(lhs[2] - rhs[2]) <= epsilon && fabs(lhs[3] - rhs[3]) <= epsilon;
}

Quaternion Quaternion::SLerp(const Quaternion &a, const Quaternion &b, float t) {
	Quaternion result = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	float dot = a[3] * b[3] + a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	float t1 = 1.0f - t;

	if (dot < 0.0f) {
		result[3] = t1 * a[3] - t * b[3];
		result[0] = t1 * a[0] - t * b[0];
		result[1] = t1 * a[1] - t * b[1];
		result[2] = t1 * a[2] - t * b[2];

	}else {
		result[3] = t1 * a[3] + t * b[3];
		result[0] = t1 * a[0] + t * b[0];
		result[1] = t1 * a[1] + t * b[1];
		result[2] = t1 * a[2] + t * b[2];
	}

	Quaternion::Normalize(result);
	return result;
}

Quaternion Quaternion::SLerp2(const Quaternion &a, const Quaternion &b, float t) {
	float cosAngle = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3] ;
	float sign = 1.0f;

	if (cosAngle < 0.0f){
		cosAngle = -cosAngle;
		sign = -sign;
	}

	float angle = acosf(cosAngle);
	float sinAngle = sinf(angle);
	float t1, t2;

	if (sinAngle > 0.001f){
		float invSinAngle = 1.0f / sinAngle;
		t1 = sinf((1.0f - t) * angle) * invSinAngle;
		t2 = sinf(t * angle) * invSinAngle;

	}else{
		t1 = 1.0f - t;
		t2 = t;
	}

	return a * t1 + sign * b * t2;
}