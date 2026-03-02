#pragma once

#include <array>
#include <vector>

class Cube {

public:

	Cube(unsigned int uResolution = 1u, unsigned int vResolution = 1u);
	Cube(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 1u, unsigned int vResolution = 1u);
	Cube(const std::array<float, 3>& position, const std::array<float, 3>& size, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 1u, unsigned int vResolution = 1u);
	~Cube();

	static void BuildMesh4Q(const std::array<float, 3>& position, const std::array<float, 3>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<std::array<float, 3>>& positions, std::vector<std::array<float, 2>>& texels, std::vector<std::array<float, 3>>& normals, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, std::vector<std::array<float, 3>>& tangents, std::vector<std::array<float, 3>>& bitangents);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;

	std::array<float, 3> m_position;
	std::array<float, 3> m_size;
	std::array<float, 3> m_center;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;

	std::vector<std::array<float, 3>> m_positions;
	std::vector<std::array<float, 2>> m_texels;
	std::vector<std::array<float, 3>> m_normals;
	std::vector<std::array<float, 3>> m_tangents;
	std::vector<std::array<float, 3>> m_bitangents;
};