#pragma once
#include <string>  
#include "Log/Log.h"
#include "Texture.h"
class IShader;

class Material {
public:
	Material(string name);

	virtual ~Material();

//private:
    string Name;
    int RenderQueue;
    Texture* DiffuseTex;
    Texture* AlphaTex;
    Texture* NormalTex;
    IShader* Shader;
};
