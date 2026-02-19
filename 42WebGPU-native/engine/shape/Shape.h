#pragma once

#include <vector>
#include <array>
#include <iterator>

class Shape {

public:

	Shape();
	Shape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride);
	Shape(Shape const& rhs);
	Shape(Shape&& rhs);
	Shape& operator=(const Shape& rhs);
	Shape& operator=(Shape&& rhs);
	~Shape();

	void fromBuffer(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, unsigned int stride);

private:

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;

	std::vector<std::array<float,3>> m_positions;
	std::vector<std::array<float,2>> m_texels;
	std::vector<std::array<float,3>> m_normals;
	std::vector<std::array<float,3>> m_tangents;
	std::vector<std::array<float,3>> m_bitangents;

	bool m_markForDelete;
};