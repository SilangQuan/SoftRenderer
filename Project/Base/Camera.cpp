#include "Camera.h"

Camera::Camera(const Vector3& _pos, const Vector3& _target, const Vector3& _up)
{
	transform.position = _pos;
	target = _target;
	up = _up;
	viewMatrix = Transform::LookAt(_pos, _target, _up);
	viewportMaxtrix = Matrix4x4::zero;
}

Camera::~Camera()
{

}

void Camera::RefreshViewMatrix()
{
	viewMatrix = Transform::LookAt(transform.position , target, up);
}

void Camera::SetDepthRange(float near, float far)
{
	DepthRangeNear = near;
	DepthRangeFar = far;
}

void Camera::SetFrustrum(float left, float right, float bottom, float top, float nearValue, float farValue, bool perspective)
{
	if (perspective)
	{
		//projectionMaxtrix = Transform::Frustum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 20);
		projectionMaxtrix = Transform::Frustum(left, right, bottom, top, nearValue, farValue);
	}
	else
	{
		//projectionMaxtrix = Transform::OrthoFrustum(-1.778f, 1.778f, -1.0f, 1.0f, nearValue, farValue);
		projectionMaxtrix = Transform::OrthoFrustum(left, right, bottom, top, nearValue, farValue);
	}
}

void Camera::SetPespective(float fovy,float aspect,float _zNear,float _zFar)
{
	zFar = _zFar;
	zNear = _zNear;
	Fov = fovy;
	Aspect = aspect;
	projectionMaxtrix = Transform::Perspective( fovy, aspect, zNear, zFar);
}

void Camera::SetViewPort(int x, int y, int width, int height, bool isLeftHand)
{
	//row major ,wrong!
    //viewportMaxtrix[3]= x + width/2.f;
    //viewportMaxtrix[1*4 + 3] = y + height/2.f;
    //viewportMaxtrix[2*4 + 3] = (zNear + zFar)/2.f;
    //viewportMaxtrix[3*4 + 3] = 1.f;
//
    //viewportMaxtrix[2*4 + 2] = (zFar - zNear)/2.f;
//
    //viewportMaxtrix[1*4 + 1] = height/2.f;
//
    //viewportMaxtrix[0] = width/2.f;

	//column major ,ok!

	viewportMaxtrix[0] = width/2.f;
	viewportMaxtrix[1*4 + 1] = height/2.f;
	viewportMaxtrix[2*4 + 2] = (zFar - zNear)/2.f;
	viewportMaxtrix[3*4] = (x + width)/2.f;
	viewportMaxtrix[3*4 + 1] = (y + height)/2.f;
	viewportMaxtrix[3*4 + 2] = (zNear + zFar)/2.f;
	viewportMaxtrix[3*4 + 3] = 1;

	//Map Zndc -> Zw
	viewportMaxtrix[2*4 + 2] = 0.5f * (DepthRangeFar-DepthRangeNear);
	viewportMaxtrix[3*4 + 2] = 0.5f* (DepthRangeFar+DepthRangeNear);
	
	if(isLeftHand)
	{
		//viewportMaxtrix = viewportMaxtrix.transpose();
	}
}