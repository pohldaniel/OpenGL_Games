#include "Barrel.h"

Barrel::Barrel(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	
	m_model = new Model();
	m_model->loadObject("res/barrel/barrel.obj");

	m_model->m_mesh[0]->generateTangents();

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

void Barrel::draw(const Camera& camera) {

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_modelView", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix());
	m_shader->loadMatrix("u_projection", Globals::projection);
	m_shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix()));
	m_shader->loadMatrix("u_modelViewLight", modelLight.getTransformationMatrix());

	m_shader->loadInt("u_texture", 0);
	m_texture->bind(0);

	m_shader->loadInt("u_normalMap", 1);
	m_normalMap->bind(1);

	m_model->drawRaw();

	
	Texture::Unbind();

	glUseProgram(0);
}

void Barrel::drawShadow(const Camera& camera) {
	drawRaw();
}

void Barrel::drawRaw() {
	m_model->drawRaw();
}


void Barrel::update() {
	if (m_rotateLight) {
		modelLight.rotate(Vector3f(0.0, 1.0, 0.0), -60.0 * m_dt);
	}
}

void Barrel::toggleLightRotation() {
	m_rotateLight = !m_rotateLight;
}