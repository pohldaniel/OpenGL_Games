#include "Tree.h"

Tree::Tree() {
	
	m_model = new Model();
	m_model->loadObject("res/lowPolyTree.obj");
	
	m_shader = Globals::shaderManager.getAssetPointer("texture");
	m_texture = &Globals::textureManager.get("tree");
}


Tree::~Tree() {

}

void Tree::draw(const Camera& camera) {
	glUseProgram(m_shader->m_program);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_modelView", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix(), true);
	m_shader->loadMatrix("u_projection", Globals::projection, true);
	m_texture->bind(0);

	m_model->drawRaw();

	Texture::Unbind();
	glUseProgram(0);
}

void Tree::drawShadow(const Camera& camera) {
	drawRaw();
}

void Tree::drawRaw() {
	m_model->drawRaw();
}