#pragma once

#include <vector>
#include <array>
#include <iterator>

class Shape {

public:

	Shape();
	Shape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride);
	Shape(Shape const& rhs);
	Shape(Shape&& rhs) noexcept;
	Shape& operator=(const Shape& rhs);
	Shape& operator=(Shape&& rhs) noexcept;
	~Shape();

	void fromBuffer(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, unsigned int stride);

	void buildCube(const std::array<float, 3>& position = { -1.0f, -1.0f, -1.0f }, const std::array<float, 3>& size = { 2.0f, 2.0f, 2.0f }, unsigned int uResolution = 1u, unsigned int vResolution = 1u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildSphere(const std::array<float, 3>& position = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f, float randomness = 0.0f, unsigned int uResolution = 49u, unsigned int vResolution = 49u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildTorus(const std::array<float, 3>& position = { 0.0f, 0.0f, 0.0f }, float radius = 0.5f, float tubeRadius = 0.25f, unsigned int uResolution = 49u, unsigned int vResolution = 49u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildTorusKnot(const std::array<float, 3>& position = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f, float tubeRadius = 0.4f, unsigned int p = 2u, unsigned int q = 3u, unsigned int uResolution = 100u, unsigned int vResolution = 16u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildSpiral(const std::array<float, 3>& position = { 0.0f, -0.75f, 0.0f }, float radius = 0.5f, float tubeRadius = 0.25f, float length = 1.5f, unsigned int numRotations = 2u, bool repeatTexture = true, unsigned int uResolution = 49u, unsigned int vResolution = 49u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildQuadXY(const  std::array<float, 3>& position = { -1.0f, -1.0f, 0.0f }, const std::array<float, 2>& size = { 2.0f, 2.0f }, unsigned int uResolution = 1u, unsigned int vResolution = 1u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildQuadXZ(const  std::array<float, 3>& position = { -1.0f, 0.0f, -1.0f }, const std::array<float, 2>& size = { 2.0f, 2.0f }, unsigned int uResolution = 1u, unsigned int vResolution = 1u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildCylinder(const std::array<float, 3>& position = { 0.0f, 0.0f, 0.0f }, float baseRadius = 1.0f, float topRadius = 1.0f, float length = 1.0f, bool bottom = true, bool top = true, unsigned int uResolution = 10u, unsigned int vResolution = 10u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildCapsule(const std::array<float, 3>& position = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f, float length = 1.0f, unsigned int uResolution = 20u, unsigned int vResolution = 20u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildSegmentXY(const std::array<float, 3>& position = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f, float startAngle = 0.0f, float endAngle = 360.0f, unsigned int uResolution = 0u, unsigned int vResolution = 10u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildSegmentXZ(const std::array<float, 3>& position = { 0.0f, 0.0f, 0.0f }, float radius = 1.0f, float startAngle = 0.0f, float endAngle = 360.0f, unsigned int uResolution = 0u, unsigned int vResolution = 10u, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);

	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	const unsigned int getStride() const;
	void rewind();
	void flatShading();

private:

	void Rewind(std::vector<unsigned int>& indexBuffer);

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;
	unsigned int m_stride;

	static std::array<float, 3> Normalize(const std::array<float, 3>& v);
	static std::array<float, 3> Cross(const std::array<float, 3>& p, const std::array<float, 3>& q);
};