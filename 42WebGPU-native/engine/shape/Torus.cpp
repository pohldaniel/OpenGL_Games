#define _USE_MATH_DEFINES
#include <math.h>
#include "Torus.h"

Torus::Torus(unsigned int uResolution, unsigned int vResolution) : Torus({ 0.0f, 0.0f, 0.0f }, 0.5f, 0.25f, true, true, false, uResolution, vResolution) {}

Torus::Torus(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : Torus({ 0.0f, 0.0f, 0.0f }, 0.5f, 0.25f, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

Torus::Torus(const std::array<float, 3>& position, float radius, float tubeRadius, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

	m_radius = radius;
	m_tubeRadius = tubeRadius;

	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
	BuildMesh(m_position, m_radius, m_tubeRadius,  m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}

Torus::~Torus() {

}

void Torus::BuildMesh(const std::array<float, 3>& position, float radius, float tubeRadius, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {

	float mainSegmentAngleStep = 1.0f / float(uResolution);
	float tubeSegmentAngleStep = 1.0f / float(vResolution);

	float currentMainSegmentAngle = 0.0f;
	for (unsigned int i = 0; i <= uResolution; i++) {

		// Calculate sine and cosine of main segment angle
		float sinMainSegment = sinf((2.0f * M_PI) * currentMainSegmentAngle);
		float cosMainSegment = cosf((2.0f * M_PI) * currentMainSegmentAngle);
		float currentTubeSegmentAngle = 0.0f;

		for (unsigned int j = 0; j <= vResolution; j++) {

			// Calculate sine and cosine of tube segment angle
			float sinTubeSegment = sinf((2.0f * M_PI) * currentTubeSegmentAngle);
			float cosTubeSegment = cosf((2.0f * M_PI) * currentTubeSegmentAngle);

			// Calculate vertex position on the surface of torus
			float x = (radius + tubeRadius * cosTubeSegment) * cosMainSegment;
			float y = tubeRadius * sinTubeSegment;
			float z = (radius + tubeRadius * cosTubeSegment) * sinMainSegment;

			vertexBuffer.push_back(x + position[0]); vertexBuffer.push_back(y + position[1]); vertexBuffer.push_back(z + position[2]);

			if (generateTexels) {
				vertexBuffer.push_back(1.0f - currentMainSegmentAngle); vertexBuffer.push_back(currentTubeSegmentAngle);
			}

			if (generateNormals) {
				vertexBuffer.push_back(cosMainSegment * cosTubeSegment); vertexBuffer.push_back(sinTubeSegment); vertexBuffer.push_back(sinMainSegment * cosTubeSegment);
			}

			if (generateTangents) {
				vertexBuffer.push_back(sinMainSegment); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(cosMainSegment);
				vertexBuffer.push_back(-sinTubeSegment * cosMainSegment); vertexBuffer.push_back(cosTubeSegment); vertexBuffer.push_back(-sinTubeSegment * sinMainSegment);
			}

			// Update current tube angle
			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

		// Update main segment angle
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
