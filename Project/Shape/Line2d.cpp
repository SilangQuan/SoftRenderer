#include "Line2d.h"

Line2d::Line2d()
{
	Vector2 zero = { 0,0 };
	start = zero;
	end = zero;
}

Line2d::Line2d(Vector2 _start, Vector2 _end)
{
	start = _start;
	end = _end;
}

Line2d::~Line2d()
{

}