#include "Barrel.h"

Barrel::Barrel(){
	m_id = 30;

	m_model = new Model();
	m_model->loadObject("res/barrel/barrel.obj");

	m_model->m_mesh[0]->generateTangents();

	m_shader = Globals::shaderManager.getAssetPointer("normal");
	m_colorShader = Globals::shaderManager.getAssetPointer("color");

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

	m_transformOutline.scale(1.01f, 1.01f, 1.01f);

	m_pickColor = Vector4f(((m_id & 0x000000FF) >> 0)* (1.0f / 255.0f), ((m_id & 0x0000FF00) >> 0)* (1.0f / 255.0f), ((m_id & 0x00FF0000) >> 0)* (1.0f / 255.0f), 1.0f);
}

Barrel::~Barrel() {

}

void Barrel::draw(const Camera& camera) {

	if (m_drawBorder) {
		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}
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
	if (m_drawBorder) {
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		glUseProgram(m_colorShader->m_program);
		m_colorShader->loadMatrix("u_transform", m_transformOutline * m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix() * Globals::projection);
		m_colorShader->loadVector("u_color", Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
		m_model->drawRaw();
		glUseProgram(0);

		glStencilMask(0xFF);
		glDisable(GL_STENCIL_TEST);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawAABB(camera);
	drawSphere(camera);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Barrel::drawShadow(const Camera& camera) {
	drawRaw();
}

void Barrel::drawRaw() {
	m_model->drawRaw();
}

void Barrel::drawRaw(const Camera& camera) {
	glUseProgram(m_colorShader->m_program);
	m_colorShader->loadMatrix("u_transform", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix() * Globals::projection);
	m_colorShader->loadVector("u_color", m_pickColor);
	m_model->drawRaw();
	glUseProgram(0);
}

void Barrel::drawAABB(const Camera& camera) {
	glUseProgram(m_colorShader->m_program);
	m_colorShader->loadMatrix("u_transform", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix() * Globals::projection);
	m_colorShader->loadVector("u_color", m_pickColor);
	m_model->drawAABB();
	glUseProgram(0);
}

void Barrel::drawSphere(const Camera& camera) {
	glUseProgram(m_colorShader->m_program);
	m_colorShader->loadMatrix("u_transform", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix() * Globals::projection);
	m_colorShader->loadVector("u_color", m_pickColor);
	m_model->drawSphere();
	glUseProgram(0);
}

void Barrel::update(float dt) {
	if (m_rotateLight) {
		modelLight.rotate(Vector3f(0.0, 1.0, 0.0), -60.0 * dt);
	}
}

void Barrel::toggleLightRotation() {
	m_rotateLight = !m_rotateLight;
}

void Barrel::setDrawBorder(bool flag) {
	m_drawBorder = flag;
}