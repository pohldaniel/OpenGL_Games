#include "../Fontrenderer.h"
#include "CheckBox.h"
#include "Globals.h"

CheckBox::CheckBox() {

	m_transform.identity();
	m_transformOutline.identity();
	m_shader = nullptr;

	m_isChecked ? m_fillColor.set(1.0f, 0.0f, 0.0f, 1.0f) : m_fillColor.set(0.0f, 0.0f, 0.0f, 1.0f);
	m_outlineColor.set(1.0f, 1.0f, 0.0f, 1.0f);
	m_outlineColorDefault.set(1.0f, 1.0f, 0.0f, 1.0f);
	m_outlineColorHover.set(1.0f, 0.0f, 1.0f, 1.0f);

	m_isChecked = false;
}

CheckBox::CheckBox(CheckBox const& rhs) : Widget(rhs) {

	m_thickness = rhs.m_thickness;
	m_transformOutline = rhs.m_transformOutline;

	m_fillColor = rhs.m_fillColor;
	m_outlineColor = rhs.m_outlineColor;
	m_outlineColorDefault = rhs.m_outlineColorDefault;
	m_outlineColorHover = rhs.m_outlineColorHover;

	m_fun = std::function<void()>(rhs.m_fun);
	m_isChecked = rhs.m_isChecked;
}

CheckBox& CheckBox::operator=(const CheckBox& rhs) {

	Widget::operator=(rhs);

	m_thickness = rhs.m_thickness;
	m_transformOutline = rhs.m_transformOutline;

	m_fillColor = rhs.m_fillColor;
	m_outlineColor = rhs.m_outlineColor;
	m_outlineColorDefault = rhs.m_outlineColorDefault;
	m_outlineColorHover = rhs.m_outlineColorHover;
	m_isChecked = rhs.m_isChecked;

	return *this;
}

CheckBox::~CheckBox() { }

void CheckBox::draw() {
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	auto shader = m_shader == nullptr ? WidgetShader.get() : m_shader;
	shader->use();
	shader->loadMatrix("u_transform", Orthographic * m_transform);
	shader->loadVector("u_color", m_fillColor);
	Widget::DrawQuad();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	shader->loadMatrix("u_transform", Orthographic * m_transform * m_transformOutline);
	shader->loadVector("u_color", m_outlineColor);
	Widget::DrawQuad();
	shader->unuse();

	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);
}

void CheckBox::setOutlineColor(const Vector4f &color) {
	m_outlineColor = color;
}

void CheckBox::setPosition(const float x, const float y) {
	m_position.set(x, y);
	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void CheckBox::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void CheckBox::setSize(const float sx, const float sy) {
	m_size.set(sx, sy);
	setOutlineThickness(m_thickness);
}

void CheckBox::setSize(const Vector2f &size) {
	m_size = size;
	setOutlineThickness(m_thickness);
}

void CheckBox::setOutlineThickness(float thickness) {
	m_thickness = thickness;
	float xScaleOutline = (m_thickness * 2.0f) / (m_size[0]);
	float yScaleOutline = (m_thickness * 2.0f) / (m_size[1]);
	m_transformOutline.scale(1.0f + xScaleOutline, 1.0f + yScaleOutline, 1.0f);

	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void CheckBox::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {

	if (mouseX > m_position[0] + m_thickness && mouseX < m_position[0] + m_size[0] + m_thickness  &&
		mouseY > m_position[1] + m_thickness && mouseY < m_position[1] + m_size[1] + m_thickness) {
		m_outlineColor = m_outlineColorHover;
		m_isChecked = button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT ? !m_isChecked : m_isChecked;
		m_isChecked ? m_fillColor.set(1.0f, 0.0f, 0.0f, 1.0f) : m_fillColor.set(0.0f, 0.0f, 0.0f, 1.0f);

		if (button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT && m_fun) {
			m_fun();
		}

	}else{
		m_outlineColor = m_outlineColorDefault;
	}
}

void CheckBox::setShader(const Shader *shader) {
	m_shader = shader;
}

float CheckBox::getTickness() {
	return m_thickness;
}

void CheckBox::setFunction(std::function<void()> fun) {
	m_fun = fun;
}

const bool CheckBox::isChecked() {
	return m_isChecked;
}

void CheckBox::setIsChecked(bool isChecked) {
	m_isChecked = isChecked;
	m_isChecked ? m_fillColor.set(1.0f, 0.0f, 0.0f, 1.0f) : m_fillColor.set(0.0f, 0.0f, 0.0f, 1.0f);
}