#include "engine/Fontrenderer.h"

#include "TextField.h"
#include "Globals.h"

TextField::TextField(const CharacterSet& charset) : charset(charset) {

	m_transform.identity();
	m_transformOutline.identity();
	m_shader = nullptr;

	m_text = std::string();

	m_fillColor.set(1.0f, 0.0f, 0.0f, 1.0f);
	m_outlineColor.set(1.0f, 1.0f, 0.0f, 1.0f);
	m_outlineColorDefault.set(1.0f, 1.0f, 0.0f, 1.0f);
	m_outlineColorHover.set(1.0f, 0.0f, 1.0f, 1.0f);
	m_offset.set(0.0f, 0.0f);
	m_padding.set(30.0f, 20.0f);
}

TextField::TextField(TextField const& rhs) : Widget(rhs), charset(rhs.charset) {
	m_thickness = rhs.m_thickness;
	m_transformOutline = rhs.m_transformOutline;

	m_fillColor = rhs.m_fillColor;
	m_outlineColor = rhs.m_outlineColor;
	m_outlineColorDefault = rhs.m_outlineColorDefault;
	m_outlineColorHover = rhs.m_outlineColorHover;
	m_text = rhs.m_text;

	if (rhs.m_shader) {
		m_shader = new Shader();
		*m_shader = *rhs.m_shader;
	}
}

TextField& TextField::operator=(const TextField& rhs) {
	Widget::operator=(rhs);
	m_thickness = rhs.m_thickness;
	m_transformOutline = rhs.m_transformOutline;

	m_fillColor = rhs.m_fillColor;
	m_outlineColor = rhs.m_outlineColor;
	m_outlineColorDefault = rhs.m_outlineColorDefault;
	m_outlineColorHover = rhs.m_outlineColorHover;
	m_text = rhs.m_text;
	//charset = rhs.charset;

	if (rhs.m_shader) {
		m_shader = new Shader();
		*m_shader = *rhs.m_shader;
	}
	return *this;
}

TextField::~TextField() { }

void TextField::draw() {
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Orthographic * m_transform);
	m_shader->loadVector("u_color", m_fillColor);
	Globals::shapeManager.get("quad").drawRaw();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	m_shader->loadMatrix("u_transform", Orthographic * m_transform * m_transformOutline);
	m_shader->loadVector("u_color", m_outlineColor);
	Globals::shapeManager.get("quad").drawRaw();
	glUseProgram(0);

	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);

	if (!m_text.empty()) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		charset.bind(0);
		Fontrenderer::Get().addText(charset, m_position[0] + m_thickness + m_padding[0] * 0.5f + m_offset[0], m_position[1] + m_thickness + m_padding[1] * 0.5f + m_offset[1], m_text, Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		Fontrenderer::Get().drawBuffer();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}

void TextField::setOffset(const float ox, const float oy) {
	m_offset.set(ox, oy);
}

void TextField::setOffset(const Vector2f &offset) {
	m_offset = offset;
}

void TextField::setPadding(const float px, const float py) {
	m_padding.set(px, py);
}

void TextField::setPadding(const Vector2f& padding) {
	m_padding = padding;
}

void TextField::setOutlineColor(const Vector4f &color) {
	m_outlineColor = color;
}

void TextField::setPosition(const float x, const float y) {
	m_position.set(x, y);
	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void TextField::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void TextField::setSize(const float sx, const float sy) {
	m_size.set(sx, sy);
	setText(m_text);
	
	setOutlineThickness(m_thickness);
}

void TextField::setSize(const Vector2f &size) {
	m_size = size;
	setText(m_text);
	
	setOutlineThickness(m_thickness);
}

void TextField::setText(std::string text) {
	m_text = text;

	if (!m_text.empty()) {
		m_size[0] = std::max(m_size[0], static_cast<float>(charset.getWidth(m_text)));
		m_size[1] = std::max(m_size[1], static_cast<float>(charset.lineHeight));
		m_size += m_padding;
	}

	setOutlineThickness(m_thickness);
}

void TextField::setOutlineThickness(float thickness) {
	m_thickness = thickness;
	float xScaleOutline = (m_thickness * 2.0f) / (m_size[0]);
	float yScaleOutline = (m_thickness * 2.0f) / (m_size[1]);
	m_transformOutline.scale(1.0f + xScaleOutline, 1.0f + yScaleOutline, 1.0f);

	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void TextField::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {

	if (mouseX > m_position[0] + m_thickness && mouseX < m_position[0] + m_size[0] + m_thickness  &&
		mouseY > m_position[1] + m_thickness && mouseY < m_position[1] + m_size[1] + m_thickness) {
		m_outlineColor = m_outlineColorHover;
		
	}else {
		m_outlineColor = m_outlineColorDefault;

	}
}

void TextField::setShader(Shader *shader) {
	m_shader = shader;
}