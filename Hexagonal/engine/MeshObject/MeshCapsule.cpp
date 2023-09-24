#include "MeshCapsule.h"

MeshCapsule::MeshCapsule(int uResolution, int vResolution) : MeshCapsule(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, true, true, false, uResolution, vResolution) { }

MeshCapsule::MeshCapsule(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) : MeshCapsule(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, generateTexels, generateNormals, generateTangents, uResolution, vResolution) { }

MeshCapsule::MeshCapsule(const Vector3f &position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) {
	
	m_radius = radius;
	m_length = length;
	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + generateTangents * 2;

	m_min = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);

	BuildMesh(m_radius, m_length, m_position, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
}

MeshCapsule::~MeshCapsule() {
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

void MeshCapsule::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void MeshCapsule::BuildMesh(float radius, float length, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {

	BuildHemisphere(radius, length, position + Vector3f(0.0f, length * 0.5f, 0.0f), true, uResolution, vResolution, generateTexels, generateNormals, generateTangents, positions, texels, normals, indexBuffer, tangents, bitangents);
	if (length != 0)
		BuildCylinder(radius, length, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, positions, texels, normals, indexBuffer, tangents, bitangents);
	BuildHemisphere(radius, length, position + Vector3f(0.0f, -length * 0.5f, 0.0f), false, uResolution, vResolution, generateTexels, generateNormals, generateTangents, positions, texels, normals, indexBuffer, tangents, bitangents);


}

void MeshCapsule::BuildHemisphere(float radius, float length, const Vector3f& position, bool north, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {

	float uAngleStep = (2.0f * PI) / float(uResolution);
	float vAngleStep = (0.5f * PI) / float(vResolution);
	float vSegmentAngle, uSegmentAngle;

	unsigned int baseIndex = positions.size();

	for (int i = 0; i <= vResolution; ++i) {
		vSegmentAngle = i * vAngleStep;
		float cosVSegment = cosf(vSegmentAngle);
		float sinVSegment = sinf(vSegmentAngle);

		for (int j = 0; j <= uResolution; ++j) {
			uSegmentAngle = j * uAngleStep;

			float cosUSegment = cosf(uSegmentAngle);
			float sinUSegment = sinf(uSegmentAngle);

			float x = cosVSegment * cosUSegment;
			float y = sinVSegment;
			float z = cosVSegment * sinUSegment;

			positions.push_back(north ? Vector3f(radius * x, radius * y, radius * z) + position : Vector3f(radius * x, -radius * y, radius * z) + position);

			if (generateTexels)
				texels.push_back(Vector2f(1.0f - (float)j / uResolution, north ? (float)i / vResolution : 1.0f - (float)i / vResolution));

			if (generateNormals)
				normals.push_back(north ? Vector3f(x, y, z) : Vector3f(x, -y, z));

			if (generateTangents) {
				tangents.push_back(Vector3f(sinUSegment, 0.0, cosUSegment)); // rotate 180 z Axis to match texture space
				bitangents.push_back(Vector3f(-sinVSegment*cosUSegment, cosVSegment, -sinVSegment*sinUSegment).normalize());
			}

			//improvte texture mapping
			//if (i == m_vResolution) {
			//	break;
			//}
		}
	}

	//improvte texture mapping
	//int cutoff = (m_vResolution + 1) * m_uResolution;
	for (unsigned int i = 0; i < vResolution; i++) {

		int k1 = i * (uResolution + 1);
		int k2 = k1 + (uResolution + 1);

		for (unsigned int j = 0; j < uResolution; j++) {
			if (north) {
				indexBuffer.push_back(k1 + j + 1 + baseIndex);			
				indexBuffer.push_back(k1 + j + baseIndex);
				indexBuffer.push_back(k2 + j + baseIndex);

				if (i < vResolution - 1) {
					indexBuffer.push_back(k2 + j + 1 + baseIndex);				
					indexBuffer.push_back(k1 + j + 1 + baseIndex);
					indexBuffer.push_back(k2 + j + baseIndex);
				}
			} else {
				indexBuffer.push_back(k1 + j + 1 + baseIndex);
				indexBuffer.push_back(k2 + j + baseIndex);
				indexBuffer.push_back(k1 + j + baseIndex);

				if (i < vResolution - 1) {
					indexBuffer.push_back(k2 + j + 1 + baseIndex);
					indexBuffer.push_back(k2 + j + baseIndex);
					indexBuffer.push_back(k1 + j + 1 + baseIndex);
				}
			}
		}

		//improvte texture mapping
		//for (unsigned int j = 0; j < m_uResolution; j++) {
		//
		//	m_indexBuffer.push_back(k1 + j + 1 + baseIndex >= cutoff ? cutoff : k1 + j + 1 + baseIndex);
		//	m_indexBuffer.push_back(k2 + j + baseIndex >= cutoff ? cutoff : k2 + j + baseIndex);
		//	m_indexBuffer.push_back(k1 + j + baseIndex >= cutoff ? cutoff : k1 + j + baseIndex);
		//
		//	if (i < m_vResolution - 1) {
		//		m_indexBuffer.push_back(k2 + j + 1 + baseIndex >= cutoff ? cutoff : k2 + j + 1 + baseIndex);
		//		m_indexBuffer.push_back(k2 + j + baseIndex >= cutoff ? cutoff : k2 + j + baseIndex);
		//		m_indexBuffer.push_back(k1 + j + 1 + baseIndex >= cutoffp ? cutoff : k1 + j + 1 + baseIndex);
		//	}
		//}
	}
}

void MeshCapsule::BuildCylinder(float radius, float length, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {
	unsigned int baseIndex = positions.size();
	float x, y, z;
	
	// put vertices of side cylinder to array by scaling unit circle
	for (int i = 0; i <= vResolution; ++i) {
		y = -(length * 0.5f) + (float)i / vResolution * length;      // vertex position z
		float t = 1.0f - (float)i / vResolution;   // top-to-bottom

		float sectorStep = 2 * PI / uResolution;
		float sectorAngle;  // radian

		for (int j = 0, k = 0; j <= uResolution; ++j, k += 3) {
			sectorAngle = j * sectorStep;
			x = cos(sectorAngle);
			z = sin(sectorAngle);
			positions.push_back(Vector3f(x * radius, y, z * radius) + position);

			if (generateTexels)
				texels.push_back(Vector2f(1.0f - (float)j / vResolution, 1.0f - t));

			if (generateNormals) {
				normals.push_back(Vector3f(x, 0.0f, z));
			}

			if (generateTangents) {
				tangents.push_back(Vector3f(-z, 0.0f, x));
				bitangents.push_back(Vector3f::Cross(tangents.back(), normals.back()));
				//bitangents.push_back(Vector3f(x, 1.0f, z) * 0.5f);
			}
		}
	}

	// put indices for sides
	unsigned int k1, k2;
	for (int i = 0; i < vResolution; ++i) {
		k1 = i * (uResolution + 1);     // bebinning of current stack
		k2 = k1 + uResolution + 1;      // beginning of next stack

		for (int j = 0; j < uResolution; ++j, ++k1, ++k2) {
			// 2 trianles per sector
			indexBuffer.push_back(k1 + baseIndex); indexBuffer.push_back(k2 + baseIndex); indexBuffer.push_back(k1 + 1 + baseIndex);
			indexBuffer.push_back(k2 + baseIndex); indexBuffer.push_back(k2 + 1 + baseIndex); indexBuffer.push_back(k1 + 1 + baseIndex);
		}
	}
}

std::vector<Vector3f>& MeshCapsule::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& MeshCapsule::getIndexBuffer() {
	return m_indexBuffer;
}


int MeshCapsule::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void MeshCapsule::createBuffer() {
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

void MeshCapsule::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshCapsule::createInstancesStatic(const std::vector<Matrix4f>& modelMTX) {
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

void MeshCapsule::addInstance(const Matrix4f& modelMTX) {
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

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

		glBindVertexArray(0);
	}
}

void MeshCapsule::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}