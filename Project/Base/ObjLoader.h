#pragma once


#include "Color.h"

#include <iostream>  
#include <vector>  
#include <string>  
//#include "Base/Mesh.h"
#include "Log/Log.h"
#include <map>
#include "ObjMaterialReader.h"
#include "ObjParseHelper.h"

namespace tinyobj
{
	class ObjLoader {
	public:
		static bool LoadObj(std::vector<ObjMesh> &meshs,       // [output]
			std::vector<ObjMaterial> &materials, // [output]
			std::string &err, const char *filename, const char *mtl_basepath = NULL,
			bool trianglulate = true);

		static bool LoadObj(std::vector<ObjMesh> &shapes,       // [output]
			std::vector<ObjMaterial> &materials, // [output]
			std::string &err, std::istream &inStream,
			MaterialReader &readMatFn, bool triangulate = true);

		static bool ExportFaceGroupToShape(ObjMesh &shape, std::map<VertexIndex, unsigned int> vertexCache,
			const std::vector<float> &in_positions,
			const std::vector<float> &in_normals,
			const std::vector<float> &in_texcoords,
			const std::vector<std::vector<VertexIndex> > &faceGroup,
			std::vector<Tag> &tags, const int material_id, const std::string &name,
			bool clearCache, bool triangulate);

		static unsigned int UpdateVertex(std::map<VertexIndex, unsigned int> &vertexCache,
			std::vector<float> &positions, std::vector<float> &normals,
			std::vector<float> &texcoords,
			const std::vector<float> &in_positions,
			const std::vector<float> &in_normals,
			const std::vector<float> &in_texcoords, const VertexIndex &i);
	};

}