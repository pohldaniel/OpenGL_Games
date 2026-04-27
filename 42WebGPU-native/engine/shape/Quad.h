#pragma once

#include <array>
#include <vector>

class Quad {

public:

	Quad(unsigned int uResolution = 1u, unsigned int vResolution = 1u);
	Quad(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 1u, unsigned int vResolution = 1u);
	Quad(const std::array<float, 3>& position, const std::array<float, 2>& size, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 1u, unsigned int vResolution = 1u);
	~Quad();

	static void BuildMeshXY(const std::array<float, 3>& position, const std::array<float, 2>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);
	static void BuildMeshXZ(const std::array<float, 3>& position, const std::array<float, 2>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;

	std::array<float, 3> m_position;
	std::array<float, 2> m_size;
	std::array<float, 3> m_center;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;
};