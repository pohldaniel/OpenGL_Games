#include "Button.h"
#include "Globals.h"
#include "Application.h"

Button::Button() {

	m_transform.identity();
	m_transformOutline.identity();
	m_shader = Globals::shaderManager.getAssetPointer("quad_color_uniform");
}

Button::Button(Button const& rhs) : Widget(rhs) {
	m_thickness = rhs.m_thickness;
	m_transformOutline = rhs.m_transformOutline;

	m_fillColor = rhs.m_fillColor;
	m_outlineColor = rhs.m_outlineColor;
	m_outlineColorDefault = rhs.m_outlineColorDefault;
	m_outlineColorHover = rhs.m_outlineColorHover;

	m_fun = std::function<void()>(rhs.m_fun);

	if (rhs.m_shader) {
		m_shader = new Shader();
		*m_shader = *rhs.m_shader;
	}
}

Button& Button::operator=(const Button& rhs) {
	Widget::operator=(rhs);
	m_thickness = rhs.m_thickness;
	m_transformOutline = rhs.m_transformOutline;

	m_fillColor = rhs.m_fillColor;
	m_outlineColor = rhs.m_outlineColor;
	m_outlineColorDefault = rhs.m_outlineColorDefault;
	m_outlineColorHover = rhs.m_outlineColorHover;

	m_fun = std::function<void()>(rhs.m_fun);

	if (rhs.m_shader) {
		m_shader = new Shader();
		*m_shader = *rhs.m_shader;
	}
	return *this;
}

Button::~Button() { }

void Button::draw() {
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Orthographic * m_transform);
	m_shader->loadVector("u_color", m_fillColor);
	Globals::shapeManager.get("quad").drawRaw();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	m_shader->loadMatrix("u_transform", Orthographic * m_transform * m_transformOutline) ;
	m_shader->loadVector("u_color", m_outlineColor);
	Globals::shapeManager.get("quad").drawRaw();
	glUseProgram(0);

	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);
}

void Button::setOutlineColor(const Vector4f &color) {
	m_outlineColor = color;
}

void Button::setPosition(const float x, const float y) {
	m_position.set(x, y);
	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void Button::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void Button::setSize(const float sx, const float sy) {
	m_size.set(sx, sy);
	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void Button::setSize(const Vector2f &size) {
	m_size = size;
	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void Button::setOutlineThickness(float thickness) {
	m_thickness = thickness;
	float xScaleOutline = (m_thickness * 2.0f) / (m_size[0]);
	float yScaleOutline = (m_thickness * 2.0f) / (m_size[1]);
	m_transformOutline.scale(1.0f + xScaleOutline, 1.0f + yScaleOutline, 1.0f);

	m_transform = Matrix4f::Translate(m_position[0] + m_size[0] * 0.5f + m_thickness, m_position[1] + m_size[1] * 0.5f + m_thickness, 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void Button::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {
	if (mouseX > m_position[0] + m_thickness && mouseX < m_position[0] + m_size[0] + m_thickness  &&
		mouseY > m_position[1] + m_thickness && mouseY < m_position[1] + m_size[1] + m_thickness) {
		m_outlineColor = m_outlineColorHover;
		
		if (button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT && m_fun) {
			m_fun();
		}

	}else {
		m_outlineColor = m_outlineColorDefault;

	}
}

void Button::setFunction(std::function<void()> fun) {
	m_fun = fun;
}