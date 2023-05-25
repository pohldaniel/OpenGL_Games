#include "Shape.h"
#include "MeshCapsule.h"
#include "MeshTorus.h"
#include "MeshSphere.h"
#include "MeshSpiral.h"
#include "MeshCylinder.h"
#include "MeshQuad.h"
#include "MeshCube.h"
#include "MeshDisk.h"
#include <iostream>

Shape::Shape() { }

void Shape::buildCapsule(float radius, float length, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshCapsule::BuildMesh(radius, length, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildTorus(float radius, float tubeRadius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshTorus::BuildMesh(radius, tubeRadius, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildSphere(float radius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshSphere::BuildMesh(radius, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildSpiral(float radius, float tubeRadius, float length, int numRotations, bool repeatTexture, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshSpiral::BuildMesh(radius, tubeRadius, length, numRotations, repeatTexture, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildCylinder(float baseRadius, float topRadius, float length, const Vector3f& position, bool top, bool bottom, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshCylinder::BuildMesh(baseRadius, topRadius, length, position, top, bottom, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildQuadXY(const Vector3f& position, const Vector2f&  size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshQuad::BuildMeshXY(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildQuadXZ(const Vector3f& position, const Vector2f& size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshQuad::BuildMeshXZ(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildCube(const Vector3f& position, const Vector3f&  size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshCube::BuildMesh4Q(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildDiskXY(float radius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshDisk::BuildMeshXY(radius, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildDiskXZ(float radius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshDisk::BuildMeshXZ(radius, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::fromBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride) {

	if (stride == 3) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
		}
	}

	if (stride == 5) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
			m_texels.push_back(Vector2f(vertexBuffer[i + 3], vertexBuffer[i + 4]));
		}
	}

	if (stride == 6) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
			m_normals.push_back(Vector3f(vertexBuffer[i + 3], vertexBuffer[i + 4], vertexBuffer[i + 5]));
		}
	}

	if (stride == 8) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
			m_texels.push_back(Vector2f(vertexBuffer[i + 3], vertexBuffer[i + 4]));
			m_normals.push_back(Vector3f(vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7]));
			
		}
	}

	if (stride == 14) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
			m_texels.push_back(Vector2f(vertexBuffer[i + 3], vertexBuffer[i + 4]));
			m_normals.push_back(Vector3f(vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7]));
			m_tangents.push_back(Vector3f(vertexBuffer[i + 8], vertexBuffer[i + 9], vertexBuffer[i + 10]));
			m_bitangents.push_back(Vector3f(vertexBuffer[i + 11], vertexBuffer[i + 12], vertexBuffer[i + 13]));

		}
	}

	m_indexBuffer.reserve(indexBuffer.size());
	std::copy(indexBuffer.begin(), indexBuffer.end(), std::back_inserter(m_indexBuffer));

	createBuffer();
}

void Shape::fromObj(const char* filename) {

	bool quads = false;
	bool check = false;

	std::vector<std::array<int, 9>> face;
	std::vector<float> positionCoords;
	std::vector<float> normalCoords;
	std::vector<float> textureCoords;
	std::vector<float> tangentCoords;
	std::vector<float> bitangentCoords;

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
				
					positionCoords.push_back(tmpx);
					positionCoords.push_back(tmpy);
					positionCoords.push_back(tmpz);
					break;

				}case 't': {

					float tmpu, tmpv;
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%f %f", &tmpu, &tmpv);

					textureCoords.push_back(tmpu);
					textureCoords.push_back(tmpv);
					break;

				}case 'n': {

					float tmpx, tmpy, tmpz;
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%f %f %f", &tmpx, &tmpy, &tmpz);
					normalCoords.push_back(tmpx);
					normalCoords.push_back(tmpy);
					normalCoords.push_back(tmpz);		
					break;

				}default: {
					break;
				}
			}
			break;

		}case 'f': {

			
			fgets(buffer, sizeof(buffer), pFile);

			if (!check) {
				quads = whitespaces(buffer) == 4;
				check == true;
			}
			
			if (quads) {
				int a, b, c, n1, n2, n3, t1, t2, t3;

				if (!textureCoords.empty() && !normalCoords.empty()) {
					sscanf(buffer, "%d/%d/%d %d/%d/%d %d/%d/%d", &a, &t1, &n1, &b, &t2, &n2, &c, &t3, &n3);
					face.push_back({ a, b, c, t1, t2, t3, n1, n2, n3 });

				}else if (!normalCoords.empty()) {
					sscanf(buffer, "%d//%d %d//%d %d//%d", &a, &n1, &b, &n2, &c, &n3);
					face.push_back({ a, b, c, 0, 0, 0, n1, n2, n3 });
				}else if (!textureCoords.empty()) {
					sscanf(buffer, "%d/%d %d/%d %d/%d", &a, &t1, &b, &t2, &c, &t3);
					face.push_back({ a, b, c, t1, t2, t3, 0, 0, 0 });
				}else {
					sscanf(buffer, "%d %d %d", &a, &b, &c);
					face.push_back({ a, b, c, 0, 0, 0, 0, 0, 0 });
				}

			} else {
				int a, b, c, d, n1, n2, n3, n4, t1, t2, t3, t4;

				if (!textureCoords.empty() && !normalCoords.empty()) {
					sscanf(buffer, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &a, &t1, &n1, &b, &t2, &n2, &c, &t3, &n3, &d, &t4, &n4);
					face.push_back({ a, b, c, t1, t2, t3, n1, n2, n3 });
					face.push_back({ a, c, d, t1, t3, t4, n1, n3, n4 });

				} else if (!normalCoords.empty()) {
					sscanf(buffer, "%d//%d %d//%d %d//%d %d//%d", &a, &n1, &b, &n2, &c, &n3, &d, &n4);
					face.push_back({ a, b, c, 0, 0, 0, n1, n2, n3 });
					face.push_back({ a, c, d, 0, 0, 0, n1, n3, n4 });

				} else if (!textureCoords.empty()) {
					sscanf(buffer, "%d/%d %d/%d %d/%d %d/%d", &a, &t1, &b, &t2, &c, &t3, &d, &t4);
					face.push_back({ a, b, c, t1, t2, t3, 0, 0, 0 });
					face.push_back({ a, c, d, t1, t3, t4, 0, 0, 0 });
				} else {
					sscanf(buffer, "%d %d %dd %d", &a, &b, &c, &d);
					face.push_back({ a, b, c, 0, 0, 0, 0, 0, 0 });
					face.push_back({ a, c, d, 0, 0, 0, 0, 0, 0 });
				}
			}
			break;

		}default: {

			break;
		}

		}//end switch
	}// end while
	fclose(pFile);

	std::vector <float> vertexBufferOut;
	std::map<int, std::vector<int>> vertexCache;

	m_indexBuffer.resize(face.size() * 3);
	if (!tangentCoords.empty()) {
		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoords[((face[i])[0] - 1) * 3], positionCoords[((face[i])[0] - 1) * 3 + 1], positionCoords[((face[i])[0] - 1) * 3 + 2],
								textureCoords[((face[i])[3] - 1) * 2], textureCoords[((face[i])[3] - 1) * 2 + 1],
								normalCoords[((face[i])[6] - 1) * 3], normalCoords[((face[i])[6] - 1) * 3 + 1], normalCoords[((face[i])[6] - 1) * 3 + 2],
								tangentCoords[((face[i])[0] - 1) * 3], tangentCoords[((face[i])[0] - 1) * 3 + 1], tangentCoords[((face[i])[0] - 1) * 3 + 2],
								bitangentCoords[((face[i])[0] - 1) * 3], bitangentCoords[((face[i])[0] - 1) * 3 + 1], bitangentCoords[((face[i])[0] - 1) * 3 + 2] };

			m_indexBuffer[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 14, vertexCache, vertexBufferOut);

			float vertex2[] = { positionCoords[((face[i])[1] - 1) * 3], positionCoords[((face[i])[1] - 1) * 3 + 1], positionCoords[((face[i])[1] - 1) * 3 + 2],
								textureCoords[((face[i])[4] - 1) * 2], textureCoords[((face[i])[4] - 1) * 2 + 1],
								normalCoords[((face[i])[7] - 1) * 3], normalCoords[((face[i])[7] - 1) * 3 + 1], normalCoords[((face[i])[7] - 1) * 3 + 2],
								tangentCoords[((face[i])[1] - 1) * 3], tangentCoords[((face[i])[1] - 1) * 3 + 1], tangentCoords[((face[i])[1] - 1) * 3 + 2],
								bitangentCoords[((face[i])[1] - 1) * 3], bitangentCoords[((face[i])[1] - 1) * 3 + 1], bitangentCoords[((face[i])[1] - 1) * 3 + 2] };

			m_indexBuffer[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 14, vertexCache, vertexBufferOut);

			float vertex3[] = { positionCoords[((face[i])[2] - 1) * 3], positionCoords[((face[i])[2] - 1) * 3 + 1], positionCoords[((face[i])[2] - 1) * 3 + 2],
								textureCoords[((face[i])[5] - 1) * 2], textureCoords[((face[i])[5] - 1) * 2 + 1],
								normalCoords[((face[i])[8] - 1) * 3], normalCoords[((face[i])[8] - 1) * 3 + 1], normalCoords[((face[i])[8] - 1) * 3 + 2],
								tangentCoords[((face[i])[2] - 1) * 3], tangentCoords[((face[i])[2] - 1) * 3 + 1], tangentCoords[((face[i])[2] - 1) * 3 + 2] ,
								bitangentCoords[((face[i])[2] - 1) * 3], bitangentCoords[((face[i])[2] - 1) * 3 + 1], bitangentCoords[((face[i])[2] - 1) * 3 + 2] };

			m_indexBuffer[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 14, vertexCache, vertexBufferOut);
		}

	}else if (!textureCoords.empty() && !normalCoords.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoords[((face[i])[0] - 1) * 3], positionCoords[((face[i])[0] - 1) * 3 + 1], positionCoords[((face[i])[0] - 1) * 3 + 2],
								textureCoords[((face[i])[3] - 1) * 2], textureCoords[((face[i])[3] - 1) * 2 + 1],
								normalCoords[((face[i])[6] - 1) * 3], normalCoords[((face[i])[6] - 1) * 3 + 1], normalCoords[((face[i])[6] - 1) * 3 + 2] };
			
			m_indexBuffer[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 8, vertexCache, vertexBufferOut);

			float vertex2[] = { positionCoords[((face[i])[1] - 1) * 3], positionCoords[((face[i])[1] - 1) * 3 + 1], positionCoords[((face[i])[1] - 1) * 3 + 2],
								textureCoords[((face[i])[4] - 1) * 2], textureCoords[((face[i])[4] - 1) * 2 + 1],
								normalCoords[((face[i])[7] - 1) * 3], normalCoords[((face[i])[7] - 1) * 3 + 1], normalCoords[((face[i])[7] - 1) * 3 + 2] };
			
			m_indexBuffer[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 8, vertexCache, vertexBufferOut);

			float vertex3[] = { positionCoords[((face[i])[2] - 1) * 3], positionCoords[((face[i])[2] - 1) * 3 + 1], positionCoords[((face[i])[2] - 1) * 3 + 2],
								textureCoords[((face[i])[5] - 1) * 2], textureCoords[((face[i])[5] - 1) * 2 + 1],
								normalCoords[((face[i])[8] - 1) * 3], normalCoords[((face[i])[8] - 1) * 3 + 1], normalCoords[((face[i])[8] - 1) * 3 + 2] };
			
			m_indexBuffer[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 8, vertexCache, vertexBufferOut);
		}

	}else if (!normalCoords.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoords[((face[i])[0] - 1) * 3], positionCoords[((face[i])[0] - 1) * 3 + 1], positionCoords[((face[i])[0] - 1) * 3 + 2],
								normalCoords[((face[i])[6] - 1) * 3], normalCoords[((face[i])[6] - 1) * 3 + 1], normalCoords[((face[i])[6] - 1) * 3 + 2] };
			
			m_indexBuffer[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 6, vertexCache, vertexBufferOut);

			float vertex2[] = { positionCoords[((face[i])[1] - 1) * 3], positionCoords[((face[i])[1] - 1) * 3 + 1], positionCoords[((face[i])[1] - 1) * 3 + 2],
								normalCoords[((face[i])[7] - 1) * 3], normalCoords[((face[i])[7] - 1) * 3 + 1], normalCoords[((face[i])[7] - 1) * 3 + 2] };

			m_indexBuffer[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 6, vertexCache, vertexBufferOut);

			float vertex3[] = { positionCoords[((face[i])[2] - 1) * 3], positionCoords[((face[i])[2] - 1) * 3 + 1], positionCoords[((face[i])[2] - 1) * 3 + 2],
								normalCoords[((face[i])[8] - 1) * 3], normalCoords[((face[i])[8] - 1) * 3 + 1], normalCoords[((face[i])[8] - 1) * 3 + 2] };

			m_indexBuffer[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 6, vertexCache, vertexBufferOut);
		}

	}else if (!textureCoords.empty()) {

		for (int i = 0; i < face.size(); i++) {
			float vertex1[] = { positionCoords[((face[i])[0] - 1) * 3], positionCoords[((face[i])[0] - 1) * 3 + 1], positionCoords[((face[i])[0] - 1) * 3 + 2],
								textureCoords[((face[i])[3] - 1) * 2], textureCoords[((face[i])[3] - 1) * 2 + 1] };

			m_indexBuffer[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 5, vertexCache, vertexBufferOut);

			float vertex2[] = { positionCoords[((face[i])[1] - 1) * 3],positionCoords[((face[i])[1] - 1) * 3 + 1], positionCoords[((face[i])[1] - 1) * 3 + 2],
								textureCoords[((face[i])[4] - 1) * 2], textureCoords[((face[i])[4] - 1) * 2 + 1] };

			m_indexBuffer[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 5, vertexCache, vertexBufferOut);

			float vertex3[] = { positionCoords[((face[i])[2] - 1) * 3], positionCoords[((face[i])[2] - 1) * 3 + 1], positionCoords[((face[i])[2] - 1) * 3 + 2],
								textureCoords[((face[i])[5] - 1) * 2], textureCoords[((face[i])[5] - 1) * 2 + 1] };

			m_indexBuffer[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 5, vertexCache, vertexBufferOut);
		}

	}else {
		for (int i = 0; i < face.size(); i++) {
			float vertex1[] = { positionCoords[((face[i])[0] - 1) * 3], positionCoords[((face[i])[0] - 1) * 3 + 1], positionCoords[((face[i])[0] - 1) * 3 + 2] };

			m_indexBuffer[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 3, vertexCache, vertexBufferOut);

			float vertex2[] = { positionCoords[((face[i])[1] - 1) * 3], positionCoords[((face[i])[1] - 1) * 3 + 1], positionCoords[((face[i])[1] - 1) * 3 + 2] };

			m_indexBuffer[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 3, vertexCache, vertexBufferOut);

			float vertex3[] = { positionCoords[((face[i])[2] - 1) * 3], positionCoords[((face[i])[2] - 1) * 3 + 1], positionCoords[((face[i])[2] - 1) * 3 + 2] };

			m_indexBuffer[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 3, vertexCache, vertexBufferOut);
		}
	}

	face.clear();
	face.shrink_to_fit();

	std::map<int, std::vector<int>>().swap(vertexCache);

	if (!tangentCoords.empty()) {
		for (unsigned int i = 0; i < vertexBufferOut.size(); i = i + 14) {
			m_positions.push_back(Vector3f(vertexBufferOut[i], vertexBufferOut[i + 1], vertexBufferOut[i + 2]));
			m_texels.push_back(Vector2f(vertexBufferOut[i + 3], vertexBufferOut[i + 4]));
			m_normals.push_back(Vector3f(vertexBufferOut[i + 5], vertexBufferOut[i + 6], vertexBufferOut[i + 7]));
			m_tangents.push_back(Vector3f(vertexBufferOut[i + 8], vertexBufferOut[i + 9], vertexBufferOut[i + 10]));
			m_bitangents.push_back(Vector3f(vertexBufferOut[i + 11], vertexBufferOut[i + 12], vertexBufferOut[i + 13]));

		}
	}else if (!textureCoords.empty() && !normalCoords.empty()) {
		for (unsigned int i = 0; i < vertexBufferOut.size(); i = i + 8) {
			m_positions.push_back(Vector3f(vertexBufferOut[i], vertexBufferOut[i + 1], vertexBufferOut[i + 2]));
			m_texels.push_back(Vector2f(vertexBufferOut[i + 3], vertexBufferOut[i + 4]));
			m_normals.push_back(Vector3f(vertexBufferOut[i + 5], vertexBufferOut[i + 6], vertexBufferOut[i + 7]));

		}
	}else if (!normalCoords.empty()) {
		for (unsigned int i = 0; i < vertexBufferOut.size(); i = i + 8) {
			m_positions.push_back(Vector3f(vertexBufferOut[i], vertexBufferOut[i + 1], vertexBufferOut[i + 2]));
			m_normals.push_back(Vector3f(vertexBufferOut[i + 3], vertexBufferOut[i + 4], vertexBufferOut[i + 5]));
		}

	}else if (!textureCoords.empty()) {
		for (unsigned int i = 0; i < vertexBufferOut.size(); i = i + 5) {
			m_positions.push_back(Vector3f(vertexBufferOut[i], vertexBufferOut[i + 1], vertexBufferOut[i + 2]));
			m_texels.push_back(Vector2f(vertexBufferOut[i + 3], vertexBufferOut[i + 4]));
		}
	}else {
		for (unsigned int i = 0; i < vertexBufferOut.size(); i = i + 3) {
			m_positions.push_back(Vector3f(vertexBufferOut[i], vertexBufferOut[i + 1], vertexBufferOut[i + 2]));
		}
	}

	positionCoords.clear();
	positionCoords.shrink_to_fit();
	normalCoords.clear();
	normalCoords.shrink_to_fit();
	textureCoords.clear();
	textureCoords.shrink_to_fit();
	tangentCoords.clear();
	tangentCoords.shrink_to_fit();
	bitangentCoords.clear();
	bitangentCoords.shrink_to_fit();
	vertexBufferOut.clear();
	vertexBufferOut.shrink_to_fit();

	createBuffer();
}

int Shape::whitespaces(const char c[]) {
	int count = 0;
	size_t n = std::strlen(c);

	for (size_t i = 0; i < n; i++) {
		if (std::isspace(c[i])) ++count;
	}

	return (count);
}

int Shape::addVertex(int hash, const float *pVertex, int stride, std::map<int, std::vector<int>>& vertexCache, std::vector <float>& vertexBuffer) {

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = vertexCache.find(hash);

	if (iter == vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(vertexBuffer.size() / stride);
		vertexBuffer.insert(vertexBuffer.end(), pVertex, pVertex + stride);
		vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));
	}else {
		// One or more vertices have been hashed to this entry in the cache.
		const std::vector<int> &vertices = iter->second;
		const float *pCachedVertex = 0;
		bool found = false;

		for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
			index = *i;
			pCachedVertex = &vertexBuffer[index * stride];

			if (memcmp(pCachedVertex, pVertex, sizeof(float)* stride) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			index = static_cast<int>(vertexBuffer.size() / stride);
			vertexBuffer.insert(vertexBuffer.end(), pVertex, pVertex + stride);
			vertexCache[hash].push_back(index);
		}
	}
	return index;
}

const std::vector<Matrix4f>& Shape::getInstances() const {
	return m_instances;
}

std::vector<Vector3f>& Shape::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& Shape::getIndexBuffer() {
	return m_indexBuffer;
}

int Shape::getNumberOfTriangles() {
	return m_drawCount / 3;
}

Shape::~Shape() {
	cleanup();
}

void Shape::cleanup() {

	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo[0])
		glDeleteBuffers(1, &m_vbo[0]);

	if (m_vbo[1])
		glDeleteBuffers(1, &m_vbo[1]);

	if (m_vbo[2])
		glDeleteBuffers(1, &m_vbo[2]);

	if (m_vbo[3])
		glDeleteBuffers(1, &m_vbo[3]);

	if (m_vbo[4])
		glDeleteBuffers(1, &m_vbo[4]);

	if (m_vboInstances)
		glDeleteBuffers(1, &m_vboInstances);

	if (m_vboAdd1)
		glDeleteBuffers(1, &m_vboAdd1);

	if (m_vboAdd2)
		glDeleteBuffers(1, &m_vboAdd2);

	if (m_vboAdd3)
		glDeleteBuffers(1, &m_vboAdd3);

	m_positions.clear();
	m_positions.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	m_instances.clear();
	m_instances.shrink_to_fit();
}

void Shape::createBuffer() {
	m_drawCount = m_indexBuffer.size();

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(!m_positions.empty() + !m_texels.empty() + !m_normals.empty() + !m_tangents.empty() + !m_bitangents.empty(), m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	if (!m_texels.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Normals
	if (!m_normals.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, !m_texels.empty() ? m_vbo[2] : m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//tangents
	if (!m_tangents.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, (!m_texels.empty() && !m_normals.empty()) ? m_vbo[3] : (!m_texels.empty() || !m_normals.empty()) ? m_vbo[2] : m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_tangents.size() * sizeof(m_tangents[0]), &m_tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, (!m_texels.empty() && !m_normals.empty()) ? m_vbo[4] : (!m_texels.empty() || !m_normals.empty()) ? m_vbo[3] : m_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, m_bitangents.size() * sizeof(m_bitangents[0]), &m_bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	//m_positions.clear();
	//m_positions.shrink_to_fit();
	//m_indexBuffer.clear();
	//m_indexBuffer.shrink_to_fit();
	m_texels.clear();
	m_texels.shrink_to_fit();
	m_normals.clear();
	m_normals.shrink_to_fit();
	m_tangents.clear();
	m_tangents.shrink_to_fit();
	m_bitangents.clear();
	m_bitangents.shrink_to_fit();
}

void Shape::drawRaw() const {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Shape::addInstance(const Matrix4f& value, unsigned int divisor) {
	m_instances.push_back(value);
	m_instanceCount = m_instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}else{
		glGenBuffers(1, &m_vboInstances);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));
		
		glVertexAttribDivisor(5, divisor);
		glVertexAttribDivisor(6, divisor);
		glVertexAttribDivisor(7, divisor);
		glVertexAttribDivisor(8, divisor);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Shape::removeInstance(unsigned int index) {
	m_instances.erase(m_instances.begin() + index);
	m_instanceCount = m_instances.size();

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Shape::addInstances(const std::vector<Matrix4f>& values, unsigned int divisor) {
	m_instances.insert(m_instances.end(), values.begin(), values.end());
	m_instanceCount = m_instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}else{
		glGenBuffers(1, &m_vboInstances);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, divisor);
		glVertexAttribDivisor(6, divisor);
		glVertexAttribDivisor(7, divisor);
		glVertexAttribDivisor(8, divisor);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Shape::addInstances(unsigned int length, unsigned int divisor, unsigned int usage) {
	m_instanceCount = length;
	
	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(float) * 4 * 4, NULL, usage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}else {
		glGenBuffers(1, &m_vboInstances);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(float) * 4 * 4, NULL, usage);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, divisor);
		glVertexAttribDivisor(6, divisor);
		glVertexAttribDivisor(7, divisor);
		glVertexAttribDivisor(8, divisor);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Shape::updateInstances(const std::vector<Matrix4f>& values) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferSubData(GL_ARRAY_BUFFER, 0, values.size() * sizeof(float) * 4 * 4, values[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Shape::addVec4Attribute(const std::vector<Vector4f>& values, unsigned int divisor) {
	if (m_vboAdd1) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd1);
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float) * 4, &values[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}else{

		glGenBuffers(1, &m_vboAdd1);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd1);
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float) * 4, &values[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(0));
		glVertexAttribDivisor(9, divisor);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Shape::addMat4Attribute(const std::vector<Matrix4f>& values, unsigned int divisor) {
	if (m_vboAdd2) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd2);
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float) * 4 * 4, values[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}else {

		glGenBuffers(1, &m_vboAdd2);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd2);
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float) * 4 * 4, values[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(10);
		glEnableVertexAttribArray(11);
		glEnableVertexAttribArray(12);
		glEnableVertexAttribArray(13);
		glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(10, divisor);
		glVertexAttribDivisor(11, divisor);
		glVertexAttribDivisor(12, divisor);
		glVertexAttribDivisor(13, divisor);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Shape::addFloatAttribute(const std::vector<float>& values, unsigned int divisor) {
	if (m_vboAdd3) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd3);
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float), &values[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}else{

		glGenBuffers(1, &m_vboAdd3);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd3);
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float), &values[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(14);
		glVertexAttribPointer(14, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(0));
		glVertexAttribDivisor(14, divisor);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Shape::addMat4Attribute(unsigned int length, unsigned int divisor, unsigned int usage) {
	glGenBuffers(1, &m_vboAdd2);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd2);
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(float) * 4 * 4, NULL, usage);

	glEnableVertexAttribArray(10);
	glEnableVertexAttribArray(11);
	glEnableVertexAttribArray(12);
	glEnableVertexAttribArray(13);
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(10, divisor);
	glVertexAttribDivisor(11, divisor);
	glVertexAttribDivisor(12, divisor);
	glVertexAttribDivisor(13, divisor);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Shape::updateMat4Attribute(const std::vector<Matrix4f>& values) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd2);
	glBufferSubData(GL_ARRAY_BUFFER, 0, values.size() * sizeof(float) * 4 * 4, values[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Shape::addFloatAttribute(unsigned int length, unsigned int divisor, unsigned int usage) {
	glGenBuffers(1, &m_vboAdd3);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd3);
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(float), NULL, usage);

	glEnableVertexAttribArray(14);
	glVertexAttribPointer(14, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(0));
	glVertexAttribDivisor(14, divisor);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Shape::updateFloatAttribute(const std::vector<float>& values) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd3);
	glBufferSubData(GL_ARRAY_BUFFER, 0, values.size() * sizeof(float), &values[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Shape::drawRawInstanced() const {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}

void Shape::updateInstance(const Matrix4f& mtx, unsigned int index) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	Matrix4f* ptr = (Matrix4f*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		ptr[index] = mtx;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}