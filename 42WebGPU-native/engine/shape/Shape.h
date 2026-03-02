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
	void cleanup();
	void markForDelete();

	void buildCube(const std::array<float, 3>& position = { -1.0f, -1.0f, -1.0f }, const std::array<float, 3>& size = { 2.0f, 2.0f, 2.0f }, unsigned int uResolution = 1, unsigned int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);

	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;

private:

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;

	std::vector<std::array<float,3>> m_positions;
	std::vector<std::array<float,2>> m_texels;
	std::vector<std::array<float,3>> m_normals;
	std::vector<std::array<float,3>> m_tangents;
	std::vector<std::array<float,3>> m_bitangents;

	bool m_markForDelete;
	unsigned int m_stride;
};