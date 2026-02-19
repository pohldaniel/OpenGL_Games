#include "Shape.h"

Shape::Shape() : m_markForDelete(false) {}

Shape::Shape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride) : m_markForDelete(false) {
	fromBuffer(vertexBuffer, indexBuffer, stride);
}

void Shape::fromBuffer(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, unsigned int stride) {
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