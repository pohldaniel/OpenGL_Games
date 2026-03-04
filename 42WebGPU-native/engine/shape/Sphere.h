#pragma once

#include <array>
#include <vector>

class Sphere {

public:

	Sphere(unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	Sphere(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	Sphere(const std::array<float, 3>& position, float radius, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 49u, unsigned int vResolution = 49u);
	~Sphere();

	static void BuildMesh(const std::array<float, 3>& position, float radius , int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;
	
	std::array<float, 3> m_position;
	float m_radius;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;

	std::array<float, 3> m_min;
	std::array<float, 3> m_max;
};