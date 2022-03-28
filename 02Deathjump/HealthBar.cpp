#include "HealthBar.h"

HealthBar::HealthBar() {
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(false, 0.0f, 2.0f, 0.0f, 2.0f, m_size[0], m_size[1]);
	initSprites();

	setOrigin(Vector2f(0.0f, m_size[1] * m_quad->getScale()[1]));
}

HealthBar::~HealthBar() {}

void HealthBar::update(bool x) {
	if (x && m_currentFrame < 3)
		m_currentFrame++;
	else if (!x && m_currentFrame > 0)
		m_currentFrame--;
}

void HealthBar::initSprites() {
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("health_bar");
	m_textureAtlas = m_spriteSheet->getAtlas();
}

void HealthBar::render() {
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shaderArray->loadInt("u_layer", m_currentFrame);
	m_quad->render(m_textureAtlas, true);
	glUseProgram(0);
}

void HealthBar::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate(m_position[0] - m_origin[0], m_position[1] - m_origin[1], 0.0f);
}

void HealthBar::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate(m_position[0] - m_origin[0], m_position[1] - m_origin[1], 0.0f);
}

int HealthBar::getHealthState() const {
	return 3 - m_currentFrame;
}