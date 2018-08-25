#pragma once
#include "CTypes.h"
#include "ObjLoader.h"
#include "RenderSystem/Material.h"
#include <string>
#include <map>

class ShaderProgram;
class Mesh;
class TextureVariable;

class Model
{
public:
	Model(std::string filePath);
	~Model();
	void Draw(ShaderProgram* shader);
	vector<Mesh*>& GetMeshes();
	vector<Material*>& GetMaterials();
	std::map<Mesh*, Material*>& GetMeshMatMap();
	
private:
	vector<Mesh*> mMeshes;
	vector<Material*> mMaterials;
	std::string directory;

	void LoadModel(std::string path);
	void ProcessMesh(tinyobj::ObjMesh* mesh);
	void ProcessMaterial(tinyobj::ObjMaterial* material);
	
	std::map<Mesh*, Material*> mMeshMatMap;

};