#include "Barrel.h"

Barrel::Barrel(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	const int HEIGHTMAP_WIDTH = 8192;

	m_model = new Model();
	m_model->loadObject("res/barrel/barrel.obj");

	m_model->m_mesh[0]->generateTangents();

	m_vao = m_model->m_mesh[0]->m_vao;
	m_drawCount = m_model->m_mesh[0]->m_drawCount;

	m_shader = Globals::shaderManager.getAssetPointer("normal");
	m_texture = &Globals::textureManager.get("barrel");
	m_normalMap = &Globals::textureManager.get("barrel_normal");

	glUseProgram(m_shader->m_program);
	m_shader->loadVector("u_lightPos[0]", Vector3f(-600, 10, 0.0));
	m_shader->loadVector("u_lightPos[1]", Vector3f(600, 10, 0.0));

	m_shader->loadVector("light[0].ambient", Vector3f(0.2, 0.2, 0.2));
	m_shader->loadVector("light[0].diffuse", Vector3f(0.5, 0.5, 0.5));
	m_shader->loadVector("light[0].specular", Vector3f(0.2, 0.2, 0.2));

	m_shader->loadVector("light[1].ambient", Vector3f(0.2, 0.2, 0.2));
	m_shader->loadVector("light[1].diffuse", Vector3f(0.5, 0.5, 0.5));
	m_shader->loadVector("light[1].specular", Vector3f(0.2, 0.2, 0.2));

	m_shader->loadVector("material.ambient", Vector3f(1.0f, 1.0f, 1.0f));
	m_shader->loadVector("material.diffuse", Vector3f(1.0f, 1.0f, 1.0f));
	m_shader->loadVector("material.specular", Vector3f(1.0f, 1.0f, 1.0f));
	m_shader->loadFloat("material.specularShininesse", 1.0f);
	glUseProgram(0);
}


Barrel::~Barrel() {

}

void Barrel::render(const Camera& camera) {

	glEnable(GL_CULL_FACE);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_modelView", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix());
	m_shader->loadMatrix("u_projection", Globals::projection);
	m_shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix()));
	m_shader->loadMatrix("u_modelViewLight", modelLight.getTransformationMatrix());

	m_shader->loadInt("u_texture", 0);
	m_texture->bind(0);

	m_shader->loadInt("u_normalMap", 1);
	m_normalMap->bind(1);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Texture::Unbind();

	glUseProgram(0);
	glDisable(GL_CULL_FACE);
}

void Barrel::update() {
	if (m_rotateLight) {
		modelLight.rotate(Vector3f(0.0, 1.0, 0.0), -60.0 * m_dt);
	}
}

void Barrel::toggleLightRotation() {
	m_rotateLight = !m_rotateLight;
}

void Barrel::renderShadow(const Camera& camera) {	
	glEnable(GL_CULL_FACE);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	
}

void Barrel::rotate(const Vector3f &axis, float degrees) {
	m_modelMatrix.rotate(axis, degrees);
}

void Barrel::translate(float dx, float dy, float dz) {
	m_modelMatrix.translate(dx, dy, dz);
}

void Barrel::scale(float a, float b, float c) {
	m_modelMatrix.scale(a, b, c);
}

const Matrix4f &Barrel::getTransformationMatrix() const {
	return m_modelMatrix.getTransformationMatrix();
}

const Matrix4f &Barrel::getInvTransformationMatrix() const {
	return m_modelMatrix.getInvTransformationMatrix();
}