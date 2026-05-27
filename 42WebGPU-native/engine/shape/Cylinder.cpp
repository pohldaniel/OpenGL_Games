#define _USE_MATH_DEFINES
#include <math.h>
#include "Cylinder.h"

Cylinder::Cylinder(unsigned int uResolution, unsigned int vResolution) : Cylinder({ 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 1.0f, true, true, true, true, false, uResolution, vResolution){}

Cylinder::Cylinder(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : Cylinder({ 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 1.0f, true, true, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

Cylinder::Cylinder(const std::array<float, 3>& position, float baseRadius, float topRadius, float length, bool top, bool bottom, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

	m_position = position;
	m_baseRadius = baseRadius;
	m_topRadius = topRadius;
	m_length = length;

	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;


	BuildMesh(m_position, m_baseRadius, m_topRadius, m_length, top, bottom, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}

Cylinder::~Cylinder() {

}

void Cylinder::BuildMesh(const std::array<float, 3>& position, float baseRadius, float topRadius, float length, bool bottom, bool top, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	float x, y, z;
	float radius;

	float zAngle = atan2(baseRadius - topRadius, length);
	float x0 = cos(zAngle);
	float y0 = sin(zAngle);

	unsigned int stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;

	for (int i = 0; i <= uResolution; ++i) {
		y = -(length * 0.5f) + (float)i / uResolution * length;      // vertex position z
		radius = baseRadius + (float)i / uResolution * (topRadius - baseRadius);     // lerp
		float t = 1.0f - (float)i / uResolution;   // top-to-bottom

		float sectorStep = 2 * M_PI / vResolution;
		float sectorAngle;

		for (int j = 0, k = 0; j <= vResolution; ++j, k += 3) {
			sectorAngle = j * sectorStep;
			x = cosf(sectorAngle);
			z = sinf(sectorAngle);
			vertexBuffer.push_back(x * radius + position[0]); vertexBuffer.push_back(y + position[1]); vertexBuffer.push_back(z * radius + position[2]);

			if (generateTexels) {
				vertexBuffer.push_back(1.0f - (float)j / vResolution); vertexBuffer.push_back(1.0f - t);
			}

			if (generateNormals) {
				vertexBuffer.push_back(x * x0); vertexBuffer.push_back(y0); vertexBuffer.push_back(z * x0);
			}

			if (generateTangents) {
				vertexBuffer.push_back(-z); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(x);
				std::array<float, 3> bitangent = Cross({ -z , 0.0f, x }, { x * x0 , y0, z * x0 });
				vertexBuffer.push_back(bitangent[0]); vertexBuffer.push_back(bitangent[1]); vertexBuffer.push_back(bitangent[2]);
			}
		}
	}

	unsigned int baseVertexIndex = 0u;
	unsigned int topVertexIndex = 0u;

	if (bottom) {
		baseVertexIndex = (unsigned int)vertexBuffer.size() / stride;
		y = -length * 0.5f;

		vertexBuffer.push_back(position[0]); vertexBuffer.push_back(y + position[1]); vertexBuffer.push_back(position[2]);

		if (generateTexels) {
			vertexBuffer.push_back(0.5f); vertexBuffer.push_back(0.5f);
		}

		if (generateNormals) {
			vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f); vertexBuffer.push_back(0.0f);
		}

		if (generateTangents) {
			vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f);
			vertexBuffer.push_back(-1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
		}

		float sectorStep = 2 * M_PI / vResolution;
		float sectorAngle;  // radian

		for (int i = 0; i < vResolution; ++i) {
			sectorAngle = i * sectorStep;
			x = cos(sectorAngle);
			z = sin(sectorAngle);

			vertexBuffer.push_back(x * baseRadius + position[0]); vertexBuffer.push_back(y + position[1]); vertexBuffer.push_back(z * baseRadius + position[2]);

			if (generateTexels) {
				vertexBuffer.push_back(-x * 0.5f + 0.5f); vertexBuffer.push_back(z * 0.5f + 0.5f);
			}

			if (generateNormals) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f); vertexBuffer.push_back(0.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f);
				vertexBuffer.push_back(-1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
			}
		}
	}

	if (top) {
		// remember where the base vertices start
		topVertexIndex = (unsigned int)vertexBuffer.size() / stride;

		// put vertices of top of cylinder
		y = length * 0.5f;

		vertexBuffer.push_back(position[0]); vertexBuffer.push_back(y + position[1]); vertexBuffer.push_back(position[2]);

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

		float sectorStep = 2 * M_PI / vResolution;
		float sectorAngle;  // radian

		for (int i = 0; i < vResolution; ++i) {
			sectorAngle = i * sectorStep;
			x = cos(sectorAngle);
			z = sin(sectorAngle);

			vertexBuffer.push_back(x * topRadius + position[0]); vertexBuffer.push_back(y + position[1]); vertexBuffer.push_back(z * topRadius + position[2]);

			if (generateTexels) {
				vertexBuffer.push_back(x * 0.5f + 0.5f); vertexBuffer.push_back(-z * 0.5f + 0.5f);
			}

			if (generateNormals) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f);
				vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
			}
		}
	}
	// put indices for sides
	unsigned int k1, k2;
	for (int i = 0; i < uResolution; ++i) {
		k1 = i * (vResolution + 1);     // bebinning of current stack
		k2 = k1 + vResolution + 1;      // beginning of next stack

		for (int j = 0; j < vResolution; ++j, ++k1, ++k2) {
			// 2 trianles per sector
			indexBuffer.push_back(k1); indexBuffer.push_back(k2); indexBuffer.push_back(k1 + 1);
			indexBuffer.push_back(k2); indexBuffer.push_back(k2 + 1); indexBuffer.push_back(k1 + 1);
		}
	}

	if (bottom) {
		for (int i = 0, k = baseVertexIndex + 1; i < vResolution; ++i, ++k) {
			if (i < (vResolution - 1)) {
				indexBuffer.push_back(baseVertexIndex);	indexBuffer.push_back(k); indexBuffer.push_back(k + 1);
			}else {
				indexBuffer.push_back(baseVertexIndex);	indexBuffer.push_back(k); indexBuffer.push_back(baseVertexIndex + 1);
			}
		}
	}

	if (top) {
		for (int i = 0, k = topVertexIndex + 1; i < vResolution; ++i, ++k) {
			if (i < (vResolution - 1)) {
				indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(k + 1); indexBuffer.push_back(k);
			}else {
				indexBuffer.push_back(topVertexIndex); indexBuffer.push_back(topVertexIndex + 1); indexBuffer.push_back(k);
			}
		}
	}
}

std::array<float, 3> Cylinder::Cross(const std::array<float, 3>& p, const std::array<float, 3>& q) {
	return { (p[1] * q[2]) - (p[2] * q[1]),
			 (p[2] * q[0]) - (p[0] * q[2]),
			 (p[0] * q[1]) - (p[1] * q[0]) };
}