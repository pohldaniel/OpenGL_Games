#define _USE_MATH_DEFINES
#include <math.h>
#include "Spiral.h"

Spiral::Spiral(unsigned int uResolution, unsigned int vResolution) : Spiral({ 0.0f, -0.75f, 0.0f }, 0.5f, 0.25f, 2, 1.5f, true, true, true, false, uResolution, vResolution) {}

Spiral::Spiral(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : Spiral({0.0f, -0.75f, 0.0f }, 0.5f, 0.25f, 2, 1.5f, true, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

Spiral::Spiral(const std::array<float, 3>& position, float radius, float tubeRadius, int numRotations, float length, bool repeatTexture, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

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

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;

	BuildMesh(m_position, m_radius, m_tubeRadius, m_length, m_numRotations, m_repeatTexture, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}

Spiral::~Spiral() {

}

void Spiral::BuildMesh(const std::array<float, 3>& position, float radius, float tubeRadius, float length, unsigned int numRotations, bool repeatTexture, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	std::vector<float> positions;
	std::vector<float> texels;
	std::vector<float> normals;
	std::vector<float> tangents;
	std::vector<float> bitangents;


	float mainSegmentAngleStep = (2.0f * M_PI) / float(uResolution);
	float tubeSegmentAngleStep = (2.0f * M_PI) / float(vResolution);
	float invTwoPi = 1.0f / (2.0f * M_PI);
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
			float x = (radius + tubeRadius * cosTubeSegment) * cosMainSegment;
			float z = (radius + tubeRadius * cosTubeSegment) * sinMainSegment;
			float y = pitch * (currentMainSegmentAngle * invTwoPi) + tubeRadius * sinTubeSegment;

			positions.push_back(x + position[0]); positions.push_back(y + position[1]); positions.push_back(z + position[2]);
			// Update current tube angle
			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

		// Update main segment angle
		currentMainSegmentAngle += mainSegmentAngleStep;
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
				texels.push_back(1.0f - currentMainSegmentTexCoordU); texels.push_back(currentTubeSegmentTexCoordV);
				currentTubeSegmentTexCoordV += tubeSegmentTextureStep;
			}
			// Update texture coordinate of main segment
			if (flip)
				currentMainSegmentTexCoordU -= mainSegmentTextureStep;
			else
				currentMainSegmentTexCoordU += mainSegmentTextureStep;
		}
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
				float n1 = tmp * cosMainSegment * cosTubeSegment + tubeRadius * pitch * invTwoPi * sinMainSegment * sinTubeSegment;
				float n2 = tmp * sinTubeSegment;
				float n3 = tmp * sinMainSegment * cosTubeSegment - tubeRadius * pitch * invTwoPi * cosMainSegment * sinTubeSegment;

				std::array<float, 3> normal = Normalize({ n1, n2, n3 });
				normals.push_back(normal[0]); normals.push_back(normal[1]); normals.push_back(normal[2]);
				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}
			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
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
				float t1 = (radius + tubeRadius * cosTubeSegment) * sinMainSegment;
				float t2 = pitch * invTwoPi;
				float t3 = (radius + tubeRadius * cosTubeSegment) * cosMainSegment;

				std::array<float, 3> tangent = Normalize({ t1, -t2, t3 });
				tangents.push_back(tangent[0]); tangents.push_back(tangent[1]); tangents.push_back(tangent[2]);
				//u = mainSegmentAngle	v = tubeSegmentAngle	R = m_radius	r = m_tubeRadius
				//dp/dv 

				//	[bt1] = [-rsin(u)cos(v) )
				//	[bt2] = [rcos(u)]
				//	[bt3] = [-rsin(u)sins(v)]
				bitangents.push_back(-sinTubeSegment * cosMainSegment); bitangents.push_back(cosTubeSegment); bitangents.push_back(-sinTubeSegment * sinMainSegment);
				// Update current tube angle
				currentTubeSegmentAngle += tubeSegmentAngleStep;
			}

			// Update main segment angle
			currentMainSegmentAngle += mainSegmentAngleStep;
		}
	}

	for (size_t i = 0u; i < positions.size() / 3u; i++) {
		vertexBuffer.push_back(positions[3 * i]); vertexBuffer.push_back(positions[3 * i + 1]); vertexBuffer.push_back(positions[3 * i + 2]);
		if(!texels.empty())
			vertexBuffer.push_back(texels[2 * i]); vertexBuffer.push_back(texels[2 * i + 1]);

		if (!normals.empty())
			vertexBuffer.push_back(normals[3 * i]); vertexBuffer.push_back(normals[3 * i + 1]); vertexBuffer.push_back(normals[3 * i + 2]);

		if (!tangents.empty()) {
			vertexBuffer.push_back(tangents[3 * i]); vertexBuffer.push_back(tangents[3 * i + 1]); vertexBuffer.push_back(tangents[3 * i + 2]);
			vertexBuffer.push_back(bitangents[3 * i]); vertexBuffer.push_back(bitangents[3 * i + 1]); vertexBuffer.push_back(bitangents[3 * i + 2]);
		}
	}

	//calculate the indices
	unsigned int currentVertexOffset = 0;
	for (unsigned int i = 0; i < uResolution * numRotations; i++) {

		for (unsigned int j = 0; j < vResolution; j++) {

			unsigned int vertexIndexA, vertexIndexB, vertexIndexC, vertexIndexD, vertexIndexE, vertexIndexF;

			if ((j > 0) && ((j + 1) % (vResolution + 1)) == 0) {
				currentVertexOffset = ((i + 1) * (vResolution + 1));
			}
			else {

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

std::array<float, 3> Spiral::Normalize(const std::array<float, 3>& v) {
	float length = sqrtf((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	float invMag = length != 0.0f ? 1.0f / length : 1.0f;
	return { v[0] * invMag, v[1] * invMag, v[2] * invMag };
}