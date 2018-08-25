#pragma once
#include "Math/Vector3.h"
#include <math.h>
//AABB
class Bounds {
public:
	Bounds();
	Bounds(Vector3 p);
	Bounds(Vector3 center, Vector3 size);
	~Bounds();
	void Encapsulate(Vector3 p);
	bool Contains(Vector3 p);
	float Volume() const;
	float SurfaceArea() const;
	bool Overlaps(const Bounds &b) const;


	//The center of the bounding box.
	Vector3 center;
	//	The extents of the box.This is always half of the size.
	Vector3	extents;
	//The maximal point of the box.This is always equal to center + extents.
	Vector3	max;
	//The minimal point of the box.This is always equal to center - extents.
	Vector3 min;

	Vector3 size;
private:
};