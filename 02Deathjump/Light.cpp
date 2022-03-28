#include "Random.h"
#include "Light.h"

Light::Light(const Vector2f &position, float radius) : Light() {
	m_position = position;
	m_shader = Globals::shaderManager.getAssetPointer("light");
	m_quad = new Quad(false, 0.0f, 2.0f, -2.0f, 0.0f, radius  * 0.5, radius * 0.5);

	setOrigin(Vector2f(radius, radius) / 2.0f);

	const int r1 = Random::RandInt(1, 10);
	const int r2 = Random::RandInt(5, 20);

	m_move = (float)r1 / (float)r2;
}

Light::Light(Light const& rhs) {
	m_position = rhs.m_position;
	m_origin = rhs.m_origin;
	m_move = rhs.m_move;
	m_transform = rhs.m_transform;

	m_shader = new Shader();
	*m_shader = *rhs.m_shader;

	m_quad = new Quad();
	std::swap(*m_quad, *rhs.m_quad);
}

Light::~Light() {
	delete m_quad;
}

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