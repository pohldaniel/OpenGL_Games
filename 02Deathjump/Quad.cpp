#include "Quad.h"

Quad::Quad(float sizeX, float sizeY, float sizeTex) {
	m_sizeX = sizeX;
	m_sizeY = sizeY;
	m_sizeTex = sizeTex;
	createBuffer();
}

Quad::~Quad() {
	glDeleteVertexArrays(1, &m_vao);	
}

void Quad::createBuffer() {

	std::vector<float> vertex;
	vertex.push_back(-1.0 * m_sizeX); vertex.push_back(-1.0 * m_sizeY); vertex.push_back(0.0); vertex.push_back(0.0 * m_sizeTex); vertex.push_back(0.0 * m_sizeTex);
	vertex.push_back(-1.0 * m_sizeX); vertex.push_back(1.0 * m_sizeY); vertex.push_back(0.0); vertex.push_back(0.0 * m_sizeTex); vertex.push_back(1.0 * m_sizeTex);
	vertex.push_back(1.0 * m_sizeX); vertex.push_back(1.0 * m_sizeY); vertex.push_back(0.0); vertex.push_back(1.0 * m_sizeTex); vertex.push_back(1.0 * m_sizeTex);
	vertex.push_back(1.0 * m_sizeX); vertex.push_back(-1.0 * m_sizeY); vertex.push_back(0.0); vertex.push_back(1.0 * m_sizeTex); vertex.push_back(0.0 * m_sizeTex);

	static const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};

	unsigned int quadVBO, indexQuad;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	//Texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//Indices
	glGenBuffers(1, &indexQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &quadVBO);
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