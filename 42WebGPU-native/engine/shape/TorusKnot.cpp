#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include "TorusKnot.h"

TorusKnot::TorusKnot(unsigned int uResolution, unsigned int vResolution) : TorusKnot({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.4f, 2u, 3u, true, true, false, uResolution, vResolution) {}

TorusKnot::TorusKnot(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : TorusKnot({ 0.0f, 0.0f, 0.0f}, 1.0f, 0.4f, 2u, 3u, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

TorusKnot::TorusKnot(const std::array<float, 3>& position, float radius, float tubeRadius, unsigned int p, unsigned int q, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

	m_radius = radius;
	m_tubeRadius = tubeRadius;
	m_p = p;
	m_q = q;

	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
	BuildMesh(m_position, m_radius, m_tubeRadius, m_p, m_q,  m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}


TorusKnot::~TorusKnot() {

}

void TorusKnot::BuildMesh(const std::array<float, 3>& position, float radius, float tubeRadius, unsigned int p, unsigned int q, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {

	float mainSegmentAngleStep = 1.0f / float(uResolution);
	float tubeSegmentAngleStep = 1.0f / float(vResolution);

	std::array<float, 3> p1;
	std::array<float, 3> p2;
	std::array<float, 3> p3;

	std::array<float, 3> T, B, N;

	float currentMainSegmentAngle = 0.0f;
	for (unsigned int i = 0; i <= uResolution; ++i) {

		float u = currentMainSegmentAngle * 2.0f * M_PI;
		CalculatePositionOnCurve(u, p, q, radius, radius * 0.5f, p1);
		float uNext = (currentMainSegmentAngle + mainSegmentAngleStep) * 2.0f * M_PI;
		CalculatePositionOnCurve(uNext, p, q, radius, radius * 0.5f, p2);
		CalculatePositionOnCurve(u, p, q, radius, 0.0f, p3);

		T = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
		B = Normalize({ p1[0] - p3[0], p1[1] - p3[1], p1[2] - p3[2] });
		N = Normalize(Cross(T, B));
		T = Normalize(Cross(B, N));

		float currentTubeSegmentAngle = 0.0f;
		for (unsigned int i = 0; i <= vResolution; ++i) {
			float v = currentTubeSegmentAngle * 2.0f * M_PI;

			float cx = tubeRadius * std::sinf(v);
			float cy = tubeRadius * std::cosf(v);

			std::array<float, 3> pos = { p1[0] + (cx * N[0] + cy * B[0]), p1[1] + (cx * N[1] + cy * B[1]), p1[2] + (cx * N[2] + cy * B[2]) };
			std::array<float, 3> surfacePosition = { pos[0] + position[0], pos[1] + position[1], pos[2] + position[2] };
			vertexBuffer.push_back(surfacePosition[0]); vertexBuffer.push_back(surfacePosition[1]); vertexBuffer.push_back(surfacePosition[2]);

			if (generateTexels) {
				vertexBuffer.push_back(currentMainSegmentAngle); vertexBuffer.push_back(1.0f - currentTubeSegmentAngle);
			}

			if (generateNormals) {
				std::array<float, 3> normal = Normalize({ pos[0] - p1[0] , pos[1] - p1[1] , pos[2] - p1[2] });
				vertexBuffer.push_back(normal[0]); vertexBuffer.push_back(normal[1]); vertexBuffer.push_back(normal[2]);
			}

			if (generateTangents) {			
				vertexBuffer.push_back(T[0]); vertexBuffer.push_back(T[1]); vertexBuffer.push_back(T[2]);

				std::array<float, 3> normal = Normalize({ pos[0] - p1[0] , pos[1] - p1[1] , pos[2] - p1[2] });
				std::array<float, 3> bitangent = Cross(T, normal);
				vertexBuffer.push_back(bitangent[0]); vertexBuffer.push_back(bitangent[1]); vertexBuffer.push_back(bitangent[1]);
			}

			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

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

void TorusKnot::CalculatePositionOnCurve(float u, float p, float q, float radius, std::array<float, 3>& position) {
	float cu = std::cosf(u);
	float su = std::sinf(u);
	float quOverP = (q / p) * u;
	float cs = std::cosf(quOverP);

	position[0] = radius * (2.0f + cs) * 0.5f * cu;
	position[1] = radius * (2.0f + cs) * su * 0.5f;
	position[2] = radius * std::sinf(quOverP) * 0.5f;
}

void TorusKnot::CalculatePositionOnCurve(float u, float p, float q, float r1, float r2, std::array<float, 3>& position) {
	position[0] = (r1 + r2 * std::cosf(q * u)) * std::cosf(p * u);
	position[1] = (r1 + r2 * std::cosf(q * u)) * std::sinf(p * u);
	position[2] = -r2 * std::sinf(q * u);
}

std::array<float, 3> TorusKnot::Normalize(const std::array<float, 3>& v) {
	float length = sqrtf((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	float invMag = length != 0.0f ? 1.0f / length : 1.0f;
	return { v[0] * invMag, v[1] * invMag, v[2] * invMag };
}

std::array<float, 3> TorusKnot::Cross(const std::array<float, 3>& p, const std::array<float, 3>& q) {
	return { (p[1] * q[2]) - (p[2] * q[1]),
		     (p[2] * q[0]) - (p[0] * q[2]),
		     (p[0] * q[1]) - (p[1] * q[0]) };
}