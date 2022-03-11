#include "Button.h"

Button::Button(std::string label, const Vector4f& color) : Button() {
	m_text = new Text(label);
	m_size = m_text->getSize() + Vector2f(30.0f, 20.0f);

	m_quad = new Quad(false, 1.0f, -1.0f, m_size[0] * 0.5, m_size[1] * 0.5);
	m_shader = Globals::shaderManager.getAssetPointer("quad_color_single");

	m_fillColor = color;
	setOrigin(m_size * 0.5f);
	setOutlineThickness(4.0f);
}

Button::Button(std::string label, const Vector2f &position, const Vector4f& color) : Button(label, color) {
	setPosition(position);
}

//copy constructor is necessary for putting the button inside a stl container at Menu.cpp
//https://stackoverflow.com/questions/8897239/stdpair-and-class-destructors
Button::Button(Button const& other) {
	m_position = other.m_position;
	m_origin = other.m_origin;
	m_thickness = other.m_thickness;
	m_size = other.m_size;
	m_transform = other.m_transform;
	m_transformOutline = other.m_transformOutline;
	m_fillColor = other.m_fillColor;
	
	//just pass over the shader but destruct them in the shaderManager once
	m_shader = new Shader();
	*m_shader = *other.m_shader;

	m_quad = new Quad();
	std::swap(*m_quad, *other.m_quad);

	m_text = new Text();
	std::swap(*m_text, *other.m_text);
}

//copy assignment operators is necessary for initialize the button at Settings.cpp
Button &Button::operator=(const Button &rhs) {
	m_position = rhs.m_position;
	m_origin = rhs.m_origin;
	m_thickness = rhs.m_thickness;
	m_size = rhs.m_size;
	m_transform = rhs.m_transform;
	m_transformOutline = rhs.m_transformOutline;
	m_fillColor = rhs.m_fillColor;

	//just pass over the shader but destruct them in the shaderManager once
	m_shader = new Shader();
	*m_shader = *rhs.m_shader;

	m_quad = new Quad();
	std::swap(*m_quad, *rhs.m_quad);

	m_text = new Text();
	std::swap(*m_text, *rhs.m_text);
	return *this;
}

Button::~Button() {
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

void Button::render() {
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

	m_text->render(m_outlineColor);
}

void Button::setOutlineColor(const Vector4f &color) {
	m_outlineColor = color;
}

void Button::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((position[0] - m_origin[0]), (HEIGHT - position[1] + m_origin[1]), 0.0f);

	m_text->setPosition((m_position - m_origin) + (m_size - m_text->getSize()) * 0.5f);
}

void Button::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (HEIGHT - m_position[1] + m_origin[1]), 0.0f);

	m_text->setPosition((m_position - m_origin) + (m_size - m_text->getSize()) * 0.5f);
}

void Button::setOutlineThickness(float thickness) {
	m_thickness = thickness * 2.0f;
	xScaleOutline = (m_thickness) / m_size[0];
	yScaleOutline = (m_thickness) / m_size[1];
	m_transformOutline.scale(1.0f + xScaleOutline, 1.0f + yScaleOutline, 1.0f);
	m_transformOutline = m_transformOutline * Matrix4f::Translate(-thickness, thickness, 0.0f);

	//m_position = m_position - Vector2f(m_thickness, m_thickness) * 0.5f;
	//m_size = m_size + Vector2f(m_thickness, m_thickness);

	m_text->setPosition((m_position - m_origin) + (m_size - m_text->getSize()) * 0.5f);
}

void Button::update() {
	if ((Globals::cursorPosScreen.x > (m_position[0] - m_origin[0] - m_thickness * 0.5f) &&
		Globals::cursorPosScreen.x < (m_position[0] - m_origin[0]) + m_size[0] + m_thickness * 0.5f) &&
		(Globals::cursorPosScreen.y >(m_position[1] - m_origin[1] - m_thickness * 0.5f) &&
			Globals::cursorPosScreen.y < (m_position[1] - m_origin[1]) + m_size[1] + m_thickness * 0.5f)) {
		m_outlineColor = m_outlineColorHover;
		m_isPressed = Globals::lMouseButton;
	}
	else {
		m_outlineColor = m_outlineColorDefault;
	}

	if (m_isPressed && m_fun) {
		m_fun();
	}
}

const bool Button::pressed() {
	return m_isPressed;
}

const Vector2f &Button::getPosition() const {
	return m_position - m_origin;
}

const Vector2f &Button::getSize() const {
	return m_size;
}

void Button::setFunction(std::function<void()> fun) {
	m_fun = fun;
}
