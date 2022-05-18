#include "Barnacle.h"

Barnacle::Barnacle(unsigned int category, const float& dt, const float& fdt) : Entity(category, dt, fdt){
	m_size = Vector2f(26.0f, 26.0f);
	m_position = Vector2f(200.0f, 600.0f);

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_size[0], m_size[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);
	m_quad->setFlipped(true);
	initAnimations();
}

Barnacle::~Barnacle() {
}


void Barnacle::initAnimations() {
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();

	m_Animations["move"].create(Globals::spritesheetManager.getAssetPointer("barnacle"), 0.5f, *m_textureAtlas, *m_currentFrame);
}

void Barnacle::update() {
	m_Animations["move"].update(m_dt);
}

void Barnacle::render() {


	//glEnable(GL_BLEND);	
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", m_transform * ViewEffect::get().getView() * Globals::projection);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);
	//glDisable(GL_BLEND);
}