#include "MeshTorusKnot.h"

MeshTorusKnot::MeshTorusKnot(int uResolution, int vResolution) : MeshTorusKnot(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 0.4f, 2, 3, true, true, false, uResolution, vResolution) {}

MeshTorusKnot::MeshTorusKnot(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) : MeshTorusKnot(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 0.4f, 2, 3, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

MeshTorusKnot::MeshTorusKnot(const Vector3f &position, float radius, float tubeRadius, int p, int q, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) {

	m_radius = radius;
	m_tubeRadius = tubeRadius;
	m_p = p;
	m_q = q;

	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents;
	BuildMesh(m_radius, m_tubeRadius, m_p, m_q, m_position, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

MeshTorusKnot::~MeshTorusKnot() {
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

void MeshTorusKnot::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void MeshTorusKnot::CalculatePositionOnCurve(float u, float p, float q, float radius, Vector3f& position) {

	float cu = std::cosf(u);
	float su = std::sinf(u);
	float quOverP = (q / p) * u;
	float cs = std::cosf(quOverP);

	position[0] = radius * (2.0f + cs) * 0.5f * cu;
	position[1] = radius * (2.0f + cs) * su * 0.5f;
	position[2] = radius * std::sinf(quOverP) * 0.5f;

}

void MeshTorusKnot::CalculatePositionOnCurve(float u, float p, float q, float r1, float r2, Vector3f& position) {

	position[0] = (r1 + r2 * std::cosf(q * u)) * std::cosf(p * u);
	position[1] = (r1 + r2 * std::cosf(q * u)) * std::sinf(p * u);
	position[2] = -r2 * std::sinf(q * u);

}

void MeshTorusKnot::BuildMesh(float radius, float tubeRadius, int p, int q, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {
	
	float mainSegmentAngleStep = 1.0f / float(uResolution);
	float tubeSegmentAngleStep = 1.0f / float(vResolution);

	Vector3f p1;
	Vector3f p2;
	Vector3f p3;

	Vector3f T, B, N;

	float currentMainSegmentAngle = 0.0f;
	for (unsigned int i = 0; i <= uResolution; ++i) {

		float u = currentMainSegmentAngle * TWO_PI ;
		CalculatePositionOnCurve(u, p, q, 2.0f * radius, radius, p1);
		float uNext = (currentMainSegmentAngle + mainSegmentAngleStep) * TWO_PI;
		CalculatePositionOnCurve(uNext, p, q, 2.0f * radius, radius, p2);
		CalculatePositionOnCurve(u, p, q, 2.0f * radius, 0.0f, p3);

		T = p2 - p1;
		B = Vector3f::Normalize(p1 - p3);
		N = Vector3f::Normalize(Vector3f::Cross(T, B));
		T = Vector3f::Normalize(Vector3f::Cross(B, N));

		Vector3f::Normalize(B);
		Vector3f::Normalize(N);

		float currentTubeSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= vResolution; ++i) {
			float v = currentTubeSegmentAngle * TWO_PI;

			float cx = tubeRadius * std::sinf(v);
			float cy = tubeRadius * std::cosf(v);

			Vector3f pos = { p1[0] + (cx * N[0] + cy * B[0]), p1[1] + (cx * N[1] + cy * B[1]), p1[2] + (cx * N[2] + cy * B[2]) };		
			Vector3f surfacePosition = pos + position;
			positions.push_back(surfacePosition);

			if (generateTexels) {
				Vector2f textureCoordinate = Vector2f(currentMainSegmentAngle, 1.0f - currentTubeSegmentAngle);
				texels.push_back(textureCoordinate);
			}

			if (generateNormals) {
				Vector3f normal = Vector3f::Normalize(pos - p1);
				normals.push_back(normal);
			}

			if (generateTangents) {
				tangents.push_back(T);
				bitangents.push_back(Vector3f::Cross(tangents.back(), normals.back()));

			}

			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	//calculate the indices
	unsigned int currentVertexOffset = 0;
	for (unsigned int i = 0; i < uResolution; i++) {

		for (unsigned int j = 0; j <= vResolution; j++) {

			unsigned int vertexIndexA, vertexIndexB, vertexIndexC, vertexIndexD, vertexIndexE, vertexIndexF;

			if ((j > 0) && ((j + 1) % (vResolution + 1)) == 0) {
				currentVertexOffset = ((i + 1) * (vResolution + 1));
			}else {

				vertexIndexA = currentVertexOffset;
				vertexIndexB = currentVertexOffset + vResolution + 1;
				vertexIndexC = currentVertexOffset + 1;

				vertexIndexD = currentVertexOffset + vResolution + 1;
				vertexIndexF = currentVertexOffset + vResolution + 2;
				vertexIndexE = currentVertexOffset + 1;

				indexBuffer.push_back(vertexIndexA); indexBuffer.push_back(vertexIndexC); indexBuffer.push_back(vertexIndexB);
				indexBuffer.push_back(vertexIndexD); indexBuffer.push_back(vertexIndexE); indexBuffer.push_back(vertexIndexF);
				currentVertexOffset++;
			}
		}
	}
}

std::vector<Vector3f>& MeshTorusKnot::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& MeshTorusKnot::getIndexBuffer() {
	return m_indexBuffer;
}

int MeshTorusKnot::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void MeshTorusKnot::createBuffer() {
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

void MeshTorusKnot::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshTorusKnot::createInstancesStatic(const std::vector<Matrix4f>& modelMTX) {
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

void MeshTorusKnot::addInstance(const Matrix4f& modelMTX) {
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else {
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

void MeshTorusKnot::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}