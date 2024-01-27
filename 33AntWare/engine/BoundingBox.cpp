#include <GL/glew.h>
#include "BoundingBox.h"

BoundingBox::~BoundingBox() {
	cleanup();
}

void BoundingBox::createBuffer() {
	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);
	vertexBuffer.push_back(position[0] + size[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2]);
	vertexBuffer.push_back(position[0] + size[0]); vertexBuffer.push_back(position[1] + size[1]); vertexBuffer.push_back(position[2]);
	vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1] + size[1]); vertexBuffer.push_back(position[2]);
	vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2] + size[2]);
	vertexBuffer.push_back(position[0] + size[0]); vertexBuffer.push_back(position[1]); vertexBuffer.push_back(position[2] + size[2]);
	vertexBuffer.push_back(position[0] + size[0]); vertexBuffer.push_back(position[1] + size[1]); vertexBuffer.push_back(position[2] + size[2]);
	vertexBuffer.push_back(position[0]); vertexBuffer.push_back(position[1] + size[1]); vertexBuffer.push_back(position[2] + size[2]);

	indexBuffer.push_back(1); indexBuffer.push_back(0); indexBuffer.push_back(2);
	indexBuffer.push_back(3); indexBuffer.push_back(2); indexBuffer.push_back(0);

	indexBuffer.push_back(5); indexBuffer.push_back(1); indexBuffer.push_back(6);
	indexBuffer.push_back(2); indexBuffer.push_back(6); indexBuffer.push_back(1);

	indexBuffer.push_back(6); indexBuffer.push_back(7); indexBuffer.push_back(5);
	indexBuffer.push_back(4); indexBuffer.push_back(5); indexBuffer.push_back(7);

	indexBuffer.push_back(0); indexBuffer.push_back(4); indexBuffer.push_back(3);
	indexBuffer.push_back(7); indexBuffer.push_back(3); indexBuffer.push_back(4);

	indexBuffer.push_back(5); indexBuffer.push_back(4); indexBuffer.push_back(1);
	indexBuffer.push_back(0); indexBuffer.push_back(1); indexBuffer.push_back(4);

	indexBuffer.push_back(2); indexBuffer.push_back(3); indexBuffer.push_back(6);
	indexBuffer.push_back(7); indexBuffer.push_back(6); indexBuffer.push_back(3);

	short stride = 3; short offset = 0;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BoundingBox::drawRaw() const {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void BoundingBox::cleanup() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}
}