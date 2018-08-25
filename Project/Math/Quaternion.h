#pragma once
#include "Vector3.h"
#include "Matrix.h"
#include "Mathf.h"
class Quaternion
{
public: 
	Quaternion();
	Quaternion(float x, float y, float z, float w);
	~Quaternion();
	static Quaternion identity;
	static float Dot(const Quaternion &lhs, const Quaternion &rhs);
	static Quaternion Lerp(const Quaternion &a, const Quaternion &b, float t);
	static Quaternion Slerp(const Quaternion &a, const Quaternion &b, float t);
	static Quaternion Euler(float x, float y, float z);
	static float Angle(const Quaternion &lhs, const Quaternion &rhs);
	//void SetEulerAngle(float yaw, float pitch, float roll);
	void Set(float _x, float _y, float _z, float _w);
	Matrix4x4 GetRotMatrix();

	Quaternion Conjugate() const;
	Quaternion Inverse() const;
	Vector3 EulerAngle() const;

	void operator+(const Quaternion &q);
	void operator*(float s);
	void operator-(const Quaternion &q);
	friend Quaternion operator * (const Quaternion& lhs, const Quaternion& rhs);
	friend Vector3 operator *(const Quaternion& rotation, const Vector3& point);

	float x;
	float y;
	float z;
	float w;

private: 
	
	Vector3 eulerAngles;
};