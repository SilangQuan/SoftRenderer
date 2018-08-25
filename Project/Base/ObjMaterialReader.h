#pragma once

#include <string>  
#include <vector>  
#include <map>
#include <iostream>  
#include <fstream>  
#include <sstream>
#include <assert.h>
#include "ObjParseHelper.h"

namespace tinyobj
{
	typedef struct {
		std::string name;
		float ambient[3];
		float diffuse[3];
		float specular[3];
		float transmittance[3];
		float emission[3];
		float shininess;
		float ior;      // index of refraction
		float dissolve; // 1 == opaque; 0 == fully transparent
						// illumination model (see http://www.fileformat.info/format/material/)
		int illum;

		int dummy; // Suppress padding warning.

		std::string ambient_texname;            // map_Ka
		std::string diffuse_texname;            // map_Kd
		std::string specular_texname;           // map_Ks
		std::string specular_highlight_texname; // map_Ns
		std::string bump_texname;               // map_bump, bump
		std::string displacement_texname;       // disp
		std::string alpha_texname;              // map_d
		std::map<std::string, std::string> unknown_parameter;
	} ObjMaterial;

	class MaterialReader {
	public:
		MaterialReader() {}
		virtual ~MaterialReader() {};

		virtual bool operator()(const std::string &matId,
			std::vector<ObjMaterial> &materials,
			std::map<std::string, int> &matMap,
			std::string &err) = 0;
	};

	class MaterialFileReader : public MaterialReader {
	public:
		MaterialFileReader(const std::string &mtl_basepath)
			: m_mtlBasePath(mtl_basepath) {}
		virtual ~MaterialFileReader() {}
		virtual bool operator()(const std::string &matId,
			std::vector<ObjMaterial> &materials,
			std::map<std::string, int> &matMap, std::string &err);
		static void LoadMtl(std::map<std::string, int> &material_map,
			std::vector<ObjMaterial> &materials, std::istream &inStream);
		static void InitMaterial(ObjMaterial &material);
	private:
		std::string m_mtlBasePath;
	};
}
