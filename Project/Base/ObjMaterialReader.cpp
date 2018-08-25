#include "ObjMaterialReader.h"
namespace tinyobj
{
	bool MaterialFileReader::operator()(const std::string &matId,
		std::vector<ObjMaterial> &materials,
		std::map<std::string, int> &matMap,
		std::string &err) {
		std::string filepath;

		if (!m_mtlBasePath.empty()) {
			filepath = std::string(m_mtlBasePath) + matId;
		}
		else {
			filepath = matId;
		}

		std::ifstream matIStream(filepath.c_str());
		LoadMtl(matMap, materials, matIStream);
		if (!matIStream) {
			std::stringstream ss;
			ss << "WARN: Material file [ " << filepath
				<< " ] not found. Created a default material.";
			err += ss.str();
		}
		return true;
	}

	void MaterialFileReader::LoadMtl(std::map<std::string, int> &material_map,
		std::vector<ObjMaterial> &materials, std::istream &inStream) {

		// Create a default material anyway.
		ObjMaterial material;
		InitMaterial(material);

		size_t maxchars = 8192;          // Alloc enough size.
		std::vector<char> buf(maxchars); // Alloc enough size.
		while (inStream.peek() != -1) {
			inStream.getline(&buf[0], static_cast<std::streamsize>(maxchars));

			std::string linebuf(&buf[0]);

			// Trim newline '\r\n' or '\n'
			if (linebuf.size() > 0) {
				if (linebuf[linebuf.size() - 1] == '\n')
					linebuf.erase(linebuf.size() - 1);
			}
			if (linebuf.size() > 0) {
				if (linebuf[linebuf.size() - 1] == '\r')
					linebuf.erase(linebuf.size() - 1);
			}

			// Skip if empty line.
			if (linebuf.empty()) {
				continue;
			}

			// Skip leading space.
			const char *token = linebuf.c_str();
			token += strspn(token, " \t");

			assert(token);
			if (token[0] == '\0')
				continue; // empty line

			if (token[0] == '#')
				continue; // comment line

						  // new mtl
			if ((0 == strncmp(token, "newmtl", 6)) && IS_SPACE((token[6]))) {
				// flush previous material.
				if (!material.name.empty()) {
					material_map.insert(std::pair<std::string, int>(
						material.name, static_cast<int>(materials.size())));
					materials.push_back(material);
				}

				// initial temporary material
				InitMaterial(material);

				// set new mtl name
				char namebuf[TINYOBJ_SSCANF_BUFFER_SIZE];
				token += 7;
#ifdef _MSC_VER
				sscanf_s(token, "%s", namebuf, (unsigned)_countof(namebuf));
#else
				sscanf(token, "%s", namebuf);
#endif
				material.name = namebuf;
				continue;
			}

			// ambient
			if (token[0] == 'K' && token[1] == 'a' && IS_SPACE((token[2]))) {
				token += 2;
				float r, g, b;
				parseFloat3(r, g, b, token);
				material.ambient[0] = r;
				material.ambient[1] = g;
				material.ambient[2] = b;
				continue;
			}

			// diffuse
			if (token[0] == 'K' && token[1] == 'd' && IS_SPACE((token[2]))) {
				token += 2;
				float r, g, b;
				parseFloat3(r, g, b, token);
				material.diffuse[0] = r;
				material.diffuse[1] = g;
				material.diffuse[2] = b;
				continue;
			}

			// specular
			if (token[0] == 'K' && token[1] == 's' && IS_SPACE((token[2]))) {
				token += 2;
				float r, g, b;
				parseFloat3(r, g, b, token);
				material.specular[0] = r;
				material.specular[1] = g;
				material.specular[2] = b;
				continue;
			}

			// transmittance
			if (token[0] == 'K' && token[1] == 't' && IS_SPACE((token[2]))) {
				token += 2;
				float r, g, b;
				parseFloat3(r, g, b, token);
				material.transmittance[0] = r;
				material.transmittance[1] = g;
				material.transmittance[2] = b;
				continue;
			}

			// ior(index of refraction)
			if (token[0] == 'N' && token[1] == 'i' && IS_SPACE((token[2]))) {
				token += 2;
				material.ior = parseFloat(token);
				continue;
			}

			// emission
			if (token[0] == 'K' && token[1] == 'e' && IS_SPACE(token[2])) {
				token += 2;
				float r, g, b;
				parseFloat3(r, g, b, token);
				material.emission[0] = r;
				material.emission[1] = g;
				material.emission[2] = b;
				continue;
			}

			// shininess
			if (token[0] == 'N' && token[1] == 's' && IS_SPACE(token[2])) {
				token += 2;
				material.shininess = parseFloat(token);
				continue;
			}

			// illum model
			if (0 == strncmp(token, "illum", 5) && IS_SPACE(token[5])) {
				token += 6;
				material.illum = parseInt(token);
				continue;
			}

			// dissolve
			if ((token[0] == 'd' && IS_SPACE(token[1]))) {
				token += 1;
				material.dissolve = parseFloat(token);
				continue;
			}
			if (token[0] == 'T' && token[1] == 'r' && IS_SPACE(token[2])) {
				token += 2;
				// Invert value of Tr(assume Tr is in range [0, 1])
				material.dissolve = 1.0f - parseFloat(token);
				continue;
			}

			// ambient texture
			if ((0 == strncmp(token, "map_Ka", 6)) && IS_SPACE(token[6])) {
				token += 7;
				material.ambient_texname = token;
				continue;
			}

			// diffuse texture
			if ((0 == strncmp(token, "map_Kd", 6)) && IS_SPACE(token[6])) {
				token += 7;
				material.diffuse_texname = token;
				continue;
			}

			// specular texture
			if ((0 == strncmp(token, "map_Ks", 6)) && IS_SPACE(token[6])) {
				token += 7;
				material.specular_texname = token;
				continue;
			}

			// specular highlight texture
			if ((0 == strncmp(token, "map_Ns", 6)) && IS_SPACE(token[6])) {
				token += 7;
				material.specular_highlight_texname = token;
				continue;
			}

			// bump texture
			if ((0 == strncmp(token, "map_bump", 8)) && IS_SPACE(token[8])) {
				token += 9;
				material.bump_texname = token;
				continue;
			}

			// alpha texture
			if ((0 == strncmp(token, "map_d", 5)) && IS_SPACE(token[5])) {
				token += 6;
				material.alpha_texname = token;
				continue;
			}

			// bump texture
			if ((0 == strncmp(token, "bump", 4)) && IS_SPACE(token[4])) {
				token += 5;
				material.bump_texname = token;
				continue;
			}

			// displacement texture
			if ((0 == strncmp(token, "disp", 4)) && IS_SPACE(token[4])) {
				token += 5;
				material.displacement_texname = token;
				continue;
			}

			// unknown parameter
			const char *_space = strchr(token, ' ');
			if (!_space) {
				_space = strchr(token, '\t');
			}
			if (_space) {
				std::ptrdiff_t len = _space - token;
				std::string key(token, static_cast<size_t>(len));
				std::string value = _space + 1;
				material.unknown_parameter.insert(
					std::pair<std::string, std::string>(key, value));
			}
		}
		// flush last material.
		material_map.insert(std::pair<std::string, int>(
			material.name, static_cast<int>(materials.size())));
		materials.push_back(material);
	}

	void MaterialFileReader::InitMaterial(ObjMaterial &material) {
		material.name = "";
		material.ambient_texname = "";
		material.diffuse_texname = "";
		material.specular_texname = "";
		material.specular_highlight_texname = "";
		material.bump_texname = "";
		material.displacement_texname = "";
		material.alpha_texname = "";
		for (int i = 0; i < 3; i++) {
			material.ambient[i] = 0.f;
			material.diffuse[i] = 0.f;
			material.specular[i] = 0.f;
			material.transmittance[i] = 0.f;
			material.emission[i] = 0.f;
		}
		material.illum = 0;
		material.dissolve = 1.f;
		material.shininess = 1.f;
		material.ior = 1.f;
		material.unknown_parameter.clear();
	}

}