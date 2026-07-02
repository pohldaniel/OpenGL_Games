#include "ObjModel.h"

ObjModel::ObjModel()  {

	m_hasMaterial = false;
	m_isStacked = false;
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;

	m_numberOfMeshes = 0u;
	m_numberOfTriangles = 0u;
	m_stride = 0u;
	m_drawCount = 0u;
}

ObjModel::ObjModel(ObjModel const& rhs) {
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_mltPath = rhs.m_mltPath;
	m_center = rhs.m_center;
	m_drawCount = rhs.m_drawCount;
}

ObjModel::ObjModel(ObjModel&& rhs) noexcept {
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_mltPath = rhs.m_mltPath;
	m_center = rhs.m_center;
	m_drawCount = rhs.m_drawCount;
}

ObjModel& ObjModel::operator=(const ObjModel& rhs) {
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_mltPath = rhs.m_mltPath;
	m_center = rhs.m_center;
	m_drawCount = rhs.m_drawCount;
	return *this;
}

ObjModel& ObjModel::operator=(ObjModel&& rhs) noexcept {
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_mltPath = rhs.m_mltPath;
	m_center = rhs.m_center;
	m_drawCount = rhs.m_drawCount;
	return *this;
}

ObjModel::~ObjModel() {
	cleanup();
}

void ObjModel::cleanup() {
	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();

	for (Mesh* mesh : m_meshes) {
		delete mesh;
	}
	m_meshes.clear();
	m_meshes.shrink_to_fit();
}

const Vector3f &ObjModel::getCenter() const {
	return m_center;
}

const unsigned int ObjModel::getStride() const {
	return m_isStacked ? m_stride : m_meshes.back()->getStride();
}

const std::string& ObjModel::getMltPath() {
	return m_mltPath;
}

const std::string& ObjModel::getModelDirectory() {
	return m_modelDirectory;
}

void ObjModel::loadModel(const char* filename, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool flipYZ, bool flipWinding, bool rescale) {
	loadModelCpu(filename, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, flipYZ, flipWinding, rescale);
}

void ObjModel::loadModel(const char* filename, const Vector3f& axis, float degree, const Vector3f& translate, float scale, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool flipYZ, bool flipWinding, bool rescale) {
	loadModelCpu(filename, axis, degree, translate, scale, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, flipYZ, flipWinding, rescale);
}

void ObjModel::loadModelCpu(const char* filename, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool flipYZ, bool flipWinding, bool rescale) {
	loadModelCpu(filename, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, flipYZ, flipWinding, rescale);
}

bool compare(const std::array<int, 10> &i_lhs, const std::array<int, 10> &i_rhs) {
	return i_lhs[9] < i_rhs[9];
}

void ObjModel::loadModelCpu(const char* _filename, const Vector3f& axis, float degree, const Vector3f& translate, float scale, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool flipYZ, bool flipWinding, bool rescale) {

	std::string filename(_filename);
	const size_t index = filename.rfind('/');

	if (std::string::npos != index) {
		m_modelDirectory = filename.substr(0, index);
	}

	std::vector<std::array<int, 10>> face;

	std::vector<float> vertexCoords;
	std::vector<float> normalCoords;
	std::vector<float> textureCoords;
	std::vector<float> tangentCoords;
	std::vector<float> bitangentCoords;

	std::map<std::string, int> name;

	int countMesh = 0;
	int assign = 0;

	float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
	float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;

	char buffer[250];

	FILE * pFile = fopen(_filename, "r");
	if (pFile == NULL) {
		std::cout << "File not found" << std::endl;
		return;
	}

	while (fscanf(pFile, "%s", buffer) != EOF) {

		switch (buffer[0]) {

		case '#': {

			fgets(buffer, sizeof(buffer), pFile);
			break;

		}case 'm': {

			fgets(buffer, sizeof(buffer), pFile);
			sscanf(buffer, "%s %s", buffer, buffer);
			m_mltPath = buffer;

			m_hasMaterial = true;
			break;

		}case 'v': {

			switch (buffer[1]) {

				case '\0': {

					float tmpx, tmpy, tmpz;
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%f %f %f", &tmpx, &tmpy, &tmpz);

					float posY = flipYZ ? tmpz : tmpy;
					float posZ = flipYZ ? tmpy : tmpz;

					Matrix4f rot;
					rot.rotate(axis, degree);

					Vector3f tmp = rot * Vector3f(tmpx, posY, posZ);

					tmpx = tmp[0] * scale + translate[0];
					tmpy = tmp[1] * scale + translate[1];
					tmpz = tmp[2] * scale + translate[2];

					vertexCoords.push_back(tmpx);
					vertexCoords.push_back(tmpy);
					vertexCoords.push_back(tmpz);

					xmin = (std::min)(tmpx, xmin);
					ymin = (std::min)(tmpy, ymin);
					zmin = (std::min)(tmpz, zmin);

					xmax = (std::max)(tmpx, xmax);
					ymax = (std::max)(tmpy, ymax);
					zmax = (std::max)(tmpz, zmax);
					break;

				}case 't': {

					float tmpu, tmpv;
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%f %f", &tmpu, &tmpv);

					textureCoords.push_back(tmpu);
					textureCoords.push_back(tmpv);
					break;

				}case 'n': {

					if (!withoutNormals) {
						float tmpx, tmpy, tmpz;
						fgets(buffer, sizeof(buffer), pFile);
						sscanf(buffer, "%f %f %f", &tmpx, &tmpy, &tmpz);
	
						float normY = flipYZ ? tmpz : tmpy;
						float normZ = flipYZ ? tmpy : tmpz;

						Matrix4f rot;
						rot.rotate(axis, degree);

						Vector3f tmp = rot * Vector3f(tmpx, normY, normZ);

						normalCoords.push_back(tmp[0]);
						normalCoords.push_back(tmp[1]);
						normalCoords.push_back(tmp[2]);
					}
					break;

				}default: {

					break;
				}
			}
			break;

		}case 'u': {

			if (m_hasMaterial) {

				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s %s", buffer, buffer);

				std::map<std::string, int >::const_iterator iter = name.find(buffer);

				if (iter == name.end()) {
					// mlt name not found
					countMesh++;
					assign = countMesh;

					name[buffer] = countMesh;

				}else {
					// mlt name found
					assign = iter->second;
				}
			}
			break;

		}case 'g': {
			if (!m_hasMaterial) {

				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s", buffer);

				countMesh++;
				assign = countMesh;
				name[buffer] = countMesh;
			}
			break;

		}case 'f': {

			fgets(buffer, sizeof(buffer), pFile);
			int p = 0, n = 0, t = 0, acc = 0, read = 0, index = 0;
			int p1 = 0, p2 = 0, p3 = 0, n1 = 0, n2 = 0, n3 = 0, t1 = 0, t2 = 0, t3 = 0;

			if (!textureCoords.empty() && !normalCoords.empty()) {
				while (sscanf(buffer + acc, "%d/%d/%d%n", &p, &t, &n, &read) > 0) {
					acc += read;
					if (index == 0) {
						p1 = p;
						t1 = t;
						n1 = n;
					}else if (index == 1) {
						p2 = p;
						t2 = t;
						n2 = n;
					}else if (index == 2) {
						p3 = p;
						t3 = t;
						n3 = n;
						face.push_back({ p1, p2, p3, t1, t2, t3, n1, n2, n3, assign });

					}else if (index > 2) {
						p2 = p3;
						t2 = t3;
						n2 = n3;

						p3 = p;
						t3 = t;
						n3 = n;
						face.push_back({ p1, p2, p3, t1, t2, t3, n1, n2, n3, assign });
					}
					index++;
				}
			}else if (!normalCoords.empty()) {
				while (sscanf(buffer + acc, "%d//%d%n", &p, &n, &read) > 0) {
					acc += read;
					if (index == 0) {
						p1 = p;
						n1 = n;
					}else if (index == 1) {
						p2 = p;
						n2 = n;
					}else if (index == 2) {
						p3 = p;
						n3 = n;
						face.push_back({ p1, p2, p3, 0, 0, 0, n1, n2, n3, assign });
					}else if (index > 2) {
						p2 = p3;
						n2 = n3;

						p3 = p;
						n3 = n;
						face.push_back({ p1, p2, p3, 0, 0, 0, n1, n2, n3, assign });
					}

					index++;
				}
			}else if (!textureCoords.empty()) {
				if (!withoutNormals) {
					while (sscanf(buffer + acc, "%d/%d/%d%n", &p, &t, &n, &read) > 0) {
						acc += read;
						if (index == 0) {
							p1 = p;
							t1 = t;
							n1 = n;
						}else if (index == 1) {
							p2 = p;
							t2 = t;
							n2 = n;
						}else if (index == 2) {
							p3 = p;
							t3 = t;
							n3 = n;

							face.push_back({ p1, p2, p3, t1, t2, t3, 0, 0, 0, assign });

						}else if (index > 2) {
							p2 = p3;
							t2 = t3;
							n2 = n3;

							p3 = p;
							t3 = t;
							n3 = n;

							face.push_back({ p1, p2, p3, t1, t2, t3, 0, 0, 0, assign });
						}
						index++;
					}
				}else {
					while (sscanf(buffer + acc, "%d/%d%n", &p, &t, &read) > 0) {
						acc += read;
						if (index == 0) {
							p1 = p;
							t1 = t;
						}else if (index == 1) {
							p2 = p;
							t2 = t;
						}else if (index == 2) {
							p3 = p;
							t3 = t;
							face.push_back({ p1, p2, p3, t1, t2, t3, 0, 0, 0, assign });
						}else if (index > 2) {
							p2 = p3;
							t2 = t3;

							p3 = p;
							t3 = t;
							face.push_back({ p1, p2, p3, t1, t2, t3, 0, 0, 0, assign });
						}
						index++;
					}
				}
			}else {
				while (sscanf(buffer + acc, "%d%n", &p, &read) > 0) {
					acc += read;
					if (index == 0) {
						p1 = p;
					}else if (index == 1) {
						p2 = p;
					}else if (index == 2) {
						p3 = p;
						face.push_back({ p1, p2, p3, 0, 0, 0, 0, 0, 0, assign });
					}else if (index > 2) {
						p2 = p3;
						p3 = p;
						face.push_back({ p1, p2, p3, 0, 0, 0, 0, 0, 0, assign });
					}
					index++;
				}
			}
			break;
		}default: {
			break;
		}

		}//end switch
	}// end while
	fclose(pFile);

	m_center = Vector3f((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);
	///////////////////////Rescale/////////////////////////////
	if (rescale) {
		Vector3f r = 0.5f * (Vector3f(xmax, ymax, zmax) - Vector3f(xmin, ymin, zmin));
		Vector3f center = Vector3f(xmin, ymin, zmin) + r;

		float oldRadius = (std::max)(r[0], (std::max)(r[1], r[2]));
		float _scale = scale / oldRadius;

		xmin = FLT_MAX; ymin = FLT_MAX; zmin = FLT_MAX;
		xmax = -FLT_MAX; ymax = -FLT_MAX; zmax = -FLT_MAX;

		for (std::vector<float>::iterator pit = vertexCoords.begin(); pit != vertexCoords.end(); pit += 3) {
			*(pit + 0) = _scale * (*(pit + 0) - center[0]);
			*(pit + 1) = _scale * (*(pit + 1) - center[1]);
			*(pit + 2) = _scale * (*(pit + 2) - center[2]);

			xmin = (std::min)(*(pit + 0), xmin);
			ymin = (std::min)(*(pit + 1), ymin);
			zmin = (std::min)(*(pit + 2), zmin);

			xmax = (std::max)(*(pit + 0), xmax);
			ymax = (std::max)(*(pit + 1), ymax);
			zmax = (std::max)(*(pit + 2), zmax);
		}

		m_center = Vector3f((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);
	}

	if (!withoutNormals) {
		if (generateSmoothNormals) {
			ObjModel::GenerateNormals(vertexCoords, face, normalCoords);
		}

		if (generateFlatNormals) {
			ObjModel::GenerateFlatNormals(vertexCoords, face, normalCoords);
		}
	}

	if (generateSmoothTangents) {
		if (normalCoords.empty()) {
			ObjModel::GenerateNormals(vertexCoords, face, normalCoords);
		}
		ObjModel::GenerateTangents(vertexCoords, textureCoords, normalCoords, face, tangentCoords, bitangentCoords);
	}

	std::sort(face.begin(), face.end(), compare);
	std::map<int, int> dup;

	for (int i = 0; i < face.size(); i++) {
		dup[face[i][9]]++;
	}

	std::vector<unsigned int> numberOfTriangles;
	std::map<int, int>::const_iterator iterDup = dup.begin();
	for (; iterDup != dup.end(); iterDup++) {

		if (name.empty()) {
			m_meshes.push_back(new ObjMesh(this));
			numberOfTriangles.push_back(iterDup->second);
		}else {

			std::map<std::string, int >::const_iterator iterName = name.begin();
			for (; iterName != name.end(); iterName++) {

				if (iterDup->first == iterName->second) {					
					m_meshes.push_back(new ObjMesh(this, iterName->first));	
					numberOfTriangles.push_back(iterDup->second);
				}
			}
		}
	}

	m_numberOfMeshes = m_meshes.size();
	dup.clear();
	name.clear();

	m_isStacked = !(m_numberOfMeshes == 1) && isStacked;

	IndexBufferCreator indexBufferCreator;
	indexBufferCreator.positionCoordsIn = vertexCoords;
	indexBufferCreator.normalCoordsIn = normalCoords;
	indexBufferCreator.textureCoordsIn = textureCoords;
	indexBufferCreator.tangentCoordsIn = tangentCoords;
	indexBufferCreator.bitangentCoordsIn = bitangentCoords;
	
	std::vector<unsigned int>::iterator triangles = numberOfTriangles.begin();
	Mesh* prevMesh = nullptr;
	unsigned int triangleOffset = 0u;
	for (Mesh* _mesh : m_meshes) {
		ObjMesh* mesh = static_cast<ObjMesh*>(_mesh);
		triangleOffset = prevMesh ? triangleOffset + (prevMesh->m_indexBuffer.size() / 3)  : 0u;

		std::vector<std::array<int, 10>>::const_iterator first = face.begin() + triangleOffset;
		std::vector<std::array<int, 10>>::const_iterator last = face.begin() + (triangleOffset + *triangles);
		std::vector<std::array<int, 10>> subFace(first, last);
		indexBufferCreator.face = subFace;
		indexBufferCreator.createIndexBuffer(flipWinding || flipYZ);
		if (!tangentCoords.empty()) {
			m_hasTextureCoords = true; m_hasNormals = true; m_hasTangents = true;
			m_stride = 14;
			mesh->m_hasTextureCoords = true; mesh->m_hasNormals = true; mesh->m_hasTangents = true;
			mesh->m_stride = 14;
		} else if (!textureCoords.empty() && !normalCoords.empty()) {
			m_hasTextureCoords = true; m_hasNormals = true;
			m_stride = 8;
			mesh->m_hasTextureCoords = true; mesh->m_hasNormals = true;
			mesh->m_stride = 8;

		}else if (!normalCoords.empty()) {
			m_hasNormals = true;
			m_stride = 6;
			mesh->m_hasNormals = true;
			mesh->m_stride = 6;

		}else if (!textureCoords.empty()) {
			m_hasTextureCoords = true;
			m_stride = 5;
			mesh->m_hasTextureCoords = true;
			mesh->m_stride = 5;

		}else {
			m_stride = 3;
			mesh->m_stride = 3;
		}

		if (m_hasMaterial) {
			ObjModel::ReadMaterialFromFile(getModelDirectory() + "/", getMltPath(), mesh->m_mltName, mesh->m_materialIndex);
		}

		if (!m_isStacked) {
			mesh->m_indexBuffer = indexBufferCreator.indexBufferOut;
			mesh->m_vertexBuffer = indexBufferCreator.vertexBufferOut;
			mesh->m_drawCount = indexBufferCreator.indexBufferOut.size();
			m_drawCount += mesh->m_indexBuffer.size();

		}else {
			
			mesh->m_baseIndex = m_indexBuffer.size();
			mesh->m_baseVertex = m_vertexBuffer.size() / m_stride;
			mesh->m_drawCount = indexBufferCreator.indexBufferOut.size();

			m_vertexBuffer.insert(m_vertexBuffer.end(), indexBufferCreator.vertexBufferOut.begin(), indexBufferCreator.vertexBufferOut.end());
			m_indexBuffer.insert(m_indexBuffer.end(), indexBufferCreator.indexBufferOut.begin(), indexBufferCreator.indexBufferOut.end());
		}
			
		indexBufferCreator.indexBufferOut.clear();
		indexBufferCreator.indexBufferOut.shrink_to_fit();
		indexBufferCreator.vertexBufferOut.clear();
		indexBufferCreator.vertexBufferOut.shrink_to_fit();

		prevMesh = _mesh;
		triangles++;
	}

	if (m_isStacked) {
		m_drawCount = m_indexBuffer.size();
	}

	indexBufferCreator.positionCoordsIn.clear();
	indexBufferCreator.positionCoordsIn.shrink_to_fit();
	indexBufferCreator.normalCoordsIn.clear();
	indexBufferCreator.normalCoordsIn.shrink_to_fit();
	indexBufferCreator.textureCoordsIn.clear();
	indexBufferCreator.textureCoordsIn.shrink_to_fit();
	indexBufferCreator.tangentCoordsIn.clear();
	indexBufferCreator.tangentCoordsIn.shrink_to_fit();
	indexBufferCreator.bitangentCoordsIn.clear();
	indexBufferCreator.bitangentCoordsIn.shrink_to_fit();

	return;
}

const Mesh* ObjModel::getMesh(unsigned short index) const {
	return m_meshes[index];
}

const std::vector<Mesh*>& ObjModel::getMeshes() const {
	return m_meshes;
}

const std::vector<float>& ObjModel::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& ObjModel::getIndexBuffer() const {
	return m_indexBuffer;
}

const unsigned int ObjModel::getNumberOfTriangles() const {
	return m_drawCount / 3;
}

void ObjModel::generateNormals() {
	if (m_isStacked) {
		Model::GenerateNormals(m_vertexBuffer, m_indexBuffer, *this, m_hasNormals, m_stride, 0, m_meshes.size());
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::GenerateNormals(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, *this, m_meshes[j]->m_hasNormals, m_meshes[j]->m_stride, j, j + 1);
		}
	}
}

void ObjModel::generateTangents() {
	if (m_isStacked) {
		Model::GenerateTangents(m_vertexBuffer, m_indexBuffer, *this, m_hasNormals, m_hasTangents,  m_stride, 0, m_meshes.size());		
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::GenerateTangents(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, *this, m_meshes[j]->m_hasNormals, m_meshes[j]->m_hasTangents, m_meshes[j]->m_stride, j, j + 1);
		}
	}
}

void ObjModel::rewind() {
	if (m_isStacked) {
		Model::Rewind(m_vertexBuffer, m_indexBuffer, m_stride);
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::Rewind(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, m_meshes[j]->m_stride);
		}
	}
}

void ObjModel::generateColors(ModelColor modelColor) {
	if (m_isStacked) {
		Model::GenerateColors(m_vertexBuffer, m_indexBuffer, m_stride, modelColor);
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::GenerateColors(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, m_meshes[j]->m_stride, modelColor);
		}
	}
}

void ObjModel::generateUVs(ProjectedPlane projectedPlane) {
	if (m_isStacked) {
		Model::GenerateUVs(m_vertexBuffer, m_stride, projectedPlane);
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::GenerateUVs(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_stride, projectedPlane);
		}
	}
}

void ObjModel::packBuffer() {
	if (m_isStacked) {
		Model::PackBuffer(m_vertexBuffer, m_stride);
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::PackBuffer(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_stride);
		}
	}
}

void ObjModel::GenerateFlatNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords) {
	normalCoords.resize(vertexCoords.size());
	float pVertex0[3] = { 0.0f, 0.0f, 0.0f };
	float pVertex1[3] = { 0.0f, 0.0f, 0.0f };
	float pVertex2[3] = { 0.0f, 0.0f, 0.0f };
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float length;

	for (unsigned int i = 0; i < face.size(); i++) {

		pVertex0[0] = vertexCoords[((face[i])[0] - 1) * 3]; pVertex0[1] = vertexCoords[((face[i])[0] - 1) * 3 + 1]; pVertex0[2] = vertexCoords[((face[i])[0] - 1) * 3 + 2];
		pVertex1[0] = vertexCoords[((face[i])[1] - 1) * 3]; pVertex1[1] = vertexCoords[((face[i])[1] - 1) * 3 + 1]; pVertex1[2] = vertexCoords[((face[i])[1] - 1) * 3 + 2];
		pVertex2[0] = vertexCoords[((face[i])[2] - 1) * 3]; pVertex2[1] = vertexCoords[((face[i])[2] - 1) * 3 + 1]; pVertex2[2] = vertexCoords[((face[i])[2] - 1) * 3 + 2];

		// Calculate triangle face normal.
		edge1[0] = pVertex1[0] - pVertex0[0];
		edge1[1] = pVertex1[1] - pVertex0[1];
		edge1[2] = pVertex1[2] - pVertex0[2];

		edge2[0] = pVertex2[0] - pVertex0[0];
		edge2[1] = pVertex2[1] - pVertex0[1];
		edge2[2] = pVertex2[2] - pVertex0[2];

		normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
		normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
		normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

		length = 1.0f / sqrtf(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

		normalCoords[((face[i])[0] - 1) * 3] = normal[0] * length;
		normalCoords[((face[i])[0] - 1) * 3 + 1] = normal[1] * length;
		normalCoords[((face[i])[0] - 1) * 3 + 2] = normal[2] * length;

		normalCoords[((face[i])[1] - 1) * 3] = normal[0] * length;
		normalCoords[((face[i])[1] - 1) * 3 + 1] = normal[1] * length;
		normalCoords[((face[i])[1] - 1) * 3 + 2] = normal[2] * length;

		normalCoords[((face[i])[2] - 1) * 3] = normal[0] * length;
		normalCoords[((face[i])[2] - 1) * 3 + 1] = normal[1] * length;
		normalCoords[((face[i])[2] - 1) * 3 + 2] = normal[2] * length;

		(face[i])[6] = (face[i])[0]; (face[i])[7] = (face[i])[1]; (face[i])[8] = (face[i])[2];
	}
}


std::string ObjModel::GetTexturePath(const std::string& texPath, const std::string& modelDirectory) {
	int foundSlash = texPath.find_last_of("/\\");
	int foundDot = texPath.find_last_of(".");
	foundDot = (foundDot < 0 ? texPath.length() : foundDot);
	foundDot = foundSlash < 0 ? foundDot : foundDot - 1;
	return foundSlash < 0 ? modelDirectory + texPath.substr(foundSlash + 1) : texPath;
}

void ObjModel::ReadMaterialFromFile(const std::string& path, const std::string& mltLib, const std::string& mltName, short& index) {
	std::vector<Material>::iterator it = std::find_if(Material::GetMaterials().begin(), Material::GetMaterials().end(), std::bind([](Material const& s1, std::string const& s2) -> bool { return s1.m_name == s2;}, std::placeholders::_1, mltName));
	if (it == Material::GetMaterials().end()) {

		Material::GetMaterials().resize(Material::GetMaterials().size() + 1);
		index = Material::GetMaterials().size() - 1;
		Material& material = Material::GetMaterials().back();
		material.m_name = mltName;

		std::vector<std::string> lines;
		int start = -1;
		int end = -1;
		std::ifstream in(path + mltLib);
		if (!in.is_open()) {
			std::cout << "mlt file not found" << std::endl;
			return;
		}

		std::string line;
		while (getline(in, line)) {
			lines.push_back(line);

		}
		in.close();

		for (int i = 0; i < lines.size(); i++) {
			if (lines[i].compare("newmtl " + mltName) == 0) {
				start = i;
				continue;
			}
			if (lines[i].find("newmtl") != std::string::npos && start >= 0) {
				end = i;
				break;
			}else {
				end = lines.size();
			}
		}

		if (start < 0 || end < 0) return;

		for (int i = start; i < end; i++) {
			if (lines[i][0] == '#') {
				continue;
			}else if (lines[i][0] == 'N' && lines[i][1] == 's') {
				int tmp;
				sscanf(lines[i].c_str(), "Ns %i", &tmp);
				material.setShininess(tmp);
			}else if (lines[i][0] == 'K' && lines[i][1] == 'a') {
				float tmpx, tmpy, tmpz;
				sscanf(lines[i].c_str(), "Ka %f %f %f", &tmpx, &tmpy, &tmpz);
				material.setAmbient({ tmpx, tmpx, tmpz, 1.0f });
			}else if (lines[i][0] == 'K' && lines[i][1] == 'd') {
				float tmpx, tmpy, tmpz;
				sscanf(lines[i].c_str(), "Kd %f %f %f", &tmpx, &tmpy, &tmpz);
				material.setDiffuse({ tmpx, tmpx, tmpz, 1.0f });
			}else if (lines[i][0] == 'K' && lines[i][1] == 's') {
				float tmpx, tmpy, tmpz;
				sscanf(lines[i].c_str(), "Ks %f %f %f", &tmpx, &tmpy, &tmpz);
				material.setSpecular({ tmpx, tmpx, tmpz, 1.0f });
			}else if (lines[i][0] == 'm') {
				char identifierBuffer[20], valueBuffer[250];;
				memset(identifierBuffer, 0, 20);
				memset(valueBuffer, 0, 250);
				sscanf(lines[i].c_str(), "%s %s", identifierBuffer, valueBuffer);
				if (strstr(identifierBuffer, "map_Kd") != 0 && valueBuffer[0] != 0) {
					material.addTexture(TextureSlot::TEXTURE_DIFFUSE, GetTexturePath(valueBuffer, path));
				}else if (strstr(identifierBuffer, "map_bump") != 0 && valueBuffer[0] != 0) {
					material.addTexture(TextureSlot::TEXTURE_NORMAL, GetTexturePath(valueBuffer, path));
				}else if (strstr(identifierBuffer, "map_Kn") != 0 && valueBuffer[0] != 0) {
					material.addTexture(TextureSlot::TEXTURE_NORMAL, GetTexturePath(valueBuffer, path));
				}else if (strstr(identifierBuffer, "map_Ks") != 0 && valueBuffer[0] != 0) {
					material.addTexture(TextureSlot::TEXTURE_SPECULAR, GetTexturePath(valueBuffer, path));
				}else if (strstr(identifierBuffer, "map_Ke") != 0 && valueBuffer[0] != 0) {
					material.addTexture(TextureSlot::TEXTURE_EMISSIVE, GetTexturePath(valueBuffer, path));
				}else if (strstr(identifierBuffer, "map_Pm") != 0 && valueBuffer[0] != 0) {
					material.addTexture(TextureSlot::TEXTURE_METALNESS, GetTexturePath(valueBuffer, path));
				}else if (strstr(identifierBuffer, "map_Pr") != 0 && valueBuffer[0] != 0) {
					material.addTexture(TextureSlot::TEXTURE_DIFFUSE_ROUGHNESS, GetTexturePath(valueBuffer, path));
				}
			}
		}
	}else {
		index = std::distance(Material::GetMaterials().begin(), it);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ObjMesh::ObjMesh(ObjModel* model, const std::string& mltName) : Mesh(){
	m_mltName = mltName;
	m_model = model;

	m_materialIndex = -1;
	m_textureIndex = -1;
}

ObjMesh::ObjMesh(ObjModel* model) : Mesh() {
	m_model = model;

	m_materialIndex = -1;
	m_textureIndex = -1;
}

ObjMesh::ObjMesh(ObjMesh const& rhs) : Mesh(rhs) {
	m_model = rhs.m_model;
	m_mltName = rhs.m_mltName;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
}

ObjMesh::ObjMesh(ObjMesh&& rhs) noexcept : Mesh(rhs) {
	m_model = rhs.m_model;
	m_mltName = rhs.m_mltName;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
}

ObjMesh& ObjMesh::operator=(const ObjMesh& rhs) {
	Mesh::operator=(rhs);
	m_model = rhs.m_model;
	m_mltName = rhs.m_mltName;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	return *this;
}

ObjMesh& ObjMesh::operator=(ObjMesh&& rhs) noexcept {
	Mesh::operator=(rhs);
	m_model = rhs.m_model;
	m_mltName = rhs.m_mltName;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	return *this;
}

ObjMesh::~ObjMesh(){
	cleanup();
}

void ObjMesh::cleanup(){
	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
}

short ObjMesh::getMaterialIndex() const {
	return m_materialIndex;
}

void ObjMesh::setMaterialIndex(short index) const {
	m_materialIndex = index;
}

short ObjMesh::getTextureIndex()const {
	return m_textureIndex;
}

void ObjMesh::setTextureIndex(short index) const {
	m_textureIndex = index;
}

const Material& ObjMesh::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

unsigned int ObjMesh::getNumberOfTriangles() const {
	return m_drawCount / 3;
}

const bool ObjMesh::hasMaterial() const {
	return m_materialIndex >= 0;
}
////////////////////////////////////////////////////////////////////////////////
void IndexBufferCreator::createIndexBuffer(bool flipWinding) {

	indexBufferOut.resize(face.size() * 3);
	if (!tangentCoordsIn.empty()) {
		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[3] - 1) * 2], textureCoordsIn[((face[i])[3] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[6] - 1) * 3], normalCoordsIn[((face[i])[6] - 1) * 3 + 1], normalCoordsIn[((face[i])[6] - 1) * 3 + 2],
			tangentCoordsIn[((face[i])[0] - 1) * 3], tangentCoordsIn[((face[i])[0] - 1) * 3 + 1], tangentCoordsIn[((face[i])[0] - 1) * 3 + 2],
			bitangentCoordsIn[((face[i])[0] - 1) * 3], bitangentCoordsIn[((face[i])[0] - 1) * 3 + 1], bitangentCoordsIn[((face[i])[0] - 1) * 3 + 2]};
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 14);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[4] - 1) * 2], textureCoordsIn[((face[i])[4] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[7] - 1) * 3], normalCoordsIn[((face[i])[7] - 1) * 3 + 1], normalCoordsIn[((face[i])[7] - 1) * 3 + 2],
			tangentCoordsIn[((face[i])[1] - 1) * 3], tangentCoordsIn[((face[i])[1] - 1) * 3 + 1], tangentCoordsIn[((face[i])[1] - 1) * 3 + 2],
			bitangentCoordsIn[((face[i])[1] - 1) * 3], bitangentCoordsIn[((face[i])[1] - 1) * 3 + 1], bitangentCoordsIn[((face[i])[1] - 1) * 3 + 2]};
			flipWinding ? indexBufferOut[i * 3 + 2] = addVertex(((face[i])[1] - 1), &vertex2[0], 14) : indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 14);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2],
			tangentCoordsIn[((face[i])[2] - 1) * 3], tangentCoordsIn[((face[i])[2] - 1) * 3 + 1], tangentCoordsIn[((face[i])[2] - 1) * 3 + 2] ,
			bitangentCoordsIn[((face[i])[2] - 1) * 3], bitangentCoordsIn[((face[i])[2] - 1) * 3 + 1], bitangentCoordsIn[((face[i])[2] - 1) * 3 + 2] };
			flipWinding ? indexBufferOut[i * 3 + 1] = addVertex(((face[i])[2] - 1), &vertex3[0], 14) : indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 14);
		}
	} else if (!textureCoordsIn.empty() && !normalCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[3] - 1) * 2], textureCoordsIn[((face[i])[3] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[6] - 1) * 3], normalCoordsIn[((face[i])[6] - 1) * 3 + 1], normalCoordsIn[((face[i])[6] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 8);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[4] - 1) * 2], textureCoordsIn[((face[i])[4] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[7] - 1) * 3], normalCoordsIn[((face[i])[7] - 1) * 3 + 1], normalCoordsIn[((face[i])[7] - 1) * 3 + 2] };
			flipWinding ? indexBufferOut[i * 3 + 2] = addVertex(((face[i])[1] - 1), &vertex2[0], 8) : indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 8);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2] };
			flipWinding ? indexBufferOut[i * 3 + 1] = addVertex(((face[i])[2] - 1), &vertex3[0], 8) : indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 8);
		}

	}else if (!normalCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
			normalCoordsIn[((face[i])[6] - 1) * 3], normalCoordsIn[((face[i])[6] - 1) * 3 + 1], normalCoordsIn[((face[i])[6] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 6);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
			normalCoordsIn[((face[i])[7] - 1) * 3], normalCoordsIn[((face[i])[7] - 1) * 3 + 1], normalCoordsIn[((face[i])[7] - 1) * 3 + 2] };
			flipWinding ? indexBufferOut[i * 3 + 2] = addVertex(((face[i])[1] - 1), &vertex2[0], 6) : indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 6);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
			normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2] };
			flipWinding ? indexBufferOut[i * 3 + 1] = addVertex(((face[i])[2] - 1), &vertex3[0], 6) : indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 6);
		}

	}else if (!textureCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {
			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[3] - 1) * 2], textureCoordsIn[((face[i])[3] - 1) * 2 + 1] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 5);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3],positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[4] - 1) * 2], textureCoordsIn[((face[i])[4] - 1) * 2 + 1] };
			flipWinding ? indexBufferOut[i * 3 + 2] = addVertex(((face[i])[1] - 1), &vertex2[0], 5) : indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 5);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1] };
			flipWinding ? indexBufferOut[i * 3 + 1] = addVertex(((face[i])[2] - 1), &vertex3[0], 5) : indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 5);
		}

	}else {
		for (int i = 0; i < face.size(); i++) {
			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 3);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2] };
			flipWinding ? indexBufferOut[i * 3 + 2] = addVertex(((face[i])[1] - 1), &vertex2[0], 3) : indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 3);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2] };
			flipWinding ? indexBufferOut[i * 3 + 1] = addVertex(((face[i])[2] - 1), &vertex3[0], 3) : indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 3);
		}
	}
	
	face.clear();
	face.shrink_to_fit();

	std::map<int, std::vector<int>>().swap(m_vertexCache);
}

int IndexBufferCreator::addVertex(int hash, const float *pVertex, int stride) {

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(vertexBufferOut.size() / stride);
		vertexBufferOut.insert(vertexBufferOut.end(), pVertex, pVertex + stride);
		m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));
	}else {
		// One or more vertices have been hashed to this entry in the cache.
		const std::vector<int> &vertices = iter->second;
		const float *pCachedVertex = 0;
		bool found = false;

		for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
			index = *i;
			pCachedVertex = &vertexBufferOut[index * stride];

			if (memcmp(pCachedVertex, pVertex, sizeof(float)* stride) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			index = static_cast<int>(vertexBufferOut.size() / stride);
			vertexBufferOut.insert(vertexBufferOut.end(), pVertex, pVertex + stride);
			m_vertexCache[hash].push_back(index);
		}
	}
	return index;
}