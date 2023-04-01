#include "Model.h"
#include "Mesh.h"
#include "ObjLoader.h"
#include "Log/Log.h"


Model::Model(std::string filePath)
{
	int index = filePath.find_last_of("/");
	directory = filePath.substr(0, index + 1);
	LoadModel(filePath);
}

Model::~Model()
{
	for(int i=0; i< mMeshes.size();i++)
	{
		free(mMeshes[i]);
	}

	for(int i=0; i< mMaterials.size();i++)
	{
		free(mMaterials[i]);
	}
	mMeshes.clear();
	mMaterials.clear();
}

// Render the mesh
void Model::Draw(ShaderProgram* shader)
{
	
}


vector<Mesh*>& Model::GetMeshes()
{
	return mMeshes;
}

vector<Material*>& Model::GetMaterials()
{
	return mMaterials;
}

void Model::LoadModel(std::string path)
{
	vector<tinyobj::ObjMesh> shapes;
	vector<tinyobj::ObjMaterial> materials;

	std::string err;
	bool ret = tinyobj::ObjLoader::LoadObj(shapes, materials, err, path.c_str(), directory.c_str());
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << endl;
	}

	if (!ret) {
		//Debug d = MessageLogger(__FILE__, __FUNCTION__, __LINE__).debug();
			//d << "Fuck";


		qDebug() << "LoadModel" << path << " error";
		exit(1);
	}else
	{
		qDebug() << path << " Loaded!";
		qDebug() << "shapes.size()" << (int)shapes.size();
		qDebug() << "materials.size()" << (int)materials.size();
	}
	
	for (int i = 0; i < materials.size(); i++)
	{
		ProcessMaterial(&materials[i]);
	}

	for (int i = 0; i < shapes.size(); i++)
	{
		ProcessMesh(&shapes[i]);
	}

	// Process materials
	for (size_t i = 0; i < materials.size(); i++)
	{
		qDebug() << "materials: " << materials[i].diffuse_texname.c_str();
	}
}

void Model::ProcessMaterial(tinyobj::ObjMaterial* material)
{
	Material* pMat = new Material(material->name);
	pMat->DiffuseTex = new Texture(directory + material->diffuse_texname);
	if(material->alpha_texname.length() > 0)
	{
		pMat->AlphaTex = new Texture(directory + material->alpha_texname);
	}
	//pMat->NormalTex = new Texture(directory + material->bump_texname);
	//mat-> = new Texture(materials[i].diffuse_texname);
	mMaterials.push_back(pMat);
	qDebug() << "ProcessMaterial:" <<material->name;
}

void Model::ProcessMesh(tinyobj::ObjMesh* mesh)
{
	Mesh* pmesh = new Mesh();

	pmesh->Vertices.reserve(mesh->positions.size() / 3);
	pmesh->Normals.reserve(mesh->normals.size() / 3);
	pmesh->UVs.reserve(mesh->texcoords.size() / 3);
	pmesh->Triangles.reserve(mesh->indices.size() / 3);
	//memcpy(positions.data(), mesh->positions.data(), sizeof(Vector3) * positions.size());

	bool hasUV = mesh->texcoords.size() > 0 ? true:false;

	// Walk through each of the mesh's vertices
	for (int i = 0; i < mesh->positions.size()/3; i++)
	{
		pmesh->Vertices.push_back(Vector3(mesh->positions[3 * i + 0], mesh->positions[3 * i + 1], mesh->positions[3 * i + 2]));

		pmesh->Normals.push_back(Vector3(mesh->normals[3 * i + 0], mesh->normals[3 * i + 1], mesh->normals[3 * i + 2]));

		Vector2 tmpTexcoord(0,1);
		// Texture Coordinates
		if (hasUV) // Does the mesh contain texture coordinates?
		{
			tmpTexcoord.x = mesh->texcoords[2 * i + 0];
			tmpTexcoord.y = mesh->texcoords[2 * i + 1];
		}

		pmesh->UVs.push_back(tmpTexcoord);
	}
	// Copying vector by copy function
    copy(mesh->indices.begin(), mesh->indices.end(), back_inserter(pmesh->Triangles));
	mMeshes.push_back(pmesh);
	qDebug() << "ProcessMesh:" <<mesh->name;

	mMeshMatMap.insert(std::make_pair(pmesh, mMaterials[mesh->material_ids[0]]));
}

std::map<Mesh*, Material*>& Model::GetMeshMatMap()
{
	return mMeshMatMap;
}
// vector<TextureVariable*> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
// {
// 	vector<TextureVariable*> tmpTextures;
// 	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
// 	{
// 		aiString str;
// 		mat->GetTexture(type, i, &str);

// 		// If texture hasn't been loaded already, load it
// 		string path = this->directory + "/" + str.C_Str();
			
// 		Texture* texture = ResourceManager::GetInstance()->TryGetResource<Texture>(path);

// 		stringstream ss;
// 		ss << "material.";
// 		ss << typeName;
// 		ss << i;
// 		TextureVariable* textureVariable = new TextureVariable(texture, -1, ss.str());

// 		tmpTextures.push_back(textureVariable);
// 	}
// 	return tmpTextures;
// }
