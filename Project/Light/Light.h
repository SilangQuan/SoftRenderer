#pragma once
#include "Base/Component.h"
#include "Base/Color.h"
#include "Math/Vector3.h"

// Light type
enum LightType {
	kLightSpot,
	kLightDirectional,
	kLightPoint,
	kLightArea,
	kLightTypeCount // keep this last
};
enum AttenuationMode
{
	kSpotCookie,		// 2D cookie projected in spot light's cone
	kPointFalloff,		// Attenuation for a point light
	kDirectionalCookie,	// Cookie projected from a directional light
	kUnused				// The attenuation texture is not used
};

class Light : public Component
{
public:
	Light();
	Light(LightType type);
	~Light();
	virtual void AddToManager();
	virtual void RemoveFromManager();

	LightType GetType() const { return static_cast<LightType>(type); }
	void SetType(LightType type);

	Vector3 position;
	Vector3 direction;
	Vector3 ambient;
	Vector3 diffuse;
	Vector3 specular;

	Color	color;
	float	intensity;				///< Light intensity range {0.0, 8.0}
	float	range; 					///< Light range
	float	spotAngle;				///< Angle of the spotlight cone.
	float	cookieSize;				///< Cookie size for directional lights.
	float	cotanHalfSpotAngle; // cotangent of half of the spot angle
	float	invCosHalfSpotAngle; // 1/cos of half of the spot angle
	int		lightmapping;				///< enum { RealtimeOnly, Auto, BakedOnly } Is light baked into lightmaps?
	LightType		type;						///< enum { Spot, Directional, Point, Area (baked only) } Light type
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
	AttenuationMode m_AttenuationMode;
};