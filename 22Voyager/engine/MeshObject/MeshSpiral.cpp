#include "MeshSpiral.h"
#include <iostream>

MeshSpiral::MeshSpiral(int uResolution, int vResolution) : MeshSpiral(Vector3f(0.0f, -0.75f, 0.0f), 0.5f, 0.25f, 2, 1.5f, true, true, true, false, uResolution, vResolution) { }

MeshSpiral::MeshSpiral(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) : MeshSpiral(Vector3f(0.0f, -0.75f, 0.0f), 0.5f, 0.25f, 2, 1.5f, true, generateTexels, generateNormals, generateTangents, uResolution, vResolution) { }

MeshSpiral::MeshSpiral(const Vector3f &position, float radius, float tubeRadius, int numRotations, float length, bool repeatTexture, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution, int vResolution) {
	
	m_radius = radius;
	m_tubeRadius = tubeRadius;
	m_numRotations = numRotations;
	m_length = length;
	m_repeatTexture = repeatTexture;
	m_position = position;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_numBuffers = 1 + generateTexels + generateNormals + 2 * generateTangents;

	BuildMesh(m_radius, m_tubeRadius, m_length, m_numRotations, m_repeatTexture, m_position, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

MeshSpiral::~MeshSpiral() {
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

void MeshSpiral::setPrecision(int uResolution, int vResolution) {

	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

void MeshSpiral::BuildMesh(float radius, float tubeRadius, float length, int numRotations, bool repeatTexture, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents) {

	float mainSegmentAngleStep = (2.0f * PI) / float(uResolution);
	float tubeSegmentAngleStep = (2.0f * PI) / float(vResolution);
	float pitch = length / numRotations;

	float currentMainSegmentAngle = 0.0f;

	for (unsigned int i = 0; i <= uResolution * numRotations; i++) {

		// Calculate sine and cosine of main segment angle
		float sinMainSegment = sinf(currentMainSegmentAngle);
		float cosMainSegment = cosf(currentMainSegmentAngle);

		float currentTubeSegmentAngle = 0.0f;

		if (i > 0 && (uResolution + 1) % i == 0) currentTubeSegmentAngle = 0.0f;

		for (unsigned int j = 0; j <= vResolution; j++) {

			// Calculate sine and cosine of tube segment angle
			float sinTubeSegment = sinf(currentTubeSegmentAngle);
			float cosTubeSegment = cosf(currentTubeSegmentAngle);

			//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius

			//	[x] = [(R + r cos(u)) cos(v))
			//	[y] = [h * v/(2PI) + rsin(u)]
			//	[z] = [(R + r cos(u)) sin(v)]
			float x = (radius + tubeRadius * cosTubeSegment)*cosMainSegment;
			float z = (radius + tubeRadius * cosTubeSegment)*sinMainSegment;
			float y = pitch * (currentMainSegmentAngle * invTWO_PI) + tubeRadius * sinTubeSegment;

			Vector3f surfacePosition = Vector3f(x, y, z) + position;
			positions.push_back(surfacePosition);

			// Update current tube angle
			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

		// Update main segment angle
		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	if (generateNormals) {

		float currentMainSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= uResolution * numRotations; i++) {

			// Calculate sine and cosine of main segment angle
			float sinMainSegment = sin(currentMainSegmentAngle);
			float cosMainSegment = cos(currentMainSegmentAngle);

			float currentTubeSegmentAngle = 0.0f;

			if (i > 0 && (uResolution + 1) % i == 0) currentTubeSegmentAngle = 0.0f;

			for (unsigned int j = 0; j <= vResolution; j++) {

				// Calculate sine and cosine of tube segment angle
				float sinTubeSegment = sin(currentTubeSegmentAngle);
				float cosTubeSegment = cos(currentTubeSegmentAngle);

				//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius
				//dp/du x dp/dv

				//	[n1] = [(Rr + r^2 cos(u)) cos(v) cos(u) + r sin(u)sin(v)h/(2PI) )
				//	[n2] = [(Rr + r^2 cos(u)) sin(u)]
				//	[n3] = [(Rr + r^2 cos(u)) sin(v) cos(u) - r sin(u)cos(v)h/(2PI)]

				float tmp = radius * tubeRadius + tubeRadius * tubeRadius * cosTubeSegment;
				float n1 = tmp * cosMainSegment * cosTubeSegment + tubeRadius * pitch * invTWO_PI * sinMainSegment * sinTubeSegment;
				float n2 = tmp * sinTubeSegment;
				float n3 = tmp * sinMainSegment * cosTubeSegment - tubeRadius * pitch * invTWO_PI * cosMainSegment * sinTubeSegment;

				Vector3f normal = Vector3f(n1, n2, n3).normalize();
				normals.push_back(normal);

				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}
			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
		}
	}

	if (generateTexels && numRotations > 0) {

		int foo = (repeatTexture) ? uResolution : uResolution * numRotations;
		int offset = (repeatTexture) ? numRotations - 1 : 0;

		float mainSegmentTextureStep = 1.0 / (float(foo));
		float tubeSegmentTextureStep = 1.0f / float(vResolution);

		float currentMainSegmentTexCoordU = 0.0f;
		bool flip = false;

		for (unsigned int i = 0; i <= uResolution * numRotations + offset; i++) {

			if (repeatTexture && i > 0 && i % (uResolution + 1) == 0) {
				flip = !flip;
				currentMainSegmentTexCoordU = flip ? 1.0f : 0.0;
			}
			//rotate the texture to like the meshTorus
			float currentTubeSegmentTexCoordV = 0.0f;
			for (unsigned int j = 0; j <= vResolution; j++) {
				Vector2f textureCoordinate = Vector2f(1.0 - currentMainSegmentTexCoordU, currentTubeSegmentTexCoordV);
				texels.push_back(textureCoordinate);

				currentTubeSegmentTexCoordV += tubeSegmentTextureStep;
			}
			// Update texture coordinate of main segment
			if(flip)
				currentMainSegmentTexCoordU -= mainSegmentTextureStep;
			else
				currentMainSegmentTexCoordU += mainSegmentTextureStep;
		}
	}



	if (generateTangents) {

		float currentMainSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= uResolution * numRotations; i++) {

			// Calculate sine and cosine of main segment angle
			float sinMainSegment = sin(currentMainSegmentAngle);
			float cosMainSegment = cos(currentMainSegmentAngle);

			float currentTubeSegmentAngle = 0.0f;

			if (i > 0 && (uResolution + 1) % i == 0) currentTubeSegmentAngle = 0.0f;

			for (unsigned int j = 0; j <= vResolution; j++) {

				// Calculate sine and cosine of tube segment angle
				float sinTubeSegment = sin(currentTubeSegmentAngle);
				float cosTubeSegment = cos(currentTubeSegmentAngle);

				//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius
				//dp/du 

				//	[t1] = [-(R + rcos(u))sin(v)]
				//	[t2] = [h/(2PI)]
				//	[t3] = [(R + rcos(u))cos(v)]
				float t1 = (radius + tubeRadius*cosTubeSegment)*sinMainSegment;
				float t2 = pitch * invTWO_PI;
				float t3 = (radius + tubeRadius*cosTubeSegment)*cosMainSegment;

				Vector3f tangent = Vector3f(t1, -t2, t3).normalize();
				tangents.push_back(tangent);

				//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius
				//dp/dv 

				//	[bt1] = [-rsin(u)cos(v) )
				//	[bt2] = [rcos(u)]
				//	[bt3] = [-rsin(u)sins(v)]
				Vector3f bitangent = Vector3f(-sinTubeSegment*cosMainSegment, cosTubeSegment, -sinTubeSegment*sinMainSegment);
				bitangents.push_back(bitangent);

				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}

			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
		}
	}

	//calculate the indices
	unsigned int currentVertexOffset = 0;
	for (unsigned int i = 0; i < uResolution * numRotations; i++) {

		for (unsigned int j = 0; j < vResolution; j++) {

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
		currentVertexOffset++;
	}
}

int MeshSpiral::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void MeshSpiral::createBuffer() {
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

void MeshSpiral::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshSpiral::createInstancesStatic(const std::vector<Matrix4f>& modelMTX) {
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

void MeshSpiral::addInstance(const Matrix4f& modelMTX) {
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

void MeshSpiral::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}