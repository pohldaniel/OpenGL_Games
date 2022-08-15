#include "Fern.h"

Fern::Fern() : RenderableObject() {
	m_model = new ObjModel();
	m_model->loadObject("res/models/fern/fern.obj");
	m_model->createAABB();
	m_model->createSphere();
	m_model->createConvexHull("res/models/fern/fern_conv.obj", false);

	m_shader = Globals::shaderManager.getAssetPointer("transperancy");
	m_texture = &Globals::textureManager.get("fern");
	m_transformOutline.scale(1.01f, 1.01f, 1.01f);
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
	m_shader->loadMatrix("u_projection", Globals::projection);
	m_shader->loadMatrix("u_view", camera.getViewMatrix());
	m_shader->loadMatrix("u_model", m_transform.getTransformationMatrix());

	m_texture->bind(0);

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
