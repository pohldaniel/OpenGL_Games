#include "Cube.h"

Cube::Cube(unsigned int uResolution, unsigned int vResolution) : Cube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, true, true, false, uResolution, vResolution) {}

Cube::Cube(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) : Cube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, generateTexels, generateNormals, generateTangents, uResolution, vResolution) {}

Cube::Cube(const std::array<float, 3>& position, const std::array<float, 3>& size, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution, unsigned int vResolution) {

	m_position = position;
	m_size = size;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;

	m_uResolution = uResolution;
	m_vResolution = vResolution;

	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
	m_center = { m_position[0] + m_size[0] * 0.5f, m_position[1] + m_size[1] * 0.5f, m_position[2] + m_size[2] * 0.5f};

	BuildMesh4Q(m_position, m_size, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_vertexBuffer, m_indexBuffer);
}

Cube::~Cube() {

}

void Cube::BuildMesh4Q(const std::array<float, 3>& _position, const std::array<float, 3>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer) {
	float vStep = (1.0f / vResolution) * size[1];
	float uStep = (1.0f / uResolution) * size[2];

	// positive X
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = size[0];
			float y = i * vStep;
			float z = j * uStep;

			std::array<float, 3> position = { x + _position[0], y + _position[1], z + _position[2] };
			vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				std::array<float, 2> textureCoordinate = { 1.0f - u, v };
				vertexBuffer.push_back(textureCoordinate[0]); vertexBuffer.push_back(textureCoordinate[1]);
			}

			if (generateNormals) {
				vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f);
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
			}
		}
	}

	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[2];

	// negative X
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = 0.0f;
			float y = i * vStep;
			float z = j * uStep;

			std::array<float, 3> position = { x + _position[0], y + _position[1], z + _position[2] };
			vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				std::array<float, 2> textureCoordinate = { u, v };
				vertexBuffer.push_back(textureCoordinate[0]); vertexBuffer.push_back(textureCoordinate[1]);
			}

			if (generateNormals) {
				vertexBuffer.push_back(-1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f);
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
			}
		}
	}

	vStep = (1.0f / vResolution) * size[2];
	uStep = (1.0f / uResolution) * size[0];

	// positive Y
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = j * uStep;
			float y = size[1];
			float z = i * vStep;

			std::array<float, 3> position = { x + _position[0], y + _position[1], z + _position[2] };
			vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				std::array<float, 2> textureCoordinate = { u, 1.0f - v };
				vertexBuffer.push_back(textureCoordinate[0]); vertexBuffer.push_back(textureCoordinate[1]);
			}

			if (generateNormals) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f);
			}
		}
	}

	vStep = (1.0f / vResolution) * size[2];
	uStep = (1.0f / uResolution) * size[0];

	// negative Y
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = j * uStep;
			float y = 0.0f;
			float z = i * vStep;

			std::array<float, 3> position = { x + _position[0], y + _position[1], z + _position[2] };
			vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				std::array<float, 2> textureCoordinate = { u, v };
				vertexBuffer.push_back(textureCoordinate[0]); vertexBuffer.push_back(textureCoordinate[1]);
			}

			if (generateNormals) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f); vertexBuffer.push_back(0.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f);
			}
		}
	}

	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[0];

	// positive Z
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = j * uStep;
			float y = i * vStep;
			float z = size[2];

			std::array<float, 3> position = { x + _position[0], y + _position[1], z + _position[2] };
			vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				std::array<float, 2> textureCoordinate = { u, v };
				vertexBuffer.push_back(textureCoordinate[0]); vertexBuffer.push_back(textureCoordinate[1]);
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

	vStep = (1.0f / vResolution) * size[1];
	uStep = (1.0f / uResolution) * size[0];

	// negative Z
	for (unsigned int i = 0; i <= vResolution; i++) {
		for (unsigned int j = 0; j <= uResolution; j++) {

			float x = j * uStep;
			float y = i * vStep;
			float z = 0.0f;

			std::array<float, 3> position = { x + _position[0], y + _position[1], z + _position[2] };
			vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);

			if (generateTexels) {
				float u = (float)j / uResolution;
				float v = (float)i / vResolution;

				std::array<float, 2> textureCoordinate = { 1.0f - u, v };
				vertexBuffer.push_back(textureCoordinate[0]); vertexBuffer.push_back(textureCoordinate[1]);
			}

			if (generateNormals) {
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(-1.0f);
			}

			if (generateTangents) {
				vertexBuffer.push_back(-1.0f); vertexBuffer.push_back(0.0f); vertexBuffer.push_back(0.0f);
				vertexBuffer.push_back(0.0f); vertexBuffer.push_back(1.0f); vertexBuffer.push_back(0.0f);
			}
		}
	}

	//calculate the indices
	for (short face = 0; face < 6; face++) {
		int k = (vResolution + 1) * (uResolution + 1) * face;
		for (int v = 0; v < vResolution; v++) {
			for (int u = 0; u < uResolution; u++) {

				if (face == 1 || face == 3 || face == 4) {
					indexBuffer.push_back(k + (v * (uResolution + 1) + u));
					indexBuffer.push_back(k + (v * (uResolution + 1) + u + 1));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u + 1));

					indexBuffer.push_back(k + (v * (uResolution + 1) + u));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u + 1));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u));
				}else {
					indexBuffer.push_back(k + (v * (uResolution + 1) + u + 1));
					indexBuffer.push_back(k + (v * (uResolution + 1) + u));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u + 1));

					indexBuffer.push_back(k + (v * (uResolution + 1) + u));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u));
					indexBuffer.push_back(k + ((v + 1) * (uResolution + 1) + u + 1));
				}
			}
		}
	}
}