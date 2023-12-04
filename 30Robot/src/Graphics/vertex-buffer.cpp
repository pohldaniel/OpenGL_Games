#include "vertex-buffer.hpp"

VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum usage) {
	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

VertexBuffer::~VertexBuffer() {
	// It is ok to delete a vertex buffer after binding it to vertex array
	// The buffer will not be deleted by opengl until it is no longer in use
	glDeleteBuffers(1, &m_rendererID);
}

void VertexBuffer::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void VertexBuffer::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
