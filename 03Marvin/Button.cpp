#include "Button.h"

Button::Button(){

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(false, 0.0f, 0.5f, 0.0f, 0.5f, m_size[0], m_size[1]);

	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("button_menu");
	m_textureAtlas = m_spriteSheet->getAtlas();
}

Button::~Button() {
	//delete m_quadBackground;
}

void Button::update() {}

void Button::render() {
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shaderArray->loadInt("u_layer", m_state);
	m_quad->render(m_textureAtlas, true);
	glUseProgram(0);	
}

void Button::setState(State state) {
	m_state = state;
}

void Button::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((position[0] - m_origin[0]), (position[1] - m_origin[1]), 0.0f);
}

void Button::setLevel(const std::string &level) {
	m_level = level;
}

std::string Button::getLevel() {
	return m_level;
}