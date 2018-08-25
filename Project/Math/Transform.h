#pragma once
#include "Math/Quaternion.h"

class GameObject;
class Transform
{
public:

	Transform();
	Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
	~Transform();

	Matrix4x4 GetLocalToWorldMatrix();

	void AttachToGameObject(GameObject* gameObject);
	GameObject* GetGameObject();

/*
	Vector3 localEulerAngle;
	Vector3 localPosition;
	Vector3 localRotation;
	Vector3 localScale;*/
	bool isDirty;
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
	GameObject* gameObject;

	static Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
	static Matrix4x4 FPSView(const Vector3& eye, Quaternion rotation);
	static Matrix4x4 Perspective(float fov, float aspect, float zNear, float zFar);
	static Matrix4x4 Frustum(float l, float r, float b, float t, float zNear, float zFar);
	static Matrix4x4 OrthoFrustum(float l, float r, float b, float t, float zNear, float zFar);
	static Vector3 TranslateToNDC(const Vector4& clipVector);
	static void TranslateToScreenCoord(const int screenWidth, const int screenHeight, Vector3& ndcCoord);

	void Translate(const Vector3 &delta);
	void Rotate(float xRot, float yRot, float zRot);
	void RotateAxis(Vector3 axis, float rotation);
	void Scale(const Vector3 &scale);

	Vector3 GetForward();
	Vector3 GetRight();
	Vector3 GetUp();

private:
	Matrix4x4 localToWorldMatrix;
	Matrix4x4 worldToLocalMatrix;
	//int CheckCvv(const Vector3& vec);
};