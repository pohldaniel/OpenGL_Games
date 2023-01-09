#include "Barrel.h"

Barrel::Barrel() : RenderableObject() {
	m_model = new ObjModel();
	//m_model->loadObject("res/models/barrel/barrel.obj");
	//m_model->generateTangents();
	m_model->loadObject("res/models/barrel/barrel.obj", false, true, false, false, true);
	m_model->createAABB();
	m_model->createSphere();
	m_model->createConvexHull("res/models/barrel/barrel_conv.obj", false);

	m_shader = Globals::shaderManager.getAssetPointer("normal_map");
	

	m_texture = &Globals::textureManager.get("barrel");
	m_normalMap = &Globals::textureManager.get("barrel_normal");


	glUseProgram(m_shader->m_program);
	m_shader->loadVector("u_lightPos[0]", Vector3f(-600.0f, 10.0f, 0.0f));
	m_shader->loadVector("u_lightPos[1]", Vector3f(600.0f, 10.0f, 0.0f));

	m_shader->loadVector("light[0].ambient", Vector3f(0.2f, 0.2f, 0.2f));
	m_shader->loadVector("light[0].diffuse", Vector3f(0.5f, 0.5f, 0.5f));
	m_shader->loadVector("light[0].specular", Vector3f(0.2f, 0.2f, 0.2f));

	m_shader->loadVector("light[1].ambient", Vector3f(0.2f, 0.2f, 0.2f));
	m_shader->loadVector("light[1].diffuse", Vector3f(0.5f, 0.5f, 0.5f));
	m_shader->loadVector("light[1].specular", Vector3f(0.2f, 0.2f, 0.2f));

	m_shader->loadVector("material.ambient", Vector3f(1.0f, 1.0f, 1.0f));
	m_shader->loadVector("material.diffuse", Vector3f(1.0f, 1.0f, 1.0f));
	m_shader->loadVector("material.specular", Vector3f(1.0f, 1.0f, 1.0f));
	m_shader->loadFloat("material.specularShininesse", 1.0f);
	glUseProgram(0);

	m_transformOutline.scale(1.01f, 1.01f, 1.01f);
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
	m_shader->loadMatrix("u_projection", Globals::projection);
	m_shader->loadMatrix("u_modelView", camera.getViewMatrix() * m_transform.getTransformationMatrix());
	m_shader->loadMatrix("u_view", camera.getViewMatrix());
	m_shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
	
	m_shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	m_shader->loadMatrix("u_modelViewLight", m_tranformLight.getTransformationMatrix());

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
		m_colorShader->loadMatrix("u_projection", Globals::projection);
		m_colorShader->loadMatrix("u_view", camera.getViewMatrix());
		m_colorShader->loadMatrix("u_model", m_transform.getTransformationMatrix() * m_transformOutline);
		m_colorShader->loadVector("u_color", Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
		m_model->drawRaw();
		glUseProgram(0);

		glStencilMask(0xFF);
		glDisable(GL_STENCIL_TEST);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//drawAABB(camera);
	//drawSphere(camera);
	//drawHull(camera);
	glPolygonMode(GL_FRONT_AND_BACK, Globals::enableWireframe ? GL_LINE : GL_FILL);
}

void Barrel::update(float dt) {
	if (m_rotateLight) {
		m_tranformLight.rotate(Vector3f(0.0f, 1.0f, 0.0f), 60.0f * dt);
	}
}

void Barrel::toggleLightRotation() {
	m_rotateLight = !m_rotateLight;
}

void Barrel::setDrawBorder(bool flag) {
	m_drawBorder = flag;
}