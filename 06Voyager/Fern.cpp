#include "Fern.h"

Fern::Fern() : RenderableObject() {
	m_model = new Model();
	m_model->loadObject("res/fern.obj");

	m_shader = Globals::shaderManager.getAssetPointer("transperancy");


	m_texture = &Globals::textureManager.get("fern");
	m_transformOutline.scale(1.01f, 1.01f, 1.01f);

	m_pickColor = Vector4f(((m_id & 0x000000FF) >> 0)* (1.0f / 255.0f), ((m_id & 0x0000FF00) >> 8)* (1.0f / 255.0f), ((m_id & 0x00FF0000) >> 16)* (1.0f / 255.0f), ((m_id & 0xFF000000) >> 24)* (1.0f / 255.0f));
}

Fern::~Fern() {

}

void Fern::draw(const Camera& camera) {
	glEnable(GL_ALPHA_TEST);

	if (m_drawBorder) {
		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_modelView", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix(), true);
	m_shader->loadMatrix("u_projection", Globals::projection, true);

	m_texture->bind(0);

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

	glDisable(GL_ALPHA_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawAABB(camera);
	drawSphere(camera);
	drawHull(camera);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Fern::drawShadow(const Camera& camera) {

	m_texture->bind(0);
	drawRaw();
	Texture::Unbind();
	
}

void Fern::setDrawBorder(bool flag) {
	m_drawBorder = flag;
}
