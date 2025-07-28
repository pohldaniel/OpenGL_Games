#include <GL/glew.h>
#include "Billboard.h"
#include "Globals.h"

Billboard::Billboard(const IsometricCamera& camera): camera(camera), m_vao(0u), m_vbo(0u){
	loadBillboards();
}

Billboard::~Billboard() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}
}

void Billboard::draw() {
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);

	Globals::textureManager.get("tree").bind(0u);
	auto shader = Globals::shaderManager.getAssetPointer("billboard_new");
	shader->use();
	shader->loadMatrix("u_viewProjection", camera.getOrthographicMatrix() * camera.getViewMatrix());
	shader->loadVector("u_camPos", camera.getPosition());
	shader->loadVector("u_right", camera.getCamX());
	shader->loadFloat("u_width", 60.0f);
	shader->loadFloat("u_height", 150.0f);
	shader->loadInt("u_texture", 0);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_POINTS, 0, 8);
	glBindVertexArray(0);
	glDisable(GL_ALPHA_TEST);
	shader->unuse();
}

void Billboard::loadBillboards() {
	m_positions.push_back({ -1100.0f, 0.0f, 1100.0f });
	m_positions.push_back({ -1050.0f, 0.0f, 1100.0f });
	m_positions.push_back({ -950.0f, 0.0f, 1100.0f });
	m_positions.push_back({ -750.0f, 0.0f, 1100.0f });
	m_positions.push_back({ -350.0f, 0.0f, 1100.0f });
	m_positions.push_back({ -1100.0f, 0.0f, 700.0f });
	m_positions.push_back({ -1100.0f, 0.0f, 300.0f });
	m_positions.push_back({ -1100.0f, 0.0f, -100.0f });

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions[0]) * m_positions.size(), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}