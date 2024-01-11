#include <GL/glew.h>
#include <engine/ObjModel.h>
#include "ObjSequence.h"

ObjSequence::ObjSequence() {
	m_numberOfMeshes = 0;
	m_stride = 0;

	m_vertexBuffer.clear();
	m_indexBuffer.clear();
	m_meshes.clear();

	m_vao = 0;
	m_vbo = 0;
	m_ibo = 0;
}

ObjSequence::~ObjSequence() {
	cleanup();
}

void ObjSequence::cleanup() {

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

void ObjSequence::setPosition(float x, float y, float z) {
	m_transform.setPosition(x, y, z);
}

void ObjSequence::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void ObjSequence::rotate(float pitch, float yaw, float roll) {
	m_transform.rotate(pitch, yaw, roll);
}

void ObjSequence::translate(float dx, float dy, float dz) {
	m_transform.translate(dx, dy, dz);
}

void ObjSequence::scale(float sx, float sy, float sz) {
	m_transform.scale(sx, sy, sz);
}

const Matrix4f &ObjSequence::getTransformationMatrix() const {
	return m_transform.getTransformationMatrix();
}

const Matrix4f &ObjSequence::getInvTransformationMatrix() {
	return m_transform.getInvTransformationMatrix();
}

void ObjSequence::loadSequence(const char* _path, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale) {
	return loadSequence(_path, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, isStacked, withoutNormals, generateSmoothNormals, generateFlatNormals, generateSmoothTangents, rescale);
}

void ObjSequence::loadSequence(const char* _path, Vector3f& axis, float degree, Vector3f& translate, float scale, bool isStacked, bool withoutNormals, bool generateSmoothNormals, bool generateFlatNormals, bool generateSmoothTangents, bool rescale) {

	std::vector<std::array<int, 10>> face;

	std::vector<float> vertexCoords;
	std::vector<float> normalCoords;
	std::vector<float> textureCoords;
	std::vector<float> tangentCoords;
	std::vector<float> bitangentCoords;

	std::map<std::string, int> name;
	std::vector<unsigned int> numberOfTriangles;

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
							}
							else if (index == 1) {
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

		numberOfTriangles.push_back(face.size() - std::accumulate(numberOfTriangles.begin(), numberOfTriangles.end(), 0));
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
	name.clear();

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
}

void ObjSequence::addMesh(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	m_numberOfMeshes++;
	m_meshes.push_back({ static_cast<unsigned int>(indexBuffer.size()) / 3, 0u, 0u, 0u });
	m_meshes.back().baseIndex = m_indexBuffer.size();
	m_meshes.back().baseVertex = m_vertexBuffer.size() / m_stride;
	m_meshes.back().drawCount = indexBuffer.size();

	m_vertexBuffer.insert(m_vertexBuffer.end(), vertexBuffer.begin(), vertexBuffer.end());
	m_indexBuffer.insert(m_indexBuffer.end(), indexBuffer.begin(), indexBuffer.end());
}

unsigned int ObjSequence::getNumberOfMeshes() {
	return m_numberOfMeshes;
}

void ObjSequence::loadSequenceGpu() {
	ObjModel::CreateBuffer(m_vertexBuffer, m_indexBuffer, m_vao, m_vbo, m_ibo, m_stride);
}

void ObjSequence::drawRaw(unsigned short frame) {
	glBindVertexArray(m_vao);
	glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[frame].drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[frame].baseIndex), m_meshes[frame].baseVertex);
	glBindVertexArray(0);
}