#define _USE_MATH_DEFINES
#include <math.h>
#include "Segment.h"

Segment::Segment(unsigned int uResolution, unsigned  int vResolution) : Segment({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f, 360.0f, true, true, false, uResolution, vResolution) {}

Segment::Segment(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : Segment({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f, 2.0f * M_PI, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

Segment::Segment(const std::array<float, 3>& position, float radius, float startAngle, float endAngle, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

	m_position = position;
	m_radius = radius;
	m_startAngle = startAngle;
	m_endAngle = endAngle;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;

	BuildMeshXZ(m_position, m_radius, 0.0f, 360.0f, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}

Segment::~Segment() {

}

void Segment::BuildMeshXY(const std::array<float, 3>& position, float radius, float startAngle, float endAngle, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	float x = 0.0f, z = 0.0f;
	unsigned int topVertexIndex = 0u;
	float piOn180 = M_PI / 180.0f;

	vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);
	
	if (generateTexels) {
		vertexBuffer.push_back(0.5f); vertexBuffer.push_back(0.5f);
	}

	if (generateNormals) {
		vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f);
	}

	if (generateTangents) {
		vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
		vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
	}

	float startRadians = startAngle * piOn180;
	float sectorStep = (endAngle * piOn180 - startRadians) / vResolution;
	float sectorAngle;

	for (int i = 0; i <= vResolution; ++i) {
		sectorAngle = i * sectorStep + startRadians;
		x = cosf(sectorAngle);
		z = sinf(sectorAngle);

		vertexBuffer.push_back(x * radius + position[0]); vertexBuffer.push_back(z * radius + position[1]); vertexBuffer.push_back(position[2]);

		if (generateTexels) {
			vertexBuffer.push_back(x * 0.5f + 0.5f); vertexBuffer.push_back(z * 0.5f + 0.5f);
		}

		if (generateNormals) {
			vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f);
		}

		if (generateTangents) {
			vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
			vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
		}
	}

	for (int i = 0, k = topVertexIndex + 1; i <= vResolution; ++i, ++k) {
		indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(k + 1); indexBuffer.push_back(k);
	}
}

void Segment::BuildMeshXZ(const std::array<float, 3>& position, float radius, float startAngle, float endAngle, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	float x = 0.0f, z = 0.0f;
	unsigned int topVertexIndex = 0u;
	float piOn180 = M_PI / 180.0f;

	vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);
	if (generateTexels) {
		vertexBuffer.push_back(0.5f); vertexBuffer.push_back(0.5f);
	}

	if (generateNormals) {
		vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
	}

	if (generateTangents) {
		vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f);
		vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
	}

	float startRadians = startAngle * piOn180;
	float sectorStep = (endAngle * piOn180 - startRadians) / vResolution;
	float sectorAngle;

	for (int i = 0; i <= vResolution; ++i) {
		sectorAngle = i * sectorStep + startRadians;
		x = cosf(sectorAngle);
		z = sinf(sectorAngle);

		vertexBuffer.push_back(x * radius + position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(z * radius + position[2]);

		if (generateTexels) {
			vertexBuffer.push_back(0.5f + x * 0.5f); vertexBuffer.push_back(0.5f - z * 0.5f);
		}

		if (generateNormals) {
			vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
		}

		if (generateTangents) {
			vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f);
			vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
		}
	}

	for (int i = 0, k = topVertexIndex + 1; i <= vResolution; ++i, ++k) {
		indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(k + 1); indexBuffer.push_back(k);
	}
}