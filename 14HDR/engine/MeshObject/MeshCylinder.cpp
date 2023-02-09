#include "MeshCylinder.h"

MeshCylinder::MeshCylinder(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) : MeshCylinder(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 1.0f, generateTexels, generateNormals, generateTangents, generateNormalDerivatives) {

}

MeshCylinder::MeshCylinder(const Vector3f &position, float baseRadius, float topRadius, float length, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) {

	this->baseRadius = baseRadius;
	this->topRadius = topRadius;
	height = length;
	m_position = position;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;
	m_generateNormalDerivatives = generateNormalDerivatives;

	m_hasTexels = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_hasNormalDerivatives = false;

	m_isInitialized = false;
	m_mainSegments = 49;
	m_tubeSegments = 49;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents + 2 * generateNormalDerivatives;

	m_model = Matrix4f::IDENTITY;

	buildMesh();
}

MeshCylinder::~MeshCylinder() {}

void MeshCylinder::setPrecision(int uResolution, int vResolution) {
	m_mainSegments = uResolution;
	m_tubeSegments = vResolution;
}

void MeshCylinder::buildMesh() {

	float x, y, z;                                  // vertex position
	float radius;                                   // radius for each stack

													// get normals for cylinder sides
	std::vector<float> sideNormals = getSideNormals();

	// put vertices of side cylinder to array by scaling unit circle
	for (int i = 0; i <= m_mainSegments; ++i) {
		y = -(height * 0.5f) + (float)i / m_mainSegments * height;      // vertex position z
		radius = baseRadius + (float)i / m_mainSegments * (topRadius - baseRadius);     // lerp
		float t = 1.0f - (float)i / m_mainSegments;   // top-to-bottom

		float sectorStep = 2 * PI / m_tubeSegments;
		float sectorAngle;  // radian

		for (int j = 0, k = 0; j <= m_tubeSegments; ++j, k += 3) {
			sectorAngle = j * sectorStep;
			x = cos(sectorAngle);
			z = sin(sectorAngle);
			m_positions.push_back(Vector3f(x * radius, y, z * radius) + m_position);
			m_texels.push_back(Vector2f(1.0f - (float)j / m_tubeSegments, 1.0f - t));
			m_normals.push_back(Vector3f(sideNormals[k], sideNormals[k + 1], sideNormals[k + 2]));
		}
	}

	// remember where the base.top vertices start
	unsigned int baseVertexIndex = (unsigned int)m_positions.size();

	// put vertices of base of cylinder
	y = -height * 0.5f;

	m_positions.push_back(Vector3f(0.0f, y, 0.0f) + m_position);
	m_texels.push_back(Vector2f(0.5f, 0.5f));
	m_normals.push_back(Vector3f(0.0f, -1.0f, 0.0f));

	float sectorStep = 2 * PI / m_tubeSegments;
	float sectorAngle;  // radian

	for (int i = 0; i < m_tubeSegments; ++i) {
		sectorAngle = i * sectorStep;
		x = cos(sectorAngle);
		z = sin(sectorAngle);

		m_positions.push_back(Vector3f(x * baseRadius, y, z * baseRadius) + m_position);
		m_texels.push_back(Vector2f(-x * 0.5f + 0.5f,  z * 0.5f + 0.5f));
		m_normals.push_back(Vector3f(0.0f, -1.0f, 0.0f));
	}

	// remember where the base vertices start
	unsigned int topVertexIndex = (unsigned int)m_positions.size();

	// put vertices of top of cylinder
	y = height * 0.5f;

	m_positions.push_back(Vector3f(0.0f, y, 0.0f) + m_position);
	m_texels.push_back(Vector2f(0.5f, 0.5f));
	m_normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));

	sectorStep = 2 * PI / m_tubeSegments;
	sectorAngle;  // radian

	for (int i = 0; i < m_tubeSegments; ++i) {
		sectorAngle = i * sectorStep;
		x = cos(sectorAngle);
		z = sin(sectorAngle);

		m_positions.push_back(Vector3f(x * topRadius, y, z * topRadius) + m_position);
		m_texels.push_back(Vector2f(x * 0.5f + 0.5f, -z * 0.5f + 0.5f));
		m_normals.push_back(Vector3f(0.0f, 1.0f, 0.0f));
	}

	// put indices for sides
	unsigned int k1, k2;
	for (int i = 0; i < m_mainSegments; ++i) {
		k1 = i * (m_tubeSegments + 1);     // bebinning of current stack
		k2 = k1 + m_tubeSegments + 1;      // beginning of next stack

		for (int j = 0; j < m_tubeSegments; ++j, ++k1, ++k2) {
			// 2 trianles per sector
			m_indexBuffer.push_back(k1);	m_indexBuffer.push_back(k1 + 1);	m_indexBuffer.push_back(k2);
			m_indexBuffer.push_back(k2);	m_indexBuffer.push_back(k1 + 1);	m_indexBuffer.push_back(k2 + 1);
		}
	}

	// remember where the base indices start
	unsigned int baseIndex = (unsigned int)m_indexBuffer.size();

	// put indices for base
	for (int i = 0, k = baseVertexIndex + 1; i < m_tubeSegments; ++i, ++k) {
		if (i < (m_tubeSegments - 1)) {
			m_indexBuffer.push_back(baseVertexIndex);	m_indexBuffer.push_back(k + 1);	m_indexBuffer.push_back(k);
		}
		else {
			m_indexBuffer.push_back(baseVertexIndex);	m_indexBuffer.push_back(baseVertexIndex + 1);	m_indexBuffer.push_back(k);
		}
	}

	// remember where the base indices start
	unsigned int topIndex = (unsigned int)m_indexBuffer.size();

	for (int i = 0, k = topVertexIndex + 1; i < m_tubeSegments; ++i, ++k) {
		if (i < (m_tubeSegments - 1)) {
			m_indexBuffer.push_back(topVertexIndex);	m_indexBuffer.push_back(k);	m_indexBuffer.push_back(k + 1);
		}
		else {
			m_indexBuffer.push_back(topVertexIndex);	m_indexBuffer.push_back(k);	m_indexBuffer.push_back(topVertexIndex + 1);
		}
	}

	m_drawCount = m_indexBuffer.size();

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(3, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	//if (m_generateTexels) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//}

	//Normals
	//if (m_generateNormals) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//}

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	/*m_positions.clear();
	m_positions.shrink_to_fit();*/
	m_texels.clear();
	m_texels.shrink_to_fit();
	m_normals.clear();
	m_normals.shrink_to_fit();
	/*m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();*/

	//m_isInitialized = true;
}

std::vector<float> MeshCylinder::getSideNormals() {
	float sectorStep = 2 * PI / m_tubeSegments;
	float sectorAngle;  // radian

						// compute the normal vector at 0 degree first
						// tanA = (baseRadius-topRadius) / height
	float zAngle = atan2(baseRadius - topRadius, height);
	float x0 = cos(zAngle);     // nx
	float y0 = sin(zAngle);     // ny

	std::vector<float> normals;
	for (int i = 0; i <= m_tubeSegments; ++i) {
		sectorAngle = i * sectorStep;
		normals.push_back(cos(sectorAngle)*x0);   // nx
		normals.push_back(y0);					  // ny
		normals.push_back(sin(sectorAngle)*x0);   // nz							
	}

	return normals;
}

void MeshCylinder::draw(const Camera& camera) {

	glUseProgram(m_shader->m_program);

	m_texture->bind(0);
	m_shader->loadMatrix("u_modelView", m_model * camera.getViewMatrix());
	m_shader->loadMatrix("u_projection", camera.getProjectionMatrix());

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

void MeshCylinder::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}