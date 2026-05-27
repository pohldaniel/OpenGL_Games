#pragma once

#include <array>
#include <vector>

class Segment {

public:

	Segment(unsigned int uResolution = 0u, unsigned int vResolution = 10u);
	Segment(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 0u, unsigned int vResolution = 10u);
	Segment(const std::array<float, 3>& position, float radius, float startAngle, float endAngle, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 0u, unsigned int vResolution = 10u);
	~Segment();


	static void BuildMeshXY(const std::array<float, 3>& position, float radius, float startAngle, float endAngle, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);
	static void BuildMeshXZ(const std::array<float, 3>& position, float radius, float startAngle, float endAngle, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;

	float m_radius, m_startAngle, m_endAngle;
	std::array<float, 3> m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;
};