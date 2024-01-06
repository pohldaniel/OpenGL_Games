#include <GL/glew.h>

#include "Line.h"

Line::Line() {

}

Line::~Line() {
	cleanup();
}

void Line::cleanup() {

	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo)
		glDeleteBuffers(1, &m_vbo);

	if (m_ibo)
		glDeleteBuffers(1, &m_ibo);

	if (m_vboAdd1)
		glDeleteBuffers(1, &m_vboAdd1);

	if (m_vboAdd2)
		glDeleteBuffers(1, &m_vboAdd2);

	m_positions.clear();
	m_positions.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
}


void Line::addToBuffer(const Vector3f& begin, const Vector3f& end) {
	m_positions.push_back(begin);
	m_positions.push_back(end);
	
	m_indexBuffer.push_back(static_cast<unsigned int>(m_positions.size() - 2));
	m_indexBuffer.push_back(static_cast<unsigned int>(m_positions.size() - 1));
	
	m_drawCount = static_cast<unsigned int>(m_indexBuffer.size());

	if (m_vao) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(Vector3f), &m_positions[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	} else {
		glGenBuffers(1, &m_ibo);
		glGenBuffers(1, &m_vbo);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(Vector3f), &m_positions[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
}

void Line::addVec4Attribute(const std::vector<Vector4f>& values) {
	if (m_vboAdd1) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd1);
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float) * 4, &values[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	} else {

		glGenBuffers(1, &m_vboAdd1);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd1);
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float) * 4, &values[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(0));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Line::addMat4Attribute(unsigned int length, unsigned int divisor, unsigned int usage) {
	
	if (m_vboAdd2) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd2);
		glBufferData(GL_ARRAY_BUFFER, length * sizeof(float) * 4 * 4, NULL, usage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {

		glGenBuffers(1, &m_vboAdd2);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd2);
		glBufferData(GL_ARRAY_BUFFER, length * sizeof(float) * 4 * 4, NULL, usage);

		glEnableVertexAttribArray(10);
		glEnableVertexAttribArray(11);
		glEnableVertexAttribArray(12);
		glEnableVertexAttribArray(13);
		glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(10, divisor);
		glVertexAttribDivisor(11, divisor);
		glVertexAttribDivisor(12, divisor);
		glVertexAttribDivisor(13, divisor);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Line::updateMat4Attribute(const std::vector<Matrix4f>& values) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboAdd2);
	glBufferSubData(GL_ARRAY_BUFFER, 0, values.size() * sizeof(float) * 4 * 4, values[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Line::drawRaw() const {
	if (m_drawCount == 0) return;
	glBindVertexArray(m_vao);
	glDrawElements(GL_LINES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Line::isActive() {
	return m_vao != 0;
}