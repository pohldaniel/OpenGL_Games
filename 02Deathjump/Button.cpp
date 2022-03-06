#include "Button.h"


Button::Button(std::string label, float sizeX, float sizeY, const Vector4f& color) {
	m_text = new Text(label);
	
	m_size[0] = sizeX;
	m_size[1] = sizeY;
	
	m_shader = new Shader("shader/quad_color.vs", "shader/quad_color.fs");
	m_shaderSingle = new Shader("shader/quad_color_single.vs", "shader/quad_color_single.fs");

	float vertices[] = {
		 0.0f        ,  -1.0f  * sizeY ,  0.0f,  color[0], color[1], color[2], color[3],
		 0.0f        ,   0.0f          ,  0.0f,  color[0], color[1], color[2], color[3],
		 1.0f * sizeX,   0.0f          ,  0.0f,  color[0], color[1], color[2], color[3],
		 1.0f * sizeX,  -1.0f  * sizeY ,  0.0f,  color[0], color[1], color[2], color[3]
	};

	static const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};

	short stride = 7, offset = 3;

	unsigned int indexQuad;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//Indices
	glGenBuffers(1, &indexQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &indexQuad);
}

Button::~Button(){}


void Button::render() {
	
	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST); 

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_transform * Globals::projection);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	glUseProgram(m_shaderSingle->m_program);
	m_shaderSingle->loadMatrix("u_transform", m_transformOutline * m_transform * Globals::projection);
	m_shaderSingle->loadVector("u_color", m_outlineColor);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

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
	m_transform.translate((m_position[0] + m_origin[0]), (HEIGHT - m_position[1] - m_origin[1]), 0.0f);

	m_text->setPosition(m_position - m_origin + (m_size - m_text->getSize()) * 0.5f);
}

void Button::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (HEIGHT - m_position[1] + m_origin[1]), 0.0f);

	m_text->setPosition(m_position - m_origin + (m_size - m_text->getSize()) * 0.5f);
}

void Button::setOutlineThickness(float thickness) {
	m_thickness = thickness * 2.0f;
	xScaleOutline = (m_thickness) / m_size[0];
	yScaleOutline = (m_thickness) / m_size[1];
	m_transformOutline.scale(1.0f + xScaleOutline, 1.0f + yScaleOutline, 1.0f);
	
	m_transformOutline = m_transformOutline * Matrix4f::Translate(-thickness, thickness, 0.0f);
	m_text->setPosition(m_position - m_origin + (m_size - m_text->getSize()) * 0.5f);

	m_position = m_position - Vector2f(m_thickness, m_thickness) * 0.5f;
	m_size = m_size + Vector2f(m_thickness, m_thickness);
}

void Button::update() {
	if ((Globals::cursorPosScreen.x > (m_position[0] - m_origin[0]) &&
		 Globals::cursorPosScreen.x < (m_position[0] - m_origin[0]) + m_size[0]) &&
		(Globals::cursorPosScreen.y > (m_position[1] - m_origin[1]) &&
		 Globals::cursorPosScreen.y < (m_position[1] - m_origin[1]) + m_size[1])) {
		m_outlineColor = m_outlineColorHover;
		m_isPressed = Globals::lMouseButton;
	}else {
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
