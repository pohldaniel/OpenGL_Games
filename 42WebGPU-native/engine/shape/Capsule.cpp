#define _USE_MATH_DEFINES
#include <math.h>
#include "Capsule.h"

Capsule::Capsule(unsigned int uResolution, unsigned int vResolution) : Capsule({ 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, true, true, false, uResolution, vResolution) {}

Capsule::Capsule(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : Capsule({ 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

Capsule::Capsule(const std::array<float, 3>& position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

	m_radius = radius;
	m_length = length;
	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;

	BuildMesh(m_position, m_radius, m_length, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}

void Capsule::BuildMesh(const std::array<float, 3>& position, float radius, float length,  int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	BuildHemisphere({position[0], position[1] + length * 0.5f,  position[1] }, radius, length, true, uResolution, vResolution, generateTexels, generateNormals, generateTangents, vertexBuffer, indexBuffer);
	if (length != 0)
		BuildCylinder(position, radius, length, uResolution, vResolution, generateTexels, generateNormals, generateTangents, vertexBuffer, indexBuffer);
	BuildHemisphere({ position[0], position[1] - length * 0.5f,  position[1] }, radius, length, false, uResolution, vResolution, generateTexels, generateNormals, generateTangents, vertexBuffer, indexBuffer);
}

void Capsule::BuildHemisphere(const std::array<float, 3>& position, float radius, float length, bool north, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	float uAngleStep = (2.0f * M_PI) / float(uResolution);
	float vAngleStep = (0.5f * M_PI) / float(vResolution);
	float vSegmentAngle, uSegmentAngle;
	unsigned int stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
	unsigned int baseIndex = (unsigned int)vertexBuffer.size() / stride;

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
	
			vertexBuffer.push_back(radius * x + position[0]); vertexBuffer.push_back(north ? radius * y + position[1] : -radius * y + position[1]); vertexBuffer.push_back(radius * z + position[2]);

			if (generateTexels) {
				vertexBuffer.push_back(1.0f - (float)j / uResolution); vertexBuffer.push_back(north ? (float)i / vResolution : 1.0f - (float)i / vResolution);
			}

			if (generateNormals) {
				vertexBuffer.push_back(x); vertexBuffer.push_back(north ? y : -y); vertexBuffer.push_back(z);
			}

			if (generateTangents) {
				vertexBuffer.push_back(sinUSegment); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(cosUSegment);
				vertexBuffer.push_back(-sinVSegment * cosUSegment); vertexBuffer.push_back(cosVSegment); vertexBuffer.push_back(-sinVSegment * sinUSegment);
			}

			//improvte texture mapping
			//if (i == vResolution) {
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
			}else {
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
		//for (unsigned int j = 0; j < uResolution; j++) {
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

void Capsule::BuildCylinder(const std::array<float, 3>& position, float radius, float length, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	unsigned int stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
	unsigned int baseIndex = (unsigned int)vertexBuffer.size() / stride;
	float x, y, z;


	for (int i = 0; i <= vResolution; ++i) {
		y = -(length * 0.5f) + (float)i / vResolution * length;
		float t = 1.0f - (float)i / vResolution;

		float sectorStep = 2 * M_PI / uResolution;
		float sectorAngle;

		for (int j = 0, k = 0; j <= uResolution; ++j, k += 3) {
			sectorAngle = j * sectorStep;
			x = cos(sectorAngle);
			z = sin(sectorAngle);

			vertexBuffer.push_back(x * radius + position[0]); vertexBuffer.push_back(y + position[1]); vertexBuffer.push_back(z * radius + position[2]);

			if (generateTexels) {
				vertexBuffer.push_back(1.0f - (float)j / vResolution); vertexBuffer.push_back(1.0f - t);
			}

			if (generateNormals) {
				vertexBuffer.push_back(x); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(z);
			}
			
			if (generateTangents) {
				vertexBuffer.push_back(-z); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(x);
				std::array<float, 3> bitangent = Cross({-z, 0.0f, x}, {x, 0.0f, z});
				vertexBuffer.push_back(bitangent[0]); vertexBuffer.push_back(bitangent[1]); vertexBuffer.push_back(bitangent[2]);
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

std::array<float, 3> Capsule::Cross(const std::array<float, 3>& p, const std::array<float, 3>& q) {
	return { (p[1] * q[2]) - (p[2] * q[1]),
			 (p[2] * q[0]) - (p[0] * q[2]),
			 (p[0] * q[1]) - (p[1] * q[0]) };
}