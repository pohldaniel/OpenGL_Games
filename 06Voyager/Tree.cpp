#include "Tree.h"

Tree::Tree() {
	
	m_model = new Model();
	m_model->loadObject("res/lowPolyTree.obj");
	
	m_shader = Globals::shaderManager.getAssetPointer("texture");
	m_aabbShader = Globals::shaderManager.getAssetPointer("aabb");
	m_colorShader = Globals::shaderManager.getAssetPointer("color");
	m_sphereShader = Globals::shaderManager.getAssetPointer("texture");

	m_texture = &Globals::textureManager.get("tree");
	m_nullTexture = &Globals::textureManager.get("null");

	m_transformOutline.scale(1.01f, 1.01f, 1.01f);

	unsigned int _r = (m_model->m_id & 0x000000FF) >> 0;
	unsigned int _g = (m_model->m_id & 0x0000FF00) >> 8;
	unsigned int _b = (m_model->m_id & 0x00FF0000) >> 16;

	r = static_cast<float>(_r) * (1.0f / 255.0f);
	g = static_cast<float>(_g) * (1.0f / 255.0f);
	b = static_cast<float>(_b) * (1.0f / 255.0f);

	m_id = m_model->m_id;
}


Tree::~Tree() {

}

void Tree::draw(const Camera& camera) {
	if (m_drawBorder) {
		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	glUseProgram(m_shader->m_program);
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawAABB(camera);
	drawSphere(camera);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Tree::drawShadow(const Camera& camera) {
	drawRaw();
}

void Tree::drawRaw() {
	m_model->drawRaw();
}

void Tree::drawRaw(const Camera& camera) {
	glUseProgram(m_colorShader->m_program);
	m_colorShader->loadMatrix("u_transform", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix() * Globals::projection);
	m_colorShader->loadVector("u_color", Vector4f(r, g, b, 1.0f));
	m_model->drawRaw();
	glUseProgram(0);
}

void Tree::drawAABB(const Camera& camera) {
	glUseProgram(m_aabbShader->m_program);
	m_aabbShader->loadMatrix("u_transform", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix() * Globals::projection);
	m_model->drawAABB();
	glUseProgram(0);
}

void Tree::drawSphere(const Camera& camera) {
	glUseProgram(m_sphereShader->m_program);

	m_sphereShader->loadMatrix("u_projection", Globals::projection);
	m_sphereShader->loadMatrix("u_modelView", m_modelMatrix.getTransformationMatrix() * camera.getViewMatrix());
	m_nullTexture->bind(0);
	m_model->drawSphere();
	Texture::Unbind();

	glUseProgram(0);
}


void Tree::setDrawBorder(bool flag) {
	m_drawBorder = flag;
}