#include <GL/glew.h>
#include "SlicedCube.h"

SlicedCube::SlicedCube() {

}

SlicedCube::SlicedCube(int width, int height, int depth)  {
	create(width, height, depth);
}

SlicedCube::~SlicedCube() {
	cleanup();
}

int SlicedCube::getWidth() {
	return m_width;
}

int SlicedCube::getHeight() {
	return m_height;
}

int SlicedCube::getDepth() {
	return m_depth;
}

unsigned int& SlicedCube::getVao() {
	return m_vao;
}

void SlicedCube::drawRaw() {

	glBindVertexArray(m_vao);
	for (int z = 0; z < m_depth; z++) {
		glDrawArrays(GL_QUADS, z * 4, 4);
	}
	glBindVertexArray(0);
}

void SlicedCube::cleanup() {

	if (m_vao) 
		glDeleteVertexArrays(1, &m_vao);
	
	if (m_vbo) 
		glDeleteBuffers(1, &m_vbo);
	
}

void SlicedCube::create(int width, int height, int depth) {

	m_width = width;
	m_height = height;
	m_depth = depth;

	std::vector <float> vertex;
	for (int z = 0; z < m_depth; z++) {

		vertex.push_back(-1.0), vertex.push_back(-1.0), vertex.push_back((2.0f * z / (float)(m_depth - 1)) - 1.0f);
		vertex.push_back(0.0), vertex.push_back(0.0), vertex.push_back(z / (float)(m_depth - 1));

		vertex.push_back(1.0), vertex.push_back(-1.0), vertex.push_back((2.0f * z / (float)(m_depth - 1)) - 1.0f);
		vertex.push_back(1.0), vertex.push_back(0.0), vertex.push_back(z / (float)(m_depth - 1));

		vertex.push_back(1.0), vertex.push_back(1.0), vertex.push_back((2.0f * z / (float)(m_depth - 1)) - 1.0f);
		vertex.push_back(1.0), vertex.push_back(1.0), vertex.push_back(z / (float)(m_depth - 1));

		vertex.push_back(-1.0), vertex.push_back(1.0), vertex.push_back((2.0f * z / (float)(m_depth - 1)) - 1.0f);
		vertex.push_back(0.0), vertex.push_back(1.0), vertex.push_back(z / (float)(m_depth - 1));
	}

	short stride = 6;

	if (m_vbo) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}else {
		glGenBuffers(1, &m_vbo);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

		//Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

		//TexutreCoords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	vertex.clear();
	vertex.shrink_to_fit();
}