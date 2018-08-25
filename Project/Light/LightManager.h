#pragma once
#include "Light.h"
#include <vector>

class Light;

class LightManager
{
public:
	LightManager();
	~LightManager();

	static void InitializeClass();
	static void CleanupClass();

	void AddLight(Light* source);
	void RemoveLight(Light* source);

	vector<Light>& GetPointLights();
	Light& GetDirectionLight();
	vector<Light>& GetSpotLights();

	//vector<Light>& GetAllLights() { return lights; }

private:
	vector<Light> pointLights;
	Light directionLight;
	vector<Light> spotLights;
};

LightManager& GetLightManager();