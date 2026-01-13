#include "ObjModel.h"

ObjModel::ObjModel() : m_center(glm::vec3(0.0f, 0.0f, 0.0f)) {

	m_hasMaterial = false;
	m_isStacked = false;
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	
	m_numberOfMeshes = 0u;
	m_numberOfTriangles = 0u;
	m_numberOfVertices = 0u;
	m_stride = 0u;
	m_drawCount = 0u;
}

ObjModel::ObjModel(ObjModel const& rhs) {
	m_numberOfVertices = rhs.m_numberOfVertices;
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
	m_numberOfVertices = rhs.m_numberOfVertices;
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
	m_numberOfVertices = rhs.m_numberOfVertices;
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
	m_numberOfVertices = rhs.m_numberOfVertices;
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

	for (ObjMesh* mesh : m_meshes) {
		delete mesh;
	}
	m_meshes.clear();
	m_meshes.shrink_to_fit();
}

const glm::vec3& ObjModel::getCenter() const {
	return m_center;
}

const std::string& ObjModel::getMltPath() {
	return m_mltPath;
}

const std::string& ObjModel::getModelDirectory() {
	return m_modelDirectory;
}

void ObjModel::loadModel(const char* filename, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool flipYZ, bool rescale) {
	loadModelCpu(filename, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, flipYZ, rescale);
}

void ObjModel::loadModel(const char* filename, const glm::vec3& axis, float degree, const glm::vec3& translate, float scale, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool flipYZ, bool rescale) {
	loadModelCpu(filename, axis, degree, translate, scale, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, flipYZ, rescale);
}

void ObjModel::loadModelCpu(const char* filename, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool flipYZ, bool rescale) {
	loadModelCpu(filename, glm::vec3(0.0, 1.0, 0.0), 0.0, glm::vec3(0.0, 0.0, 0.0), 1.0, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, flipYZ, rescale);
}

bool compare(const std::array<int, 10> &i_lhs, const std::array<int, 10> &i_rhs) {
	return i_lhs[9] < i_rhs[9];
}

void ObjModel::loadModelCpu(const char* _filename, const glm::vec3& axis, float degree, const glm::vec3& translate, float scale, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool flipYZ, bool rescale) {

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

					glm::mat4 rot = glm::rotate(Camera::IDENTITY, degree * PI_ON_180, axis);
					glm::vec3 tmp = rot * glm::vec4(tmpx, posY, posZ, 1.0f);

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
	
						glm::mat4 rot = glm::rotate(Camera::IDENTITY, degree * PI_ON_180, axis);
						glm::vec3 tmp = rot * glm::vec4(tmpx, normY, normZ, 0.0f);

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

	m_center = glm::vec3((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);
	///////////////////////Rescale/////////////////////////////
	if (rescale) {
		glm::vec3 r = 0.5f * (glm::vec3(xmax, ymax, zmax) - glm::vec3(xmin, ymin, zmin));
		glm::vec3 center = glm::vec3(xmin, ymin, zmin) + r;

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

		m_center = glm::vec3((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);
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

	std::map<int, int>::const_iterator iterDup = dup.begin();
	for (iterDup; iterDup != dup.end(); iterDup++) {

		if (name.empty()) {
			m_meshes.push_back(new ObjMesh(iterDup->second, this));
		}else {

			std::map<std::string, int >::const_iterator iterName = name.begin();
			for (iterName; iterName != name.end(); iterName++) {

				if (iterDup->first == iterName->second) {
					m_meshes.push_back(new ObjMesh(iterName->first, iterDup->second, this));
					if (m_meshes.size() > 1) {
						m_meshes[m_meshes.size() - 1]->m_triangleOffset = m_meshes[m_meshes.size() - 2]->m_numberOfTriangles + m_meshes[m_meshes.size() - 2]->m_triangleOffset;
					}
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

	for (int j = 0; j < m_numberOfMeshes; j++) {
		std::vector<std::array<int, 10>>::const_iterator first = face.begin() + m_meshes[j]->m_triangleOffset;
		std::vector<std::array<int, 10>>::const_iterator last = face.begin() + (m_meshes[j]->m_triangleOffset + m_meshes[j]->m_numberOfTriangles);
		std::vector<std::array<int, 10>> subFace(first, last);
		indexBufferCreator.face = subFace;
		indexBufferCreator.createIndexBuffer();
		if (!tangentCoords.empty()) {
			m_hasTextureCoords = true; m_hasNormals = true; m_hasTangents = true;
			m_stride = 14;
			m_meshes[j]->m_hasTextureCoords = true; m_meshes[j]->m_hasNormals = true; m_meshes[j]->m_hasTangents = true;
			m_meshes[j]->m_stride = 14;
		} else if (!textureCoords.empty() && !normalCoords.empty()) {
			m_hasTextureCoords = true; m_hasNormals = true;
			m_stride = 8;
			m_meshes[j]->m_hasTextureCoords = true; m_meshes[j]->m_hasNormals = true;
			m_meshes[j]->m_stride = 8;

		}else if (!normalCoords.empty()) {
			m_hasNormals = true;
			m_stride = 6;
			m_meshes[j]->m_hasNormals = true;
			m_meshes[j]->m_stride = 6;

		}else if (!textureCoords.empty()) {
			m_hasTextureCoords = true;
			m_stride = 5;
			m_meshes[j]->m_hasTextureCoords = true;
			m_meshes[j]->m_stride = 5;

		}else {
			m_stride = 3;
			m_meshes[j]->m_stride = 3;
		}

		if (m_hasMaterial) {
			ObjModel::ReadMaterialFromFile(getModelDirectory() + "/", getMltPath(), m_meshes[j]->m_mltName, m_meshes[j]->m_materialIndex);
		}

		if (!m_isStacked) {
			m_meshes[j]->m_indexBuffer = indexBufferCreator.indexBufferOut;
			m_meshes[j]->m_vertexBuffer = indexBufferCreator.vertexBufferOut;
			m_meshes[j]->m_drawCount = indexBufferCreator.indexBufferOut.size();
			m_drawCount += m_meshes[j]->m_indexBuffer.size();

		}else {			
			m_meshes[j]->m_baseIndex = m_indexBuffer.size();
			m_meshes[j]->m_baseVertex = m_vertexBuffer.size() / m_stride;
			m_meshes[j]->m_drawCount = indexBufferCreator.indexBufferOut.size();

			std::for_each(indexBufferCreator.indexBufferOut.begin(), indexBufferCreator.indexBufferOut.end(), [&value = m_meshes[j]->m_baseVertex](unsigned int& n) { n += value; });

			m_vertexBuffer.insert(m_vertexBuffer.end(), indexBufferCreator.vertexBufferOut.begin(), indexBufferCreator.vertexBufferOut.end());
			m_indexBuffer.insert(m_indexBuffer.end(), indexBufferCreator.indexBufferOut.begin(), indexBufferCreator.indexBufferOut.end());
		}
			
		indexBufferCreator.indexBufferOut.clear();
		indexBufferCreator.indexBufferOut.shrink_to_fit();
		indexBufferCreator.vertexBufferOut.clear();
		indexBufferCreator.vertexBufferOut.shrink_to_fit();
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

const ObjMesh* ObjModel::getMesh(unsigned short index) const {
	return m_meshes[index];
}

const std::vector<ObjMesh*>& ObjModel::getMeshes() const {
	return m_meshes;
}

const std::vector<float>& ObjModel::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& ObjModel::getIndexBuffer() const {
	return m_indexBuffer;
}

unsigned int ObjModel::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void ObjModel::generateTangents() {
	if (m_isStacked) {
		if (m_hasTangents) 
			return; 
		ObjModel::GenerateTangents(m_vertexBuffer, m_indexBuffer, *this, m_hasNormals, m_hasTangents,  m_stride, 0, m_meshes.size());		
	}else {

		for (int j = 0; j < m_meshes.size(); j++) {
			if (m_meshes[j]->m_hasTangents) continue;
			ObjModel::GenerateTangents(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, *this, m_meshes[j]->m_hasNormals, m_meshes[j]->m_hasTangents, m_meshes[j]->m_stride, j, j + 1);
		}
	}
}

void ObjModel::generateNormals() {	
	if (m_isStacked) {
		if (m_hasNormals)
			return; 
		ObjModel::GenerateNormals(m_vertexBuffer, m_indexBuffer, *this, m_hasNormals, m_stride, 0, m_meshes.size());
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			if (m_meshes[j]->m_hasNormals) continue;
			ObjModel::GenerateNormals(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, *this, m_meshes[j]->m_hasNormals, m_meshes[j]->m_stride, j, j + 1);
		}
	}
}

void ObjModel::packBuffer() {
	if (m_isStacked) {
		ObjModel::PackBuffer(m_vertexBuffer, m_stride);
	}else {

		for (int j = 0; j < m_meshes.size(); j++) {
			ObjModel::PackBuffer(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_stride);
		}
	}
}

void ObjModel::GenerateNormals(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, unsigned int& stride, unsigned int startIndex, unsigned int endIndex) {
	if (hasNormals) { return; }

	std::vector<float> tmpVertex;
	const unsigned int *pTriangle = 0;
	float *pVertex0 = 0;
	float *pVertex1 = 0;
	float *pVertex2 = 0;
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float length = 0.0f;
	int vertexLength = stride == 5 ? 8 : 6;
	int vertexOffset = stride == 5 ? 2 : 0;

	for (int i = 0; i < vertexBuffer.size(); i++) {
		tmpVertex.push_back(vertexBuffer[i]);
		if ((i + 1) % stride == 0) {

			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
		}
	}
	
	for (int j = startIndex; j < endIndex; j++) {
		for (int i = 0; i < model.m_meshes[j]->m_numberOfTriangles; i++) {

			pTriangle = &indexBuffer[i * 3 + model.m_meshes[j]->m_baseIndex];

			pVertex0 = &vertexBuffer[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * stride];
			pVertex1 = &vertexBuffer[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * stride];
			pVertex2 = &vertexBuffer[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * stride];

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

			// Accumulate the normals.
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] + normal[2];

			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] + normal[2];

			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 3 + vertexOffset] + normal[0];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 4 + vertexOffset] + normal[1];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * vertexLength + 5 + vertexOffset] + normal[2];
		}
	}

	for (int i = 0; i < tmpVertex.size(); i = i + vertexLength) {

		float length = 1.0f / sqrtf(tmpVertex[i + 3 + vertexOffset] * tmpVertex[i + 3 + vertexOffset] +
			tmpVertex[i + 4 + vertexOffset] * tmpVertex[i + 4 + vertexOffset] +
			tmpVertex[i + 5 + vertexOffset] * tmpVertex[i + 5 + vertexOffset]);

		tmpVertex[i + 3 + vertexOffset] *= length;
		tmpVertex[i + 4 + vertexOffset] *= length;
		tmpVertex[i + 5 + vertexOffset] *= length;
	}

	vertexBuffer.clear();
	copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(vertexBuffer));
	tmpVertex.clear();	

	stride = stride + 3;
	hasNormals = true;
}

void ObjModel::GenerateTangents(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, bool& hasTangents, unsigned int& stride, unsigned int startIndex, unsigned int endIndex) {
	if (hasTangents) { return; }
	if (stride == 3 || stride == 6) { std::cout << "TextureCoords needed!" << std::endl; return; }
	if (!hasNormals) {
		ObjModel::GenerateNormals(vertexBuffer, indexBuffer, model, hasNormals, stride, startIndex, endIndex);
	}

	std::vector<float> tmpVertex;
	const unsigned int *pTriangle = 0;
	float *pVertex0 = 0;
	float *pVertex1 = 0;
	float *pVertex2 = 0;
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float texEdge1[2] = { 0.0f, 0.0f };
	float texEdge2[2] = { 0.0f, 0.0f };
	float tangent[3] = { 0.0f, 0.0f, 0.0f };
	float bitangent[3] = { 0.0f, 0.0f, 0.0f };
	float det = 0.0f;
	float nDotT = 0.0f;
	float bDotB = 0.0f;
	float length = 0.0f;

	for (int i = 0; i < vertexBuffer.size(); i++) {

		tmpVertex.push_back(vertexBuffer[i]);

		if ((i + 1) % stride == 0) {

			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
		}
	}

	for (int j = startIndex; j < endIndex; j++) {
		for (int i = 0; i < model.m_meshes[j]->m_numberOfTriangles; i++) {
		
			pTriangle = &indexBuffer[i * 3 + model.m_meshes[j]->m_baseIndex];

			pVertex0 = &vertexBuffer[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * stride];
			pVertex1 = &vertexBuffer[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * stride];
			pVertex2 = &vertexBuffer[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * stride];

			// Calculate triangle face normal.
			edge1[0] = pVertex1[0] - pVertex0[0];
			edge1[1] = pVertex1[1] - pVertex0[1];
			edge1[2] = pVertex1[2] - pVertex0[2];

			edge2[0] = pVertex2[0] - pVertex0[0];
			edge2[1] = pVertex2[1] - pVertex0[1];
			edge2[2] = pVertex2[2] - pVertex0[2];
			
			texEdge1[0] = pVertex1[3] - pVertex0[3];
			texEdge1[1] = pVertex1[4] - pVertex0[4];

			texEdge2[0] = pVertex2[3] - pVertex0[3];
			texEdge2[1] = pVertex2[4] - pVertex0[4];

			det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

			if (fabs(det) < 1e-6f) {

				tangent[0] = 1.0f;
				tangent[1] = 0.0f;
				tangent[2] = 0.0f;

				bitangent[0] = 0.0f;
				bitangent[1] = 1.0f;
				bitangent[2] = 0.0f;

			}else {

				det = 1.0f / det;

				tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
				tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
				tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

				bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
				bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
				bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
			}
			
			// Accumulate the tangents and bitangents.
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 8] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 8] + tangent[0];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 9] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 9] + tangent[1];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 10] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 10] + tangent[2];

			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 11] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 11] + bitangent[0];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 12] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 12] + bitangent[1];
			tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 13] = tmpVertex[(pTriangle[0] + model.m_meshes[j]->m_baseVertex) * 14 + 13] + bitangent[2];

			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 8] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 8] + tangent[0];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 9] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 9] + tangent[1];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 10] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 10] + tangent[2];

			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 11] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 11] + bitangent[0];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 12] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 12] + bitangent[1];
			tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 13] = tmpVertex[(pTriangle[1] + model.m_meshes[j]->m_baseVertex) * 14 + 13] + bitangent[2];

			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 8] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 8] + tangent[0];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 9] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 9] + tangent[1];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 10] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 10] + tangent[2];

			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 11] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 11] + bitangent[0];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 12] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 12] + bitangent[1];
			tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 13] = tmpVertex[(pTriangle[2] + model.m_meshes[j]->m_baseVertex) * 14 + 13] + bitangent[2];
		}
	}

	// Orthogonalize and normalize the vertex tangents.
	for (int i = 0; i < tmpVertex.size(); i = i + 14) {

		pVertex0 = &tmpVertex[i];

		// Gram-Schmidt orthogonalize tangent with normal.

		nDotT = pVertex0[5] * pVertex0[8] +
			pVertex0[6] * pVertex0[9] +
			pVertex0[7] * pVertex0[10];

		pVertex0[8] -= pVertex0[5] * nDotT;
		pVertex0[9] -= pVertex0[6] * nDotT;
		pVertex0[10] -= pVertex0[7] * nDotT;

		// Normalize the tangent.

		length = 1.0f / sqrtf(pVertex0[8] * pVertex0[8] +
			pVertex0[9] * pVertex0[9] +
			pVertex0[10] * pVertex0[10]);

		pVertex0[8] *= length;
		pVertex0[9] *= length;
		pVertex0[10] *= length;

		bitangent[0] = (pVertex0[6] * pVertex0[10]) -
			(pVertex0[7] * pVertex0[9]);
		bitangent[1] = (pVertex0[7] * pVertex0[8]) -
			(pVertex0[5] * pVertex0[10]);
		bitangent[2] = (pVertex0[5] * pVertex0[9]) -
			(pVertex0[6] * pVertex0[8]);

		bDotB = bitangent[0] * pVertex0[11] +
			bitangent[1] * pVertex0[12] +
			bitangent[2] * pVertex0[13];

		// Calculate handedness
		if (bDotB < 0.0f) {
			pVertex0[11] = -bitangent[0];
			pVertex0[12] = -bitangent[1];
			pVertex0[13] = -bitangent[2];

		}else {

			pVertex0[11] = bitangent[0];
			pVertex0[12] = bitangent[1];
			pVertex0[13] = bitangent[2];
		}
	}

	vertexBuffer.clear();
	copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(vertexBuffer));
	tmpVertex.clear();

	hasTangents = true;
	stride = 14;
}

void ObjModel::PackBuffer(std::vector<float>& vertexBuffer, unsigned int stride) {
	std::vector<float> vertexBufferNew;
	unsigned int strideNew;

	if (stride == 3) {
		vertexBufferNew.resize(vertexBuffer.size() / 3 * 4);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 4, k = k + 3) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;
		}
		strideNew = 4;
	}

	if (stride == 5) {
		vertexBufferNew.resize(vertexBuffer.size() / 5 * 8);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 8, k = k + 5) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;
		}
		strideNew = 8;
	}

	if (stride == 8) {
		vertexBufferNew.resize(vertexBuffer.size() / 8 * 12);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 12, k = k + 8) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;

			vertexBufferNew[i + 8] = vertexBuffer[k + 5];
			vertexBufferNew[i + 9] = vertexBuffer[k + 6];
			vertexBufferNew[i + 10] = vertexBuffer[k + 7];
			vertexBufferNew[i + 11] = 0.0f;
		}
		strideNew = 12;
	}

	if (stride == 14) {
		vertexBufferNew.resize(vertexBuffer.size() / 14 * 20);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 16, k = k + 20) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;

			vertexBufferNew[i + 8] = vertexBuffer[k + 5];
			vertexBufferNew[i + 9] = vertexBuffer[k + 6];
			vertexBufferNew[i + 10] = vertexBuffer[k + 7];
			vertexBufferNew[i + 11] = 0.0f;

			vertexBufferNew[i + 12] = vertexBuffer[k + 8];
			vertexBufferNew[i + 13] = vertexBuffer[k + 9];
			vertexBufferNew[i + 14] = vertexBuffer[k + 10];
			vertexBufferNew[i + 15] = 0.0f;

			vertexBufferNew[i + 16] = vertexBuffer[k + 11];
			vertexBufferNew[i + 17] = vertexBuffer[k + 12];
			vertexBufferNew[i + 18] = vertexBuffer[k + 13];
			vertexBufferNew[i + 19] = 0.0f;
		}
		strideNew = 20;
	}

	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();
	vertexBuffer.insert(vertexBuffer.end(), vertexBufferNew.begin(), vertexBufferNew.end());	
}

void ObjModel::GenerateNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords) {
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

		normalCoords[((face[i])[0] - 1) * 3] = normalCoords[((face[i])[0] - 1) * 3] + normal[0];
		normalCoords[((face[i])[0] - 1) * 3 + 1] = normalCoords[((face[i])[0] - 1) * 3 + 1] + normal[1];
		normalCoords[((face[i])[0] - 1) * 3 + 2] = normalCoords[((face[i])[0] - 1) * 3 + 2] + normal[2];

		normalCoords[((face[i])[1] - 1) * 3] = normalCoords[((face[i])[1] - 1) * 3] + normal[0];
		normalCoords[((face[i])[1] - 1) * 3 + 1] = normalCoords[((face[i])[1] - 1) * 3 + 1] + normal[1];
		normalCoords[((face[i])[1] - 1) * 3 + 2] = normalCoords[((face[i])[1] - 1) * 3 + 2] + normal[2];

		normalCoords[((face[i])[2] - 1) * 3] = normalCoords[((face[i])[2] - 1) * 3] + normal[0];
		normalCoords[((face[i])[2] - 1) * 3 + 1] = normalCoords[((face[i])[2] - 1) * 3 + 1] + normal[1];
		normalCoords[((face[i])[2] - 1) * 3 + 2] = normalCoords[((face[i])[2] - 1) * 3 + 2] + normal[2];

		(face[i])[6] = (face[i])[0]; (face[i])[7] = (face[i])[1]; (face[i])[8] = (face[i])[2];
	}

	for (int i = 0; i < normalCoords.size(); i = i + 3) {

		length = 1.0f / sqrtf(normalCoords[i] * normalCoords[i] + normalCoords[i + 1] * normalCoords[i + 1] + normalCoords[i + 2] * normalCoords[i + 2]);

		normalCoords[i] *= length;
		normalCoords[i + 1] *= length;
		normalCoords[i + 2] *= length;
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

void ObjModel::GenerateTangents(std::vector<float>& vertexCoords, std::vector<float>& textureCoords, std::vector<float>& normalCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& tangentCoords, std::vector<float>& bitangentCoords) {

	if (textureCoords.empty()) return;
	
	tangentCoords.resize(vertexCoords.size());
	bitangentCoords.resize(vertexCoords.size());
	std::vector<float> tmpNormalCoords;
	tmpNormalCoords.resize(vertexCoords.size());

	float pVertex0[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float pVertex1[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float pVertex2[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float texEdge1[2] = { 0.0f, 0.0f };
	float texEdge2[2] = { 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float tangent[3] = { 0.0f, 0.0f, 0.0f };
	float bitangent[3] = { 0.0f, 0.0f, 0.0f };
	float det = 0.0f;

	for (unsigned int i = 0; i < face.size(); i++) {

		pVertex0[0] = vertexCoords[((face[i])[0] - 1) * 3]; pVertex0[1] = vertexCoords[((face[i])[0] - 1) * 3 + 1]; pVertex0[2] = vertexCoords[((face[i])[0] - 1) * 3 + 2];
		pVertex0[3] = textureCoords[((face[i])[3] - 1) * 2]; pVertex0[4] = textureCoords[((face[i])[3] - 1) * 2 + 1];
		tmpNormalCoords[((face[i])[0] - 1) * 3] = normalCoords[((face[i])[6] - 1) * 3];
		tmpNormalCoords[((face[i])[0] - 1) * 3 + 1] = normalCoords[((face[i])[6] - 1) * 3 + 1];
		tmpNormalCoords[((face[i])[0] - 1) * 3 + 2] = normalCoords[((face[i])[6] - 1) * 3 + 2];

		pVertex1[0] = vertexCoords[((face[i])[1] - 1) * 3]; pVertex1[1] = vertexCoords[((face[i])[1] - 1) * 3 + 1]; pVertex1[2] = vertexCoords[((face[i])[1] - 1) * 3 + 2];
		pVertex1[3] = textureCoords[((face[i])[4] - 1) * 2]; pVertex1[4] = textureCoords[((face[i])[4] - 1) * 2 + 1];
		tmpNormalCoords[((face[i])[1] - 1) * 3] = normalCoords[((face[i])[7] - 1) * 3];
		tmpNormalCoords[((face[i])[1] - 1) * 3 + 1] = normalCoords[((face[i])[7] - 1) * 3 + 1];
		tmpNormalCoords[((face[i])[1] - 1) * 3 + 2] = normalCoords[((face[i])[7] - 1) * 3 + 2];

		pVertex2[0] = vertexCoords[((face[i])[2] - 1) * 3]; pVertex2[1] = vertexCoords[((face[i])[2] - 1) * 3 + 1]; pVertex2[2] = vertexCoords[((face[i])[2] - 1) * 3 + 2];
		pVertex2[3] = textureCoords[((face[i])[5] - 1) * 2]; pVertex2[4] = textureCoords[((face[i])[5] - 1) * 2 + 1];
		tmpNormalCoords[((face[i])[2] - 1) * 3] = normalCoords[((face[i])[8] - 1) * 3];
		tmpNormalCoords[((face[i])[2] - 1) * 3 + 1] = normalCoords[((face[i])[8] - 1) * 3 + 1];
		tmpNormalCoords[((face[i])[2] - 1) * 3 + 2] = normalCoords[((face[i])[8] - 1) * 3 + 2];

		edge1[0] = pVertex1[0] - pVertex0[0];
		edge1[1] = pVertex1[1] - pVertex0[1];
		edge1[2] = pVertex1[2] - pVertex0[2];

		edge2[0] = pVertex2[0] - pVertex0[0];
		edge2[1] = pVertex2[1] - pVertex0[1];
		edge2[2] = pVertex2[2] - pVertex0[2];

		texEdge1[0] = pVertex1[3] - pVertex0[3];
		texEdge1[1] = pVertex1[4] - pVertex0[4];

		texEdge2[0] = pVertex2[3] - pVertex0[3];
		texEdge2[1] = pVertex2[4] - pVertex0[4];

		det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

		if (fabs(det) < 1e-6f) {

			tangent[0] = 1.0f;
			tangent[1] = 0.0f;
			tangent[2] = 0.0f;

			bitangent[0] = 0.0f;
			bitangent[1] = 1.0f;
			bitangent[2] = 0.0f;

		}else {
			det = 1.0f / det;

			tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
			tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
			tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

			bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
			bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
			bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
		}
		
		tangentCoords[((face[i])[0] - 1) * 3] = tangentCoords[((face[i])[0] - 1) * 3] + tangent[0];
		tangentCoords[((face[i])[0] - 1) * 3 + 1] = tangentCoords[((face[i])[0] - 1) * 3 + 1] + tangent[1];
		tangentCoords[((face[i])[0] - 1) * 3 + 2] = tangentCoords[((face[i])[0] - 1) * 3 + 2] + tangent[2];

		tangentCoords[((face[i])[1] - 1) * 3] = tangentCoords[((face[i])[1] - 1) * 3] + tangent[0];
		tangentCoords[((face[i])[1] - 1) * 3 + 1] = tangentCoords[((face[i])[1] - 1) * 3 + 1] + tangent[1];
		tangentCoords[((face[i])[1] - 1) * 3 + 2] = tangentCoords[((face[i])[1] - 1) * 3 + 2] + tangent[2];

		tangentCoords[((face[i])[2] - 1) * 3] = tangentCoords[((face[i])[2] - 1) * 3] + tangent[0];
		tangentCoords[((face[i])[2] - 1) * 3 + 1] = tangentCoords[((face[i])[2] - 1) * 3 + 1] + tangent[1];
		tangentCoords[((face[i])[2] - 1) * 3 + 2] = tangentCoords[((face[i])[2] - 1) * 3 + 2] + tangent[2];

		bitangentCoords[((face[i])[0] - 1) * 3] = bitangentCoords[((face[i])[0] - 1) * 3] + bitangent[0];
		bitangentCoords[((face[i])[0] - 1) * 3 + 1] = bitangentCoords[((face[i])[0] - 1) * 3 + 1] + bitangent[1];
		bitangentCoords[((face[i])[0] - 1) * 3 + 2] = bitangentCoords[((face[i])[0] - 1) * 3 + 2] + bitangent[2];

		bitangentCoords[((face[i])[1] - 1) * 3] = bitangentCoords[((face[i])[1] - 1) * 3] + bitangent[0];
		bitangentCoords[((face[i])[1] - 1) * 3 + 1] = bitangentCoords[((face[i])[1] - 1) * 3 + 1] + bitangent[1];
		bitangentCoords[((face[i])[1] - 1) * 3 + 2] = bitangentCoords[((face[i])[1] - 1) * 3 + 2] + bitangent[2];

		bitangentCoords[((face[i])[2] - 1) * 3] = bitangentCoords[((face[i])[2] - 1) * 3] + bitangent[0];
		bitangentCoords[((face[i])[2] - 1) * 3 + 1] = bitangentCoords[((face[i])[2] - 1) * 3 + 1] + bitangent[1];
		bitangentCoords[((face[i])[2] - 1) * 3 + 2] = bitangentCoords[((face[i])[2] - 1) * 3 + 2] + bitangent[2];
	}
	
	float nDotT = 0.0f;
	float bDotB = 0.0f;
	float length = 0.0f;

	for (unsigned int i = 0; i < tangentCoords.size(); i = i + 3) {

		normal[0] = tmpNormalCoords[i]; normal[1] = tmpNormalCoords[i + 1]; normal[2] = tmpNormalCoords[i + 2];
		tangent[0] = tangentCoords[i]; tangent[1] = tangentCoords[i + 1]; tangent[2] = tangentCoords[i + 2];
	
		// Gram-Schmidt orthogonalize tangent with normal.
		nDotT = normal[0] * tangent[0] +
			normal[1] * tangent[1] +
			normal[2] * tangent[2];

		tangent[0] -= normal[0] * nDotT;
		tangent[1] -= normal[1] * nDotT;
		tangent[2] -= normal[2] * nDotT;

		// Normalize the tangent.
		length = 1.0f / sqrtf(tangent[0] * tangent[0] +
			tangent[1] * tangent[1] +
			tangent[2] * tangent[2]);

		tangentCoords[i] *= length;
		tangentCoords[i + 1] *= length;
		tangentCoords[i + 2] *= length;

		bitangent[0] = (normal[1] * tangentCoords[i + 2]) -
			(normal[2] * tangentCoords[i + 1]);
		bitangent[1] = (normal[2] * tangentCoords[i]) -
			(normal[0] * tangentCoords[i + 2]);
		bitangent[2] = (normal[0] * tangentCoords[i + 1]) -
			(normal[1] * tangentCoords[i]);

		bDotB = bitangent[0] * bitangentCoords[i] +
			bitangent[1] * bitangentCoords[i + 1] +
			bitangent[2] * bitangentCoords[i + 2];

		// Calculate handedness
		if (bDotB < 0.0f) {
			bitangentCoords[i] = -bitangent[0];
			bitangentCoords[i + 1] = -bitangent[1];
			bitangentCoords[i + 2] = -bitangent[2];

		}else {
			bitangentCoords[i] = bitangent[0];
			bitangentCoords[i + 1] = bitangent[1];
			bitangentCoords[i + 2] = bitangent[2];
		}

	}
}

std::string ObjModel::GetTexturePath(std::string texPath, std::string modelDirectory) {

	int foundSlash = texPath.find_last_of("/\\");

	int foundDot = texPath.find_last_of(".");
	foundDot = (foundDot < 0 ? texPath.length() : foundDot);
	foundDot = foundSlash < 0 ? foundDot : foundDot - 1;
	std::string textureName = texPath.substr(foundSlash + 1, foundDot);

	return textureName, foundSlash < 0 ? modelDirectory + texPath.substr(foundSlash + 1) : texPath;
}

void ObjModel::ReadMaterialFromFile(std::string path, std::string mltLib, std::string mltName, short& index) {
	std::vector<Material>::iterator it = std::find_if(Material::GetMaterials().begin(), Material::GetMaterials().end(), std::bind([](Material const& s1, std::string const& s2) -> bool { return s1.name == s2; }, std::placeholders::_1, mltName));
	if (it == Material::GetMaterials().end()) {

		Material::GetMaterials().resize(Material::GetMaterials().size() + 1);
		index = Material::GetMaterials().size() - 1;
		Material& material = Material::GetMaterials().back();
		material.name = mltName;

		std::vector<std::string>lines;
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

			if (strcmp(lines[i].c_str(), ("newmtl " + mltName).c_str()) == 0) {
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
				char identifierBuffer[20], valueBuffer[250];
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
				}
			}
		}
	}
	else {
		index = std::distance(Material::GetMaterials().begin(), it);
	}

	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ObjMesh::ObjMesh(std::string mltName, unsigned int numberTriangles, ObjModel* model){
	m_numberOfTriangles = numberTriangles;
	m_mltName = mltName;
	m_model = model;

	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;

	m_triangleOffset = 0u;
	m_stride = 0u;
	m_baseVertex = 0u;
	m_baseIndex = 0u;
	m_drawCount = 0u;
	m_materialIndex = -1;
	m_textureIndex = -1;
}

ObjMesh::ObjMesh(unsigned int numberTriangles, ObjModel* model){
	m_numberOfTriangles = numberTriangles;
	m_model = model;

	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;

	m_triangleOffset = 0u;
	m_stride = 0u;
	m_baseVertex = 0u;
	m_baseIndex = 0u;
	m_drawCount = 0u;
	
	m_materialIndex = -1;
	m_textureIndex = -1;
}

ObjMesh::ObjMesh(ObjMesh const& rhs) {
	m_model = rhs.m_model;
	m_mltName = rhs.m_mltName;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_triangleOffset = rhs.m_triangleOffset;
	m_stride = rhs.m_stride;
	m_baseVertex = rhs.m_baseVertex;
	m_baseIndex = rhs.m_baseIndex;
	m_drawCount = rhs.m_drawCount;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
}

ObjMesh::ObjMesh(ObjMesh&& rhs) noexcept {
	m_model = rhs.m_model;
	m_mltName = rhs.m_mltName;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_triangleOffset = rhs.m_triangleOffset;
	m_stride = rhs.m_stride;
	m_baseVertex = rhs.m_baseVertex;
	m_baseIndex = rhs.m_baseIndex;
	m_drawCount = rhs.m_drawCount;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
}

ObjMesh& ObjMesh::operator=(const ObjMesh& rhs) {
	m_model = rhs.m_model;
	m_mltName = rhs.m_mltName;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_triangleOffset = rhs.m_triangleOffset;
	m_stride = rhs.m_stride;
	m_baseVertex = rhs.m_baseVertex;
	m_baseIndex = rhs.m_baseIndex;
	m_drawCount = rhs.m_drawCount;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	return *this;
}

ObjMesh& ObjMesh::operator=(ObjMesh&& rhs) noexcept {
	m_model = rhs.m_model;
	m_mltName = rhs.m_mltName;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_triangleOffset = rhs.m_triangleOffset;
	m_stride = rhs.m_stride;
	m_baseVertex = rhs.m_baseVertex;
	m_baseIndex = rhs.m_baseIndex;
	m_drawCount = rhs.m_drawCount;
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

const std::vector<float>& ObjMesh::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& ObjMesh::getIndexBuffer() const {
	return m_indexBuffer;
}

const int ObjMesh::getStride() const {
	return m_stride;
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

unsigned int ObjMesh::getNumberOfTriangles() {
	return m_drawCount / 3;
}

const Material& ObjMesh::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}
////////////////////////////////////////////////////////////////////////////////
void IndexBufferCreator::createIndexBuffer() {

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

			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 14);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1],
				normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2],
				tangentCoordsIn[((face[i])[2] - 1) * 3], tangentCoordsIn[((face[i])[2] - 1) * 3 + 1], tangentCoordsIn[((face[i])[2] - 1) * 3 + 2] ,
				bitangentCoordsIn[((face[i])[2] - 1) * 3], bitangentCoordsIn[((face[i])[2] - 1) * 3 + 1], bitangentCoordsIn[((face[i])[2] - 1) * 3 + 2] };

			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 14);
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
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 8);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 8);
		}

	}else if (!normalCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[6] - 1) * 3], normalCoordsIn[((face[i])[6] - 1) * 3 + 1], normalCoordsIn[((face[i])[6] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 6);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[7] - 1) * 3], normalCoordsIn[((face[i])[7] - 1) * 3 + 1], normalCoordsIn[((face[i])[7] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 6);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 6);
		}

	}else if (!textureCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {
			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[3] - 1) * 2], textureCoordsIn[((face[i])[3] - 1) * 2 + 1] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 5);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3],positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[4] - 1) * 2], textureCoordsIn[((face[i])[4] - 1) * 2 + 1] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 5);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 5);
		}

	}else {
		for (int i = 0; i < face.size(); i++) {
			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 3);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 3);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 3);
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