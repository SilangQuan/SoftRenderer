#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <math.h>
class Plane {
	public:


	Plane();
	Plane( const Plane &i_vVal );
	Plane( const float i_fA, const float i_fB, const float i_fC, const float i_fD );
	Plane( const struct Vector3 &i_vNormal, const float i_fD );

	const Plane &operator =( const Plane &i_vVal );

	operator float*();
	operator const float*() const;

	Plane operator +() const;
	Plane operator -() const; ///< Returns the plane with negated normal.

	float operator *( const struct Vector3 &i_vVal ) const; ///< Calculates the dot product with the vector.
	float operator *( const struct Vector4 &i_vVal ) const; ///< Calculates the dot product with the vector.

	private:
	    Vector3 normal;
		float d;
};