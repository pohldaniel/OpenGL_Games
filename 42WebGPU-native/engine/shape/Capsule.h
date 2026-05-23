#pragma once

#include <array>
#include <vector>

class Capsule {

public:

	Capsule(unsigned int uResolution = 20u, unsigned int vResolution = 20u);
	Capsule(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 20u, unsigned int vResolution = 20u);
	Capsule(const std::array<float, 3>& position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 20u, unsigned int vResolution = 20u);
	~Capsule();

	static void BuildMesh(const std::array<float, 3>& position, float radius, float length, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;

	float m_radius, m_length;
	std::array<float, 3> m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;

	static void BuildHemisphere(const std::array<float, 3>& position, float radius, float length, bool north, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);
	static void BuildCylinder(const std::array<float, 3>& position, float radius, float length, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);
	static std::array<float, 3> Cross(const std::array<float, 3>& p, const std::array<float, 3>& q);
};