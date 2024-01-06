#include "Tetraedron.h"

Tetraedron::Tetraedron() {
	m_position = Vector3f(-1.0f, -1.0f, -1.0f);
	m_size = Vector3f(2.0f, 2.0f, 2.0f);
	createBufferNormal();
}

Tetraedron::Tetraedron(Vector3f& position, Vector3f size) {
	m_position = position;
	m_size = size;
	createBufferNormal();
}

Tetraedron::~Tetraedron() {
	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
	}

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
	}
}

void Tetraedron::createBuffer() {
	std::vector<float> vertex;

	Vector3f pos = m_position;
	float w = m_size[0];
	float h = m_size[1];
	float d = m_size[2];

	
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d);	
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);

	unsigned short indices[] = {
		0, 3, 1,
		0, 1, 2,
		0, 2, 3,
		1, 3, 2,
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Tetraedron::createBufferTex() {

}

void Tetraedron::createBufferNormal() {
	std::vector<float> vertex;

	Vector3f pos = m_position;
	float w = m_size[0];
	float h = m_size[1];
	float d = m_size[2];
	Vector3f center = Vector3f(pos[0] + 0.5f * w, pos[1] + 0.5f * h, pos[2] + 0.5f * d);

	Vector3f n1 = Vector3f(pos[0] + w, pos[1] + h, pos[2]) - center;
	Vector3f::Normalize(n1); // 1 / sqrtf(3)

	Vector3f n2 = Vector3f(pos[0], pos[1] + h, pos[2] + d) - center;
	Vector3f::Normalize(n2);

	Vector3f n3 = Vector3f(pos[0] + w, pos[1], pos[2] + d) - center;
	Vector3f::Normalize(n3);

	Vector3f n4 = Vector3f(pos[0], pos[1], pos[2]) - center;
	Vector3f::Normalize(n4);

	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(n1[0]); vertex.push_back(n1[1]); vertex.push_back(n1[2]);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(n1[0]); vertex.push_back(n1[1]); vertex.push_back(n1[2]);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(n2[0]); vertex.push_back(n2[1]); vertex.push_back(n2[2]);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(n1[0]); vertex.push_back(n1[1]); vertex.push_back(n1[2]);


	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(n2[0]); vertex.push_back(n2[1]); vertex.push_back(n2[2]);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(n2[0]); vertex.push_back(n2[1]); vertex.push_back(n2[2]);

	vertex.push_back(pos[0] + w); vertex.push_back(pos[1] + h); vertex.push_back(pos[2] + d); vertex.push_back(n3[0]); vertex.push_back(n3[1]); vertex.push_back(n3[2]);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(n3[0]); vertex.push_back(n3[1]); vertex.push_back(n3[2]);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(n3[0]); vertex.push_back(n3[1]); vertex.push_back(n3[2]);

	vertex.push_back(pos[0]);     vertex.push_back(pos[1] + h); vertex.push_back(pos[2]);     vertex.push_back(n4[0]); vertex.push_back(n4[1]); vertex.push_back(n4[2]);
	vertex.push_back(pos[0] + w); vertex.push_back(pos[1]);     vertex.push_back(pos[2]);     vertex.push_back(n4[0]); vertex.push_back(n4[1]); vertex.push_back(n4[2]);
	vertex.push_back(pos[0]);     vertex.push_back(pos[1]);     vertex.push_back(pos[2] + d); vertex.push_back(n4[0]); vertex.push_back(n4[1]); vertex.push_back(n4[2]);

	unsigned short indices[] = {
		0, 3, 1,
		4, 5, 2,
		6, 7, 8,
		9, 10, 11,
	};

	short stride = 6;

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

	//Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	vertex.clear();
	vertex.shrink_to_fit();
}

void Tetraedron::createBufferTexNormal() {

}

void Tetraedron::setPosition(const Vector3f &position) {
	m_position = position;
}

void Tetraedron::setSize(const Vector3f &size) {
	m_size = size;
}

const Vector3f &Tetraedron::getPosition() const {
	return m_position;
}

const Vector3f &Tetraedron::getSize() const {
	return m_size;
}

void Tetraedron::draw(unsigned int texture) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Tetraedron::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}