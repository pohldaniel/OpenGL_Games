#include "Quad.h"

Quad::Quad(unsigned int uResolution, unsigned int vResolution) : Quad({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f }, true, true, false, uResolution, vResolution) {}

Quad::Quad(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : Quad({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f }, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

Quad::Quad(const std::array<float, 3>& position, const std::array<float, 2>& size, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

	m_position = position;
	m_size = size;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
	m_center = { m_position[0] + m_size[0] * 0.5f, m_position[1] + m_size[1] * 0.5f, 0.0f };

	BuildMeshXY(m_position, m_size, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}

Quad::~Quad() {

}

void Quad::BuildMeshXY(const std::array<float, 3>& _position, const std::array<float, 2>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	float vStep = (1.0f / vResolution) * size[1];
	float uStep = (1.0f / uResolution) * size[0];

	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = i * vStep;
			float z = 0.0f;

			std::array<float, 3> position = { x + _position[0], y + _position[1], z + _position[2] };
			vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);

			if (generateTexels) {
				// Calculate texels on the surface of a quad
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;
				vertexBuffer.push_back(u); vertexBuffer.push_back(v);
			}

			if (generateNormals) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
			}
		}
	}

	//calculate the indices
	for (int z = 0; z < vResolution; z++) {
		for (int x = 0; x < uResolution; x++) {
			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
			//      4		3 -* 4
			indexBuffer.push_back(z * (uResolution + 1) + x);
			indexBuffer.push_back(z * (uResolution + 1) + x + 1);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);

			indexBuffer.push_back(z * (uResolution + 1) + x);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x);

		}
	}
}

void Quad::BuildMeshXZ(const std::array<float, 3>& _position, const std::array<float, 2>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	float vStep = (1.0f / vResolution) * size[1];
	float uStep = (1.0f / uResolution) * size[0];

	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			// Calculate vertex position on the surface of a quad
			float x = j * uStep;
			float y = 0.0f;
			float z = i * vStep;

			std::array<float, 3> position = { x + _position[0], y + _position[1], z + _position[2] };
			vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);

			if (generateTexels) {
				// Calculate texels on the surface of a quad
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;
				vertexBuffer.push_back(u); vertexBuffer.push_back(v);
			}

			if (generateNormals) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f);
			}
		}
	}

	//calculate the indices
	for (int z = 0; z < vResolution; z++) {
		for (int x = 0; x < uResolution; x++) {
			// 0 *- 1		0
			//	\	*		|  *
			//	 *	|		*	\
			//      4		3 -* 4
			indexBuffer.push_back(z * (uResolution + 1) + x);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);
			indexBuffer.push_back(z * (uResolution + 1) + x + 1);

			indexBuffer.push_back((z + 1) * (uResolution + 1) + x);
			indexBuffer.push_back((z + 1) * (uResolution + 1) + x + 1);
			indexBuffer.push_back(z * (uResolution + 1) + x);
		}
	}
}