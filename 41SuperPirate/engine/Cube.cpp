#include <GL/glew.h>

#include "Cube.h"

Cube::Cube() {
	m_position = Vector3f(-1.0f, -1.0f, -1.0f);
	m_size = Vector3f(2.0f, 2.0f, 2.0f);
	createBufferTexNormal();
}

Cube::Cube(Vector3f& position, Vector3f size) {
	m_position = position;
	m_size = size;
	createBufferTexNormal();
}

Cube::~Cube() {
	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
	}

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
	}
}

void Cube::createBuffer() {
	std::vector<float> vertex;

	Vector3f pos = m_position;
	float w = m_size[0];
	float h = m_size[1];
	float d = m_size[2];

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2]);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);

	unsigned short indices[] = {	
		// positive X
		1, 5, 6,
		6, 2, 1,		
		// negative X
		4, 0, 3,
		3, 7, 4,
		// positive Y
		3, 2, 6,
		6, 7, 3,
		// negative Y
		4, 5, 1,
		1, 0, 4,
		// positive Z
		0, 1, 2,
		2, 3, 0,
		// negative Z
		7, 6, 5,
		5, 4, 7
	};

	short stride = 3;

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
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);	
	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Cube::createBufferTex() {

	std::vector<float> vertex;
	Vector3f pos = m_position;
	float w = m_size[0];
	float h = m_size[1];
	float d = m_size[2];

	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(1.0f);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(1.0f);

	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(1.0f);

	vertex.push_back(pos[0]); vertex.push_back(pos[1]);         vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0]); vertex.push_back(pos[1]);         vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0]); vertex.push_back(pos[1] + h);     vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0]); vertex.push_back(pos[1] + h);     vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(1.0f);

	unsigned short indices[] = {
		// positive X
		8, 10, 9,
		10, 8, 11,
		// negative X
		12, 14, 13,
		14, 12, 15,		
		// positive Y
		3, 2, 5,
		5, 4, 3,
		// negative Y
		7, 6, 1,
		1, 0, 7,
		// positive Z
		4, 5, 6,
		6, 7, 4,
		// negative Z
		0, 1, 2,
		2, 3, 0
	};

	short stride = 5;

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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Cube::createBufferTexNormal() {
	std::vector<float> vertex;
	Vector3f pos = m_position;
	float w = m_size[0];
	float h = m_size[1];
	float d = m_size[2];

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f); vertex.push_back(0.0f);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(-1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(1.0f); vertex.push_back(1.0f); vertex.push_back(-1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(-1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);

	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(0.0f); vertex.push_back(1.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(1.0f); vertex.push_back(1.0f); vertex.push_back(1.0f); vertex.push_back(0.0f); vertex.push_back(0.0f);

	unsigned short indices[] = {
		// negative X
		16, 17, 18,
		18, 19, 16,

		// positive X
		20, 22, 21,
		22, 20, 23,

		// positive Y
		8, 9, 10,
		10, 11, 8,

		// negative Y
		12, 14, 13,
		14, 12, 15,

		// positive Z
		4, 6, 5,
		6, 4, 7,

		// negative Z
		0, 1, 2,
		2, 3, 0,
	};

	short stride = 8;

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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

	//Normal Coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(5 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Cube::setPosition(const Vector3f &position) {
	m_position = position;
}

void Cube::setSize(const Vector3f &size) {
	m_size = size;
}

const Vector3f &Cube::getPosition() const {
	return m_position;
}

const Vector3f &Cube::getSize() const {
	return m_size;
}

void Cube::draw(unsigned int texture) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Cube::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}