#include "Quad.h"

Quad::Quad(bool flippable, float sizeX, float sizeY, float sizeTexX, float sizeTexY) {
	
	m_flippable = flippable;

	if (flippable) {
		createBuffer(m_vao, true, sizeX, sizeY, sizeTexX, sizeTexY, 0, 0);
		//createBuffer(m_vaoFlipped, false, sizeX, sizeY, sizeTexX, sizeTexY, 1, 0);
		//createBuffer(m_vao, false, sizeX, sizeY, sizeTexX, sizeTexY, 0, 0);
	}else {
		createBuffer(m_vao, false, sizeX, sizeY, sizeTexX, sizeTexY);
	}
}

Quad::~Quad() {

	glDeleteBuffers(1, &m_quadVBO);
	glDeleteVertexArrays(1, &m_vao);

	if (m_vaoFlipped) {
		glDeleteVertexArrays(1, &m_vaoFlipped);
	}
}

void Quad::createBuffer(unsigned int& vao, bool flippable, float sizeX, float sizeY, float sizeTexX, float sizeTexY, short x, short y) {
	std::vector<float> vertex;

	if (flippable) {
		vertex.push_back(-1.0 * sizeX); vertex.push_back(-1.0 * sizeY); vertex.push_back(0.0); vertex.push_back(x * sizeTexX); vertex.push_back(y * sizeTexY); vertex.push_back((1 - x) * sizeTexX); vertex.push_back(y * sizeTexY);
		vertex.push_back(-1.0 * sizeX); vertex.push_back(1.0 * sizeY); vertex.push_back(0.0); vertex.push_back(x * sizeTexX); vertex.push_back((1 - y) * sizeTexY); vertex.push_back((1 - x) * sizeTexX); vertex.push_back((1 - y) * sizeTexY);
		vertex.push_back(1.0 * sizeX); vertex.push_back(1.0 * sizeY); vertex.push_back(0.0); vertex.push_back((1 - x) * sizeTexX); vertex.push_back((1 - y) * sizeTexY); vertex.push_back(x * sizeTexX); vertex.push_back((1 - y) * sizeTexY);
		vertex.push_back(1.0 * sizeX); vertex.push_back(-1.0 * sizeY); vertex.push_back(0.0); vertex.push_back((1 - x) * sizeTexX); vertex.push_back(y * sizeTexY); vertex.push_back(x * sizeTexX); vertex.push_back(y * sizeTexY);
	}else {
		vertex.push_back(-1.0 * sizeX); vertex.push_back(-1.0 * sizeY); vertex.push_back(0.0); vertex.push_back(x * sizeTexX); vertex.push_back(y * sizeTexY);
		vertex.push_back(-1.0 * sizeX); vertex.push_back(1.0 * sizeY); vertex.push_back(0.0); vertex.push_back(x * sizeTexX); vertex.push_back((1 - y) * sizeTexY);
		vertex.push_back(1.0 * sizeX); vertex.push_back(1.0 * sizeY); vertex.push_back(0.0); vertex.push_back((1 - x) * sizeTexX); vertex.push_back((1 - y) * sizeTexY);
		vertex.push_back(1.0 * sizeX); vertex.push_back(-1.0 * sizeY); vertex.push_back(0.0); vertex.push_back((1 - x) * sizeTexX); vertex.push_back(y * sizeTexY);
	}
	static const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};

	short stride = flippable ? 7 : 5;
	short offset = 3;

	unsigned int indexQuad;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &m_quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
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
	glGenBuffers(1, &indexQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &indexQuad);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Quad::render(unsigned int texture, bool array) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, texture);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, 0);
}

void Quad::setFlipped(bool flipped) { 
	if (m_flipped == flipped) return;
	m_flipped = flipped; 

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (m_flippable ? 7 : 5) * sizeof(float), (void*)((m_flipped ? 5 : 3) * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/*void  Quad::render(unsigned int texture, bool array) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, texture);
	glBindVertexArray(m_flipped ? m_vaoFlipped : m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, 0);
}

void Quad::setFlipped(bool flipped) {
	if (m_flipped == flipped) return;
	m_flipped = flipped;
}*/