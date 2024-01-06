#include <GL/glew.h>
#include "BoundingBox.h"

void BoundingBox::createBuffer() {
	m_vertexBuffer.push_back(position[0]); m_vertexBuffer.push_back(position[1]); m_vertexBuffer.push_back(position[2]);
	m_vertexBuffer.push_back(position[0] + size[0]); m_vertexBuffer.push_back(position[1]); m_vertexBuffer.push_back(position[2]);
	m_vertexBuffer.push_back(position[0] + size[0]); m_vertexBuffer.push_back(position[1] + size[1]); m_vertexBuffer.push_back(position[2]);
	m_vertexBuffer.push_back(position[0]); m_vertexBuffer.push_back(position[1] + size[1]); m_vertexBuffer.push_back(position[2]);
	m_vertexBuffer.push_back(position[0]); m_vertexBuffer.push_back(position[1]); m_vertexBuffer.push_back(position[2] + size[2]);
	m_vertexBuffer.push_back(position[0] + size[0]); m_vertexBuffer.push_back(position[1]); m_vertexBuffer.push_back(position[2] + size[2]);
	m_vertexBuffer.push_back(position[0] + size[0]); m_vertexBuffer.push_back(position[1] + size[1]); m_vertexBuffer.push_back(position[2] + size[2]);
	m_vertexBuffer.push_back(position[0]); m_vertexBuffer.push_back(position[1] + size[1]); m_vertexBuffer.push_back(position[2] + size[2]);

	m_indexBuffer.push_back(1); m_indexBuffer.push_back(0); m_indexBuffer.push_back(2);
	m_indexBuffer.push_back(3); m_indexBuffer.push_back(2); m_indexBuffer.push_back(0);

	m_indexBuffer.push_back(5); m_indexBuffer.push_back(1); m_indexBuffer.push_back(6);
	m_indexBuffer.push_back(2); m_indexBuffer.push_back(6); m_indexBuffer.push_back(1);

	m_indexBuffer.push_back(6); m_indexBuffer.push_back(7); m_indexBuffer.push_back(5);
	m_indexBuffer.push_back(4); m_indexBuffer.push_back(5); m_indexBuffer.push_back(7);

	m_indexBuffer.push_back(0); m_indexBuffer.push_back(4); m_indexBuffer.push_back(3);
	m_indexBuffer.push_back(7); m_indexBuffer.push_back(3); m_indexBuffer.push_back(4);

	m_indexBuffer.push_back(5); m_indexBuffer.push_back(4); m_indexBuffer.push_back(1);
	m_indexBuffer.push_back(0); m_indexBuffer.push_back(1); m_indexBuffer.push_back(4);

	m_indexBuffer.push_back(2); m_indexBuffer.push_back(3); m_indexBuffer.push_back(6);
	m_indexBuffer.push_back(7); m_indexBuffer.push_back(6); m_indexBuffer.push_back(3);

	short stride = 3; short offset = 0;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.size() * sizeof(float), &m_vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BoundingBox::drawRaw() const {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}