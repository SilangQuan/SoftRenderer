#include "Color.h"
#include "Math/Random.h"
#include "Math/Mathf.h"

Color Color::red(1, 0, 0, 1);
Color Color::white(1, 1, 1, 1);
Color Color::green(0, 1, 0, 1);
Color Color::blue(0, 0, 1, 1);
Color Color::black(0, 0, 0, 1);

Color::Color()
{

}

Color::Color(uint32 c)
{
	uintR =  c >> 16 & 0xFF;
	uintG =  c >>  8 & 0xFF;
	uintB =  c & 0xFF;
	uintA = 255;

	float inv255 = 1.0 / 255;

	r = uintR * inv255;
	g = uintG* inv255;
	b = uintB* inv255;
	a = 1;
}

Color::Color(float _r, float _g, float _b, float _a)
{
	r = _r;
	g = _g;
	b = _b;
	a = _a;
	
	uintR = (uint8_t)(r * 255);
	uintG = (uint8_t)(g * 255);
	uintB = (uint8_t)(b * 255);
	uintA = (uint8_t)(a * 255);
}

Color::Color(const Color& color)	
{
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;

	uintR = color.uintR;
	uintG = color.uintG;
	uintB = color.uintB;
	uintA = color.uintA;
} 


//void SetData(uint32 r, uint32 g, uint32 b, uint32 a, bool needRefresh);

/*
uint8_t Color::GetUintR() { return (uint8_t)(r * 255); }
uint8_t Color::GetUintG() { return (uint8_t)(g * 255); }
uint8_t Color::GetUintB() { return (uint8_t)(b * 255); }
uint8_t Color::GetUintA() { return (uint8_t)(a * 255); }*/

int Color::GetUintR() { return (int)(r * 255); }
int Color::GetUintG() { return (int)(g * 255); }
int Color::GetUintB() { return (int)(b * 255); }
int Color::GetUintA() { return (int)(a * 255); }

Color Color::RandomColor()
{
	return Color(Random::Value(), Random::Value(), Random::Value(), 1);
}


Color& Color::operator*=(float num)
{
	r *= num; g *= num; b *= num; a *= num;
	return *this;
}

Color& Color::operator/=(float num)
{
	r /= num; g /= num; b /= num;a /= num;
	return *this;
}

Color& Color::operator+=(const Color &c)
{
	r += c.r; g += c.g; b += c.b;a += c.a;
	return *this;
}

Color& Color::operator*=(const Color &c)
{
	r *= c.r; g *= c.g; b *= c.b;a *= c.a;
	return *this;
}


Color& Color::operator-=(const Color &c)
{
	r -= c.r; g -= c.g; b -= c.b;	a -= c.a;
	return *this;
}

Color operator * (const Color& lhs, double rhs)
{
	return Color(rhs*lhs.r, rhs*lhs.g, rhs*lhs.b, rhs*lhs.a);
}

Color operator * (double lhs, const Color& rhs)
{
	return Color(lhs*rhs.r, lhs*rhs.g, lhs*rhs.b, lhs*rhs.a);

}

Color operator + (const Color& lhs, const Color& rhs)
{
	return Color(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);

}