#include "Marvin.h"

Marvin::Marvin(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_playerSize[0], m_playerSize[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);
	m_quad->setFlipped(true);
	m_characterControllerCS = new CharacterControllerCS(dt, fdt);

	initAnimations();
}

Marvin::~Marvin() {
	delete m_quad;
}

void Marvin::initAnimations() {
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();

	m_Animations["move"].create(Globals::spritesheetManager.getAssetPointer("marvin_move"), 0.08f, *m_textureAtlas, *m_currentFrame);
	m_Animations["jump"].create(Globals::spritesheetManager.getAssetPointer("marvin_jump"), 0.08f, *m_textureAtlas, *m_currentFrame);
	m_Animations["fall"].create(Globals::spritesheetManager.getAssetPointer("marvin_fall"), 0.08f, *m_textureAtlas, *m_currentFrame);

	m_Animations["idle"].create(Globals::spritesheetManager.getAssetPointer("marvin_idle"), 0.08f, *m_textureAtlas, *m_currentFrame);

	setPosition(Vector2f(m_characterControllerCS->m_position.x, m_characterControllerCS->m_position.y));
	setOrigin(Vector2f(13.0f, 13.0f));
}

void  Marvin::fixedUpdate() {
	m_characterControllerCS->fixedUpdate();

	setPosition(Vector2f(m_characterControllerCS->m_position.x, m_characterControllerCS->m_position.y));

	ViewEffect::get().setPosition(m_position);
}

void  Marvin::update() {

	if (m_characterControllerCS->m_velocity.x < 0) {
		m_quad->setFlipped(false);
	}else if (m_characterControllerCS->m_velocity.x > 0) {
		m_quad->setFlipped(true);
	}

	if (m_characterControllerCS->isGrounded()) { // NOT JUMPING
		if (m_characterControllerCS->m_velocity.x != 0) {
			
			m_Animations["move"].update(m_dt);
		} else {
			m_Animations["idle"].update(m_dt);
		}
	}else {
		if (m_characterControllerCS->m_velocity.y > 0) {
			m_Animations["jump"].update(m_dt);
		}if (m_characterControllerCS->m_velocity.y < 0) {
			m_Animations["fall"].update(m_dt);
		}
	}	
}

void  Marvin::render() {
	

	//glEnable(GL_BLEND);	
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", m_transform * ViewEffect::get().getView() * Globals::projection);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);
	//glDisable(GL_BLEND);
	//m_characterControllerCS->render();
}

void Marvin::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((position[0] - m_origin[0]), (position[1] - m_origin[1]), 0.0f);
}

void Marvin::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}