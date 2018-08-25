#pragma once
#include "Math/Vector3.h"
#include "Math/Vector2.h"

class Line2d
{
public:
	Vector2 start;
	Vector2 end;
	Line2d();
	Line2d(Vector2 start, Vector2 end);
	~Line2d();
};