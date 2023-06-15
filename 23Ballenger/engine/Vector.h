#ifndef _VECTOR_H
#define _VECTOR_H

#include <cfloat>
#include <cmath>
#include <algorithm>
//-----------------------------------------------------------------------------
// Common math functions and constants.
//-----------------------------------------------------------------------------
#define PI  3.1415926535897932384f
#define HALF_PI  1.5707963267948966192f
#define TWO_PI  6.2831853071795864769f 
#define PI_ON_180  0.0174532925199432957f
#define HALF_PI_ON_180  0.0087266462599716479f
#define _180_ON_PI  57.295779513082320877f
#define invPI  0.3183098861837906715f
#define	invTWO_PI  0.1591549430918953358f
#define	EPSILON  1e-6f

namespace Math {

	inline bool CloseEnough(float f1, float f2) {
		return fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < EPSILON;
	}

	template <typename T>
	inline T bilerp(const T &a, const T &b, const T &c, const T &d, float u, float v) {
		// Performs a bilinear interpolation.
		//  P(u,v) = e + v(f - e)
		//  
		//  where
		//  e = a + u(b - a)
		//  f = c + u(d - c)
		//  u in range [0,1]
		//  v in range [0,1]

		return a * ((1.0f - u) * (1.0f - v))
			+ b * (u * (1.0f - v))
			+ c * (v * (1.0f - u))
			+ d * (u * v);
	}

	template <typename T>
	inline T  Mix(const T &p, const T &q, float a) {
		return p * (1 - a) + q * a;
	}
};

class Vector2f {

	friend Vector2f operator-(const Vector2f &v);
	friend bool operator ==(Vector2f lhs, Vector2f rhs);

public:

	Vector2f();
	Vector2f(float x_, float y_);
	Vector2f(const Vector2f &rhs);
	~Vector2f();

	void set(float x_, float y_);

	float &operator[](int index);
	const float operator[](int index) const;

	const float* getVec()const;

	Vector2f& operator= (const Vector2f& rhs);

	Vector2f &operator+=(const Vector2f &rhs);
	Vector2f &operator-=(const Vector2f &rhs);
	Vector2f &operator*=(float scalar);
	Vector2f &operator/=(float scalar);

	Vector2f operator+(const Vector2f &rhs) const;
	Vector2f operator-(const Vector2f &rhs) const;
	Vector2f operator*(float scalar) const;
	Vector2f operator/(float scalar) const;


private:

	float vec[2];

};

class Vector3f {

	friend Vector3f operator-(const Vector3f &v);
	friend Vector3f operator*(float lhs, const Vector3f &rhs);
	friend bool operator ==(Vector3f lhs, Vector3f rhs);
	friend bool operator !=(Vector3f lhs, Vector3f rhs);

public:

	Vector3f();
	Vector3f(float x_);
	Vector3f(float x_, float y_, float z_);
	~Vector3f();

	Vector3f& normalize();
	float length() const;
	float lengthSq() const;

	void set(float x_, float y_, float z_);
	void translate(const Vector3f &rhs);
	void translate(const float dx, const float dy, const float dz);
	void scale(const Vector3f &rhs);
	void scale(const float sx, const float sy, const float sz);
	bool compare(const Vector3f &rhs, float precision);

	float &operator[](int index);
	const float operator[](int index) const;

	const float* getVec()const;

	Vector3f &operator+=(const Vector3f &rhs);
	Vector3f &operator-=(const Vector3f &rhs);
	Vector3f &operator*=(const Vector3f &rhs);
	Vector3f &operator/=(const Vector3f &rhs);
	Vector3f &operator*=(float scalar);
	Vector3f &operator/=(float scalar);

	Vector3f operator+(const Vector3f &rhs) const;
	Vector3f operator-(const Vector3f &rhs) const;
	Vector3f operator*(const Vector3f &rhs) const;
	Vector3f operator/(const Vector3f &rhs) const;

	Vector3f operator*(float scalar) const;
	Vector3f operator/(float scalar) const;

	bool zero();

	static Vector3f Cross(const Vector3f &p, const Vector3f &q);
	static float Dot(const Vector3f &p, const Vector3f &q);	
	static Vector3f& Normalize(Vector3f &p);
	static Vector3f Min(const Vector3f &p, const Vector3f &q);
	static Vector3f Max(const Vector3f &p, const Vector3f &q);
	static Vector3f& Clamp(Vector3f &v, float min, float max);
	static Vector3f Fract(const Vector3f &p);

private:
	float vec[3];
};

class Vector4f {

	friend Vector4f operator*(float lhs, const Vector4f &rhs);
	friend Vector4f operator*(const Vector4f &lhs, float rhs);
	friend Vector4f operator/(const Vector4f &lhs, float rhs);
	friend Vector4f operator+(float lhs, const Vector4f &rhs);
	friend Vector4f operator+(const Vector4f &lhs, float rhs);

public:

	Vector4f();
	Vector4f(float x_, float y_, float z_, float w_);
	Vector4f(const Vector3f &rhs, float w = 1.0f);
	~Vector4f();

	float &operator[](int index);
	const float operator[](int index) const;

	Vector4f &operator+=(const Vector4f &rhs);
	Vector4f &operator-=(const Vector4f &rhs);
	Vector4f &operator*=(const Vector4f &rhs);
	Vector4f &operator/=(const Vector4f &rhs);
	Vector4f &operator*=(float scalar);
	Vector4f &operator/=(float scalar);

	Vector4f operator+(const Vector4f &rhs) const;
	Vector4f operator-(const Vector4f &rhs) const;
	Vector4f operator*(const Vector4f &rhs) const;
	Vector4f operator/(const Vector4f &rhs) const;


	void normalize3();

	static float Dot(const Vector4f &p, const Vector4f &q);
	static float Dot(const Vector4f &p, const Vector3f &q);
private:

	float vec[4];
};

class Quaternion;
class Matrix4f {

	friend Vector3f operator*(const Vector4f &lhs, const Matrix4f &rhs);
	friend Vector3f operator*(const Matrix4f &rhs, const Vector4f &lhs);
	friend Vector3f operator*(const Vector3f &lhs, const Matrix4f &rhs);
	friend Vector3f operator*(const Matrix4f &lhs, const Vector3f &rhs);
	friend Vector4f operator^(const Vector4f &rhs, const Matrix4f &lhs);
	friend Vector4f operator^(const Matrix4f &lhs, const Vector4f &rhs);
	friend Matrix4f operator*(float scalar, const Matrix4f &rhs);

public:

	static const Matrix4f IDENTITY;
	static const Matrix4f BIAS;
	static const Matrix4f SIGN;

	Matrix4f();
	Matrix4f(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);
	//Matrix4f(const Matrix4f& rhs);
	~Matrix4f();

	float *operator[](int row);
	const float *operator[](int row) const;
	Matrix4f &operator+=(const Matrix4f &rhs);
	Matrix4f &operator*=(const Matrix4f &rhs);
	Matrix4f &operator^=(const Matrix4f &rhs);
	//Matrix4f &operator= (const Matrix4f &rhs);

	Matrix4f operator+(const Matrix4f &rhs) const;
	Matrix4f operator*(const Matrix4f &rhs) const;
	Matrix4f operator^(const Matrix4f &rhs) const;
	void transpose();
	void transpose3();
	Matrix4f inverse() const;
	float determinant() const;

	void identity();

	void rotate(const Vector3f &axis, float degrees);
	void rotate(const Quaternion &orientation);
	void rotate(float pitch, float yaw, float roll);

	void rotate(const Vector3f &axis, float degrees, const Vector3f &centerOfRotation);
	void rotate(const Quaternion &orientation, const Vector3f &centerOfRotation);
	void rotate(float pitch, float yaw, float roll, const Vector3f &centerOfRotation);

	void invRotate(const Vector3f &axis, float degrees);
	void invRotate(const Quaternion &orientation);
	void invRotate(float pitch, float yaw, float roll);

	void invRotate(const Vector3f &axis, float degrees, const Vector3f &centerOfRotation);
	void invRotate(const Quaternion &orientation, const Vector3f &centerOfRotation);
	void invRotate(float pitch, float yaw, float roll, const Vector3f &centerOfRotation);

	void translate(float dx, float dy, float dz);
	void translate(const Vector3f &trans);

	void invTranslate(float dx, float dy, float dz);
	void invTranslate(const Vector3f &trans);

	void scale(float a, float b, float c);
	void scale(const Vector3f &scale);

	void scale(float a, float b, float c, const Vector3f &centerOfScale);
	void scale(const Vector3f &scale, const Vector3f &centerOfScale);

	void invScale(float a, float b, float c);
	void invScale(const Vector3f &scale);

	void invScale(float a, float b, float c, const Vector3f &centerOfScale);
	void invScale(const Vector3f &scale, const Vector3f &centerOfScale);

	void perspective(float fovx, float aspect, float znear, float zfar);
	//void perspective(float left, float right, float bottom, float top, float znear, float zfar);
	void orthographic(float left, float right, float bottom, float top, float znear, float zfar);
	void perspectiveD3D(float fovx, float aspect, float znear, float zfar);
	void linearPerspectiveD3D(float fovx, float aspect, float znear, float zfar);
	void invPerspective(float fovx, float aspect, float znear, float zfar);
	void invOrthographic(float left, float right, float bottom, float top, float znear, float zfar);
	void lookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);
	void invLookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);

	void toHeadPitchRoll(float &pitch, float &yaw, float &roll) const;

	void print() const;
	void set(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);

	void set(const Matrix4f &rhs);

	static Matrix4f GetNormalMatrix(const Matrix4f &modelViewMatrix);
	static Matrix4f &GetNormalMatrix(Matrix4f &mtx, const Matrix4f &modelViewMatrix);

	static Matrix4f Scale(float x, float y, float z);
	static Matrix4f Scale(const Vector3f &scale);
	static Matrix4f &Scale(Matrix4f &mtx, float x, float y, float z);
	static Matrix4f Scale(float x, float y, float z, const Vector3f &centerOfScale);

	static Matrix4f Translate(float dx, float dy, float dz);
	static Matrix4f Translate(const Vector3f &trans);
	static Matrix4f &Translate(Matrix4f &mtx, float dx, float dy, float dz);

	static Matrix4f InvTranslate(float dx, float dy, float dz);
	static Matrix4f InvTranslate(const Vector3f &trans);
	static Matrix4f &InvTranslate(Matrix4f &mtx, float dx, float dy, float dz);

	static Matrix4f Rotate(const Vector3f &axis, float degrees);
	static Matrix4f Rotate(const Quaternion &orientation);
	static Matrix4f Rotate(float pitch, float yaw, float roll);

	static Matrix4f Rotate(const Vector3f &axis, float degrees, const Vector3f &centerOfRotation);
	static Matrix4f Rotate(const Quaternion &orientation, const Vector3f &centerOfRotation);
	static Matrix4f Rotate(float pitch, float yaw, float roll, const Vector3f &centerOfRotation);

	static Matrix4f &Rotate(Matrix4f &mtx, const Vector3f &axis, float degrees);
	static Matrix4f &Rotate(Matrix4f &mtx, const Vector3f &axis, float degrees, const Vector3f &centerOfRotation);

	static Matrix4f Rotate(const Vector3f &direction);
	static Matrix4f Rotate(const Vector3f &direction, const Vector3f &poisiton);

	static Matrix4f InvRotate(const Vector3f &axis, float degrees);
	static Matrix4f InvRotate(const Quaternion &orientation);
	static Matrix4f InvRotate(float pitch, float yaw, float roll);

	static Matrix4f InvRotate(const Vector3f &axis, float degrees, const Vector3f &centerOfRotation);
	static Matrix4f InvRotate(const Quaternion &orientation, const Vector3f &centerOfRotation);
	static Matrix4f InvRotate(float pitch, float yaw, float roll, const Vector3f &centerOfRotation);

	static Matrix4f &InvRotate(Matrix4f &mtx, const Vector3f &axis, float degrees);
	static Matrix4f &InvRotate(Matrix4f &mtx, const Vector3f &axis, float degrees, const Vector3f &centerOfRotation);

	static Matrix4f Perspective(float fovx, float aspect, float znear, float zfar);
	static Matrix4f &Perspective(Matrix4f &mtx, float fovx, float aspect, float znear, float zfar);
	static Matrix4f Perspective(float left, float right, float bottom, float top, float znear, float zfar);

	static Matrix4f LookAt(const Vector3f &eye, const Vector3f &target, const Vector3f &up);

	static Matrix4f InvPerspective(float fovx, float aspect, float znear, float zfar);
	static Matrix4f &InvPerspective(Matrix4f &mtx, float fovx, float aspect, float znear, float zfar);
	static Matrix4f InvPerspective(const Matrix4f &pers);

	static Matrix4f Orthographic(float left, float right, float bottom, float top, float znear, float zfar);
	static Matrix4f &Orthographic(Matrix4f &mtx, float left, float right, float bottom, float top, float znear, float zfar);

	static Matrix4f InvOrthographic(float left, float right, float bottom, float top, float znear, float zfar);
	static Matrix4f &InvOrthographic(Matrix4f &mtx, float left, float right, float bottom, float top, float znear, float zfar);
	static Matrix4f InvOrthographic(const Matrix4f &ortho);

	static Matrix4f InvViewMatrix(const Matrix4f &viewMatrix);

	static Matrix4f &Transpose(Matrix4f &m);
	static Matrix4f Transpose(const Matrix4f &m);
	static Matrix4f Inverse(Matrix4f &m);

private:

	float mtx[4][4];
};

class Quaternion {

	friend Quaternion operator*(float lhs, const Quaternion &rhs);
	friend bool operator ==(const Quaternion &lhs, const Quaternion &rhs);

public:

	static const Quaternion IDENTITY;

	Quaternion();
	Quaternion(float x, float y, float z, float w);
	Quaternion(float pitch, float yaw, float roll);
	Quaternion(const Vector3f &axis, float degrees);
	explicit Quaternion(const Matrix4f &m);
	~Quaternion() {}

	float &operator[](int index);
	const float operator[](int index) const;
	bool operator==(const Quaternion &rhs) const;
	bool operator!=(const Quaternion &rhs) const;

	Quaternion &operator+=(const Quaternion &rhs);
	Quaternion &operator-=(const Quaternion &rhs);
	Quaternion &operator*=(const Quaternion &rhs);
	Quaternion &operator*=(float scalar);
	Quaternion &operator/=(float scalar);

	Quaternion operator+(const Quaternion &rhs) const;
	Quaternion operator-(const Quaternion &rhs) const;
	Quaternion operator*(const Quaternion &rhs) const;
	Quaternion operator*(float scalar) const;
	Quaternion operator/(float scalar) const;

	void identity();
	float length() const;
	void normalize();
	void set(float x, float y, float z, float w);
	void set(const Vector3f &axis, float degrees);
	void conjugate();
	void inverse();
	void rotate(float pitch, float yaw, float roll);
	void rotate(const Vector3f &axis, float degrees);

	void fromAxisAngle(const Vector3f &axis, float degrees);
	void fromMatrix(const Matrix4f &m);
	void fromPitchYawRoll(float pitch, float yaw, float roll);

	void toPitchYawRoll(float& pitch, float& yaw, float& roll) const;
	void toAxisAngle(Vector3f &axis, float &degrees) const;
	const Matrix4f toMatrix4f() const;
	const Matrix4f toMatrix4f(const Vector3f &centerOfRotation) const;
	float getPitch() const;
	float getYaw() const;
	float getRoll() const;

	static Quaternion& FromMatrix(Quaternion &quat, const Matrix4f &m);
	static Quaternion& Conjugate(Quaternion &quat);
	static Quaternion& Inverse(Quaternion &quat);
	static void Normalize(Quaternion &p);

private:

	float quat[4];
};
#endif