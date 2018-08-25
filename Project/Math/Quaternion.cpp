#include "Quaternion.h"

Quaternion Quaternion::identity(0, 0, 0, 1);

Quaternion::Quaternion():x(0), y(0), z(0), w(1)
{
	
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w)
{
	float mag = _x *_x + _y*_y + _z *_z + _w*_w;
	x = _x / mag;
	y = _y / mag;
	z = _z / mag;
	w = _w / mag;
}

Quaternion::~Quaternion()
{

}


//Cos theta of two quaternion
float Quaternion::Dot(const Quaternion &lhs, const Quaternion &rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

Quaternion Quaternion::Euler(float yaw, float pitch, float roll)
{
	yaw *= Mathf::Deg2Rad;
	pitch *= Mathf::Deg2Rad;
	roll *= Mathf::Deg2Rad;
	// Assuming the angles are in radians.
/*
	double c1 = Mathf.Cos(yaw / 2);
	double s1 = Mathf.sin(yaw / 2);
	double c2 = Mathf.cos(pitch / 2);
	double s2 = Mathf.sin(pitch / 2);
	double c3 = Mathf.cos(roll / 2);
	double s3 = Mathf.sin(roll / 2);
	double c1c2 = c1*c2;
	double s1s2 = s1*s2;
	w = c1c2*c3 - s1s2*s3;
	x = c1c2*s3 + s1s2*c3;
	y = s1*c2*c3 + c1*s2*s3;
	z = c1*s2*c3 - s1*c2*s3;
*/


	float  angle;
	float  sinRoll, sinPitch, sinYaw, cosRoll, cosPitch, cosYaw;

	angle = yaw * 0.5f;
	sinYaw = sin(angle);
	cosYaw = cos(angle);

	angle = pitch * 0.5f;
	sinPitch = sin(angle);
	cosPitch = cos(angle);

	angle = roll * 0.5f;
	sinRoll = sin(angle);
	cosRoll = cos(angle);

	float _x = cosRoll*sinPitch*cosYaw + sinRoll*cosPitch*sinYaw;
	float _y = cosRoll*cosPitch*sinYaw - sinRoll*sinPitch*cosYaw;
	float _z = sinRoll*cosPitch*cosYaw - cosRoll*sinPitch*sinYaw;
	float _w = cosRoll*cosPitch*cosYaw + sinRoll*sinPitch*sinYaw;

	float invMag = 1.0f / (_x *_x + _y*_y + _z *_z + _w*_w);

	return Quaternion(_x * invMag, _y * invMag, _z* invMag, _w * invMag);
	/*x = _x / mag;
	y = _y / mag;
	z = _z / mag;
	w = _w / mag;*/
}
Quaternion Quaternion::Slerp(const Quaternion &a, const Quaternion &b, float t)
{
	float cos_theta = Dot(a, b);

	// if B is on opposite hemisphere from A, use -B instead
	float sign;
	if (cos_theta < 0.f)
	{
		cos_theta = -cos_theta;
		sign = -1.f;
	}
	else sign = 1.f;

	float c1, c2;
	if (cos_theta > 1.f - Mathf::EPSILON)
	{
		// if q2 is (within precision limits) the same as q1,
		// just linear interpolate between A and B.

		c2 = t;
		c1 = 1.f - t;
	}
	else
	{
		//float theta = gFloat::ArcCosTable(cos_theta);
		// faster than table-based :
		//const float theta = myacos(cos_theta);
		float theta = acos(cos_theta);
		float sin_theta = sin(theta);
		float t_theta = t*theta;
		float inv_sin_theta = 1.f / sin_theta;
		c2 = sin(t_theta) * inv_sin_theta;
		c1 = sin(theta - t_theta) * inv_sin_theta;
	}

	c2 *= sign; // or c1 *= sign
				// just affects the overrall sign of the output

				// interpolate
	return Quaternion(a.x * c1 + b.x * c2, a.y * c1 + b.y * c2, a.z * c1 + b.z * c2, a.w * c1 + b.w * c2);
}

Quaternion Quaternion::Lerp(const Quaternion &a, const Quaternion &b, float t)
{
	return Quaternion((1 - t) * a.x + t * b.x,
		(1 - t) * a.y + t * b.y,
		(1 - t) * a.z + t * b.z,
		(1 - t) * a.w + t * b.w);
}

float Quaternion::Angle(const Quaternion &lhs, const Quaternion &rhs)
{
	float cos_theta = Dot(lhs, rhs);

	// if B is on opposite hemisphere from A, use -B instead
	if (cos_theta < 0.f)
	{
		cos_theta = -cos_theta;
	}
	float theta = acos(cos_theta);
	return 2 * Mathf::Rad2Deg * theta;
}


void Quaternion::Set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}


void Quaternion::operator+(const Quaternion &q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
}

void Quaternion::operator-(const Quaternion &q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
}

void Quaternion::operator*(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
}

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::Inverse() const
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion operator * (const Quaternion& lhs, const Quaternion& rhs)
{
	float w1 = lhs.w;
	float w2 = rhs.w;
	Vector3 v1(lhs.x, lhs.y, lhs.z);
	Vector3 v2(rhs.x, rhs.y, rhs.z);
	float w3 = w1 * w2 - Vector3::Dot(v1, v2);
	Vector3 v3 = Vector3::Cross(v1, v2) + w1 * v2 + w2 * v1;
	return Quaternion(v3.x, v3.y, v3.z, w3);
}

Vector3 operator *(const Quaternion& q, const Vector3& v)
{

/*
	Quaternion tmp(v.x, v.y, v.z, 0); //This will normalise the quaternion. this will case error.
	Quaternion result = q * tmp * q.Conjugate();
	return Vector3(result.x, result.y, result.z);*/

	// Extract the vector part of the quaternion
	Vector3 u(q.x, q.y, q.z);

	// Extract the scalar part of the quaternion
	float s = q.w;

	// Do the math
	return 2.0f * Vector3::Dot(u, v) * u
		+ (s*s - Vector3::Dot(u, u)) * v
		+ 2.0f * s * Vector3::Cross(u, v);
}

Vector3 Quaternion::EulerAngle() const
{
	float yaw = atan2(2 * (w * x + z * y), 1 - 2 * (x * x + y * y));
	float pitch = asin(Mathf::Clamp(2 * (w * y - x * z), -1.0f, 1.0f));
	float roll = atan2(2 * (w * z + x * y), 1 - 2 * (z * z + y * y));
	return Vector3(Mathf::Rad2Deg * yaw, Mathf::Rad2Deg * pitch, Mathf::Rad2Deg * roll);
}

Matrix4x4 Quaternion::GetRotMatrix()
{
	const float n = 1.0f / sqrt(x*x + y*y + z*z + w*w);
	x *= n;
	y *= n;
	z *= n;
	w *= n;

	return Matrix4x4(
		1.0f - 2.0f*y*y - 2.0f*z*z, 2.0f*x*y - 2.0f*z*w, 2.0f*x*z + 2.0f*y*w, 0.0f,
		2.0f*x*y + 2.0f*z*w, 1.0f - 2.0f*x*x - 2.0f*z*z, 2.0f*y*z - 2.0f*x*w, 0.0f,
		2.0f*x*z - 2.0f*y*w, 2.0f*y*z + 2.0f*x*w, 1.0f - 2.0f*x*x - 2.0f*y*y, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		).transpose();
}

