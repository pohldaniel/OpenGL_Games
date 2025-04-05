
#define CONVHULL_3D_USE_SINGLE_PRECISION
#define CONVHULL_3D_ENABLE
#include "../convhull/convhull_3d.h"

#include "ObjModel.h"

AssetManager<Shader> ObjModel::ShaderManager;

ObjModel::ObjModel()  {

	m_hasMaterial = false;
	m_isStacked = false;
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_hasAABB = false;
	m_hasBoundingSphere = false;
	m_hasConvexHull = false;
	m_markForDelete = false;

	m_numberOfMeshes = 0u;
	m_numberOfTriangles = 0u;
	m_numberOfVertices = 0u;
	m_stride = 0u;
	m_instanceCount = 0u;
	m_drawCount = 0u;
	m_vao = 0u;
	m_vbo = 0u;
	m_ibo = 0u;
	m_vboInstances = 0u;

	m_transform.reset();
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
	m_hasAABB = rhs.m_hasAABB;
	m_hasBoundingSphere = rhs.m_hasBoundingSphere;
	m_hasConvexHull = rhs.m_hasConvexHull;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_mltPath = rhs.m_mltPath;
	m_center = rhs.m_center;
	m_aabb = rhs.m_aabb;
	m_boundingSphere = rhs.m_boundingSphere;
	m_convexHull = rhs.m_convexHull;
	m_shader = rhs.m_shader;
	m_transform = rhs.m_transform;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_instances = rhs.m_instances;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_markForDelete = false;
}

ObjModel::ObjModel(ObjModel&& rhs) {
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_hasAABB = rhs.m_hasAABB;
	m_hasBoundingSphere = rhs.m_hasBoundingSphere;
	m_hasConvexHull = rhs.m_hasConvexHull;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_mltPath = rhs.m_mltPath;
	m_center = rhs.m_center;
	m_aabb = rhs.m_aabb;
	m_boundingSphere = rhs.m_boundingSphere;
	m_convexHull = rhs.m_convexHull;
	m_shader = rhs.m_shader;
	m_transform = rhs.m_transform;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_instances = rhs.m_instances;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_markForDelete = false;
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
	m_hasAABB = rhs.m_hasAABB;
	m_hasBoundingSphere = rhs.m_hasBoundingSphere;
	m_hasConvexHull = rhs.m_hasConvexHull;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_mltPath = rhs.m_mltPath;
	m_center = rhs.m_center;
	m_aabb = rhs.m_aabb;
	m_boundingSphere = rhs.m_boundingSphere;
	m_convexHull = rhs.m_convexHull;
	m_shader = rhs.m_shader;
	m_transform = rhs.m_transform;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_instances = rhs.m_instances;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_markForDelete = false;
	return *this;
}

ObjModel& ObjModel::operator=(ObjModel&& rhs) {
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_hasAABB = rhs.m_hasAABB;
	m_hasBoundingSphere = rhs.m_hasBoundingSphere;
	m_hasConvexHull = rhs.m_hasConvexHull;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_mltPath = rhs.m_mltPath;
	m_center = rhs.m_center;
	m_aabb = rhs.m_aabb;
	m_boundingSphere = rhs.m_boundingSphere;
	m_convexHull = rhs.m_convexHull;
	m_shader = rhs.m_shader;
	m_transform = rhs.m_transform;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_instances = rhs.m_instances;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_markForDelete = false;
	return *this;
}

ObjModel::~ObjModel() {
	if (m_markForDelete) {
		cleanup();
	}
}

void ObjModel::cleanup() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}

	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0u;
	}

	if (m_vboInstances) {
		glDeleteBuffers(1, &m_vboInstances);
		m_vboInstances = 0u;
	}

	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	m_instances.clear();
	m_instances.shrink_to_fit();

	for (ObjMesh* mesh : m_meshes) {
		delete mesh;
	}

	m_meshes.clear();
	m_meshes.shrink_to_fit();

	m_shader.clear();
	m_aabb.cleanup();
}

void ObjModel::markForDelete() {
	m_markForDelete = true;
	for (auto&& mesh : m_meshes) {
		mesh->markForDelete();
	}
}

void ObjModel::setPosition(float x, float y, float z) {
	m_transform.setPosition(x, y, z);
}

void ObjModel::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void ObjModel::rotate(float pitch, float yaw, float roll) {
	m_transform.rotate(pitch, yaw, roll);
}

void ObjModel::translate(float dx, float dy, float dz) {
	m_transform.translate(dx, dy, dz);
}

void ObjModel::scale(float sx, float sy, float sz) {
	m_transform.scale(sx, sy, sz);
}

const Matrix4f &ObjModel::getTransformationMatrix() const {
	return m_transform.getTransformationMatrix();
}

const Matrix4f &ObjModel::getInvTransformationMatrix() {
	return m_transform.getInvTransformationMatrix();
}

const Vector3f &ObjModel::getCenter() const {
	return m_center;
}

const std::string& ObjModel::getMltPath() {
	return m_mltPath;
}

const std::string& ObjModel::getModelDirectory() {
	return m_modelDirectory;
}

void ObjModel::loadModel(const char* filename, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale) {
	loadModelCpu(filename, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, rescale);
	loadModelGpu();
}

void ObjModel::loadModel(const char* filename, const Vector3f& axis, float degree, const Vector3f& translate, float scale, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale) {
	loadModelCpu(filename, axis, degree, translate, scale, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, rescale);
	loadModelGpu();
}

void ObjModel::loadModelCpu(const char* filename, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale) {
	loadModelCpu(filename, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, rescale);
}

bool compare(const std::array<int, 10> &i_lhs, const std::array<int, 10> &i_rhs) {
	return i_lhs[9] < i_rhs[9];
}

void ObjModel::loadModelCpu(const char* _filename, const Vector3f& axis, float degree, const Vector3f& translate, float scale, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale) {

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
					Matrix4f rot;
					rot.rotate(axis, degree);

					Vector3f tmp = rot * Vector3f(tmpx, tmpy, tmpz);

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
	
						Matrix4f rot;
						rot.rotate(axis, degree);

						Vector3f tmp = rot * Vector3f(tmpx, tmpy, tmpz);

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

	m_aabb.min = Vector3f(xmin, ymin, zmin);
	m_aabb.max = Vector3f(xmax, ymax, zmax);

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

void ObjModel::loadModelGpu() {
	if (m_isStacked) {
		ObjModel::CreateBuffer(m_vertexBuffer, m_indexBuffer, m_vao, m_vbo, m_ibo, m_stride);
		//m_vertexBuffer.clear();
        //m_vertexBuffer.shrink_to_fit();
        //m_indexBuffer.clear();
        //m_indexBuffer.shrink_to_fit();
	}else {
		for (auto&& mesh : m_meshes) {
			ObjModel::CreateBuffer(mesh->m_vertexBuffer, mesh->m_indexBuffer, mesh->m_vao, mesh->m_vbo, mesh->m_ibo, mesh->m_stride);
			//mesh->m_vertexBuffer.clear(); 
			//mesh->m_vertexBuffer.shrink_to_fit();
			//mesh->m_indexBuffer.clear();
			//mesh->m_indexBuffer.shrink_to_fit();
		}
	}
}

void ObjModel::drawRaw() const{
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_meshes[j]->drawRaw();
	}
}

void ObjModel::drawRawInstanced() const{
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_meshes[j]->drawRawInstanced();
	}
}

void ObjModel::drawRawStacked() const{
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_meshes[i]->m_baseVertex);
	}
	glBindVertexArray(0);
}

void ObjModel::drawRawInstancedStacked() const{
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_instanceCount, m_meshes[i]->m_baseVertex, 0);
	}
	glBindVertexArray(0);
}

void ObjModel::drawRawSequence(unsigned short frame) const{
	glBindVertexArray(m_vao);
	glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[frame]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[frame]->m_baseIndex), m_meshes[frame]->m_baseVertex);
	glBindVertexArray(0);
}

void ObjModel::draw(const Camera& camera) const{
	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->m_materialIndex];
		material.updateMaterialUbo(BuiltInShader::materialUbo);
		if (!m_shader[i]->inUse()) {
			m_shader[i]->use();

			m_shader[i]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
			m_shader[i]->loadMatrix("u_view", camera.getViewMatrix());
			m_shader[i]->loadMatrix("u_model", m_transform.getTransformationMatrix());
		}

		material.bind();		
		m_meshes[i]->drawRaw();
				
	}
	unuseAllShader();
	Texture::Unbind();	
}

void ObjModel::draw(const Camera& camera, unsigned short meshIndex) const {
	Material& material = Material::GetMaterials()[m_meshes[meshIndex]->m_materialIndex];
	material.updateMaterialUbo(BuiltInShader::materialUbo);
	if (!m_shader[meshIndex]->inUse()) {
		m_shader[meshIndex]->use();

		m_shader[meshIndex]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		m_shader[meshIndex]->loadMatrix("u_view", camera.getViewMatrix());
		m_shader[meshIndex]->loadMatrix("u_model", m_transform.getTransformationMatrix());
	}

	material.bind();
	m_meshes[meshIndex]->drawRaw();

	unuseAllShader();
	Texture::Unbind();
}

void ObjModel::drawInstanced(const Camera& camera) const{
	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->m_materialIndex];
		material.updateMaterialUbo(BuiltInShader::materialUbo);
		if (!m_shader[i]->inUse()) {
			m_shader[i]->use();
			m_shader[i]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		}
		material.bind();
		m_meshes[i]->drawRawInstanced();

		
	}
	unuseAllShader();
	Texture::Unbind();
}

void ObjModel::drawStacked(const Camera& camera) const{
	glBindVertexArray(m_vao);

	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->m_materialIndex];
		material.updateMaterialUbo(BuiltInShader::materialUbo);
		if (!m_shader[i]->inUse()) {
			m_shader[i]->use();
			m_shader[i]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
			m_shader[i]->loadMatrix("u_view", camera.getViewMatrix());
			m_shader[i]->loadMatrix("u_model", m_transform.getTransformationMatrix());
		}
		material.bind();
		glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_meshes[i]->m_baseVertex);
		
	}
	unuseAllShader();
	Texture::Unbind();
	glBindVertexArray(0);
}

void ObjModel::drawInstancedStacked(const Camera& camera) const{
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->m_materialIndex];
		material.updateMaterialUbo(BuiltInShader::materialUbo);
		if (!m_shader[i]->inUse()) {
			m_shader[i]->use();
			m_shader[i]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		}
		material.bind();
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_instanceCount, m_meshes[i]->m_baseVertex, 0);				
	}
	unuseAllShader();
	Texture::Unbind();
	glBindVertexArray(0);
}

void ObjModel::unuseAllShader() const{
	for (Shader* shader : m_shader) {
		if (shader->inUse()) {
			shader->unuse();
		}
	}
}

void ObjModel::drawAABB() const{
	m_aabb.drawRaw();
}

void ObjModel::drawSphere() const {
	m_boundingSphere.drawRaw();
}

void ObjModel::drawHull() const {
	m_convexHull.drawRaw();
}

void ObjModel::createAABB() {
	m_aabb.createBuffer();
	m_hasAABB = true;
}

void ObjModel::createSphere() {
	m_boundingSphere.createBuffer(*this);
}

void ObjModel::createConvexHull(const char* filename, bool useConvhull) {
	createConvexHull(filename, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, useConvhull);
}

void ObjModel::createConvexHull(const char* filename, const Vector3f &rotate, float degree, const Vector3f& translate, float scale, bool useConvhull) {
	m_convexHull.createBuffer(filename, rotate, degree, translate, scale, useConvhull, *this);
}

const ObjMesh* ObjModel::getMesh(unsigned short index) const {
	return m_meshes[index];
}

const BoundingBox& ObjModel::getAABB() const{
	return m_aabb;
}

const BoundingSphere& ObjModel::getBoundingSphere() const {
	return m_boundingSphere;
}

const ConvexHull& ObjModel::getConvexHull() const {
	return m_convexHull;
}

const Transform& ObjModel::getTransform() const {
	return m_transform;
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

const unsigned int& ObjModel::getVbo() const {
	return m_vbo;
}

const unsigned int& ObjModel::getIbo() const {
	return m_ibo;
}

void ObjModel::generateTangents() {

	if (m_isStacked) {
		if (m_hasTangents) { return; }

		ObjModel::GenerateTangents(m_vertexBuffer, m_indexBuffer, *this, m_hasNormals, m_hasTangents,  m_stride, 0, m_meshes.size());		
		ObjModel::CreateBuffer(m_vertexBuffer, m_indexBuffer, m_vao, m_vbo, m_ibo, m_stride);

	}else {

		for (int j = 0; j < m_meshes.size(); j++) {
			if (m_meshes[j]->m_hasTangents) continue;
			ObjModel::GenerateTangents(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, *this, m_meshes[j]->m_hasNormals, m_meshes[j]->m_hasTangents, m_meshes[j]->m_stride, j, j + 1);
			ObjModel::CreateBuffer(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, m_meshes[j]->m_vao, m_meshes[j]->m_vbo, m_meshes[j]->m_ibo, m_meshes[j]->m_stride);
		}
	}
}

void ObjModel::generateNormals() {
	
	if (m_isStacked) {
		if (m_hasNormals) { return; }

		ObjModel::GenerateNormals(m_vertexBuffer, m_indexBuffer, *this, m_hasNormals, m_stride, 0, m_meshes.size());
		ObjModel::CreateBuffer(m_vertexBuffer, m_indexBuffer, m_vao, m_vbo, m_ibo, m_stride);

	}else {

		for (int j = 0; j < m_meshes.size(); j++) {
			if (m_meshes[j]->m_hasNormals) continue;
			ObjModel::GenerateNormals(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, *this, m_meshes[j]->m_hasNormals, m_meshes[j]->m_stride, j, j + 1);
			ObjModel::CreateBuffer(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, m_meshes[j]->m_vao, m_meshes[j]->m_vbo, m_meshes[j]->m_ibo, m_meshes[j]->m_stride);
		}
	}
}

void ObjModel::packBuffer() {
	if (m_isStacked) {
		ObjModel::PackBuffer(m_vertexBuffer, m_vao, m_vbo, m_stride);

	}else {

		for (int j = 0; j < m_meshes.size(); j++) {
			ObjModel::PackBuffer(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_vao, m_meshes[j]->m_vbo, m_meshes[j]->m_stride);
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

void ObjModel::PackBuffer(std::vector<float>& vertexBuffer, unsigned int& vao, unsigned int& vbo, unsigned int stride) {
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferNew.size() * sizeof(float), &vertexBufferNew[0], GL_STATIC_DRAW);
	

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideNew * sizeof(float), (void*)0);

	//Texture Coordinates
	if (stride == 5 || stride == 8 || stride == 14) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideNew * sizeof(float), (void*)(4 * sizeof(float)));
	}

	//Normals
	if (stride == 6 || stride == 8 || stride == 14) {
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, strideNew * sizeof(float), (void*)((stride == 8 || stride == 14) ? 8 * sizeof(float) : 4 * sizeof(float)));
	}

	//Tangents Bitangents
	if (stride == 14) {
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, strideNew * sizeof(float), (void*)(12 * sizeof(float)));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, strideNew * sizeof(float), (void*)(16 * sizeof(float)));
	}
	
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ObjModel::createInstancesStatic(std::vector<Matrix4f>& modelMTX) {
	if (m_isStacked) {
		m_instanceCount = modelMTX.size();

		glGenBuffers(1, &m_vboInstances);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}else {

		for (int j = 0; j < m_numberOfMeshes; j++) {
			m_meshes[j]->createInstancesStatic(modelMTX);
		}
	}
}

void ObjModel::addInstance(const Matrix4f& modelMTX) {
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();
	
	if (m_isStacked) {
		m_instanceCount = m_instances.size();

		glGenBuffers(1, &m_vboInstances);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(GLfloat) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}else {

		for (int j = 0; j < m_numberOfMeshes; j++) {
			m_meshes[j]->addInstance(*this);
		}
	}
}

void ObjModel::createInstancesDynamic(unsigned int numberOfInstances){
	if (m_isStacked) {
		m_instanceCount = numberOfInstances;
		glGenBuffers(1, &m_vboInstances);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);


		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}else {
		for (int j = 0; j < m_numberOfMeshes; j++) {
			m_meshes[j]->createInstancesDynamic(numberOfInstances);
		}
	}
}

void ObjModel::updateInstances(std::vector<Matrix4f>& modelMTX) {
	if (m_isStacked) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferSubData(GL_ARRAY_BUFFER, 0, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}else {
		for (int j = 0; j < m_numberOfMeshes; j++) {
			m_meshes[j]->updateInstances(modelMTX);
		}
	}
}

void ObjModel::initShader(bool instanced) {

	if (!BuiltInShader::materialUbo) {
		glGenBuffers(1, &BuiltInShader::materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::materialBinding, BuiltInShader::materialUbo, 0, 56);
	}

	if (!BuiltInShader::viewUbo && instanced) {
		glGenBuffers(1, &BuiltInShader::viewUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::viewUbo);
		glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::viewBinding, BuiltInShader::viewUbo, 0, 64);
	}

	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->getMaterialIndex()];

		if (material.textures.size() > 0) {

			if (!ShaderManager.checkAsset(instanced ? "diffuse_texture_instance" : "diffuse_texture")) {
				ShaderManager.loadShaderFromString(instanced ? "diffuse_texture_instance" : "diffuse_texture", instanced ? DIFFUSE_TEXTURE_INSTANCE_VS : DIFFUSE_TEXTURE_VS, instanced ? DIFFUSE_TEXTURE_INSTANCE_FS : DIFFUSE_TEXTURE_FS);

				glUniformBlockBinding(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->getProgram(), glGetUniformBlockIndex(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->getProgram(), "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(ShaderManager.getAssetPointer("diffuse_texture_instance")->getProgram(), glGetUniformBlockIndex(ShaderManager.getAssetPointer("diffuse_texture_instance")->getProgram(), "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader.push_back(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture"));
		}else {

			if (!ShaderManager.checkAsset(instanced ? "diffuse_instance" : "diffuse")) {
				ShaderManager.loadShaderFromString(instanced ? "diffuse_instance" : "diffuse", instanced ? DIFFUSE_INSTANCE_VS : DIFFUSE_VS, instanced ? DIFFUSE_INSTANCE_FS : DIFFUSE_FS);

				glUniformBlockBinding(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->getProgram(), glGetUniformBlockIndex(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->getProgram(), "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(ShaderManager.getAssetPointer("diffuse_instance")->getProgram(), glGetUniformBlockIndex(ShaderManager.getAssetPointer("diffuse_instance")->getProgram(), "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader.push_back(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse"));
		}
	}
}

void ObjModel::initShader(AssetManager<Shader>& shaderManager, bool instanced) {

	if (!BuiltInShader::materialUbo) {
		glGenBuffers(1, &BuiltInShader::materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::materialBinding, BuiltInShader::materialUbo, 0, 56);
	}

	if (!BuiltInShader::viewUbo && instanced) {
		glGenBuffers(1, &BuiltInShader::viewUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::viewUbo);
		glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::viewBinding, BuiltInShader::viewUbo, 0, 64);
	}

	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->getMaterialIndex()];

		if (material.textures.size() > 0) {

			if (!ShaderManager.checkAsset(instanced ? "diffuse_texture_instance" : "diffuse_texture")) {
				ShaderManager.loadShaderFromString(instanced ? "diffuse_texture_instance" : "diffuse_texture", instanced ? DIFFUSE_TEXTURE_INSTANCE_VS : DIFFUSE_TEXTURE_VS, instanced ? DIFFUSE_TEXTURE_INSTANCE_FS : DIFFUSE_TEXTURE_FS);

				glUniformBlockBinding(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->getProgram(), glGetUniformBlockIndex(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->getProgram(), "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(ShaderManager.getAssetPointer("diffuse_texture_instance")->getProgram(), glGetUniformBlockIndex(ShaderManager.getAssetPointer("diffuse_texture_instance")->getProgram(), "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader.push_back(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture"));
		}else {

			if (!ShaderManager.checkAsset(instanced ? "diffuse_instance" : "diffuse")) {
				ShaderManager.loadShaderFromString(instanced ? "diffuse_instance" : "diffuse", instanced ? DIFFUSE_INSTANCE_VS : DIFFUSE_VS, instanced ? DIFFUSE_INSTANCE_FS : DIFFUSE_FS);

				glUniformBlockBinding(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->getProgram(), glGetUniformBlockIndex(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->getProgram(), "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(ShaderManager.getAssetPointer("diffuse_instance")->getProgram(), glGetUniformBlockIndex(ShaderManager.getAssetPointer("diffuse_instance")->getProgram(), "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader.push_back(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse"));
		}
	}
}

void ObjModel::CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int& vbo, unsigned int& ibo, unsigned int stride) {

	if (vao)
		glDeleteVertexArrays(1, &vao);

	if (vbo)
		glDeleteBuffers(1, &vbo);

	if (ibo)
		glDeleteBuffers(1, &ibo);

	glGenBuffers(1, &ibo);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Texture Coordinates
	if (stride == 5 || stride == 8 || stride == 14) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
	}

	//Normals
	if (stride == 6 || stride == 8 || stride == 14) {
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)((stride == 8 || stride == 14) ? 5 * sizeof(float) : 3 * sizeof(float)));
	}

	//Tangents Bitangents
	if (stride == 14) {		
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(8 * sizeof(float)));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(11 * sizeof(float)));

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
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

	return textureName, foundSlash < 0 ? modelDirectory + "/" + texPath.substr(foundSlash + 1) : texPath;
}

void ObjModel::ReadMaterialFromFile(std::string path, std::string mltLib, std::string mltName, short& index) {
	

	std::vector<Material>::iterator it = std::find_if(Material::GetMaterials().begin(), Material::GetMaterials().end(), std::bind([](Material const& s1, std::string const& s2) -> bool { return s1.name == s2;}, std::placeholders::_1, mltName));
	if (it == Material::GetMaterials().end()) {

		Material::GetMaterials().resize(Material::GetMaterials().size() + 1);
		index = Material::GetMaterials().size() - 1;
		Material& material = Material::GetMaterials().back();
		material.name = mltName;

		std::vector<std::string*>lines;
		int start = -1;
		int end = -1;

		std::ifstream in(path + mltLib);

		if (!in.is_open()) {
			std::cout << "mlt file not found" << std::endl;
			return;
		}

		std::string line;
		while (getline(in, line)) {
			lines.push_back(new std::string(line));

		}
		in.close();

		for (int i = 0; i < lines.size(); i++) {

			if (strcmp((*lines[i]).c_str(), ("newmtl " + mltName).c_str()) == 0) {
				start = i;
				continue;
			}

			if ((*lines[i]).find("newmtl") != std::string::npos && start >= 0) {
				end = i;
				break;
			}
			else {
				end = lines.size();
			}

		}

		if (start < 0 || end < 0) return;

		for (int i = start; i < end; i++) {

			if ((*lines[i])[0] == '#') {

				continue;

			}else if ((*lines[i])[0] == 'N' && (*lines[i])[1] == 's') {
				int tmp;
				sscanf(lines[i]->c_str(), "Ns %i", &tmp);
				material.setShininess(tmp);

			}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 'a') {
				float tmpx, tmpy, tmpz;
				sscanf(lines[i]->c_str(), "Ka %f %f %f", &tmpx, &tmpy, &tmpz);
				material.setAmbient({ tmpx, tmpx, tmpz, 1.0f });
	
			}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 'd') {
				float tmpx, tmpy, tmpz;
				sscanf(lines[i]->c_str(), "Kd %f %f %f", &tmpx, &tmpy, &tmpz);
				material.setDiffuse({ tmpx, tmpx, tmpz, 1.0f });

			}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 's') {
				float tmpx, tmpy, tmpz;
				sscanf(lines[i]->c_str(), "Ks %f %f %f", &tmpx, &tmpy, &tmpz);
				material.setSpecular({ tmpx, tmpx, tmpz, 1.0f });

			}else if ((*lines[i])[0] == 'm') {

				char identifierBuffer[20], valueBuffer[250];;
				memset(identifierBuffer, 0, 20);
				memset(valueBuffer, 0, 250);
				sscanf(lines[i]->c_str(), "%s %s", identifierBuffer, valueBuffer);

				if (strstr(identifierBuffer, "map_Kd") != 0 && valueBuffer[0] != 0) {
					material.textures[0] = Texture();
					material.textures[0].loadFromFile(GetTexturePath(valueBuffer, path), true);
					material.textures[0].setFilter(GL_LINEAR_MIPMAP_LINEAR);
				}else if (strstr(identifierBuffer, "map_bump") != 0 && valueBuffer[0] != 0) {
					material.textures[1] = Texture();
					material.textures[1].loadFromFile(GetTexturePath(valueBuffer, path), true);
					material.textures[1].setFilter(GL_LINEAR_MIPMAP_LINEAR);
				}else if (strstr(identifierBuffer, "map_Kn") != 0 && valueBuffer[0] != 0) {
					material.textures[1] = Texture();
					material.textures[1].loadFromFile(GetTexturePath(valueBuffer, path), true);
					material.textures[1].setFilter(GL_LINEAR_MIPMAP_LINEAR);
				}else if (strstr(identifierBuffer, "map_Ks") != 0 && valueBuffer[0] != 0) {
					material.textures[2] = Texture();
					material.textures[2].loadFromFile(GetTexturePath(valueBuffer, path), true);
					material.textures[2].setFilter(GL_LINEAR_MIPMAP_LINEAR);
				}
			}
		}

		for (int i = 0; i < lines.size(); i++) {
			delete lines[i];
		}

	}else {
		index = std::distance(Material::GetMaterials().begin(), it);
	}
}

void ObjModel::CleanupShader() {
	ShaderManager.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ObjMesh::ObjMesh(std::string mltName, unsigned int numberTriangles, ObjModel* model){
	m_numberOfTriangles = numberTriangles;
	m_mltName = mltName;
	m_model = model;

	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_markForDelete = false;

	m_triangleOffset = 0u;
	m_stride = 0u;
	m_baseVertex = 0u;
	m_baseIndex = 0u;
	m_drawCount = 0u;
	m_instanceCount = 0u;
	m_vao = 0u;
	m_vbo = 0u;
	m_vboInstances = 0u;
	m_ibo = 0u;
	m_materialIndex = -1;
	m_textureIndex = -1;
}

ObjMesh::ObjMesh(unsigned int numberTriangles, ObjModel* model){
	m_numberOfTriangles = numberTriangles;
	m_model = model;

	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_markForDelete = false;

	m_triangleOffset = 0u;
	m_stride = 0u;
	m_baseVertex = 0u;
	m_baseIndex = 0u;
	m_drawCount = 0u;
	m_instanceCount = 0u;
	m_vao = 0u;
	m_vbo = 0u;
	m_ibo = 0u;
	m_vboInstances = 0u;
	
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
	m_instanceCount = rhs.m_instanceCount;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	m_markForDelete = false;
}

ObjMesh::ObjMesh(ObjMesh&& rhs) {
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
	m_instanceCount = rhs.m_instanceCount;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	m_markForDelete = false;
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
	m_instanceCount = rhs.m_instanceCount;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	m_markForDelete = false;
	return *this;
}

ObjMesh& ObjMesh::operator=(ObjMesh&& rhs) {
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
	m_instanceCount = rhs.m_instanceCount;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	m_markForDelete = false;
	return *this;
}

ObjMesh::~ObjMesh(){
	if(m_markForDelete)
		cleanup();
}

void ObjMesh::cleanup(){

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}

	if (m_vboInstances) {
		glDeleteBuffers(1, &m_vboInstances);
		m_vboInstances = 0;
	}

	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
}

void ObjMesh::markForDelete() {
	m_markForDelete = true;
}

void ObjMesh::createInstancesStatic(std::vector<Matrix4f>& modelMTX){
	m_instanceCount = modelMTX.size();
	
	glGenBuffers(1, &m_vboInstances);

	glBindVertexArray(m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferData(GL_ARRAY_BUFFER, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ObjMesh::addInstance(ObjModel& model) {
	
	std::vector<Matrix4f>& instances = model.m_instances;
	m_instanceCount = instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(float) * 4 * 4, instances[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}else {
		glGenBuffers(1, &m_vboInstances);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(float) * 4 * 4, instances[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void ObjMesh::createInstancesDynamic(unsigned int numberOfInstances) {
	m_instanceCount = numberOfInstances;

	glGenBuffers(1, &m_vboInstances);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ObjMesh::updateInstances(std::vector<Matrix4f>& modelMTX) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferSubData(GL_ARRAY_BUFFER, 0, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ObjMesh::drawRaw() const{

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ObjMesh::drawRawInstanced() const{

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}

const std::vector<float>& ObjMesh::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& ObjMesh::getIndexBuffer() const {
	return m_indexBuffer;
}

int ObjMesh::getStride() {
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

const Material& ObjMesh::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

unsigned int ObjMesh::getNumberOfTriangles() {
	return m_drawCount / 3;
}

const unsigned int& ObjMesh::getVbo() const {
	return m_vbo;
}

const unsigned int& ObjMesh::getIbo() const {
	return m_ibo;
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

void BoundingSphere::createBuffer(ObjModel& model) {

	std::vector<float> vertices;
	for (unsigned int j = 0; j < model.getMeshes().size(); j++) {
		for (unsigned int i = 0; i < model.getMeshes()[j]->getVertexBuffer().size(); i = i + model.getMeshes()[j]->getStride()) {
			vertices.push_back(model.getMeshes()[j]->getVertexBuffer()[i]);
			vertices.push_back(model.getMeshes()[j]->getVertexBuffer()[i + 1]);
			vertices.push_back(model.getMeshes()[j]->getVertexBuffer()[i + 2]);
		}
	}

	int numberOfVertices = vertices.size() / 3;

	float** ap = new float*[numberOfVertices];
	for (int i = 0; i < numberOfVertices; ++i) {
		float* p = new float[3];
		for (int dim = 0; dim < 3; ++dim) {
			p[dim] = vertices[i * 3 + dim];
		}
		ap[i] = p;
	}

	vertices.clear();
	vertices.shrink_to_fit();

	Miniball::Miniball<Miniball::CoordAccessor<float* const*, const float*>> mb(3, ap, ap + numberOfVertices);
	m_radius = sqrtf(mb.squared_radius());
	m_position = Vector3f(mb.center()[0], mb.center()[1], mb.center()[2]);
	
	for (int i = 0; i < numberOfVertices; ++i)
		delete[] ap[i];
	delete[] ap;

	float uAngleStep = (2.0f * PI) / float(m_uResolution);
	float vAngleStep = PI / float(m_vResolution);

	float vSegmentAngle;
	for (unsigned int i = 0; i <= m_vResolution; i++) {

		vSegmentAngle = i * vAngleStep;
		float cosVSegment = cosf(vSegmentAngle);
		float sinVSegment = sinf(vSegmentAngle);

		for (int j = 0; j <= m_uResolution; j++) {

			float uSegmentAngle = j * uAngleStep;

			float cosUSegment = cosf(uSegmentAngle);
			float sinUSegment = sinf(uSegmentAngle);

			// Calculate vertex position on the surface of a sphere
			float x = m_radius * sinVSegment * cosUSegment + m_position[0];
			float y = m_radius * cosVSegment + m_position[1];
			float z = m_radius * sinVSegment * sinUSegment + m_position[2];


			m_vertexBuffer.push_back(x); m_vertexBuffer.push_back(y); m_vertexBuffer.push_back(z);
		}
	}

	//north pole
	for (unsigned int j = 0; j < m_uResolution; j++) {
		m_indexBuffer.push_back(0);
		m_indexBuffer.push_back((m_uResolution + 1) + j + 1);
		m_indexBuffer.push_back((m_uResolution + 1) + j);
	}

	for (unsigned int i = 1; i < m_vResolution - 1; i++) {

		int k1 = i * (m_uResolution + 1);
		int k2 = k1 + (m_uResolution + 1);

		for (unsigned int j = 0; j < m_uResolution; j++) {

			m_indexBuffer.push_back(k1 + j + 1);
			m_indexBuffer.push_back(k2 + j);
			m_indexBuffer.push_back(k1 + j);

			m_indexBuffer.push_back(k2 + j + 1);
			m_indexBuffer.push_back(k2 + j);
			m_indexBuffer.push_back(k1 + j + 1);
		}
	}

	//south pole
	for (unsigned int j = 0; j < m_uResolution; j++) {

		m_indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j);
		m_indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j + 1);
		m_indexBuffer.push_back(m_vResolution * (m_uResolution + 1));

	}

	short stride = 3; short offset = 0;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.size() * sizeof(float), &m_vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	model.m_hasBoundingSphere = true;
}

void BoundingSphere::drawRaw() const {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ConvexHull::createBuffer(const char* filename, const Vector3f &rotate, float degree, const Vector3f& translate, float scale, bool useConvhull, ObjModel& model) {
	std::vector<float> vertexCoords;

	char buffer[250];

	FILE * pFile = fopen(filename, "r");
	if (pFile == NULL) {
		std::cout << "File not found" << std::endl;
		return;
	}

	while (fscanf(pFile, "%s", buffer) != EOF) {

		switch (buffer[0]) {
			case 'v': {

				switch (buffer[1]) {

					case '\0': {						
						float tmpx, tmpy, tmpz;						
						fgets(buffer, sizeof(buffer), pFile);
						sscanf(buffer, "%f %f %f", &tmpx, &tmpy, &tmpz);

						tmpx = tmpx * scale + translate[0];
						tmpy = tmpy * scale + translate[1];
						tmpz = tmpz * scale + translate[2];

						useConvhull ? vertexCoords.push_back(tmpx) : m_vertexBuffer.push_back(tmpx);
						useConvhull ? vertexCoords.push_back(tmpy) : m_vertexBuffer.push_back(tmpy);
						useConvhull ? vertexCoords.push_back(tmpz) : m_vertexBuffer.push_back(tmpz);						
						break;

					}
				}
				break;
			}case 'f': {
				if (useConvhull) break;
			
				int a, b, c;
				fgets(buffer, sizeof(buffer), pFile);
	
				sscanf(buffer, "%d %d %d", &a, &b, &c);
				m_indexBuffer.push_back(a - 1); m_indexBuffer.push_back(b - 1); m_indexBuffer.push_back(c - 1);				
				break;
			}
		}
	}
	fclose(pFile);

	if (useConvhull) {

		int numberOfVertices = vertexCoords.size() / 3;
		ch_vertex* vertices;
		vertices = (ch_vertex*)malloc(numberOfVertices * sizeof(ch_vertex));
		for (unsigned int i = 0; i < numberOfVertices; i++) {
			vertices[i].x = vertexCoords[i * 3 + 0];
			vertices[i].y = vertexCoords[i * 3 + 1];
			vertices[i].z = vertexCoords[i * 3 + 2];
		}

		vertexCoords.clear();
		vertexCoords.shrink_to_fit();

	
		int* faceIndices = NULL;
		int nFaces;

		convhull_3d_build(vertices, numberOfVertices, &faceIndices, &nFaces);

		for (int i = 0; i < nFaces; i++) {
			m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 0]].x); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 0]].y); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 0]].z);
			m_indexBuffer.push_back(i * 3 + 0);

			m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 1]].x); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 1]].y); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 1]].z);
			m_indexBuffer.push_back(i * 3 + 1);

			m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 2]].x); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 2]].y); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 2]].z);
			m_indexBuffer.push_back(i * 3 + 2);
		}

		free(vertices);
		free(faceIndices);

		model.m_hasConvexHull = nFaces > 0;

	}else {
		model.m_hasConvexHull = true;
	}

	if (model.m_hasConvexHull) {
		short stride = 3; short offset = 0;

		glGenBuffers(1, &m_ibo);
		glGenBuffers(1, &m_vbo);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.size() * sizeof(float), &m_vertexBuffer[0], GL_STATIC_DRAW);

		//positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

		//indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void ConvexHull::drawRaw() const {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}