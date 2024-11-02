#include "MeshCube.h"
#include <iostream>

MeshCube::MeshCube(int uResolution, int vResolution) : MeshCube(Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(2.0f, 2.0f, 2.0f), true, true, false, uResolution, vResolution) { }

MeshCube::MeshCube(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) : MeshCube(Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(2.0f, 2.0f, 2.0f), generateTexels, generateNormals, generateTangents, uResolution, vResolution) { }

MeshCube::MeshCube(const Vector3f &position, const Vector3f& size, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) {

	m_position = position;
	m_size = size;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents;

	m_center = m_position + m_size * 0.5f;

	BuildMesh4Q(m_position, m_size, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

MeshCube::~MeshCube() {
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
}

void MeshCube::setPrecision(int uResolution, int vResolution) {

	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

const Vector3f& MeshCube::getPosition() const {
	return m_position;
}

const Vector3f& MeshCube::getSize() const {
	return m_size;
}

const Vector3f& MeshCube::getCenter() const {
	return m_center;
}

std::vector<Vector3f>& MeshCube::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& MeshCube::getIndexBuffer() {
	return m_indexBuffer;
}

void MeshCube::BuildMesh(const Vector3f& _position, const Vector3f& size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {

	float vStep = (1.0f / vResolution) * size[1];
	float uStep = (1.0f / uResolution) * size[0];

	//front
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j < uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = i * vStep;
			float z = size[2];

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;
				Vector2f textureCoordinate = Vector2f(u, 1 - v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, 0.0f, 1.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[2];

	//right
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = uResolution; j > 0; j--) {

			// Calculate vertex position on the surface of a quad
			float x = size[0];
			float y = i * vStep;
			float z = j * uStep;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;
				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(1.0f, 0.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
				bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[0];

	//back
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = uResolution; j > 0; j--) {
			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = i * vStep;
			float z = 0.0f;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, 0.0f, -1.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(-1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[2];

	//left
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j < uResolution; j++) {
			// Calculate vertex position on the surface of a quad
			float x = 0.0f;
			float y = i * vStep;
			float z = j * uStep;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(-1.0f, 0.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(0.0f, 0.0f, 1.0f));
				bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[2];
	uStep = (1.0f / uResolution) * size[0];

	//bottom
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {
			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = 0.0f;
			float z = i * vStep;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, -1.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 0.0f, 1.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[2];
	uStep = (1.0f / uResolution) * size[0];

	//top
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = size[1];
			float z = i * vStep;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
			}
		}
	}

	int face = 0;
	for (unsigned int v = 0; v < vResolution; v++) {

		for (unsigned int u = 0; u < 4 * uResolution * (vResolution + 1); u++) {

			//Connect the penultimate vertex to the first
			if (u == ((uResolution - 1) + face * (uResolution * (1 + vResolution)))) {
				face++;

				indexBuffer.push_back(u + (1 + v) * uResolution);
				indexBuffer.push_back(u + v * (uResolution));
				indexBuffer.push_back((uResolution * (v + 1 + vResolution) + u + 1) % (4 * uResolution * (vResolution + 1)));

				indexBuffer.push_back(u + v * (uResolution));
				indexBuffer.push_back((uResolution * (v + vResolution) + u + 1) % (4 * uResolution * (vResolution + 1)));
				indexBuffer.push_back((uResolution * (v + 1 + vResolution) + u + 1) % (4 * uResolution * (vResolution + 1)));

			}
			else if (v == 0 && u < uResolution * (vResolution + 1) - uResolution) {

				if ((u + 1) % (uResolution) == 0) continue;

				indexBuffer.push_back(u);
				indexBuffer.push_back(u + 1);
				indexBuffer.push_back(u + uResolution + 1);

				indexBuffer.push_back(u + uResolution + 1);
				indexBuffer.push_back(u + uResolution);
				indexBuffer.push_back(u);

			}
			else if (v == 0 && uResolution * (vResolution + 1) <= u && u < 2 * uResolution * (vResolution + 1) - uResolution) {

				if ((u + 1) % (uResolution) == 0) continue;

				indexBuffer.push_back(u);
				indexBuffer.push_back(u + 1);
				indexBuffer.push_back(u + uResolution + 1);

				indexBuffer.push_back(u + uResolution + 1);
				indexBuffer.push_back(u + uResolution);
				indexBuffer.push_back(u);

			}
			else if (v == 0 && uResolution * (vResolution + 1) * 2 <= u && u < 3 * uResolution * (vResolution + 1) - uResolution) {

				if ((u + 1) % (uResolution) == 0) continue;

				indexBuffer.push_back(u);
				indexBuffer.push_back(u + 1);
				indexBuffer.push_back(u + uResolution + 1);

				indexBuffer.push_back(u + uResolution + 1);
				indexBuffer.push_back(u + uResolution);
				indexBuffer.push_back(u);

			}
			else if (v == 0 && uResolution * (vResolution + 1) * 3 <= u && u < 4 * uResolution * (vResolution + 1) - uResolution) {

				if ((u + 1) % (uResolution) == 0) continue;

				indexBuffer.push_back(u);
				indexBuffer.push_back(u + 1);
				indexBuffer.push_back(u + uResolution + 1);

				indexBuffer.push_back(u + uResolution + 1);
				indexBuffer.push_back(u + uResolution);
				indexBuffer.push_back(u);
			}
		}
		face = 0;
	}

	for (unsigned short face = 0; face < 2; face++) {

		int k = (4 + face) * uResolution * (vResolution + 1) + face * (uResolution + 1);
		for (int i = k; i < k + vResolution; i++) {
			for (int j = k; j < k + uResolution; j++) {
				if (face % 2 == 0) {
					indexBuffer.push_back((i - k) * (uResolution + 1) + j);
					indexBuffer.push_back((i - k) * (uResolution + 1) + j + 1);
					indexBuffer.push_back((i - k + 1) * (uResolution + 1) + j);

					indexBuffer.push_back((i - k + 1) * (uResolution + 1) + j);
					indexBuffer.push_back((i - k) * (uResolution + 1) + j + 1);
					indexBuffer.push_back((i - k + 1) * (uResolution + 1) + j + 1);
				}
				else {
					indexBuffer.push_back((i - k) * (uResolution + 1) + j);
					indexBuffer.push_back((i - k + 1) * (uResolution + 1) + j);
					indexBuffer.push_back((i - k) * (uResolution + 1) + j + 1);

					indexBuffer.push_back((i - k + 1) * (uResolution + 1) + j);
					indexBuffer.push_back((i - k + 1) * (uResolution + 1) + j + 1);
					indexBuffer.push_back((i - k) * (uResolution + 1) + j + 1);
				}
			}
		}
	}
}

void MeshCube::BuildMesh4Q(const Vector3f& _position, const Vector3f& size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {

	float vStep = (1.0f / vResolution) * size[1];
	float uStep = (1.0f / uResolution) * size[2];

	// positive X
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = size[0];
			float y = i * vStep;
			float z = j * uStep;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(1.0f, 0.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
				bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[2];

	// negative X
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = 0.0f;
			float y = i * vStep;
			float z = j * uStep;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(-1.0f, 0.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(0.0f, 0.0f, 1.0f));
				bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[2];
	uStep = (1.0f / uResolution) * size[0];

	// positive Y
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = j * uStep;
			float y = size[1];
			float z = i * vStep;
			
			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(u, 1.0f - v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[2];
	uStep = (1.0f / uResolution) * size[0];

	// negative Y
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = j * uStep;
			float y = 0.0f;
			float z = i * vStep;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, -1.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 0.0f, 1.0f));
			}
		}
	}
	
	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[0];

	// positive Z
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = j * uStep;
			float y = i * vStep;
			float z = size[2];

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, 0.0f, 1.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[0];

	// negative Z
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = j * uStep;
			float y = i * vStep;
			float z = 0.0f;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				Vector2f textureCoordinate = Vector2f(1 - u, v);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, 0.0f, -1.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(-1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}
		}
	}

	//calculate the indices
	for (short face = 0; face < 6; face++) {
		int k = (vResolution + 1) * (uResolution + 1) * face;
		for (int v = 0; v < vResolution; v++) {
			for (int u = 0; u < uResolution; u++) {

				if (face == 1 || face == 3 || face == 4) {
					indexBuffer.push_back(k + (v * (uResolution + 1) + u));
					indexBuffer.push_back(k + (v * (uResolution + 1) + u + 1));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u + 1));

					indexBuffer.push_back(k + (v * (uResolution + 1) + u));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u + 1));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u));
				}else {
					indexBuffer.push_back(k + (v * (uResolution + 1) + u + 1));
					indexBuffer.push_back(k + (v * (uResolution + 1) + u));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u + 1));

					indexBuffer.push_back(k + (v * (uResolution + 1) + u));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u + 1));
				}
			}
		}
	}
}

int MeshCube::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void MeshCube::createBuffer() {
	m_drawCount = m_indexBuffer.size();

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(m_numBuffers, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	if (m_generateTexels) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Normals
	if (m_generateNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, m_generateTexels ? m_vbo[2] : m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//tangents
	if (m_generateTangents) {
		glBindBuffer(GL_ARRAY_BUFFER, (m_generateTexels && m_generateNormals) ? m_vbo[3] : (m_generateTexels || m_generateNormals) ? m_vbo[2] : m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_tangents.size() * sizeof(m_tangents[0]), &m_tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, (m_generateTexels && m_generateNormals) ? m_vbo[4] : (m_generateTexels || m_generateNormals) ? m_vbo[3] : m_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, m_bitangents.size() * sizeof(m_bitangents[0]), &m_bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

void MeshCube::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshCube::createInstancesStatic(const std::vector<Matrix4f>& modelMTX) {
	m_instances.clear();
	m_instances.shrink_to_fit();
	m_instances = modelMTX;

	m_instanceCount = m_instances.size();

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

	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}

void MeshCube::addInstance(const Matrix4f& modelMTX) {
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}else {
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

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
	}
}

void MeshCube::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}