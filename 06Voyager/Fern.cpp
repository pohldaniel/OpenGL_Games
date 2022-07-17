#include "Fern.h"

Fern::Fern() {
	m_model = new Model();
	m_model->loadObject("res/fern.obj");

	m_shader = Globals::shaderManager.getAssetPointer("transperancy");
	m_texture = &Globals::textureManager.get("fern");
}

Fern::~Fern() {

}

void Fern::draw(const Camera& camera) {
	glEnable(GL_ALPHA_TEST);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_modelView", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix(), true);
	m_shader->loadMatrix("u_projection", Globals::projection, true);

	m_texture->bind(0);

	m_model->drawRaw();

	Texture::Unbind();

	glUseProgram(0);
	glDisable(GL_ALPHA_TEST);
}

void Fern::drawShadow(const Camera& camera) {
	m_texture->bind(0);
	drawRaw();
	Texture::Unbind();
}

void Fern::drawRaw() {
	m_model->drawRaw();
}

