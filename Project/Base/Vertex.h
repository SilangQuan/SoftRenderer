#pragma once
#pragma once
#include "Color.h"
#include "Math/Vector3.h"
#include "Math/Mathf.h"

struct VSOutput
{
    Vector4 position;
	Vector3 normal;
	Vector2 uv;
	Color color;

	Vector3 varying[4];
};



class Vertex {
public:
	Vector3 position;
	Vector3 normal;
	float u;
	float v;
	Color color;
	float w;
	Vertex()
	{

	}
	Vertex(const Vector3 &position, const Vector3 normal, float u, float v, const Color &color = Color()) {
		this->position = position;
		this->normal = normal;
		this->u = u;
		this->v = v;
		this->color = color;
	};

	Vertex interpolate(const Vertex &vertex, float t) const {
		//Vector3 p = position.interpolate(vertex.position, factor);
		Vector3 p = Vector3::Lerp(position, vertex.position, t);
		Vector3 n = Vector3::Lerp(normal, vertex.normal, t);
		float invZ1 = 1.0f / position.z;
		float invZ2 = 1.0f / vertex.position.z;
		float Z = 1.0f / Mathf::Lerp(invZ1, invZ2, t);

		float tu = Mathf::Lerp(u * invZ1, vertex.u * invZ2, t);
		float tv = Mathf::Lerp(v * invZ1, vertex.v * invZ2, t);
		Color c = Color::Lerp(color, vertex.color, t);
		return Vertex(p, n, tu * Z, tv * Z, c);

		/*float tu = Mathf::Lerp(u , vertex.u , t);
		float tv = Mathf::Lerp(v , vertex.v , t);
		Color c = Color::Lerp(color, vertex.color, t);
		return Vertex(p, n, tu, tv, c);*/
	};
};

class Vertex2D {
public:
	Vector2 position;
	Vector2 uv;
	Color color;

	Vertex2D()
	{

	}

	Vertex2D(const Vector2 &position, const Color &color = Color()) {
		this->position = position;
		this->color = color;
	};

	Vertex2D interpolate(const Vertex2D &vertex, float t) const
	{
		Vector2 p = Vector2::Lerp(position, vertex.position, t);
		Color c = Color::Lerp(color, vertex.color, t);
		return Vertex2D(p, c);
	};
};