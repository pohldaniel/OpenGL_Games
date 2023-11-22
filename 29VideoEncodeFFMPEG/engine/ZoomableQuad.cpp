#include <GL/glew.h>
#include <vector>
#include "ZoomableQuad.h"

ZoomableQuad::ZoomableQuad() {
	m_position = Vector3f(-1.0f, -1.0f, 0.0f);
	m_size = Vector2f(2.0f, 2.0f);
	createBuffer();
}

ZoomableQuad::ZoomableQuad(const Vector3f& position, const Vector2f& size) {
	m_position = position;
	m_size = size;
	createBuffer();
}

ZoomableQuad::~ZoomableQuad() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
}

void ZoomableQuad::createBuffer() {
	const float vertices[] = {
		m_position[0], m_position[1],                         m_position[2] , 0.0f, 0.0f,
		m_position[0], m_position[1] + m_size[1],             m_position[2] , 0.0f, 1.0f,
		m_position[0] + m_size[0], m_position[1] + m_size[1], m_position[2] , 1.0f, 1.0f,
		m_position[0] + m_size[0], m_position[1],             m_position[2] , 1.0f, 0.0f,
	};

	const GLushort index[] = {
		0, 2, 1,
		0, 3, 2
	};

	short stride = 5;
	short offset = 3;

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);
}

void ZoomableQuad::mapBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		ptr[0] = m_position[0];              ptr[1] = m_position[1];              ptr[2] = m_position[2];
		ptr[5] = m_position[0];              ptr[6] = m_position[1]  + m_size[1]; ptr[7] = m_position[2];
		ptr[10] = m_position[0] + m_size[0]; ptr[11] = m_position[1] + m_size[1]; ptr[12] = m_position[2];
		ptr[14] = m_position[0] + m_size[0]; ptr[16] = m_position[1];             ptr[17] = m_position[2];
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ZoomableQuad::mapBuffer(float offsetX, float offsetY, float width, float height) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		ptr[3] = offsetX; ptr[4] = offsetY;
		ptr[8] = offsetX; ptr[9] = height + offsetY;
		ptr[13] = width + offsetX; ptr[14] = height + offsetY;
		ptr[18] = width + offsetX; ptr[19] = offsetY;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ZoomableQuad::mapBuffer(float zoom, float originX, float originY) {

	float left = originX - 0.5f / m_zoom;
	float right = originX + 0.5f / m_zoom;
	float bottom = originY - 0.5f / m_zoom;
	float top = originY + 0.5f / m_zoom;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		ptr[3] = left; ptr[4] = bottom;
		ptr[8] = left; ptr[9] = top;
		ptr[13] = right; ptr[14] = top;
		ptr[18] = right; ptr[19] = bottom;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ZoomableQuad::setZoom(float zoom, float originX, float originY) {
	m_zoom = zoom;
	m_zoom = Math::Clamp(m_zoom, 1.0f, 7.0f);
	mapBuffer(m_zoom, originX, originY);
}

const float ZoomableQuad::getZoom() const {
	return m_zoom;
}

void ZoomableQuad::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void ZoomableQuad::setPosition(const Vector3f& position) {
	m_position = position;
	mapBuffer();
}

void ZoomableQuad::setSize(const Vector2f& size) {
	m_size = size;
	mapBuffer();
}

const Vector3f& ZoomableQuad::getPosition() const {
	return m_position;
}

const Vector2f& ZoomableQuad::getSize() const {
	return m_size;
}