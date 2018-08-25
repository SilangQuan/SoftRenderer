#pragma once
#include "Math/Transform.h"

class Camera
{
public: 
	Camera(const Vector3& pos, const Vector3& target, const Vector3& up);
	~Camera();

	void SetFrustrum(float left, float right, float bottom, float top, float nearValue, float farValue, bool perspective);

	void SetPespective(float fovy,float aspect,float zNear,float zFar);

	//Default set right hand coord
	void SetViewPort(int startX, int startY, int width, int height, bool isLeftHand = false);
	
	void RefreshViewMatrix();
	
	void SetDepthRange(float near, float far);

	Vector3 target;
	Vector3 up;
	Transform transform;
	
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMaxtrix;
	Matrix4x4 viewportMaxtrix;

	float zNear;
	float zFar;

	float DepthRangeNear;
	float DepthRangeFar;

	float Fov;
	float Aspect;
};