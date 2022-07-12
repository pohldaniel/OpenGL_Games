#include "Fern.h"

Fern::Fern() {
	const int HEIGHTMAP_WIDTH = 8192;

	m_model = new Model();
	m_model->loadObject("res/fern.obj");

	m_vao = m_model->m_mesh[0]->m_vao;
	m_drawCount = m_model->m_mesh[0]->m_drawCount;

	m_shader = Globals::shaderManager.getAssetPointer("transperancy");
	m_texture = &Globals::textureManager.get("fern");
}


Fern::~Fern() {

}

void Fern::render(const Camera& camera) {
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_modelView", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix(), true);
	m_shader->loadMatrix("u_projection", Globals::projection, true);

	m_texture->bind(0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Texture::Unbind();

	glUseProgram(0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);}

void Fern::renderShadow(const Camera& camera) {
	//glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	m_texture->bind(0);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	Texture::Unbind();
	glDisable(GL_CULL_FACE);
	//glDisable(GL_ALPHA_TEST);
}

void Fern::rotate(const Vector3f &axis, float degrees) {
	m_modelMatrix.rotate(axis, degrees);
}

void Fern::translate(float dx, float dy, float dz) {
	m_modelMatrix.translate(dx, dy, dz);
}

void Fern::scale(float a, float b, float c) {
	m_modelMatrix.scale(a, b, c);
}

const Matrix4f &Fern::getTransformationMatrix() const {
	return m_modelMatrix.getTransformationMatrix();
}

const Matrix4f &Fern::getInvTransformationMatrix() const {
	return m_modelMatrix.getInvTransformationMatrix();
}