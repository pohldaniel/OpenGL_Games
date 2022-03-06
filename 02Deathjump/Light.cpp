#include "Random.h"
#include "Light.h"

Light::Light(const Vector2f &position, float radius) {
	m_position = position;
	m_radius = radius;
	m_shader= new Shader("shader/light.vs", "shader/light.fs");
	m_quad = new Quad(false, 1.0f, -1.0f, radius  * 0.5, radius * 0.5 , 1.0, 1.0, 0, 1);


	setOrigin(Vector2f(radius, radius) / 2.0f);

	const int r1 = Random::RandInt(1, 10);
	const int r2 = Random::RandInt(5, 20);

	m_move = (float)r1 / (float)r2;
}

Light::~Light() {}

void Light::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((m_position[0] - m_origin[0]), (HEIGHT - m_position[1] + m_origin[1]), 0.0f);
}

void Light::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (HEIGHT - m_position[1] + m_origin[1]), 0.0f);
}

void Light::render() const {
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shader->loadFloat("u_move", m_move);
	m_quad->render();
	glUseProgram(0);
}

Shader&  Light::getShader() const {
	return *m_shader;
}