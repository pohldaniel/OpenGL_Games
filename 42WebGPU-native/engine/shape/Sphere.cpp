#define _USE_MATH_DEFINES
#include <math.h>
#include "Sphere.h"

Sphere::Sphere(unsigned int uResolution, unsigned int vResolution) : Sphere({ 0.0f, 0.0f, 0.0f }, 1.0f, true, true, false, uResolution, vResolution) {}

Sphere::Sphere(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : Sphere({ 0.0f, 0.0f, 0.0f }, 1.0f, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

Sphere::Sphere(const std::array<float, 3>& position, float radius, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

	m_radius = radius;
	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;

	m_min = { FLT_MAX, FLT_MAX, FLT_MAX };
	m_max = { FLT_MIN, FLT_MIN, FLT_MIN };

	BuildMesh(m_position, m_radius, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}

Sphere::~Sphere() {

}

void Sphere::BuildMesh(const std::array<float, 3>& position, float radius, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {


	float uAngleStep = (2.0f * M_PI) / float(uResolution);
	float vAngleStep = M_PI / float(vResolution);
	float vSegmentAngle, uSegmentAngle;

	for (int i = 0; i <= vResolution; ++i) {
		vSegmentAngle = M_PI * 0.5f - i * vAngleStep;

		float cosVSegment = cosf(vSegmentAngle);
		float sinVSegment = sinf(vSegmentAngle);

		for (int j = 0; j <= uResolution; ++j) {
			uSegmentAngle = j * uAngleStep;

			float cosUSegment = cosf(uSegmentAngle);
			float sinUSegment = sinf(uSegmentAngle);
			float x = cosVSegment * cosUSegment;
			float z = cosVSegment * sinUSegment;
			vertexBuffer.push_back(radius * x + position[0]); vertexBuffer.push_back(radius * sinVSegment + position[1]); vertexBuffer.push_back(radius * z + position[2]);

			if (generateTexels) {
				vertexBuffer.push_back(1.0f - (float)j / uResolution); vertexBuffer.push_back(1.0f - (float)i / vResolution);
			}

			if (generateNormals) {
				vertexBuffer.push_back(x); vertexBuffer.push_back(sinVSegment); vertexBuffer.push_back(z);
			}

			if (generateTangents) {
				vertexBuffer.push_back(sinUSegment); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(cosUSegment);
				vertexBuffer.push_back(-sinVSegment * cosUSegment); vertexBuffer.push_back(cosVSegment); vertexBuffer.push_back(-sinVSegment * sinUSegment);
			}
		}
	}

	unsigned int k1, k2;
	for (int i = 0; i < vResolution; ++i) {
		k1 = i * (uResolution + 1);
		k2 = k1 + uResolution + 1;

		for (int j = 0; j < uResolution; ++j, ++k1, ++k2) {

			if (i != 0) {
				indexBuffer.push_back(k1); indexBuffer.push_back(k1 + 1); indexBuffer.push_back(k2);
			}

			if (i != (vResolution - 1)) {
				indexBuffer.push_back(k1 + 1);  indexBuffer.push_back(k2 + 1); indexBuffer.push_back(k2);
			}
		}
	}
}