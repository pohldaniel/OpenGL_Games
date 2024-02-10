#include <GL/glew.h>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "MeshSequence.h"

MeshSequence::MeshSequence() : m_markForDelete(false) {
	m_numberOfMeshes = 0;
	m_stride = 0;

	m_vertexBuffer.clear();
	m_indexBuffer.clear();
	m_meshes.clear();

	m_vao = 0;
	m_vbo = 0;
	m_ibo = 0;
}

MeshSequence::MeshSequence(MeshSequence const& rhs) {
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_transform = rhs.m_transform;
	m_stride = rhs.m_stride;
	m_meshes = rhs.m_meshes;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfIndices = rhs.m_numberOfIndices;
	m_localBoundingBoxes = rhs.m_localBoundingBoxes;
	m_boundingBoxCache = rhs.m_boundingBoxCache;
	m_markForDelete = false;	
}

MeshSequence::MeshSequence(MeshSequence&& rhs) {
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_transform = rhs.m_transform;
	m_stride = rhs.m_stride;
	m_meshes = rhs.m_meshes;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfIndices = rhs.m_numberOfIndices;
	m_localBoundingBoxes = rhs.m_localBoundingBoxes;
	m_boundingBoxCache = rhs.m_boundingBoxCache;
	m_markForDelete = false;
}

MeshSequence& MeshSequence::operator=(const MeshSequence& rhs) {
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_transform = rhs.m_transform;
	m_stride = rhs.m_stride;
	m_meshes = rhs.m_meshes;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfIndices = rhs.m_numberOfIndices;
	m_localBoundingBoxes = rhs.m_localBoundingBoxes;
	m_boundingBoxCache = rhs.m_boundingBoxCache;
	m_markForDelete = false;
	return *this;
}

MeshSequence& MeshSequence::operator=(MeshSequence&& rhs) {
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_transform = rhs.m_transform;
	m_stride = rhs.m_stride;
	m_meshes = rhs.m_meshes;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfIndices = rhs.m_numberOfIndices;
	m_localBoundingBoxes = rhs.m_localBoundingBoxes;
	m_boundingBoxCache = rhs.m_boundingBoxCache;
	m_markForDelete = false;
	return *this;
}

MeshSequence::~MeshSequence() {
	if (m_markForDelete) {
		cleanup();
	}
}

void MeshSequence::cleanup() {

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

	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	m_meshes.clear();
	m_meshes.shrink_to_fit();
}

void MeshSequence::markForDelete() {
	m_markForDelete = true;
}

const std::vector<MeshSequence::Mesh>& MeshSequence::getMeshes() const {
	return m_meshes;
}

void MeshSequence::setPosition(float x, float y, float z) {
	m_transform.setPosition(x, y, z);
}

void MeshSequence::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void MeshSequence::rotate(float pitch, float yaw, float roll) {
	m_transform.rotate(pitch, yaw, roll);
}

void MeshSequence::translate(float dx, float dy, float dz) {
	m_transform.translate(dx, dy, dz);
}

void MeshSequence::scale(float sx, float sy, float sz) {
	m_transform.scale(sx, sy, sz);
}

const Matrix4f &MeshSequence::getTransformationMatrix() const {
	return m_transform.getTransformationMatrix();
}

const Matrix4f &MeshSequence::getInvTransformationMatrix() {
	return m_transform.getInvTransformationMatrix();
}

void MeshSequence::loadSequence(const char* _path, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale) {
	return loadSequence(_path, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, rescale);
}

void MeshSequence::loadSequence(const char* _path, Vector3f& axis, float degree, Vector3f& translate, float scale, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale) {

	std::vector<std::array<int, 10>> face;

	std::vector<float> vertexCoords;
	std::vector<float> normalCoords;
	std::vector<float> textureCoords;
	std::vector<float> tangentCoords;
	std::vector<float> bitangentCoords;

	std::vector<unsigned int> numberOfTriangles;
	//std::vector<BoundingBox> boundingBoxes;

	int assign = 0;

	for (auto const& dir_entry : std::filesystem::directory_iterator(std::filesystem::path(_path))) {
		int offsetPos = vertexCoords.size() / 3;
		int offsetTex = textureCoords.size() / 2;
		int offsetNorm = normalCoords.size() / 3;

		float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
		float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;

		char buffer[250];

		FILE * pFile = fopen(dir_entry.path().string().c_str(), "r");
		if (pFile == NULL) {
			std::cout << "File not found" << std::endl;
		}

		while (fscanf(pFile, "%s", buffer) != EOF) {

			switch (buffer[0]) {

			case '#': {
				fgets(buffer, sizeof(buffer), pFile);
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
							face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, t1 + offsetTex, t2 + offsetTex, t3 + offsetTex, n1 + offsetNorm, n2 + offsetNorm, n3 + offsetNorm, assign });
						}else if (index > 2) {
							p2 = p3;
							t2 = t3;
							n2 = n3;

							p3 = p;
							t3 = t;
							n3 = n;
							face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, t1 + offsetTex, t2 + offsetTex, t3 + offsetTex, n1 + offsetNorm, n2 + offsetNorm, n3 + offsetNorm, assign });
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
							face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, 0, 0, 0, n1 + offsetNorm, n2 + offsetNorm, n3 + offsetNorm, assign });
						}else if (index > 2) {
							p2 = p3;
							n2 = n3;

							p3 = p;
							n3 = n;
							face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, 0, 0, 0, n1 + offsetNorm, n2 + offsetNorm, n3 + offsetNorm, assign });
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
								face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, t1 + offsetTex, t2 + offsetTex, t3 + offsetTex, 0, 0, 0, assign });

							}else if (index > 2) {
								p2 = p3;
								t2 = t3;
								n2 = n3;

								p3 = p;
								t3 = t;
								n3 = n;
								face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, t1 + offsetTex, t2 + offsetTex, t3 + offsetTex, 0, 0, 0, assign });
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
								face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, t1 + offsetTex, t2 + offsetTex, t3 + offsetTex, 0, 0, 0, assign });
							}else if (index > 2) {
								p2 = p3;
								t2 = t3;

								p3 = p;
								t3 = t;
								face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, t1 + offsetTex, t2 + offsetTex, t3 + offsetTex, 0, 0, 0, assign });
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
							face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, 0, 0, 0, 0, 0, 0, assign });
						}else if (index > 2) {
							p2 = p3;
							p3 = p;
							face.push_back({ p1 + offsetPos, p2 + offsetPos, p3 + offsetPos, 0, 0, 0, 0, 0, 0, assign });
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
		
		numberOfTriangles.push_back(static_cast<unsigned int>(face.size() - std::accumulate(numberOfTriangles.begin(), numberOfTriangles.end(), 0)));
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
		}
		//boundingBoxes.push_back(BoundingBox({ xmin , ymin , zmin }, { xmax , ymax , zmax }));

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

		assign++;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < numberOfTriangles.size(); i++) {
		m_meshes.push_back({ numberOfTriangles[i], 0u, 0u, 0u});
		if (m_meshes.size() > 1) {
			m_meshes[m_meshes.size() - 1].baseIndex = m_meshes[m_meshes.size() - 2].numberOfTriangles + m_meshes[m_meshes.size() - 2].baseIndex;
		}
	}

	m_numberOfMeshes = m_meshes.size();

	IndexBufferCreator indexBufferCreator;
	indexBufferCreator.positionCoordsIn = vertexCoords;
	indexBufferCreator.normalCoordsIn = normalCoords;
	indexBufferCreator.textureCoordsIn = textureCoords;
	indexBufferCreator.tangentCoordsIn = tangentCoords;
	indexBufferCreator.bitangentCoordsIn = bitangentCoords;

	for (int j = 0; j < m_numberOfMeshes; j++) {
		std::vector<std::array<int, 10>>::const_iterator first = face.begin() + m_meshes[j].baseIndex;
		std::vector<std::array<int, 10>>::const_iterator last = face.begin() + (m_meshes[j].baseIndex + m_meshes[j].numberOfTriangles);
		std::vector<std::array<int, 10>> subFace(first, last);
		indexBufferCreator.face = subFace;
		indexBufferCreator.createIndexBuffer();

		m_stride = !tangentCoords.empty() ? 14 : (!textureCoords.empty() && !normalCoords.empty()) ? 8 : !normalCoords.empty() ? 6 : !textureCoords.empty() ? 5 : 3;

		m_meshes[j].baseIndex = m_meshes[j].baseIndex * 3;
		m_meshes[j].baseVertex = m_vertexBuffer.size() / m_stride;
		m_meshes[j].drawCount = indexBufferCreator.indexBufferOut.size();

		m_vertexBuffer.insert(m_vertexBuffer.end(), indexBufferCreator.vertexBufferOut.begin(), indexBufferCreator.vertexBufferOut.end());
		m_indexBuffer.insert(m_indexBuffer.end(), indexBufferCreator.indexBufferOut.begin(), indexBufferCreator.indexBufferOut.end());

		indexBufferCreator.indexBufferOut.clear();
		indexBufferCreator.indexBufferOut.shrink_to_fit();
		indexBufferCreator.vertexBufferOut.clear();
		indexBufferCreator.vertexBufferOut.shrink_to_fit();
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

	numberOfTriangles.clear();
	numberOfTriangles.shrink_to_fit();
	//boundingBoxes.clear();
	//boundingBoxes.shrink_to_fit();

	m_numberOfVertices = m_vertexBuffer.size() / m_stride;
	m_numberOfIndices = m_indexBuffer.size();
}

void MeshSequence::addMeshFromFile(const char* path, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale, bool flipYZ, bool flipWinding) {
	return addMeshFromFile(path, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, rescale, flipYZ, flipWinding);
}

void MeshSequence::addMeshFromFile(const char* path, Vector3f& axis, float degree, Vector3f& translate, float scale, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale, bool flipYZ, bool flipWinding) {
	
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(path, (generateSmoothTangents && flipWinding) ? (aiProcess_Triangulate | aiProcess_FindDegenerates) | aiProcess_CalcTangentSpace | aiProcess_FlipWindingOrder :
                                                     generateSmoothTangents                 ? (aiProcess_Triangulate | aiProcess_FindDegenerates) | aiProcess_CalcTangentSpace :
                                                     flipWinding                            ? (aiProcess_Triangulate | aiProcess_FindDegenerates) | aiProcess_FlipWindingOrder :
                                                                                              (aiProcess_Triangulate | aiProcess_FindDegenerates));

	//Importer.GetOrphanedScene();
	bool exportTangents = generateSmoothTangents;
	unsigned int numberOfMeshes = pScene->mNumMeshes;

	float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
	float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;

	for (int j = 0; j < pScene->mNumMeshes; j++) {
		const aiMesh* aiMesh = pScene->mMeshes[j];
		bool hasTextureCoords = aiMesh->HasTextureCoords(0);
		bool hasNormals = aiMesh->HasNormals();
		bool hasTangents = aiMesh->HasTangentsAndBitangents();
		short stride = hasTangents ? 14 : (hasNormals && hasTextureCoords) ? 8 : hasNormals ? 6 : hasTextureCoords ? 5 : 3;

		std::vector<float> vertexBuffer;
		std::vector<unsigned int> indexBuffer;

		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {

			float posY = flipYZ ? aiMesh->mVertices[i].z : aiMesh->mVertices[i].y;
			float posZ = flipYZ ? aiMesh->mVertices[i].y : aiMesh->mVertices[i].z;

			xmin = (std::min)(aiMesh->mVertices[i].x, xmin);
			ymin = (std::min)(posY, ymin);
			zmin = (std::min)(posZ, zmin);

			xmax = (std::max)(aiMesh->mVertices[i].x, xmax);
			ymax = (std::max)(posY, ymax);
			zmax = (std::max)(posZ, zmax);

			vertexBuffer.push_back(aiMesh->mVertices[i].x); vertexBuffer.push_back(posY); vertexBuffer.push_back(posZ);

			if (hasTextureCoords) {
				vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].x); vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].y);
			}

			if (hasNormals) {
				float normY = flipYZ ? aiMesh->mNormals[i].z : aiMesh->mNormals[i].y;
				float normZ = flipYZ ? aiMesh->mNormals[i].y : aiMesh->mNormals[i].z;
				vertexBuffer.push_back(aiMesh->mNormals[i].x); vertexBuffer.push_back(normY); vertexBuffer.push_back(normZ);
			}

			if (hasTangents) {
				float tangY = flipYZ ? aiMesh->mTangents[i].z : aiMesh->mTangents[i].y;
				float tangZ = flipYZ ? aiMesh->mTangents[i].y : aiMesh->mTangents[i].z;

				float bitangY = flipYZ ? aiMesh->mBitangents[i].z : aiMesh->mBitangents[i].y;
				float bitangZ = flipYZ ? aiMesh->mBitangents[i].y : aiMesh->mBitangents[i].z;

				vertexBuffer.push_back(aiMesh->mTangents[i].x); vertexBuffer.push_back(tangY); vertexBuffer.push_back(tangZ);
				vertexBuffer.push_back(aiMesh->mBitangents[i].x); vertexBuffer.push_back(bitangY); vertexBuffer.push_back(bitangZ);
			}
								
		}

		for (unsigned int t = 0; t < aiMesh->mNumFaces; ++t) {
			const aiFace* face = &aiMesh->mFaces[t];
			indexBuffer.push_back(face->mIndices[0]);
			indexBuffer.push_back(face->mIndices[1]);
			indexBuffer.push_back(face->mIndices[2]);
		}

		m_localBoundingBoxes.push_back({ { xmin, ymin, zmin }, { xmax, ymax, zmax } });
		m_boundingBoxCache.insert({ m_numberOfMeshes, m_localBoundingBoxes.size() - 1 });

		m_numberOfMeshes++;
		m_meshes.push_back({ static_cast<unsigned int>(indexBuffer.size()) / 3, 0u, 0u, 0u });
		m_meshes.back().baseIndex = m_numberOfIndices;
		m_meshes.back().baseVertex = m_numberOfVertices;
		m_meshes.back().drawCount = indexBuffer.size();

		m_vertexBuffer.insert(m_vertexBuffer.end(), vertexBuffer.begin(), vertexBuffer.end());
		m_indexBuffer.insert(m_indexBuffer.end(), indexBuffer.begin(), indexBuffer.end());

		m_numberOfVertices = m_vertexBuffer.size() / m_stride;
		m_numberOfIndices = m_indexBuffer.size();

		vertexBuffer.clear();
		vertexBuffer.shrink_to_fit();

		indexBuffer.clear();
		indexBuffer.shrink_to_fit();
	}
}


const std::vector<BoundingBox>& MeshSequence::getLocalBoundingBoxes() const {
	return m_localBoundingBoxes;
}

const BoundingBox& MeshSequence::getLocalBoundingBox(unsigned int meshIndex) const {
	return m_localBoundingBoxes[m_boundingBoxCache.at(meshIndex)];
}

void MeshSequence::addMesh(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	float xmin = FLT_MAX, ymin = FLT_MAX, zmin = FLT_MAX;
	float xmax = -FLT_MAX, ymax = -FLT_MAX, zmax = -FLT_MAX;

	for (std::vector<float>::iterator pit = vertexBuffer.begin(); pit != vertexBuffer.end(); pit = pit + m_stride) {
		xmin = (std::min)(*pit, xmin);
		ymin = (std::min)(*(pit + 1), ymin);
		zmin = (std::min)(*(pit + 2), zmin);

		xmax = (std::max)(*pit, xmax);
		ymax = (std::max)(*(pit + 1), ymax);
		zmax = (std::max)(*(pit + 2), zmax);
	}
	m_localBoundingBoxes.push_back({ { xmin, ymin, zmin }, { xmax, ymax, zmax } });
	m_boundingBoxCache.insert({ m_numberOfMeshes, m_localBoundingBoxes.size() - 1 });

	m_numberOfMeshes++;
	m_meshes.push_back({ static_cast<unsigned int>(indexBuffer.size()) / 3, 0u, 0u, 0u });
	m_meshes.back().baseIndex = m_numberOfIndices;
	m_meshes.back().baseVertex = m_numberOfVertices;
	m_meshes.back().drawCount = indexBuffer.size();

	m_vertexBuffer.insert(m_vertexBuffer.end(), vertexBuffer.begin(), vertexBuffer.end());
	m_indexBuffer.insert(m_indexBuffer.end(), indexBuffer.begin(), indexBuffer.end());

	m_numberOfVertices = m_vertexBuffer.size() / m_stride;
	m_numberOfIndices = m_indexBuffer.size();
}

void MeshSequence::addMeshAfter(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	
	m_numberOfMeshes++;
	m_meshes.push_back({ static_cast<unsigned int>(indexBuffer.size()) / 3, 0u, 0u, 0u });
	m_meshes.back().baseIndex = m_numberOfIndices;
	m_meshes.back().baseVertex = m_numberOfVertices;
	m_meshes.back().drawCount = indexBuffer.size();

	float* vertices = (float*)malloc(((m_numberOfVertices * m_stride) + vertexBuffer.size()) * sizeof(float));

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_numberOfVertices * m_stride * sizeof(float), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (unsigned int i = 0; i < vertexBuffer.size(); i++) {
		vertices[m_numberOfVertices * m_stride + i] = vertexBuffer[i];
	}
	m_numberOfVertices += static_cast<unsigned int>(vertexBuffer.size()) / m_stride;

	unsigned int* indices = (unsigned int*)malloc((m_numberOfIndices + indexBuffer.size()) * sizeof(unsigned int));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_numberOfIndices * sizeof(unsigned int), indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (unsigned int i = 0; i < indexBuffer.size(); i++) {
		indices[m_numberOfIndices + i] = indexBuffer[i];
	}
	m_numberOfIndices += static_cast<unsigned int>(indexBuffer.size());

	glDeleteBuffers(1, &m_vbo);
	m_vbo = 0;

	glDeleteBuffers(1, &m_ibo);
	m_ibo = 0;

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glBindVertexArray(m_vao);

	//Positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_numberOfVertices * m_stride * sizeof(float), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_stride * sizeof(float), (void*)0);

	//Texture Coordinates
	if (m_stride == 5 || m_stride == 8 || m_stride == 14) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, m_stride * sizeof(float), (void*)(3 * sizeof(float)));
	}

	//Normals
	if (m_stride == 6 || m_stride == 8 || m_stride == 14) {
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, m_stride * sizeof(float), (void*)((m_stride == 8 || m_stride == 14) ? 5 * sizeof(float) : 3 * sizeof(float)));
	}

	//Tangents Bitangents
	if (m_stride == 14) {
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, m_stride * sizeof(float), (void*)(8 * sizeof(float)));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, m_stride * sizeof(float), (void*)(11 * sizeof(float)));

	}

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numberOfIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);	
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	free(vertices);
	free(indices);

}

const unsigned int MeshSequence::getNumberOfMeshes() const{
	return m_numberOfMeshes;
}

void MeshSequence::loadSequenceGpu() {
	ObjModel::CreateBuffer(m_vertexBuffer, m_indexBuffer, m_vao, m_vbo, m_ibo, m_stride);
}

const void MeshSequence::draw(unsigned short meshIndex, short textureIndex, short materialIndex) const {

	if(materialIndex >= 0)
		Material::GetMaterials()[materialIndex].updateMaterialUbo(BuiltInShader::materialUbo);

	textureIndex >= 0 ? Material::GetTextures()[textureIndex].bind() : Texture::Unbind();

	glBindVertexArray(m_vao);
	glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[meshIndex].drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[meshIndex].baseIndex), m_meshes[meshIndex].baseVertex);
	glBindVertexArray(0);
}