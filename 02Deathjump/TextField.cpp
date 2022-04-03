#include "TextField.h"

TextField::TextField(size_t maxChar) : TextField() {
	m_text = new Text(maxChar, Globals::fontManager.get("font_200"), 90.0f / 200.0f);
	m_quad = new Quad(false, 0.0f, 2.0f, 0.0f, 2.0f, m_size[0] * 0.5, m_size[1] * 0.5);
	m_shader = Globals::shaderManager.getAssetPointer("quad_color_single");
	setOutlineThickness(4.0f);
}

TextField::TextField(size_t maxChar, const Vector4f& color) : TextField(maxChar) {
	m_fillColor = color;
}

TextField::TextField(size_t maxChar, const Vector4f& color, const Vector2f &position) : TextField(maxChar, color) {
	setPosition(position);
}

TextField::TextField(std::string label) : TextField() {
	m_label = true;
	m_text = new Text(label, Globals::fontManager.get("font_200"), 90.0f / 200.0f);
	m_size = m_text->getSize() + Vector2f(30.0f, 20.0f);

	m_quad = new Quad(false, 0.0f, 2.0f, 0.0f, 2.0f, m_size[0] * 0.5, m_size[1] * 0.5);
	m_shader = Globals::shaderManager.getAssetPointer("quad_color_single");
	setOutlineThickness(4.0f);
}

TextField::TextField(std::string label, const Vector4f& color) : TextField(label) {
	m_fillColor = color;
}

TextField::TextField(TextField const& rhs) {
	m_position = rhs.m_position;
	m_origin = rhs.m_origin;
	m_thickness = rhs.m_thickness;
	m_size = rhs.m_size;
	m_transform = rhs.m_transform;
	m_transformOutline = rhs.m_transformOutline;
	m_fillColor = rhs.m_fillColor;
	m_label = rhs.m_label;

	m_shader = new Shader();
	*m_shader = *rhs.m_shader;

	m_quad = new Quad();
	std::swap(*m_quad, *rhs.m_quad);

	m_text = new Text(rhs.m_text->m_charset);
	std::swap(*m_text, *rhs.m_text);
}

TextField& TextField::operator=(const TextField& rhs) {
	m_position = rhs.m_position;
	m_origin = rhs.m_origin;
	m_thickness = rhs.m_thickness;
	m_size = rhs.m_size;
	m_transform = rhs.m_transform;
	m_transformOutline = rhs.m_transformOutline;
	m_fillColor = rhs.m_fillColor;
	m_label = rhs.m_label;

	m_shader = new Shader();
	*m_shader = *rhs.m_shader;

	m_quad = new Quad();
	std::swap(*m_quad, *rhs.m_quad);

	m_text = new Text(rhs.m_text->m_charset);
	std::swap(*m_text, *rhs.m_text);
	return *this;
}

TextField::TextField(std::string label, const Vector4f& color, const Vector2f &position) : TextField(label, color) {
	setPosition(position);
}

TextField::~TextField() {
	if (m_quad) {
		delete m_quad;
		m_quad = NULL;
	}

	if (m_text) {
		delete m_text;
		m_text = NULL;
	}

	//shader will be destructed with the shaderManager
	/*if (m_shader) {
	delete m_shader;
	m_shader = NULL;
	}*/
}

void TextField::render() {
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shader->loadVector("u_color", m_fillColor);
	m_quad->render();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	m_shader->loadMatrix("u_transform", m_transformOutline * m_transform * Globals::projection);
	m_shader->loadVector("u_color", m_outlineColor);
	m_quad->render();
	glUseProgram(0);

	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);

	m_text->render(m_textColor);
}

void TextField::render(std::string label) {
	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shader->loadVector("u_color", m_fillColor);
	m_quad->render();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	m_shader->loadMatrix("u_transform", m_transformOutline * m_transform * Globals::projection);
	m_shader->loadVector("u_color", m_outlineColor);
	m_quad->render();
	glUseProgram(0);

	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);

	m_text->render(label, m_textColor);
}

void TextField::setOutlineColor(const Vector4f &color) {
	m_outlineColor = color;
}

void TextField::setFillColor(const Vector4f &color) {
	m_fillColor = color;
}

void TextField::setTextColor(const Vector4f &color) {
	m_textColor = color;
}

void TextField::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((position[0] - m_origin[0]), (position[1] - m_origin[1]), 0.0f);

	if (m_label) {
		m_text->setPosition(m_position - m_origin + (m_size - m_text->getSize()) * 0.5);
	}else {
		float x = m_position[0] - m_origin[0] + m_size[0] * 0.25;
		float y = m_position[1] - m_origin[1] + m_size[1] * 0.25;

		m_text->setPosition(Vector2f(x, y));
	}
}

void TextField::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
	if (m_label) {
		m_text->setPosition(m_position - m_origin + (m_size - m_text->getSize()) * 0.5);
	}else {
		float x = m_position[0] - m_origin[0] + m_size[0] * 0.25;
		float y = m_position[1] - m_origin[1] + m_size[1] * 0.25;

		m_text->setPosition(Vector2f(x, y));
	}
}

void TextField::setOutlineThickness(float thickness) {
	Matrix4f trans;
	m_thickness = thickness * 2.0f;
	float xScaleOutline = (m_thickness) / m_size[0];
	float yScaleOutline = (m_thickness) / m_size[1];
	m_transformOutline.scale(1.0f + xScaleOutline, 1.0f + yScaleOutline, 1.0f);
	m_transformOutline = m_transformOutline * Matrix4f::Translate(trans, -thickness, -thickness, 0.0f);

	if (m_label) {
		m_text->setPosition(m_position - m_origin + (m_size - m_text->getSize()) * 0.5);
	}else {
		float x = m_position[0] - m_origin[0] + m_size[0] * 0.25;
		float y = m_position[1] - m_origin[1] + m_size[1] * 0.25;

		m_text->setPosition(Vector2f(x, y));
	}
}

const Vector2f &TextField::getPosition() const {
	return m_position - m_origin;
}

const Vector2f &TextField::getSize() const {
	return m_size;
}