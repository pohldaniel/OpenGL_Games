#include "Tree.h"

Tree::Tree() {
	const int HEIGHTMAP_WIDTH = 8192;

	m_model = new Model();

	m_model->loadObject("res/lowPolyTree.obj");
	m_model->translate(HEIGHTMAP_WIDTH * 0.5f + 300.0f, 400.1f, HEIGHTMAP_WIDTH * 0.5f + 300.0f);

	
	m_vao = m_model->m_mesh[0]->m_vao;
	m_drawCount = m_model->m_mesh[0]->m_drawCount;

	m_shader = Globals::shaderManager.getAssetPointer("texture");
	m_texture = &Globals::textureManager.get("tree");
}


Tree::~Tree() {

}

void Tree::render(const Camera& camera) {
	//m_model->draw(camera);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_modelView", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix(), true);
	m_shader->loadMatrix("u_projection", Globals::projection, true);

	m_texture->bind(0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Texture::Unbind();

	glUseProgram(0);
}

void Tree::renderShadow(const Camera& camera) {	
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Tree::rotate(const Vector3f &axis, float degrees) {
	m_modelMatrix.rotate(axis, degrees);
}

void Tree::translate(float dx, float dy, float dz) {
	m_modelMatrix.translate(dx, dy, dz);
}

void Tree::scale(float a, float b, float c) {
	m_modelMatrix.scale(a, b, c);
}

const Matrix4f &Tree::getTransformationMatrix() const {
	return m_modelMatrix.getTransformationMatrix();
}

const Matrix4f &Tree::getInvTransformationMatrix() const {
	return m_modelMatrix.getInvTransformationMatrix();
}