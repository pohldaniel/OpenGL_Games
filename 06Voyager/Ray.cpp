#include "Ray.h"

Ray::Ray() {
	m_rayShader = Globals::shaderManager.getAssetPointer("ray");
	createBuffer();
}

Ray::~Ray() {

}

void Ray::draw(const Camera& camera) {

	
	glUseProgram(m_rayShader->m_program);
	m_rayShader->loadMatrix("u_transform", camera.getViewMatrix() * Globals::projection);
	//m_rayShader->loadMatrix("u_transform", camera.getViewMatrix());
	
	glBindVertexArray(m_vao);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Ray::update(const Vector3f& origin, const Vector3f& direction) {

	float vertices[] = { origin[0], origin[1], origin[2], 1.0f, 0.0f, 0.0f, 1.0f, 		
						 direction[0], direction[1],  direction[2], 0.0f, 1.0f, 0.0f, 1.0f};
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
}

void Ray::createBuffer() {
	glLineWidth(20.0);
	
	unsigned int indices[] = {
		0, 1
	};

	short stride = 7, offset = 3;

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 2 * stride * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//positions
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
}