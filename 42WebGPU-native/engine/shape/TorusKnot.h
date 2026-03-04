#pragma once

#include <array>
#include <vector>

class TorusKnot {

public:

	TorusKnot(unsigned int uResolution = 100u, unsigned int vResolution = 16u);
	TorusKnot(bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 100u, unsigned int vResolution = 16u);
	TorusKnot(const std::array<float, 3>& position, float radius, float tubeRadius, unsigned int p, unsigned int q, bool generateTexels, bool generateNormals, bool generateTangents, unsigned int uResolution = 100u, unsigned int vResolution = 16u);
	~TorusKnot();


	static void BuildMesh(const std::array<float, 3>& position,  float radius, float tubeRadius, unsigned int p, unsigned int q, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);

private:

	unsigned int m_uResolution;
	unsigned int m_vResolution;
	float m_radius;
	float m_tubeRadius;
	unsigned int m_p, m_q;
	std::array<float, 3> m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	unsigned int m_stride;
	

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;


	static void CalculatePositionOnCurve(float u, float p, float q, float radius, std::array<float, 3>& position);
	static void CalculatePositionOnCurve(float u, float p, float q, float r1, float r2, std::array<float, 3>& position);
	static std::array<float, 3> Normalize(const std::array<float, 3>& v);
	static std::array<float, 3> Cross(const std::array<float, 3>& p, const std::array<float, 3>& q);
};