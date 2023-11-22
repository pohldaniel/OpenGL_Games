#include "Button_ls.h"

ButtonLS::ButtonLS() {

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(false, 0.0f, 0.5f, 0.0f, 0.5f, m_size[0], m_size[1]);

	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("button_ls");
	m_textureAtlas = m_spriteSheet->getAtlas();

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quadPointer = new Quad(false, 0.0f, 0.5f, 0.0f, 0.5f, m_sizePointer[0], m_sizePointer[1]);

	m_sprites["pointer"] = Globals::textureManager.get("pointer").getTexture();
}

ButtonLS::~ButtonLS() {
	//delete m_quadBackground;
}

void ButtonLS::update() {}

void ButtonLS::render() {
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Globals::projection * m_transform);
	m_shaderArray->loadInt("u_layer", 0);
	m_quad->render(m_textureAtlas, true);
	glUseProgram(0);
	
	if (m_state == State::SELECTED) {
		glUseProgram(m_shader->m_program);
		m_shader->loadMatrix("u_transform", Globals::projection * m_transformPointer);
		m_quadPointer->render(m_sprites["pointer"]);
		glUseProgram(0);
	}
}

void ButtonLS::setState(State state) {
	m_state = state;
}

void ButtonLS::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((position[0] - m_origin[0]), (position[1] - m_origin[1]), 0.0f);

	m_transformPointer.translate((position[0] - m_origin[0] - m_sizePointer[0]), (position[1] - (m_origin[1] - m_size[2] * 0.25f + m_sizePointer[1] * 0.45f)), 0.0f);
}

void ButtonLS::setLevel(const std::string &level) {
	m_level = level;
}

std::string ButtonLS::getLevel() {
	return m_level;
}