#pragma once

#include <vector>
#include <engine/Vector.h>

struct BoundingBox {

	void createBuffer();
	void drawRaw() const;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;

	Vector3f position;
	Vector3f size;
};
