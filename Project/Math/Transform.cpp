#include "Transform.h"
#include "Log/Log.h"

Transform::Transform()
{
	isDirty = true;
	position = Vector3::zero;
	rotation = Quaternion::identity;
	scale = Vector3::one;
	gameObject = NULL;
}

Transform::Transform(const Vector3& _position, const Quaternion& _rotation, const Vector3& _scale)
	:position(_position),rotation(_rotation), scale(_scale), isDirty(true)
{
	gameObject = NULL;
}

Transform::~Transform()
{

}


void Transform::AttachToGameObject(GameObject* _gameObject)
{
	gameObject = _gameObject;
}

GameObject* Transform::GetGameObject()
{
	return gameObject;
}

Matrix4x4 Transform::GetLocalToWorldMatrix()
{
	if (isDirty)
	{
		Matrix4x4 transMatrix(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			position.x, position.y, position.z, 1);

		Matrix4x4 scaleMatrix(scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1);

		localToWorldMatrix = transMatrix * rotation.GetRotMatrix() * scaleMatrix;
		isDirty = false;
	}
	return localToWorldMatrix;
}

void Transform::Translate(const Vector3 &delta)
{
	position.x += delta.x;
	position.y += delta.y;
	position.z += delta.z;

	isDirty = true;

/*
	Matrix4x4 mInv(1, 0, 0, -delta.x,
		0, 1, 0, -delta.y,
		0, 0, 1, -delta.z,
		0, 0, 0, 1);*/
}

void Transform::RotateAxis(Vector3 axis, float _rotation)
{
	rotation = rotation * Quaternion(0.01f, 0.02f, 0, 1);
	isDirty = true;
}

void Transform::Rotate(float xRot, float yRot, float zRot)
{
	rotation = rotation * Quaternion::Euler(xRot, yRot, zRot);
	isDirty = true;
}

Vector3 Transform::GetForward()
{
	return rotation * Vector3::forward;
}

Vector3 Transform::GetRight()
{
	return rotation * Vector3::right;
}

Vector3 Transform::GetUp()
{
	return rotation * Vector3::up;
}

Matrix4x4 Transform :: LookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	Vector3 z((eye - target).normalized());
	Vector3 x((Vector3::Cross(z, up).normalized()));
	Vector3 y(Vector3::Cross(x, z));

	Matrix4x4 result;

	result[0] = x.x;
	result[4] = x.y;
	result[8] = x.z;
	result[12] = -Vector3::Dot(x, eye);

	result[1] = y.x;
	result[5] = y.y;
	result[9] = y.z;
	result[13] = -Vector3::Dot(y, eye);

	result[2] = z.x;
	result[6] = z.y;
	result[10] = z.z;
	result[14] = -Vector3::Dot(z, eye);

	result[3] = result[7] = result[11] = 0.0f;
	result[15] = 1.0f;
	return result;
}

Matrix4x4 Transform::FPSView(const Vector3& eye, Quaternion rotation)
{
	Matrix4x4 rotMatrix = rotation.GetRotMatrix().transpose();
	Vector3 x(rotMatrix[0], rotMatrix[4], rotMatrix[8]);
	Vector3 y(rotMatrix[1], rotMatrix[5], rotMatrix[9]);
	Vector3 z(-rotMatrix[2], -rotMatrix[6], -rotMatrix[10]);

	Matrix4x4 result;

	result[0] = x.x;
	result[4] = x.y;
	result[8] = x.z;
	result[12] = -Vector3::Dot(x, eye);

	result[1] = y.x;
	result[5] = y.y;
	result[9] = y.z;
	result[13] = -Vector3::Dot(y, eye);

	result[2] = z.x;
	result[6] = z.y;
	result[10] = z.z;
	result[14] = -Vector3::Dot(z, eye);

	result[3] = result[7] = result[11] = 0.0f;
	result[15] = 1.0f;
	return result;
}


Matrix4x4 Transform::Perspective(float fovy, float aspect, float zNear, float zFar)
{
	
	float tanHalfFovy = tan(Mathf::Deg2Rad * fovy / 2);

	Matrix4x4 result(0.0f);
	result[0 * 4 + 0] = 1.0f / (aspect * tanHalfFovy);
	result[1 * 4 + 1] = 1.0f / (tanHalfFovy);
	result[2 * 4 + 3] = -1.0f;

	result[2 * 4 + 2] = -(zFar + zNear) / (zFar - zNear);
	result[3 * 4 + 2] = -(2.0f * zFar * zNear) / (zFar - zNear);
	return result;
}

Matrix4x4 Transform::Frustum(float l, float r, float b, float t, float zNear, float zFar)
{
	Matrix4x4 result(0.0f);

	result[0] = 2 * zNear / (r - l);
	result[8] = (r + l) / (r - l);
	result[5] = 2 * zNear / (t - b);
	result[9] = (t + b) / (t - b);
	result[10] = -(zFar + zNear) / (zFar - zNear);
	result[14] = -(2 * zFar * zNear) / (zFar - zNear);
	result[11] = -1;
	result[15] = 0;
	return result;
}

Matrix4x4 Transform::OrthoFrustum(float l, float r, float b, float t, float zNear, float zFar)
{
	Matrix4x4 result(0.0f);
	result[0] = 2 / (r - l);
	result[5] = 2 / (t - b);
	result[10] = -2 / (zFar - zNear);
	result[15] = 1;

	//result[12] = -(r + l) / (r - l);
	//result[13] = -(t + b) / (t - b);
	result[14] = -(zFar + zNear) / (zFar - zNear);

	return result;
}
Vector3 Transform::TranslateToNDC(const Vector4& clipVector)
{
	float inveW = 1.0f / clipVector.w;
	return Vector3(clipVector.x * inveW, clipVector.y * inveW, clipVector.z * inveW);
}

void Transform::TranslateToScreenCoord(const int screenWidth, const int screenHeight, Vector3& ndcCoord)
{
	int w = screenWidth;
	int h = screenHeight;
	ndcCoord.x = 0.5f * w + 0.5f *ndcCoord.x * w;
	ndcCoord.y = 0.5f * h + 0.5f *ndcCoord.y * h;
}
void Transform::Scale(const Vector3 &_scale)
{
	scale = _scale;
	isDirty = true;
}


/*
int Transform::CheckCvv(const Vector3& vec)
{
	float w = v->w;
	int check = 0;
	if (v->z < 0.0f) check |= 1;
	if (v->z > w) check |= 2;
	if (v->x < -w) check |= 4;
	if (v->x > w) check |= 8;
	if (v->y < -w) check |= 16;
	if (v->y > w) check |= 32;
	return check;
}
*/
