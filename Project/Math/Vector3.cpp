#include "Vector3.h"

Vector3 Vector3::up(0, 1, 0);
Vector3 Vector3::down(0,-1,0);
Vector3 Vector3::back(0, 0, -1);
Vector3 Vector3::forward(0,0,1);
Vector3 Vector3::left(1, 0, 0);
Vector3 Vector3::right(-1, 0, 0);
Vector3 Vector3::zero(0,0,0);
Vector3 Vector3::one(1, 1, 1);

Vector3::Vector3()
{
	x = y = z = 0.0f;
}

Vector3::Vector3(float _x, float _y, float _z) :x(_x), y(_y), z(_z)
{
	assert(!HasNaNs());
}


Vector3::Vector3(float value)
{
	x = y = z = value;
}

Vector3::~Vector3()
{
	//dtor
}


bool Vector3::HasNaNs() const
{
	return isnan(x) || isnan(y) || isnan(z);
}


float Vector3::magnitude() const
{

	return sqrt(x*x + y*y + z*z);

}

float Vector3::sqrtMagnitude() const
{
	return x*x + y*y + z*z;
}

void Vector3::set(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

Vector3 Vector3::operator+(const Vector3 &v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3 Vector3::operator-(const Vector3 &v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

/*
Vector3 Vector3::operator*(float num) const
{
	return Vector3(num*x, num*y, num*z);
}*/
Vector3 operator * (const Vector3& lhs, double rhs)
{
	return Vector3(rhs*lhs.x, rhs*lhs.y, rhs*lhs.z);
}

Vector3 operator * (double lhs, const Vector3& rhs)
{
	return Vector3(lhs*rhs.x, lhs*rhs.y, lhs*rhs.z);

}

Vector3 Vector3::operator/(float num) const
{
	return Vector3(x / num, y / num, z / num);
}

Vector3& Vector3::operator*=(float num)
{
	x *= num; y *= num; z *= num;
	return *this;
}

Vector3& Vector3::operator/=(float num)
{
	x /= num; y /= num; z /= num;
	return *this;
}

Vector3& Vector3::operator+=(const Vector3 &v)
{
	x += v.x; y += v.y; z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3 &v)
{
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

bool Vector3::operator!=(Vector3 &v) const
{
	return x == v.x && y == v.y && z == v.z;
}


Vector3 Vector3::normalized()
{
	float tmpLength = this->magnitude();
	return Vector3(x / tmpLength, y / tmpLength, z / tmpLength);
}

void Vector3::Normalize()
{
	float lengthInv = 1.0 / this->magnitude();
	this->x *= lengthInv;
	this->y *= lengthInv;
	this->z *= lengthInv;
}