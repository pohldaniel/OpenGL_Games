#include "Capsule.h"

Capsule::Capsule(int uResolution, int vResolution) : Capsule(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, true, true, false, false, uResolution, vResolution) { }

Capsule::Capsule(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution, int vResolution) : Capsule(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, generateTexels, generateNormals, generateTangents, generateNormalDerivatives, uResolution, vResolution) { }

Capsule::Capsule(const Vector3f &position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution, int vResolution) {
	
	m_radius = radius;
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
	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + generateTangents * 2 + generateNormalDerivatives * 2;

	m_min = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	m_max = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);

	BuildMesh(m_position, m_radius, m_length, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_positions, m_texels, m_normals, m_indexBuffer);
}

Capsule::~Capsule() {}

void Capsule::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void Capsule::createBuffer() {
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


int Capsule::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void Capsule::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Capsule::BuildMesh(const Vector3f& position, float radius, float length, int uResolution, int vResolution, bool generateTexels, bool generateNormals, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer) {

	BuildHemisphere(position, radius, length, Vector3f(0.0f, length * 0.5f, 0.0f), true, uResolution, vResolution, generateTexels, generateNormals, positions, texels, normals, indexBuffer);
	if (length != 0)
		BuildCylinder(position, radius, length, uResolution, vResolution, generateTexels, generateNormals, positions, texels, normals, indexBuffer);
	BuildHemisphere(position, radius, length, Vector3f(0.0f, -length * 0.5f, 0.0f), false, uResolution, vResolution, generateTexels, generateNormals, positions, texels, normals, indexBuffer);


}

void Capsule::BuildHemisphere(const Vector3f& position, float radius, float length, const Vector3f &offset, bool north, int uResolution, int vResolution, bool generateTexels, bool generateNormals, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer) {

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

			positions.push_back((north ? Vector3f(radius * x, radius * y, radius * z) + offset : Vector3f(radius * x, -radius * y, radius * z) + offset) + position);

			if (generateTexels)
				texels.push_back(Vector2f(1.0f - (float)j / uResolution, north ? (float)i / vResolution : 1.0f - (float)i / vResolution));

			if (generateNormals)
				normals.push_back(north ? Vector3f(x, y, z) : Vector3f(x, -y, z));

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

			indexBuffer.push_back(k1 + j + 1 + baseIndex);
			indexBuffer.push_back(k2 + j + baseIndex);
			indexBuffer.push_back(k1 + j + baseIndex);

			if (i < vResolution - 1) {
				indexBuffer.push_back(k2 + j + 1 + baseIndex);
				indexBuffer.push_back(k2 + j + baseIndex);
				indexBuffer.push_back(k1 + j + 1 + baseIndex);
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

std::vector<float> Capsule::GetSideNormals(int uResolution) {

	float sectorStep = 2 * PI / uResolution;
	float sectorAngle;
	std::vector<float> normals;
	for (int i = 0; i <= uResolution; ++i) {
		sectorAngle = i * sectorStep;
		normals.push_back(cos(sectorAngle));
		normals.push_back(0.0f);
		normals.push_back(sin(sectorAngle));
	}

	return normals;
}

void Capsule::BuildCylinder(const Vector3f& position, float radius, float length, int uResolution, int vResolution, bool generateTexels, bool generateNormals, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer) {
	unsigned int baseIndex = positions.size();
	float x, y, z;                                  // vertex position
													// get normals for cylinder sides
	std::vector<float> sideNormals = GetSideNormals(uResolution);

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
				texels.push_back(Vector2f(1.0f - (float)j / uResolution, 1.0f - t));

			if (generateNormals)
				normals.push_back(Vector3f(sideNormals[k], sideNormals[k + 1], sideNormals[k + 2]));
		}
	}

	// put indices for sides
	unsigned int k1, k2;
	for (int i = 0; i < vResolution; ++i) {
		k1 = i * (uResolution + 1);     // bebinning of current stack
		k2 = k1 + uResolution + 1;      // beginning of next stack

		for (int j = 0; j < uResolution; ++j, ++k1, ++k2) {
			// 2 trianles per sector
			indexBuffer.push_back(k1 + baseIndex);	indexBuffer.push_back(k1 + 1 + baseIndex);	indexBuffer.push_back(k2 + baseIndex);
			indexBuffer.push_back(k2 + baseIndex);	indexBuffer.push_back(k1 + 1 + baseIndex);	indexBuffer.push_back(k2 + 1 + baseIndex);
		}
	}
}
