#pragma once

#include <array>
#include <vector>

class Cylinder {

public:

	Cylinder(unsigned int uResolution = 10u, unsigned int vResolution = 10u);
	Cylinder(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 10u, unsigned int vResolution = 10u);
	Cylinder(const std::array<float, 3>& position, float baseRadius, float topRadius, float length, bool top, bool bottom, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	~Cylinder();

	static void BuildMesh(const std::array<float, 3>& position, float baseRadius, float topRadius, float length, bool top, bool bottom, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;

	float m_baseRadius, m_topRadius, m_length;
	std::array<float, 3> m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;

	static std::array<float, 3> Cross(const std::array<float, 3>& p, const std::array<float, 3>& q);
};