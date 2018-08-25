#include "Vector4.h"

void Vector4::set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _x;
	z = _x;
	w = _x;

	xyz.x = x;
	xyz.y = y;
	xyz.z = z;
	
}

Vector4& Vector4::operator/=(float num)
{
	x /= num; y /= num; z /= num;
	return *this;
}

Vector4 Vector4::operator/(float num) const
{
	return Vector4(x / num, y / num, z / num, w/num);
}

Vector4 Vector4::operator-(const Vector4 &v) const
{
	return Vector4(x - v.x, y - v.y, z - v.z, w-v.w);
}

Vector4 Vector4::operator+(const Vector4 &v) const
{
	return Vector4(x + v.x, y + v.y, z + v.z, w+ v.w);
}


Vector4 Vector4::operator*(float num) const
{
	return Vector4(x*num, y*num, z*num, w*num);
}

