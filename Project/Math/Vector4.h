#pragma once
#include "Base/Color.h"
#include "Math/Vector3.h"

class Vector4
{
public:
	float x, y, z, w;

	Vector4() : x(0), y(0), z(0), w(0) {};
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) 
	{
		xyz.x = x;
		xyz.y = y;
		xyz.z = z;
	};
	Vector4(Vector3& vec, float w) : x(vec.x), y(vec.y), z(vec.z), w(w) 
	{
		xyz.x = x;
		xyz.y = y;
		xyz.z = z;
	};
	Vector4(Color c): x(c.r), y(c.g), z(c.b), w(c.a) {};
	~Vector4() {};

	void set(float _x, float _y, float _z, float _w);

	Vector4 operator-(const Vector4 &v) const;
	Vector4 operator*(float num) const;

	Vector4 &operator/=(float num);
	Vector4 operator/(float num) const;

	Vector4 operator+(const Vector4 &v) const;

	static inline Vector4 Lerp(const Vector4 &v1, const Vector4 &v2, float factor)
	{
		return v1 + (v2 - v1) * factor;
	}

	Vector3 xyz;
protected:
private:
};