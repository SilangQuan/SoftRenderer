#pragma once
#include "TestCases.h"
#include <random>
#include <iostream>

double erand48(unsigned short seed[3]) {
	return (double)rand()
		/ (double)RAND_MAX;
}// *** Added for VS2012

void DebugTest()
{
	Vector2 v = Vector2(1, 1);
	Vector2 v2 = Vector2(2, 1);
	Vector3 v3 = Vector3(0, 2, 1);
	Vector3 v4 = Vector3(0, 2, 1);
	Vector3 v5 = Vector3(23, 112, 22);
	Vector3 v6 = Vector3(23, 112, 22);
	std::vector<Vector3> vec;
	vec.push_back(v3);
	vec.push_back(v4);
	vec.push_back(v5);
	vec.push_back(v6);
	vec.push_back(v6);
	vec.push_back(v6);
	vec.push_back(v6);
	vec.push_back(v6);
	std::string testStr = "vector Test";
	qDebug() << "Hello Debug";
	qDebug() <<""<< v << v2<< v3;
	qDebug() << v3;
	qWarning() << vec;
}



void PaintTest()
{
	

}

void SDLApiTest()
{
/*
	CSDL2 *sdl = new CSDL2(640, 480);
	Rect rect = { 100, 100, 50, 100 };
	Line2d line = { Vector2(50, 50), Vector2(100, 100) };

	sdl->Clean(Color::black);
	sdl->SetDrawColor(Color::red);
	sdl->DrawPixel(10, 10);
	sdl->SetDrawColor(Color::green);
	sdl->DrawRect(rect);
	sdl->SetDrawColor(Color::blue);
	sdl->DrawLine(line);
	sdl->Render();*/
}

void BoundsTest()
{
	Vector3 p0 = Vector3::zero;
	Vector3 p1 = Vector3::one;
	Vector3 p2 = 3 * Vector3::one;
	Vector3 p3 = 5 * Vector3::one;
	Bounds bounds0(p0);
	Bounds bounds1(p0, p2);
	qDebug() << bounds1.min << bounds1.max;
	qDebug() <<"bounds1.Contains(p1)"<< bounds1.Contains(p1);

	qDebug() << "bounds0.Contains(p1)" << bounds0.Contains(p1);
	bounds0.Encapsulate(p3);
	qDebug() << "After Encapsulate bounds0.Contains(p1)" << bounds0.Contains(p1);

	qDebug() << bounds0.Overlaps(bounds1);
	qDebug() << "SurfaceArea" << bounds0.SurfaceArea();
	qDebug() << "Volumn" << bounds0.Volume();

//	Debug("Finish!");

}


void QuaternionTest()
{
	qDebug() << Mathf::Rad2Deg * Mathf::Pi * 0.25f;// 45
	qDebug() << Mathf::Deg2Rad * 45;// 0.785398163397

	Quaternion a = Quaternion::identity;
	Quaternion b(Mathf::Pi * 0.5, 0, 0, 0);

	qDebug() << "a" << a << a.EulerAngle();
	qDebug() << "b" << b << b.EulerAngle();
	qDebug() << Quaternion::Angle(a, b);
	Quaternion c = Quaternion::Slerp(a, b, 0.5f);
	qDebug() <<"c" <<c << c.EulerAngle();
	qDebug() << Quaternion::Angle(a, c);
	Quaternion d = b * c;
	qDebug() << "d" << d << d.EulerAngle();
	Vector3 pos(1, 2, 3);
	qDebug() << "c * (1, 2, 3) "<<c * pos;
}