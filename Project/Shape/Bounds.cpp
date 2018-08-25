#include "Bounds.h"
Bounds::Bounds()
{

}

Bounds::Bounds(Vector3 p):min(p), max(p),center(p)
{
	size = Vector3::zero;
	extents = Vector3::zero;
}

Bounds::Bounds(Vector3 _center, Vector3 _size): center(_center), size(_size)
{
	extents = 0.5f * size;
	min = center - extents;
	max = center + extents;
}

/*
Bounds::Bounds(Vector3 p1, Vector3 p2)
{
	Vector3 pMin(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
	Vector3 pMax(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
	min = pMin;
	max = pMax;
	center = (pMax + pMin) / 2;
	size = pMax - pMin;
	extents = 0.5f * size;
}*/

Bounds::~Bounds()
{

}

void Bounds::Encapsulate(Vector3 p)
{
	if (Contains(p))
	{
		return;
	}
	else
	{
		min.x = fmin(p.x, min.x);
		min.y = fmin(p.y, min.y);
		min.z = fmin(p.z, min.z);

		max.x = fmax(p.x, max.x);
		max.y = fmax(p.y, max.y);
		max.z = fmax(p.z, max.z);
	}

	size = max - min;
	extents = 0.5f * size;
	center = min + extents;
}

bool Bounds::Contains(Vector3 p)
{
	return(p.x > min.x && p.y > min.y && p.z > min.z
		&&p.x < max.x && p.y < max.y && p.z < max.z);
}

float Bounds::Volume() const
{
	Vector3 d = max - min;
	return d.x * d.y * d.z;
}

float Bounds::SurfaceArea() const
{
	Vector3 d = max - min;
	return 2.f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

bool Bounds::Overlaps(const Bounds &b) const
{
	bool x = (max.x >= b.min.x) && (b.max.x >= min.x);
	bool y = (max.y >= b.min.y) && (b.max.y >= min.y);
	bool z = (max.z >= b.min.z) && (b.max.z >= min.z);
	return  x && y && z;
}