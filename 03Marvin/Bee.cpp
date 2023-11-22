#include "Bee.h"

Bee::Bee(unsigned int category, const float& dt, const float& fdt) : Entity(category, dt, fdt) {
	m_size = Vector2f(26.0f, 26.0f);
	m_position = Vector2f(200.0f, 600.0f);

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(false, 0.0f, 1.0f, 0.0f, 1.0f, m_size[0], m_size[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);
	m_quad->setFlipped(true);
	initAnimations();
}

Bee::~Bee() {
}


void Bee::initAnimations() {
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();

	m_Animations["move"].create(Globals::spritesheetManager.getAssetPointer("bee"), 0.5f, *m_textureAtlas, *m_currentFrame);
}

void Bee::update() {
	m_Animations["move"].update(m_dt);
}

void Bee::render() {


	//glEnable(GL_BLEND);	
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Globals::projection * ViewEffect::get().getView() * m_transform);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);
	//glDisable(GL_BLEND);
}