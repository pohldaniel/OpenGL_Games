#include "Shape.h"
#include "Cube.h"

Shape::Shape() : m_markForDelete(false), m_stride(0u) {

}

Shape::Shape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride) : m_markForDelete(false) {
	fromBuffer(vertexBuffer, indexBuffer, stride);
}

Shape::Shape(Shape const& rhs) {
	m_positions = rhs.m_positions;
	m_indexBuffer = rhs.m_indexBuffer;
	m_stride = rhs.m_stride;
	m_markForDelete = false;
}

Shape::Shape(Shape&& rhs) noexcept {
	m_positions = rhs.m_positions;
	m_indexBuffer = rhs.m_indexBuffer;
	m_stride = rhs.m_stride;
	m_markForDelete = false;
}

Shape& Shape::operator=(const Shape& rhs) {
	m_positions = rhs.m_positions;
	m_indexBuffer = rhs.m_indexBuffer;
	m_stride = rhs.m_stride;
	m_markForDelete = false;
	return *this;
}

Shape& Shape::operator=(Shape&& rhs) noexcept {
	m_positions = rhs.m_positions;
	m_indexBuffer = rhs.m_indexBuffer;
	m_stride = rhs.m_stride;
	m_markForDelete = false;
	return *this;
}

Shape::~Shape() {
	if (m_markForDelete) {
		cleanup();
	}
}

void Shape::cleanup() {

}

void Shape::markForDelete() {
	m_markForDelete = true;
}

void Shape::fromBuffer(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, unsigned int stride) {
	m_stride = stride;
	if (stride == 3) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back({ vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2] });
		}
	}

	if (stride == 5) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back({ vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2] });
			m_texels.push_back({ vertexBuffer[i + 3], vertexBuffer[i + 4] });
		}
	}

	if (stride == 6) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back({ vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2] });
			m_normals.push_back({ vertexBuffer[i + 3], vertexBuffer[i + 4], vertexBuffer[i + 5] });
		}
	}

	if (stride == 8) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back({ vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2] });
			m_texels.push_back({ vertexBuffer[i + 3], vertexBuffer[i + 4] });
			m_normals.push_back({ vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7] });
		}
	}

	if (stride == 14) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back({ vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2] });
			m_texels.push_back({ vertexBuffer[i + 3], vertexBuffer[i + 4] });
			m_normals.push_back({ vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7] });
			m_tangents.push_back({ vertexBuffer[i + 8], vertexBuffer[i + 9], vertexBuffer[i + 10] });
			m_bitangents.push_back({ vertexBuffer[i + 11], vertexBuffer[i + 12], vertexBuffer[i + 13] });
		}
	}

	m_indexBuffer.reserve(indexBuffer.size());
	std::copy(indexBuffer.begin(), indexBuffer.end(), std::back_inserter(m_indexBuffer));
}

void Shape::buildCube(const std::array<float, 3>& position, const std::array<float, 3>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Cube::BuildMesh4Q(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_vertexBuffer, m_indexBuffer, m_tangents, m_bitangents);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

const std::vector<float>& Shape::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& Shape::getIndexBuffer() const {
	return m_indexBuffer;
}