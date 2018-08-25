#include "LightManager.h"


static LightManager* s_LightManager = NULL;

LightManager& GetLightManager()
{
	if (s_LightManager != NULL)
	{
		return *s_LightManager;
	}
}

void LightManager::InitializeClass()
{
	s_LightManager = new LightManager();
}

void LightManager::CleanupClass()
{
	delete s_LightManager;
	s_LightManager = NULL;
}

LightManager::LightManager()
{

}

LightManager::~LightManager() {
}

void LightManager::AddLight(Light* source)
{
	//lights.push_back(*source);
	switch(source->type)
	{
	case kLightPoint:
		s_LightManager->pointLights.push_back(*source);
		break;

	case kLightDirectional:
		directionLight = *source;
		//s_LightManager->directionLights.push_back(*source);
		break;

	case kLightSpot:
		s_LightManager->spotLights.push_back(*source);
		break;
	}
}

void LightManager::RemoveLight(Light* source)
{
/*
	lights.erase(*source);

	// If this was the last main light, clear it
	if (lights == source)
		lastMainLight = NULL;*/
}


vector<Light>& LightManager::GetPointLights()
{
	return s_LightManager->pointLights;
}

Light& LightManager::GetDirectionLight()
{
	return s_LightManager->directionLight;
}

vector<Light>& LightManager::GetSpotLights()
{
	return s_LightManager->spotLights;
}