#pragma once

#include <array>
#include <vector>

class Torus {

public:

	Torus(unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	Torus(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	Torus(const std::array<float, 3>& position, float radius, float tubeRadius, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	~Torus();

	static void BuildMesh(const std::array<float, 3>& position, float radius, float tubeRadius, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;

	float m_radius, m_tubeRadius;
	std::array<float, 3> m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;
};