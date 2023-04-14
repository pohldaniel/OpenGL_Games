#include <GL/glew.h>
#include <iostream>
#include "Quad.h"

Quad::Quad() {
	m_position = Vector3f(-1.0f, -1.0f, 0.0f);
	m_size = Vector3f(2.0f, 2.0f, 0.0f);
	createBuffer();
}

Quad::Quad(bool flippable, float leftEdge, float rightEdge, float bottomEdge, float topEdge, float sizeX, float sizeY, float offsetX, float offsetY, float sizeTexX, float sizeTexY, short x, short y) {
	
	m_flippable = flippable;

	if (flippable) {
		createBuffer(m_vao, true, leftEdge, rightEdge, bottomEdge, topEdge, sizeX, sizeY, offsetX, offsetY, sizeTexX, sizeTexY,  x, y);
		//createBuffer(m_vaoFlipped, false, shiftX_1, shiftY_1, shiftX_2, shiftY_2, sizeX, sizeY, offsetX, offsetY, sizeTexX, sizeTexY,  1, 0);
		//createBuffer(m_vao, false, shiftX_1, shiftY_1, shiftX_2, shiftY_2, sizeX, sizeY, offsetX, offsetY, sizeTexX, sizeTexY,  0, 0);
	}else {
		createBuffer(m_vao, false, leftEdge, rightEdge, bottomEdge, topEdge, sizeX, sizeY, offsetX, offsetY, sizeTexX, sizeTexY,  x, y);
	}
}

Quad::Quad(const Vector3f& position, const Vector3f& size) {
	m_position = position;
	m_size = size;
	createBuffer();
}

Quad::~Quad() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vaoFlipped) {
		glDeleteVertexArrays(1, &m_vaoFlipped);
		m_vaoFlipped = 0;
	}
	
	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
}

void Quad::createBuffer(unsigned int& vao, bool flippable, float leftEdge, float rightEdge, float bottomEdge, float topEdge, float sizeX, float sizeY, float offsetX, float offsetY, float sizeTexX, float sizeTexY, short x, short y) {
	std::vector<float> vertex;

	if (flippable) {
		vertex.push_back(leftEdge * sizeX); vertex.push_back(bottomEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + y * sizeTexY); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + y * sizeTexY);
		vertex.push_back(leftEdge * sizeX); vertex.push_back(topEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY);
		vertex.push_back(rightEdge * sizeX); vertex.push_back(topEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY);
		vertex.push_back(rightEdge * sizeX); vertex.push_back(bottomEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + y * sizeTexY); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + y * sizeTexY);
	}else {
		
		vertex.push_back(leftEdge * sizeX); vertex.push_back(bottomEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + y * sizeTexY);
		vertex.push_back(leftEdge * sizeX); vertex.push_back(topEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + x * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY);
		vertex.push_back(rightEdge * sizeX); vertex.push_back(topEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + (1 - y) * sizeTexY);
		vertex.push_back(rightEdge * sizeX); vertex.push_back(bottomEdge * sizeY); vertex.push_back(0.0); vertex.push_back(offsetX + (1 - x) * sizeTexX); vertex.push_back(offsetY + y * sizeTexY);
	}

	static const GLushort index[] = {
		0, 2, 1,
		0, 3, 2
	};

	short stride = flippable ? 7 : 5;
	short offset = 3;

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Texture Coordinates
	if (flippable) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
	}else {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
	}
	//Indices
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Quad::createBuffer() {
	std::vector<float> vertex;
	vertex.push_back(m_position[0]);			 vertex.push_back(m_position[1]);			  vertex.push_back(m_position[2] + m_size[2]); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(m_position[0]);			 vertex.push_back(m_position[1] + m_size[1]); vertex.push_back(m_position[2] + m_size[2]); vertex.push_back(0.0f); vertex.push_back(1.0f);
	vertex.push_back(m_position[0] + m_size[0]); vertex.push_back(m_position[1] + m_size[1]); vertex.push_back(m_position[2] + m_size[2]); vertex.push_back(1.0f); vertex.push_back(1.0f);
	vertex.push_back(m_position[0] + m_size[0]); vertex.push_back(m_position[1]);			  vertex.push_back(m_position[2] + m_size[2]); vertex.push_back(1.0f); vertex.push_back(0.0f);

	static const GLushort index[] = {
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
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

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

	vertex.clear();
	vertex.shrink_to_fit();
}

void Quad::createBuffer(const Vector3f& position, const Vector3f& size) {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
	
	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	std::vector<float> vertex;
	vertex.push_back(position[0]);			 vertex.push_back(position[1]);			  vertex.push_back(position[2] + size[2]); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(position[0]);			 vertex.push_back(position[1] + size[1]); vertex.push_back(position[2] + size[2]); vertex.push_back(0.0f); vertex.push_back(1.0f);
	vertex.push_back(position[0] + size[0]); vertex.push_back(position[1] + size[1]); vertex.push_back(position[2] + size[2]); vertex.push_back(1.0f); vertex.push_back(1.0f);
	vertex.push_back(position[0] + size[0]); vertex.push_back(position[1]);			  vertex.push_back(position[2] + size[2]); vertex.push_back(1.0f); vertex.push_back(0.0f);

	static const GLushort index[] = {
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
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

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

	vertex.clear();
	vertex.shrink_to_fit();
}

void Quad::mapBuffer() {
	float data[] = {
		m_position[0],			   m_position[1],			  m_position[2] + m_size[2], 0.0f, 0.0f,
		m_position[0],			   m_position[1] + m_size[1], m_position[2] + m_size[2], 0.0f, 1.0f,
		m_position[0] + m_size[0], m_position[1] + m_size[1], m_position[2] + m_size[2], 1.0f, 1.0f,
		m_position[0] + m_size[0], m_position[1],			  m_position[2] + m_size[2], 1.0f, 0.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), NULL, GL_STATIC_DRAW);

	void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr){
		memcpy(ptr, data, sizeof(data));
		glUnmapBuffer(GL_ARRAY_BUFFER); 
	}
}

void Quad::draw(unsigned int texture, bool array) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, texture);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, 0);
}

void Quad::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void Quad::drawRawInstanced(unsigned int instanceCount) {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, instanceCount);
	glBindVertexArray(0);
}

void Quad::setFlipped(bool flipped) { 
	if (m_flipped == flipped || !m_flippable) return;
	m_flipped = flipped; 

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (m_flippable ? 7 : 5) * sizeof(float), (void*)((m_flipped ? 5 : 3) * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Quad::setPosition(const Vector3f& position) {
	m_position = position;
	mapBuffer();
}

void Quad::setSize(const Vector3f& size) {
	m_size = size;
	mapBuffer();
}

const Vector3f& Quad::getPosition() const {
	return m_position;
}

const Vector3f& Quad::getSize() const {
	return m_size;
}