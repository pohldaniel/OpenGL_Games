#include <GL/glew.h>
#include <assert.h>

#include "index-buffer.hpp"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_count(count)
{
	assert(sizeof(unsigned int) == sizeof(GLuint));

	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer() {}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &m_rendererID);
}

void IndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void IndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::init(const unsigned int* data, unsigned int count) {
	assert(sizeof(unsigned int) == sizeof(GLuint));
	m_count = count;

	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}
