#pragma once
#include <math.h>
#include "Math/Mathf.h"
#include <assert.h>
#include <iostream>
#include <stdint.h>


#define PI 3.14159265
using namespace std;

class Color
{
private:


public: 
	float r;
	float g;
	float b;
	float a;
	uint8_t uintR;
	uint8_t uintG;
	uint8_t uintB;
	uint8_t uintA;
	static Color red;
	static Color white;
	static Color green;
	static Color blue;
	static Color black;
	Color();
	Color(float r, float g, float b, float a = 1);
	int GetUintR();
	int GetUintG();
	int GetUintB();
	int GetUintA();

	static Color RandomColor();

	friend std::ostream& operator<<(std::ostream& output, const Color& color)
	{
		output << "(" << color.r << "," << color.g << "," << color.b << ")" << "\n";
		return output;
	}

	static inline Color Lerp(const Color& start, const Color& end, float t)
	{
		float r = Mathf::Lerp(start.r, end.r, t);
		float g = Mathf::Lerp(start.g, end.g, t);
		float b = Mathf::Lerp(start.b, end.b, t);
		float a = Mathf::Lerp(start.a, end.a, t);
		return Color(r, g, b, a);
	}

};
