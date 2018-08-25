#include "Plane.h"

Plane::Plane() {}
Plane::Plane( const Plane &i_vVal )
	: normal( i_vVal.normal ), d( i_vVal.d )
{}
Plane::Plane( const float i_fA, const float i_fB, const float i_fC, const float i_fD )
	: d( i_fD )
{
	normal = Vector3( i_fA, i_fB, i_fC );
}
Plane::Plane( const Vector3 &i_vNormal, const float i_fD )
	: normal( i_vNormal ), d( i_fD )
{}

const Plane &Plane::operator =( const Plane &i_vVal )
{
	normal = i_vVal.normal;
	d = i_vVal.d;
	return *this;
}

Plane::operator float*() { return &normal.x; }
Plane::operator const float*() const { return &normal.x; }

Plane Plane::operator +() const { return *this; }
Plane Plane::operator -() const { return Plane( -1 * normal, d ); }

float Plane::operator *( const Vector3 &i_vVal ) const
{
	return Vector3::Dot( normal, i_vVal ) + d;
}

float Plane::operator *( const Vector4 &i_vVal ) const
{
	return Vector3::Dot( normal, i_vVal.xyz ) + d * i_vVal.w;
}