#include "ObjLoader.h"
#include <stdio.h>

namespace tinyobj
{
	
	bool ObjLoader::LoadObj(std::vector<ObjMesh> &meshs,       // [output]
		std::vector<ObjMaterial> &materials, // [output]
		std::string &err, const char *filename, const char *mtl_basepath,
		bool trianglulate) {

		meshs.clear();

		std::stringstream errss;

		std::ifstream ifs(filename);
		if (!ifs) {
			errss << "Cannot open file [" << filename << "]" << std::endl;
			err = errss.str();
			return false;
		}

		std::string basePath;
		if (mtl_basepath) {
			basePath = mtl_basepath;
		}
		MaterialFileReader matFileReader(basePath);

		return LoadObj(meshs, materials, err, ifs, matFileReader, trianglulate);
	}

	bool ObjLoader::LoadObj(std::vector<ObjMesh> &shapes,       // [output]
		std::vector<ObjMaterial> &materials, // [output]
		std::string &err, std::istream &inStream,
		MaterialReader &readMatFn, bool triangulate) 
	{
		std::stringstream errss;

		std::vector<float> v;
		std::vector<float> vn;
		std::vector<float> vt;
		std::vector<Tag> tags;
		std::vector<std::vector<VertexIndex> > faceGroup;
		std::string name;

		// material
		std::map<std::string, int> material_map;
		std::map<VertexIndex, unsigned int> vertexCache;
		int material = -1;

		ObjMesh shape;

		int maxchars = 8192;                                  // Alloc enough size.
		std::vector<char> buf(static_cast<size_t>(maxchars)); // Alloc enough size.
		while (inStream.peek() != -1) {
			inStream.getline(&buf[0], maxchars);

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

						  // vertex
			if (token[0] == 'v' && IS_SPACE((token[1]))) {
				token += 2;
				float x, y, z;
				parseFloat3(x, y, z, token);
				v.push_back(x);
				v.push_back(y);
				v.push_back(z);
				continue;
			}

			// normal
			if (token[0] == 'v' && token[1] == 'n' && IS_SPACE((token[2]))) {
				token += 3;
				float x, y, z;
				parseFloat3(x, y, z, token);
				vn.push_back(x);
				vn.push_back(y);
				vn.push_back(z);
				continue;
			}

			// texcoord
			if (token[0] == 'v' && token[1] == 't' && IS_SPACE((token[2]))) {
				token += 3;
				float x, y;
				parseFloat2(x, y, token);
				vt.push_back(x);
				vt.push_back(y);
				continue;
			}

			// face
			if (token[0] == 'f' && IS_SPACE((token[1]))) {
				token += 2;
				token += strspn(token, " \t");

				std::vector<VertexIndex> face;
				face.reserve(3);

				while (!IS_NEW_LINE(token[0])) {
					VertexIndex vi = parseTriple(token, static_cast<int>(v.size() / 3),
						static_cast<int>(vn.size() / 3),
						static_cast<int>(vt.size() / 2));
					face.push_back(vi);
					size_t n = strspn(token, " \t\r");
					token += n;
				}

				// replace with emplace_back + std::move on C++11
				faceGroup.push_back(std::vector<VertexIndex>());
				faceGroup[faceGroup.size() - 1].swap(face);

				continue;
			}

			// use mtl
			if ((0 == strncmp(token, "usemtl", 6)) && IS_SPACE((token[6]))) {

				char namebuf[TINYOBJ_SSCANF_BUFFER_SIZE];
				token += 7;
#ifdef _MSC_VER
				sscanf_s(token, "%s", namebuf, (unsigned)_countof(namebuf));
#else
				sscanf(token, "%s", namebuf);
#endif

				int newMaterialId = -1;
				if (material_map.find(namebuf) != material_map.end()) {
					newMaterialId = material_map[namebuf];
				}
				else {
					// { error!! material not found }
				}

				if (newMaterialId != material) {
					// Create per-face material
					ExportFaceGroupToShape(shape, vertexCache, v, vn, vt, faceGroup, tags,
						material, name, true, triangulate);
					faceGroup.clear();
					material = newMaterialId;
				}

				continue;
			}

			// load mtl
			if ((0 == strncmp(token, "mtllib", 6)) && IS_SPACE((token[6]))) {
				char namebuf[TINYOBJ_SSCANF_BUFFER_SIZE];
				token += 7;
#ifdef _MSC_VER
				sscanf_s(token, "%s", namebuf, (unsigned)_countof(namebuf));
#else
				sscanf(token, "%s", namebuf);
#endif

				std::string err_mtl;
				bool ok = readMatFn(namebuf, materials, material_map, err_mtl);
				err += err_mtl;

				if (!ok) {
					faceGroup.clear(); // for safety
					return false;
				}

				continue;
			}

			// group name
			if (token[0] == 'g' && IS_SPACE((token[1]))) {

				// flush previous face group.
				bool ret =
					ExportFaceGroupToShape(shape, vertexCache, v, vn, vt, faceGroup, tags,
						material, name, true, triangulate);
				if (ret) {
					shapes.push_back(shape);
				}

				shape = ObjMesh();

				// material = -1;
				faceGroup.clear();

				std::vector<std::string> names;
				names.reserve(2);

				while (!IS_NEW_LINE(token[0])) {
					std::string str = parseString(token);
					names.push_back(str);
					token += strspn(token, " \t\r"); // skip tag
				}

				assert(names.size() > 0);

				// names[0] must be 'g', so skip the 0th element.
				if (names.size() > 1) {
					name = names[1];
				}
				else {
					name = "";
				}

				continue;
			}

			// object name
			if (token[0] == 'o' && IS_SPACE((token[1]))) {

				// flush previous face group.
				bool ret =
					ExportFaceGroupToShape(shape, vertexCache, v, vn, vt, faceGroup, tags,
						material, name, true, triangulate);
				if (ret) {
					shapes.push_back(shape);
				}

				// material = -1;
				faceGroup.clear();
				shape = ObjMesh();

				// @todo { multiple object name? }
				char namebuf[TINYOBJ_SSCANF_BUFFER_SIZE];
				token += 2;
#ifdef _MSC_VER
				sscanf_s(token, "%s", namebuf, (unsigned)_countof(namebuf));
#else
				sscanf(token, "%s", namebuf);
#endif
				name = std::string(namebuf);

				continue;
			}

			if (token[0] == 't' && IS_SPACE(token[1])) {
				Tag tag;

				char namebuf[4096];
				token += 2;
#ifdef _MSC_VER
				sscanf_s(token, "%s", namebuf, (unsigned)_countof(namebuf));
#else
				sscanf(token, "%s", namebuf);
#endif
				tag.name = std::string(namebuf);

				token += tag.name.size() + 1;

				TagSizes ts = parseTagTriple(token);

				tag.intValues.resize(static_cast<size_t>(ts.num_ints));

				for (size_t i = 0; i < static_cast<size_t>(ts.num_ints); ++i) {
					tag.intValues[i] = atoi(token);
					token += strcspn(token, "/ \t\r") + 1;
				}

				tag.floatValues.resize(static_cast<size_t>(ts.num_floats));
				for (size_t i = 0; i < static_cast<size_t>(ts.num_floats); ++i) {
					tag.floatValues[i] = parseFloat(token);
					token += strcspn(token, "/ \t\r") + 1;
				}

				tag.stringValues.resize(static_cast<size_t>(ts.num_strings));
				for (size_t i = 0; i < static_cast<size_t>(ts.num_strings); ++i) {
					char stringValueBuffer[4096];

#ifdef _MSC_VER
					sscanf_s(token, "%s", stringValueBuffer, 4096);
#else
					sscanf(token, "%s", stringValueBuffer);
#endif


					//sscanf_s(token, "%s", stringValueBuffer);
					tag.stringValues[i] = stringValueBuffer;
					token += tag.stringValues[i].size() + 1;
				}

				tags.push_back(tag);
			}

			// Ignore unknown command.
		}
		bool ret = ExportFaceGroupToShape(shape, vertexCache, v, vn, vt, faceGroup,
			tags, material, name, true, triangulate);
		if (ret) {
			shapes.push_back(shape);
		}
		faceGroup.clear(); // for safety

		err += errss.str();
		return true;
	}

	bool ObjLoader::ExportFaceGroupToShape(ObjMesh &shape, std::map<VertexIndex, unsigned int> vertexCache,
		const std::vector<float> &in_positions,
		const std::vector<float> &in_normals,
		const std::vector<float> &in_texcoords,
		const std::vector<std::vector<VertexIndex> > &faceGroup,
		std::vector<Tag> &tags, const int material_id, const std::string &name,
		bool clearCache, bool triangulate)
	{
		if (faceGroup.empty()) 
		{
			return false;
		}

		// Flatten vertices and indices
		for (size_t i = 0; i < faceGroup.size(); i++) {
			const std::vector<VertexIndex> &face = faceGroup[i];

			VertexIndex i0 = face[0];
			VertexIndex i1(-1);
			VertexIndex i2 = face[1];

			size_t npolys = face.size();

			if (triangulate) {

				// Polygon -> triangle fan conversion
				for (size_t k = 2; k < npolys; k++) {
					i1 = i2;
					i2 = face[k];

					unsigned int v0 = UpdateVertex(
						vertexCache, shape.positions, shape.normals,
						shape.texcoords, in_positions, in_normals, in_texcoords, i0);
					unsigned int v1 = UpdateVertex(
						vertexCache, shape.positions, shape.normals,
						shape.texcoords, in_positions, in_normals, in_texcoords, i1);
					unsigned int v2 = UpdateVertex(
						vertexCache, shape.positions, shape.normals,
						shape.texcoords, in_positions, in_normals, in_texcoords, i2);

					shape.indices.push_back(v0);
					shape.indices.push_back(v1);
					shape.indices.push_back(v2);

					shape.num_vertices.push_back(3);
					shape.material_ids.push_back(material_id);
				}
			}
			else {

				for (size_t k = 0; k < npolys; k++) {
					unsigned int v =
						UpdateVertex(vertexCache, shape.positions, shape.normals,
							shape.texcoords, in_positions, in_normals,
							in_texcoords, face[k]);

					shape.indices.push_back(v);
				}

				shape.num_vertices.push_back(static_cast<unsigned char>(npolys));
				shape.material_ids.push_back(material_id); // per face
			}
		}

		shape.name = name;
		shape.tags.swap(tags);

		if (clearCache)
			vertexCache.clear();

		return true;
	}

	unsigned int ObjLoader::UpdateVertex(std::map<VertexIndex, unsigned int> &vertexCache,
			std::vector<float> &positions, std::vector<float> &normals,
			std::vector<float> &texcoords,
			const std::vector<float> &in_positions,
			const std::vector<float> &in_normals,
			const std::vector<float> &in_texcoords, const VertexIndex &i){
		const std::map<VertexIndex, unsigned int>::iterator it = vertexCache.find(i);

		if (it != vertexCache.end()) {
			// found cache
			return it->second;
		}

		assert(in_positions.size() > static_cast<unsigned int>(3 * i.v_idx + 2));

		positions.push_back(in_positions[3 * static_cast<size_t>(i.v_idx) + 0]);
		positions.push_back(in_positions[3 * static_cast<size_t>(i.v_idx) + 1]);
		positions.push_back(in_positions[3 * static_cast<size_t>(i.v_idx) + 2]);

		if ((i.vn_idx >= 0) &&
			(static_cast<size_t>(i.vn_idx * 3 + 2) < in_normals.size())) {
			normals.push_back(in_normals[3 * static_cast<size_t>(i.vn_idx) + 0]);
			normals.push_back(in_normals[3 * static_cast<size_t>(i.vn_idx) + 1]);
			normals.push_back(in_normals[3 * static_cast<size_t>(i.vn_idx) + 2]);
		}

		if ((i.vt_idx >= 0) &&
			(static_cast<size_t>(i.vt_idx * 2 + 1) < in_texcoords.size())) {
			texcoords.push_back(in_texcoords[2 * static_cast<size_t>(i.vt_idx) + 0]);
			texcoords.push_back(in_texcoords[2 * static_cast<size_t>(i.vt_idx) + 1]);
		}

		unsigned int idx = static_cast<unsigned int>(positions.size() / 3 - 1);
		vertexCache[i] = idx;

		return idx;
	}

}