#include "Tree.h"

Tree::Tree() : RenderableObject() {
	m_model = new ObjModel();
	m_model->loadObject("res/models/tree/lowPolyTree.obj");
	m_model->createAABB();
	m_model->createSphere();
	m_model->createConvexHull("res/models/tree/lowPolyTree.obj");

	m_shader = Globals::shaderManager.getAssetPointer("texture");
	m_texture = &Globals::textureManager.get("tree");

	m_transformOutline.scale(1.01f, 1.01f, 1.01f);
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
	m_shader->loadMatrix("u_modelView", m_transform.getTransformationMatrix() * camera.getViewMatrix(), true);
	m_shader->loadMatrix("u_projection", Globals::projection, true);
	m_texture->bind(0);

	m_model->drawRaw();

	Texture::Unbind();
	glUseProgram(0);

	if (m_drawBorder) {
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		glUseProgram(m_colorShader->m_program);
		m_colorShader->loadMatrix("u_transform", m_transformOutline * m_transform.getTransformationMatrix() * camera.getViewMatrix() * Globals::projection);
		m_colorShader->loadVector("u_color", Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
		m_model->drawRaw();
		glUseProgram(0);

		glStencilMask(0xFF);
		glDisable(GL_STENCIL_TEST);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//drawAABB(camera);
	//drawSphere(camera);
	drawHull(camera);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Tree::setDrawBorder(bool flag) {
	m_drawBorder = flag;
}