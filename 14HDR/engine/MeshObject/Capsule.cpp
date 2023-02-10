#include "Capsule.h"

Capsule::Capsule(const Vector3f &position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) {
	m_radius = radius;
	m_invRadius = 1.0 / radius;
	m_length = length;
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
	m_uResolution = 20;
	m_vResolution = 20;

	m_numBuffers = 1 + generateTexels + generateNormals + generateTangents * 2 + generateNormalDerivatives * 2;

	m_model = Matrix4f::IDENTITY;

	m_min = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);

	buildMesh();
}

Capsule::Capsule(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) : Capsule(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, generateTexels, generateNormals, generateTangents, generateNormalDerivatives) { }

Capsule::Capsule(const Vector3f &position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, const std::string &texture) {

	m_radius = radius;
	m_invRadius = 1.0 / radius;
	m_length = length;
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
	m_uResolution = 49;
	m_vResolution = 49;

	m_numBuffers = 2 + generateTexels + generateNormals + generateTangents * 2 + generateNormalDerivatives * 2;

	m_model = Matrix4f::IDENTITY;

	m_texture = std::make_shared<Texture>(texture);
	m_shader = std::make_shared<Shader>("shader/texture.vert", "shader/texture.frag");

	m_min = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);
}

Capsule::Capsule(const Vector3f &position, float radius, float length, const std::string &texture) : Capsule(position, radius, length, true, true, false, false, texture) {

}

Capsule::Capsule(float radius, float length, const std::string &texture) : Capsule(Vector3f(0.0f, 0.0f, 0.0f), radius, 1.0f, true, true, false, false, texture) { }

Capsule::~Capsule() {}

void Capsule::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}


void Capsule::buildMesh() {
	
	buildHemisphere(Vector3f(0.0f, m_length * 0.5f, 0.0f), true);
	buildCylinder();
	buildHemisphere(Vector3f(0.0f, -m_length * 0.5f, 0.0f), false);

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

	m_isInitialized = true;
}

void Capsule::buildHemisphere(const Vector3f &offset, bool north) {

	float uAngleStep = (2.0f * PI) / float(m_uResolution);
	float vAngleStep = (0.5f * PI) / float(m_vResolution);
	float vSegmentAngle, uSegmentAngle;

	unsigned int baseIndex = m_positions.size();

	for (int i = 0; i <= m_vResolution; ++i) {
		vSegmentAngle = i * vAngleStep;
		float cosVSegment = cosf(vSegmentAngle);
		float sinVSegment = sinf(vSegmentAngle);

		for (int j = 0; j <= m_uResolution; ++j) {
			uSegmentAngle = j * uAngleStep;

			float cosUSegment = cosf(uSegmentAngle);
			float sinUSegment = sinf(uSegmentAngle);

			float x = cosVSegment * cosUSegment;
			float y = sinVSegment;
			float z = cosVSegment * sinUSegment;

			m_positions.push_back(north ? Vector3f(m_radius * x, m_radius * y, m_radius * z) + offset : Vector3f(m_radius * x, -m_radius * y, m_radius * z) + offset);

			if (m_generateTexels) {
				m_texels.push_back(Vector2f(1.0f - (float)j / m_uResolution, north ? (float)i / m_vResolution : 1.0f - (float)i / m_vResolution));
			}

			if (m_generateNormals)
				m_normals.push_back(north ? Vector3f(x, y, z): Vector3f(x, -y, z));
		}
	}

	for (unsigned int i = 0; i < m_vResolution; i++) {

		int k1 = i * (m_uResolution + 1);
		int k2 = k1 + (m_uResolution + 1);

		for (unsigned int j = 0; j < m_uResolution; j++) {

			m_indexBuffer.push_back(k1 + j + 1 + baseIndex);
			m_indexBuffer.push_back(k2 + j + baseIndex);
			m_indexBuffer.push_back(k1 + j + baseIndex);

			m_indexBuffer.push_back(k2 + j + 1 + baseIndex);
			m_indexBuffer.push_back(k2 + j + baseIndex);
			m_indexBuffer.push_back(k1 + j + 1 + baseIndex);
		}
	}
}

std::vector<float> Capsule::getSideNormals() {
	
	float sectorStep = 2 * PI / m_uResolution;
	float sectorAngle;
	std::vector<float> normals;
	for (int i = 0; i <= m_uResolution; ++i) {
		sectorAngle = i * sectorStep;
		normals.push_back(cos(sectorAngle));
		normals.push_back(0.0f);
		normals.push_back(sin(sectorAngle));						
	}

	return normals;
}

void Capsule::buildCylinder() {
	unsigned int baseIndex = m_positions.size();
	float x, y, z;                                  // vertex position
	float radius;                                   // radius for each stack

													// get normals for cylinder sides
	std::vector<float> sideNormals = getSideNormals();

	// put vertices of side cylinder to array by scaling unit circle
	for (int i = 0; i <= m_vResolution; ++i) {
		y = -(m_length * 0.5f) + (float)i / m_vResolution * m_length;      // vertex position z
		radius = m_radius;
		float t = 1.0f - (float)i / m_vResolution;   // top-to-bottom

		float sectorStep = 2 * PI / m_uResolution;
		float sectorAngle;  // radian

		for (int j = 0, k = 0; j <= m_uResolution; ++j, k += 3) {
			sectorAngle = j * sectorStep;
			x = cos(sectorAngle);
			z = sin(sectorAngle);
			m_positions.push_back(Vector3f(x * radius, y, z * radius) + m_position);
			m_texels.push_back(Vector2f(1.0f - (float)j / m_uResolution, 1.0f - t));
			m_normals.push_back(Vector3f(sideNormals[k], sideNormals[k + 1], sideNormals[k + 2]));
		}
	}


	// put indices for sides
	unsigned int k1, k2;
	for (int i = 0; i < m_vResolution; ++i) {
		k1 = i * (m_uResolution + 1);     // bebinning of current stack
		k2 = k1 + m_uResolution + 1;      // beginning of next stack

		for (int j = 0; j < m_uResolution; ++j, ++k1, ++k2) {
			// 2 trianles per sector
			m_indexBuffer.push_back(k1 + baseIndex);	m_indexBuffer.push_back(k1 + 1 + baseIndex);	m_indexBuffer.push_back(k2 + baseIndex);
			m_indexBuffer.push_back(k2 + baseIndex);	m_indexBuffer.push_back(k1 + 1 + baseIndex);	m_indexBuffer.push_back(k2 + 1 + baseIndex);
		}
	}


}

void Capsule::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}