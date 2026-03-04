#pragma once

#include <array>
#include <vector>

class Spiral {

public:

	Spiral(unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	Spiral(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	Spiral(const std::array<float, 3>& position, float radius, float tubeRadius, int numRotations, float length, bool repeatTexture, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	~Spiral();

	static void BuildMesh(const std::array<float, 3>& position, float radius, float tubeRadius, float length, unsigned int numRotations, bool repeatTexture, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;

	float m_radius;
	float m_tubeRadius;
	float m_length;
	unsigned int m_numRotations;
	bool m_repeatTexture;

	std::array<float, 3> m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;

	static std::array<float, 3> Normalize(const std::array<float, 3>& v);
};