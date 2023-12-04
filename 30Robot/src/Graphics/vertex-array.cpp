#include "vertex-array.hpp"

#include <GL/glew.h>

VertexArray::VertexArray() {
	glGenVertexArrays(1, &m_rendererID);
}

VertexArray::~VertexArray() {
	//GLCall(glDeleteVertexArrays(1, &m_rendererID));  // Deleted in sprite or primitive factory
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
	bind();
	vb.bind();
	const auto& elements = layout.getElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*) offset);
		offset += element.count * VertexBufferElement::getSizeOfType(element.type);
	}
	vb.unbind();
	unbind();
}

void VertexArray::bind() const {
	glBindVertexArray(m_rendererID);
}

void VertexArray::unbind() const {
	glBindVertexArray(0);
}
