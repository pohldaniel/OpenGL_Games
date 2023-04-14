#include "MeshTorus.h"

MeshTorus::MeshTorus(int uResolution, int vResolution) : MeshTorus(Vector3f(0.0f, 0.0f, 0.0f), 0.5f, 0.25f, true, true, false, false, uResolution, vResolution) {}

MeshTorus::MeshTorus(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution, int vResolution) : MeshTorus(Vector3f(0.0f, 0.0f, 0.0f), 0.5f, 0.25f, generateTexels, generateNormals, generateTangents, generateNormalDerivatives, uResolution, vResolution) {}

MeshTorus::MeshTorus(const Vector3f &position, float radius, float tubeRadius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution, int vResolution) {
	
	m_radius = radius;
	m_tubeRadius = tubeRadius;
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

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents + 2 * generateNormalDerivatives;
	buildMesh();
}

MeshTorus::~MeshTorus() {}

void MeshTorus::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void MeshTorus::buildMesh() {

	if (m_isInitialized) return;

	std::vector<Vector3f> tangents;
	std::vector<Vector3f> bitangents;
	std::vector<Vector3f> normalsDu;
	std::vector<Vector3f> normalsDv;

	float mainSegmentAngleStep = (2.0f * PI) / float(m_uResolution);
	float tubeSegmentAngleStep = (2.0f * PI) / float(m_vResolution);

	float currentMainSegmentAngle = 0.0f;
	for (unsigned int i = 0; i <= m_uResolution; i++) {

		// Calculate sine and cosine of main segment angle
		float sinMainSegment = sinf(currentMainSegmentAngle);
		float cosMainSegment = cosf(currentMainSegmentAngle);
		float currentTubeSegmentAngle = 0.0f;

		for (unsigned int j = 0; j <= m_vResolution; j++) {

			// Calculate sine and cosine of tube segment angle
			float sinTubeSegment = sinf(currentTubeSegmentAngle);
			float cosTubeSegment = cosf(currentTubeSegmentAngle);

			// Calculate vertex position on the surface of torus
			float x = (m_radius + m_tubeRadius * cosTubeSegment)*cosMainSegment;
			float z = (m_radius + m_tubeRadius * cosTubeSegment)*sinMainSegment;
			float y = m_tubeRadius*sinTubeSegment;

			Vector3f surfacePosition = Vector3f(x, y, z) + m_position;
			m_positions.push_back(surfacePosition);
			// Update current tube angle
			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

		// Update main segment angle
		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	if (m_generateTexels) {

		float mainSegmentTextureStep = 1.0f / (float(m_uResolution));
		float tubeSegmentTextureStep = 1.0f / (float(m_vResolution));

		//rotate the texture to get the same mapping like the primitive
		float currentMainSegmentTexCoordV = 0.0f;

		for (unsigned int i = 0; i <= m_uResolution; i++) {

			//rotate the texture to get the same mapping like the primitive
			float currentTubeSegmentTexCoordU = 0.0f;

			for (unsigned int j = 0; j <= m_vResolution; j++) {
				Vector2f textureCoordinate = Vector2f(1.0 - currentMainSegmentTexCoordV, currentTubeSegmentTexCoordU);
				m_texels.push_back(textureCoordinate);
				currentTubeSegmentTexCoordU += tubeSegmentTextureStep;
			}
			// Update texture coordinate of main segment
			currentMainSegmentTexCoordV += mainSegmentTextureStep;
		}

		m_hasTexels = true;
	}

	if (m_generateNormals) {

		float currentMainSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= m_uResolution; i++) {

			// Calculate sine and cosine of main segment angle
			float sinMainSegment = sin(currentMainSegmentAngle);
			float cosMainSegment = cos(currentMainSegmentAngle);

			float currentTubeSegmentAngle = 0.0f;

			for (unsigned int j = 0; j <= m_vResolution; j++) {

				// Calculate sine and cosine of tube segment angle
				float sinTubeSegment = sin(currentTubeSegmentAngle);
				float cosTubeSegment = cos(currentTubeSegmentAngle);

				float tmp = m_radius + m_tubeRadius *cosTubeSegment;

				Vector3f normal = Vector3f(cosMainSegment*cosTubeSegment, sinTubeSegment, sinMainSegment*cosTubeSegment);

				m_normals.push_back( normal);

				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}
			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
		}
		m_hasNormals = true;
	}

	if (m_generateTangents) {

		float currentMainSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= m_uResolution; i++) {

			// Calculate sine and cosine of main segment angle
			float sinMainSegment = sin(currentMainSegmentAngle);
			float cosMainSegment = cos(currentMainSegmentAngle);

			float currentTubeSegmentAngle = 0.0f;

			for (unsigned int j = 0; j <= m_vResolution; j++) {

				// Calculate sine and cosine of tube segment angle
				float sinTubeSegment = sin(currentTubeSegmentAngle);
				float cosTubeSegment = cos(currentTubeSegmentAngle);

				tangents.push_back(Vector3f(-sinTubeSegment *cosMainSegment, cosTubeSegment, -sinTubeSegment *sinMainSegment));
				bitangents.push_back(Vector3f(-sinMainSegment, 0.0, cosMainSegment));
				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}
			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
		}
		m_hasTangents = true;
	}

	if (m_generateNormalDerivatives) {

		float currentMainSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= m_uResolution; i++) {

			// Calculate sine and cosine of main segment angle
			float sinMainSegment = sin(currentMainSegmentAngle);
			float cosMainSegment = cos(currentMainSegmentAngle);

			float currentTubeSegmentAngle = 0.0f;

			for (unsigned int j = 0; j <= m_vResolution; j++) {

				// Calculate sine and cosine of tube segment angle
				float sinTubeSegment = sin(currentTubeSegmentAngle);
				float cosTubeSegment = cos(currentTubeSegmentAngle);

				//first way: Weingarten equations see pbrt section 3.2.3

				/*Vector3f normal = Vector3f(cosMainSegment*cosTubeSegment, sinTubeSegment, sinMainSegment*cosTubeSegment);
				Vector3f tangent = Vector3f(-sinTubeSegment *cosMainSegment, cosTubeSegment, -sinTubeSegment *sinMainSegment);
				Vector3f bitangent = Vector3f(-sinMainSegment, 0.0, cosMainSegment);

				Vector3f d2Pduu = -Vector3f(cosTubeSegment *cosMainSegment, sinTubeSegment, cosTubeSegment *sinMainSegment);
				Vector3f d2Pdvv = -Vector3f(cosMainSegment, 0.0, sinMainSegment);
				Vector3f d2Pduv = Vector3f(sinMainSegment * sinTubeSegment, 0.0, -sinTubeSegment*cosMainSegment);

				float E = Vector3f::dot(tangent, tangent);
				float F = Vector3f::dot(tangent, bitangent);
				float G = Vector3f::dot(bitangent, bitangent);

				float e = Vector3f::dot(normal, d2Pduu);
				float f = Vector3f::dot(normal, d2Pduv);
				float g = Vector3f::dot(normal, d2Pdvv);

				float invEGF2 = 1.0 / (E * G - F * F);
				int sgn = (invEGF2 >= 0) ? 1 : -1;
				Vector3f dndu = ((f * F - e * G) * sgn * tangent + (e * F - f * E) * sgn * bitangent).normalize();
				Vector3f dndv = ((g * F - f * G) * sgn * tangent + (f * F - g * E) * sgn * bitangent).normalize();

				normalsDu.push_back(dndu);
				normalsDv.push_back(dndv);*/

				//second way: dn/du		and dn/dv	
				float tmp = (m_radius + 2 * m_tubeRadius * cosTubeSegment);

				float n1u = -tmp * cosMainSegment * sinTubeSegment;
				float n2u = tmp * cosTubeSegment - m_tubeRadius;
				float n3u = -tmp * sinMainSegment * sinTubeSegment;
				Vector3f dndu = Vector3f(n1u, n2u, n3u).normalize();
				normalsDu.push_back(dndu);

				int sgn = ((m_radius + m_tubeRadius *cosTubeSegment) *cosTubeSegment >= 0) ? 1 : -1;
				Vector3f dndv = Vector3f(-sinMainSegment * sgn, 0.0, cosMainSegment * sgn);
				normalsDv.push_back(dndv);

				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}
			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
		}
		m_hasNormalDerivatives = true;
	}

	//calculate the indices
	unsigned int currentVertexOffset = 0;
	for (unsigned int i = 0; i < m_uResolution; i++) {

		for (unsigned int j = 0; j <= m_vResolution; j++) {

			unsigned int vertexIndexA, vertexIndexB, vertexIndexC, vertexIndexD, vertexIndexE, vertexIndexF;

			if ((j > 0) && ((j + 1) % (m_vResolution + 1)) == 0) {
				currentVertexOffset = ((i + 1) * (m_vResolution + 1));
			} else {

				vertexIndexA = currentVertexOffset;
				vertexIndexB = currentVertexOffset + m_vResolution + 1;
				vertexIndexC = currentVertexOffset + 1;

				vertexIndexD = currentVertexOffset + m_vResolution + 1;
				vertexIndexF = currentVertexOffset + m_vResolution + 2;
				vertexIndexE = currentVertexOffset + 1;

				m_indexBuffer.push_back(vertexIndexA); m_indexBuffer.push_back(vertexIndexC); m_indexBuffer.push_back(vertexIndexB);
				m_indexBuffer.push_back(vertexIndexD); m_indexBuffer.push_back(vertexIndexE); m_indexBuffer.push_back(vertexIndexF);
				currentVertexOffset++;
			}
		}
	}


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
	tangents.clear();
	tangents.shrink_to_fit();
	bitangents.clear();
	bitangents.shrink_to_fit();
	normalsDu.clear();
	normalsDu.shrink_to_fit();
	normalsDv.clear();
	normalsDv.shrink_to_fit();
	/*m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();*/

	m_isInitialized = true;
}

void MeshTorus::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshTorus::createInstancesStatic(const std::vector<Matrix4f>& modelMTX) {
	m_instances.clear();
	m_instances.shrink_to_fit();
	m_instances = modelMTX;

	m_instanceCount = m_instances.size();

	glGenBuffers(1, &m_vboInstances);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);
}

void MeshTorus::addInstance(const Matrix4f& modelMTX){
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

	}else {
		glGenBuffers(1, &m_vboInstances);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}

void MeshTorus::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}