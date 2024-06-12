#include <engine/Material.h>
#include "Skybox.h"

std::unique_ptr<Shader> Skybox::SkyboxShader = nullptr;

Skybox::Skybox() : Object(), Entity(), m_vao(0u), m_vbo(0u), m_camera(nullptr) {
	if (!SkyboxShader) {
		SkyboxShader = std::unique_ptr<Shader>(new Shader(SKYBOX_VERTEX, SKYBOX_FRGAMENT, false));
		SkyboxShader->use();
		SkyboxShader->loadInt("u_texture", 0);
		SkyboxShader->unuse();
	}

	createCube();
}

Skybox::~Skybox() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}
}

void Skybox::createCube() {

	const float vertices[] = {
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f
	};

	const unsigned short indices[] = {
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

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Skybox::draw() {
	glFrontFace(GL_CW);

	Matrix4f view = m_camera->getViewMatrix();
	view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;

	SkyboxShader->use();
	SkyboxShader->loadMatrix("u_projection", m_camera->getPerspectiveMatrix());
	SkyboxShader->loadMatrix("u_view", view * getTransformationSOP());

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	drawCube();
	SkyboxShader->unuse();

	glFrontFace(GL_CCW);
}

void Skybox::setCamera(const Camera& camera) {
	m_camera = &camera;
}

void Skybox::drawCube() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}