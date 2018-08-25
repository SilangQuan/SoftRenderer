#pragma once
#include <cmath>
#include <cfloat>

class Mathf
{
public:
	Mathf() {}
	~Mathf() {}

	static float Pi;
	static float Rad2Deg;
	static float Deg2Rad;
	static float Infinity;
	static float NegativeInfinity;
	static float EPSILON;
	static float Clamp(float x, float min =0, float max = 1)
	{
		return fmin(max, fmax(x, min));
	}
	static float Min(float a, float b)
	{
		return fmin(a, b);
	}

	static float Max(float a, float b)
	{
		return fmax(a, b);
	}

	static float Acos(float theta)
	{
		return acos(theta);
	}

	static float Asin(float theta)
	{
		return asin(theta);
	}

	static float Sin(float theta)
	{
		return sin(theta);
	}

	static float Cos(float theta)
	{
		return cos(theta);
	}

	static float Tan(float theta)
	{
		return tan(theta);
	}

	static inline float Lerp(float a, float b, float t)
	{
		return a + (b - a) * t;
	}
};



