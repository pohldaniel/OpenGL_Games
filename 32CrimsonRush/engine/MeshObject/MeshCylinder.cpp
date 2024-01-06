#include "MeshCylinder.h"

MeshCylinder::MeshCylinder(int uResolution, int vResolution) : MeshCylinder(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 1.0f, true, true, true, true, false, uResolution, vResolution) { }

MeshCylinder::MeshCylinder(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) : MeshCylinder(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 1.0f, true, true, generateTexels, generateNormals, generateTangents, uResolution, vResolution) { }

MeshCylinder::MeshCylinder(const Vector3f &position, float baseRadius, float topRadius, float length, bool top, bool bottom, bool generateTexels, bool generateNormals, bool generateTangents,  int uResolution, int vResolution) {

	m_baseRadius = baseRadius;
	m_topRadius = topRadius;
	m_length = length;
	m_position = position;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents;

	BuildMesh(m_baseRadius, m_topRadius, m_length, m_position, top, bottom, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

MeshCylinder::~MeshCylinder() {
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

void MeshCylinder::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void MeshCylinder::BuildMesh(float baseRadius, float topRadius, float length, const Vector3f& position, bool top, bool bottom, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {
	float x, y, z;                                  // vertex position
	float radius;                                   // radius for each stack

	float zAngle = atan2(baseRadius - topRadius, length);
	float x0 = cos(zAngle);     // nx
	float y0 = sin(zAngle);     // ny
	// put vertices of side cylinder to array by scaling unit circle
	for (int i = 0; i <= uResolution; ++i) {
		y = -(length * 0.5f) + (float)i / uResolution * length;      // vertex position z
		radius = baseRadius + (float)i / uResolution * (topRadius - baseRadius);     // lerp
		float t = 1.0f - (float)i / uResolution;   // top-to-bottom

		float sectorStep = 2 * PI / vResolution;
		float sectorAngle;  // radian

		for (int j = 0, k = 0; j <= vResolution; ++j, k += 3) {
			sectorAngle = j * sectorStep;
			x = cos(sectorAngle);
			z = sin(sectorAngle);
			positions.push_back(Vector3f(x * radius, y, z * radius) + position);

			if (generateTexels)
				texels.push_back(Vector2f(1.0f - (float)j / vResolution, 1.0f - t));

			if (generateNormals) {
				normals.push_back(Vector3f(cos(sectorAngle)*x0, y0, sin(sectorAngle)*x0));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(-z, 0.0f, x));
				bitangents.push_back(Vector3f::Cross(tangents.back(), normals.back()));
				//bitangents.push_back(Vector3f(x, 1.0f, z) * 0.5f);
			}
		}
	}

	unsigned int baseVertexIndex = 0u;
	unsigned int topVertexIndex = 0u;

	if (bottom) {
		// remember where the base.top vertices start
		baseVertexIndex = (unsigned int)positions.size();

		// put vertices of base of cylinder
		y = -length * 0.5f;

		positions.push_back(Vector3f(0.0f, y, 0.0f) + position);
		if (generateTexels)
			texels.push_back(Vector2f(0.5f, 0.5f));

		if (generateNormals)
			normals.push_back(Vector3f(0.0f, -1.0f, 0.0f));

		if (generateTangents) {
			tangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
			bitangents.push_back(Vector3f(-1.0f, 0.0f, 0.0f));
		}

		float sectorStep = 2 * PI / vResolution;
		float sectorAngle;  // radian

		for (int i = 0; i < vResolution; ++i) {
			sectorAngle = i * sectorStep;
			x = cos(sectorAngle);
			z = sin(sectorAngle);

			positions.push_back(Vector3f(x * baseRadius, y, z * baseRadius) + position);
			if (generateTexels)
				texels.push_back(Vector2f(-x * 0.5f + 0.5f, z * 0.5f + 0.5f));

			if (generateNormals)
				normals.push_back(Vector3f(0.0f, -1.0f, 0.0f));

			if (generateTangents) {
				tangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
				bitangents.push_back(Vector3f(-1.0f, 0.0f, 0.0f));
			}
		}
	}

	if (top) {
		// remember where the base vertices start
		topVertexIndex = (unsigned int)positions.size();

		// put vertices of top of cylinder
		y = length * 0.5f;

		positions.push_back(Vector3f(0.0f, y, 0.0f) + position);
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

			positions.push_back(Vector3f(x * topRadius, y, z * topRadius) + position);
			if (generateTexels)
				texels.push_back(Vector2f(x * 0.5f + 0.5f, -z * 0.5f + 0.5f));

			if (generateNormals)
				normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));

			if (generateTangents) {
				tangents.push_back(Vector3f(0.0f, 0.0f, -1.0f));
				bitangents.push_back(Vector3f(1.0f, 0.0f, 0.0f));
			}
		}
	}
	// put indices for sides
	unsigned int k1, k2;
	for (int i = 0; i < uResolution; ++i) {
		k1 = i * (vResolution + 1);     // bebinning of current stack
		k2 = k1 + vResolution + 1;      // beginning of next stack

		for (int j = 0; j < vResolution; ++j, ++k1, ++k2) {
			// 2 trianles per sector
			indexBuffer.push_back(k1); indexBuffer.push_back(k2); indexBuffer.push_back(k1 + 1);
			indexBuffer.push_back(k2); indexBuffer.push_back(k2 + 1); indexBuffer.push_back(k1 + 1);
		}
	}

	if (bottom) {
		// remember where the base indices start
		unsigned int baseIndex = (unsigned int)indexBuffer.size();

		// put indices for base
		for (int i = 0, k = baseVertexIndex + 1; i < vResolution; ++i, ++k) {
			if (i < (vResolution - 1)) {
				indexBuffer.push_back(baseVertexIndex);	indexBuffer.push_back(k); indexBuffer.push_back(k + 1);
			}else {
				indexBuffer.push_back(baseVertexIndex);	indexBuffer.push_back(k); indexBuffer.push_back(baseVertexIndex + 1);
			}
		}
	}

	if (top) {
		// remember where the base indices start
		unsigned int topIndex = (unsigned int)indexBuffer.size();

		for (int i = 0, k = topVertexIndex + 1; i < vResolution; ++i, ++k) {
			if (i < (vResolution - 1)) {
				indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(k + 1); indexBuffer.push_back(k);
			}else {
				indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(topVertexIndex + 1); indexBuffer.push_back(k);
			}
		}
	}
}

std::vector<Vector3f>& MeshCylinder::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& MeshCylinder::getIndexBuffer() {
	return m_indexBuffer;
}

int MeshCylinder::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void MeshCylinder::createBuffer() {
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

void MeshCylinder::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshCylinder::createInstancesStatic(const std::vector<Matrix4f>& modelMTX) {
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

	glBindVertexArray(0);
}

void MeshCylinder::addInstance(const Matrix4f& modelMTX) {
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();

	if(m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

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

		glBindVertexArray(0);
	}
}

void MeshCylinder::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}