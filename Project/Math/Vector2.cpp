#include "Vector2.h"

Vector2i::Vector2i()
{
	x = y = 0;
}

Vector2i::Vector2i(int _x, int _y) :x(_x), y(_y)
{
}


Vector2i::~Vector2i()
{
	//dtor
}



Vector2::Vector2()
{
	x = y = 0.0f;
}

Vector2::Vector2(float _x, float _y) :x(_x), y(_y)
{
}


Vector2::~Vector2()
{
	//dtor
}

void Vector2::set(float _x, float _y)
{
	x = _x;
	y = _y;
}

Vector2 Vector2::operator+(const Vector2 &v) const
{
	return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator-(const Vector2 &v) const
{
	return Vector2(x - v.x, y - v.y);
}

Vector2 operator * (const Vector2& lhs, double rhs)
{
	return Vector2(rhs*lhs.x, rhs*lhs.y);
}

Vector2 operator * (double lhs, const Vector2& rhs)
{
	return Vector2(lhs*rhs.x, lhs*rhs.y);

}

Vector2 Vector2::operator/(float num) const
{
	return Vector2(x / num, y / num);
}

Vector2& Vector2::operator*=(float num)
{
	x *= num; y *= num;
	return *this;
}

Vector2& Vector2::operator/=(float num)
{
	x /= num; y /= num;
	return *this;
}

Vector2& Vector2::operator+=(const Vector2 &v)
{
	x += v.x; y += v.y; 
	return *this;
}

Vector2& Vector2::operator-=(const Vector2 &v)
{
	x -= v.x; y -= v.y;
	return *this;
}

bool Vector2::operator!=(Vector2 &v) const
{
	return x == v.x && y == v.y;
}
