#include "RenderSystem/Material.h"
#include "RenderSystem/Shader.h"

Material::Material(string name)
{
    Name = name;
    DiffuseTex = NULL;
    NormalTex = NULL;
    AlphaTex = NULL;
};

Material:: ~Material() {
    if(DiffuseTex)
    {
        delete DiffuseTex;
    }
    if(NormalTex)
    {
        delete NormalTex;
    }

    if(AlphaTex)
    {
        delete AlphaTex;
    }

    if(Shader)
    {
        delete Shader;
    }
};