#include "MeshDisk.h"

MeshDisk::MeshDisk(int uResolution, int vResolution) : MeshDisk(1.0f, Vector3f(0.0f, 0.0f, 0.0f), true, true, false, uResolution, vResolution) { }

MeshDisk::MeshDisk(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) : MeshDisk(1.0f, Vector3f(0.0f, 0.0f, 0.0f), generateTexels, generateNormals, generateTangents, uResolution, vResolution) { }

MeshDisk::MeshDisk(float radius, const Vector3f &position, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) {

	m_position = position;
	m_radius = radius;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents;

	BuildMeshXZ(m_radius, m_position,  m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

MeshDisk::~MeshDisk() {
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

void MeshDisk::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

int MeshDisk::getNumberOfTriangles() {
	return m_drawCount / 3;
}

const Vector3f& MeshDisk::getPosition() const {
	return m_position;
}

std::vector<Vector3f>& MeshDisk::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& MeshDisk::getIndexBuffer() {
	return m_indexBuffer;
}

void MeshDisk::BuildMeshXY(float radius, const Vector3f& _position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {
	// put vertices of base of cylinder
	float x = 0.0f, z = 0.0f;
	unsigned int topVertexIndex = (unsigned int)positions.size();

	positions.push_back(Vector3f(0.0f, 0.0f, 0.0f) + _position);
	if (generateTexels)
		texels.push_back(Vector2f(0.5f, 0.5f));

	if (generateNormals) {
		normals.push_back(Vector3f(0.0f, 0.0f, 1.0f));
	}

	if (generateTangents) {
		tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
		bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
	}

	float sectorStep = 2 * PI / vResolution;
	float sectorAngle;  // radian

	for (int i = 0; i < vResolution; ++i) {
		sectorAngle = i * sectorStep;
		x = cos(sectorAngle);
		z = sin(sectorAngle);

		positions.push_back(Vector3f(x * radius, z * radius, 0.0f) + _position);
		if (generateTexels)
			texels.push_back(Vector2f(x * 0.5f + 0.5f, z * 0.5f + 0.5f));

		if (generateNormals) {
			normals.push_back(Vector3f(0.0f, 0.0f, 1.0f));
		}

		if (generateTangents) {
			tangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
			bitangents.push_back(Vector3f(0.0f, 1.0f, 0.0f));
		}
	}

	for (int i = 0, k = topVertexIndex + 1; i < vResolution; ++i, ++k) {
		if (i < (vResolution - 1)) {
			indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(k + 1); indexBuffer.push_back(k);
		}
		else {
			indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(topVertexIndex + 1); indexBuffer.push_back(k);
		}
	}
}

void MeshDisk::BuildMeshXZ(float radius, const Vector3f& _position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {
	// put vertices of base of cylinder
	float x = 0.0f, z = 0.0f;
	unsigned int topVertexIndex = (unsigned int)positions.size();

	positions.push_back(Vector3f(0.0f, 0.0f, 0.0f) + _position);
	if (generateTexels)
		texels.push_back(Vector2f(0.5f, 0.5f));

	if (generateNormals)
		normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));

	if (generateTangents) {
		tangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
		bitangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
	}

	float sectorStep = 2 * PI / vResolution;
	float sectorAngle;  // radian

	for (int i = 0; i < vResolution; ++i) {
		sectorAngle = i * sectorStep;
		x = cos(sectorAngle);
		z = sin(sectorAngle);

		positions.push_back(Vector3f(x * radius, 0.0f, z * radius) + _position);
		if (generateTexels)
			texels.push_back(Vector2f(0.5f + x * 0.5f, 0.5f - z * 0.5f));

		if (generateNormals)
			normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));

		if (generateTangents) {
			tangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
			bitangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
		}
	}

	for (int i = 0, k = topVertexIndex + 1; i < vResolution; ++i, ++k) {
		if (i < (vResolution - 1)) {
			indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(k + 1); indexBuffer.push_back(k);
		}else {
			indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(topVertexIndex + 1); indexBuffer.push_back(k);
		}
	}
}

void MeshDisk::createBuffer() {
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

void MeshDisk::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshDisk::createInstancesStatic(const std::vector<Matrix4f>& modelMTX) {
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

void MeshDisk::addInstance(const Matrix4f& modelMTX) {
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

void MeshDisk::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}