#pragma once
#include <math.h>
#include <assert.h>
#include <iostream>

#define PI 3.14159265
using namespace std;

class Vector2i
{
	public:
	int x;
	int y;
	Vector2i();
	Vector2i(int x, int y);
	~Vector2i();
	int& operator[](int i)
	{
		switch(i)
		{
			case 0:
			return x;
			case 1:
			return y;
		}
		return x;
	}
};

class Vector2
{
public:
	float x;
	float y;
	Vector2();
	Vector2(float x, float y);
	~Vector2();
	void set(float _x, float _y);
	//Add two Vectors
	Vector2 operator+(const Vector2 &v) const;
	//Subtracts one vector from another
	Vector2 operator-(const Vector2 &v) const;
	Vector2 operator/(float num) const;
	Vector2 &operator+=(const Vector2 &v);
	Vector2 &operator-=(const Vector2 &v);
	Vector2 &operator*=(float num);
	friend Vector2 operator * (const Vector2& lhs, double rhs);
	friend Vector2 operator * (double lhs, const Vector2& rhs);

	Vector2 &operator/=(float num);
	//Returns true if vectors different.
	bool operator!=(Vector2 &v) const;

	static inline Vector2 Lerp(const Vector2 &v1, const Vector2 &v2, float factor)
	{
		return v1 + (v2 - v1) * factor;
	}

	float& operator[](int i)
	{
		switch(i)
		{
			case 0:
			return x;
			case 1:
			return y;
		}
		return x;
	}

};