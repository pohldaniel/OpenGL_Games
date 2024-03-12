#include <iostream>
#include "MeshQuad.h"

MeshQuad::MeshQuad(int uResolution, int vResolution) : MeshQuad(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), true, true, false, uResolution, vResolution) { }

MeshQuad::MeshQuad(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) : MeshQuad(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), generateTexels, generateNormals, generateTangents, uResolution, vResolution) { }

MeshQuad::MeshQuad(const Vector3f &position, const Vector2f& size, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) {

	m_position = position;
	m_size = size;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents;

	m_center = m_position + Vector3f(m_size[0], m_size[1], 0.0f) * 0.5f;
	BuildMeshXY(m_position, m_size, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void MeshQuad::create(const Vector3f &position, const Vector2f& size, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) {
	m_position = position;
	m_size = size;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents;

	m_center = m_position + Vector3f(m_size[0], m_size[1], 0.0f) * 0.5f;
	BuildMeshXY(m_position, m_size, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}


MeshQuad::~MeshQuad() {
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

void MeshQuad::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

int MeshQuad::getNumberOfTriangles() {
	return m_drawCount / 3;
}

const Vector3f& MeshQuad::getPosition() const {
	return m_position;
}

const Vector2f& MeshQuad::getSize() const {
	return m_size;
}

const Vector3f& MeshQuad::getCenter() const {
	return m_center;
}

std::vector<Vector3f>& MeshQuad::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& MeshQuad::getIndexBuffer() {
	return m_indexBuffer;
}

void MeshQuad::BuildMeshXY(const Vector3f& _position, const Vector2f& size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents){
	float vStep = (1.0f / vResolution) * size[1];
	float uStep = (1.0f /uResolution) * size[0];

	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {
			
			// Calculate vertex position on the surface of a quad
			float x = j * uStep;		
			float y = i * vStep;
			float z = 0.0f;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				// Calculate texels on the surface of a quad
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;
				texels.push_back(Vector2f(u, v));
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

	//calculate the indices
	for (int z = 0; z < vResolution ; z++) {
		for (int x = 0; x < uResolution; x++) {
			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
			//      4		3 -* 4
			indexBuffer.push_back(z * (uResolution + 1) + x);				
			indexBuffer.push_back(z * (uResolution + 1) + x + 1);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);

			indexBuffer.push_back(z * (uResolution + 1) + x);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x);			
			
		}
	}
}

void MeshQuad::BuildMeshXZ(const Vector3f& _position, const Vector2f& size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {
	float vStep = (1.0f / vResolution) * size[1];
	float uStep = (1.0f / uResolution) * size[0];

	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = 0.0f;
			float z = i * vStep;

			Vector3f position = Vector3f(x, y, z) + _position;
			positions.push_back(position);

			if (generateTexels) {
				// Calculate texels on the surface of a quad
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;
				texels.push_back(Vector2f(u, v));
			}

			if (generateNormals) {
				normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
				bitangents.push_back(Vector3f(0.0f, 0.0f, 1.0f));
			}
		}
	}

	//calculate the indices
	for (int z = 0; z < vResolution; z++) {
		for (int x = 0; x < uResolution; x++) {
			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
			//      4		3 -* 4
			indexBuffer.push_back(z * (uResolution + 1) + x);			
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);
			indexBuffer.push_back(z * (uResolution + 1) + x + 1);

			indexBuffer.push_back((z + 1) * (uResolution + 1) + x);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);
			indexBuffer.push_back(z * (uResolution + 1) + x);			
		}
	}
}

void MeshQuad::BuildDiamondXY(const Vector2f& size, float border, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {
	Matrix4f rot;
	rot.rotate(Vector3f(0.0f, 0.0f, 1.0f), 45.0f);

	Matrix4f uniformScale;
	uniformScale.scale( M_SQRT1_2, M_SQRT1_2, M_SQRT1_2);

	Matrix4f sizeScale;
	sizeScale.scale(size[0] + border, size[1] + border, 1.0f);

	Matrix4f transform = uniformScale * rot;
		
	float vStep = (1.0f / vResolution);
	float uStep = (1.0f / uResolution);

	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = i * vStep;
			float z = 0.0f;

			positions.push_back(sizeScale * (transform * (Vector3f(x, y, z) + Vector3f(-0.5f, -0.5f, 0.0f))));

			if (generateTexels) {
				// Calculate texels on the surface of a quad
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;
				texels.push_back(Vector2f(u, v));
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

	//calculate the indices
	for (int z = 0; z < vResolution; z++) {
		for (int x = 0; x < uResolution; x++) {
			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
						//      4		3 -* 4
			indexBuffer.push_back(z * (uResolution + 1) + x);
			indexBuffer.push_back(z * (uResolution + 1) + x + 1);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);

			indexBuffer.push_back(z * (uResolution + 1) + x);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x);

		}
	}
}


void MeshQuad::createBuffer() {
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

void MeshQuad::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshQuad::createInstancesStatic(const std::vector<Matrix4f>& modelMTX) {
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

void MeshQuad::addInstance(const Matrix4f& modelMTX) {
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
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

void MeshQuad::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}