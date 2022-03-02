#include "Button.h"


Button::Button(float sizeX, float sizeY, const Vector4f& color) {
	float xScale = sizeX / (float)(WIDTH);
	float yScale = sizeY / (float)(HEIGHT);

	m_size[0] = sizeX;
	m_size[1] = sizeY;

	setOutlineThickness(m_thickness);
	
	m_shader = new Shader("shader/quad_color.vs", "shader/quad_color.fs");
	m_shaderSingle = new Shader("shader/quad_color_single.vs", "shader/quad_color_single.fs");

	float size = 1.0f;
	float vertices[] = {
		-1.0f * xScale, -1.0f * yScale,  0.0f,  color[0], color[1], color[2], color[3],
		-1.0f * xScale,  1.0f * yScale,  0.0f,  color[0], color[1], color[2], color[3],
		 1.0f * xScale,  1.0f * yScale,  0.0f,  color[0], color[1], color[2], color[3],
		 1.0f * xScale, -1.0f * yScale,  0.0f,  color[0], color[1], color[2], color[3]
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
	m_shader->loadMatrix("u_transform", m_transform);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	glUseProgram(m_shaderSingle->m_program);
	m_shaderSingle->loadMatrix("u_transform", m_scaleOutline * m_transform);
	m_shaderSingle->loadVector("u_color", m_outlineColor);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	//glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
}

void Button::setOutlineColor(const Vector4f &color) {
	m_outlineColor = color;
}

void Button::setPosition(const Vector2f &position) {
	m_position = position;
	//m_transform.translate(position[0]  * xTrans - 1.0f, yTrans * (HEIGHT - position[1]) - 1.0f, 0.0f);
	m_transform.translate((m_position[0] + m_origin[0])* xTrans - 1.0f, yTrans * (HEIGHT - m_position[1] - m_origin[1]) - 1.0f, 0.0f);
}

void Button::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] + m_origin[0])* xTrans - 1.0f, yTrans * (HEIGHT - (m_position[1] + 0.5f * m_size[1] + m_origin[1])) - 1.0f, 0.0f);
}

void Button::setOutlineThickness(float thickness) {
	m_thickness = thickness;

	xScaleOutline = (2.0f * m_thickness) / m_size[0];
	yScaleOutline = (2.0f * m_thickness) / m_size[1];

	m_scaleOutline.scale(1.0f + xScaleOutline, 1.0f + yScaleOutline, 1.0f);
}