#include "Widget.h"

std::unique_ptr<Shader> Widget::WidgetShader = nullptr;
std::unique_ptr<Shader> Widget::BatchShader = nullptr;
Matrix4f Widget::Orthographic;
unsigned int Widget::Vao = 0u;
unsigned int Widget::Vbo = 0u;

Widget::Widget() {
	m_size.set(1.0f, 1.0f);
	m_position.set(0.0f, 0.0f);
	m_transform.identity();
}

Widget::Widget(Widget const& rhs) {
	m_position = rhs.m_position;
	m_size = rhs.m_size;
	m_transform = rhs.m_transform;
}

Widget& Widget::operator=(const Widget& rhs) {
	m_position = rhs.m_position;
	m_size = rhs.m_size;
	m_transform = rhs.m_transform;
	return *this;
}

Widget::~Widget() {

}

void Widget::setPosition(const float x, const float y) {
	m_position.set(x, y);
	m_transform = Matrix4f::Translate(m_position[0], m_position[1], 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void Widget::setPosition(const Vector2f& position) {
	m_position = position;
	m_transform = Matrix4f::Translate(m_position[0], m_position[1], 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void Widget::setSize(const float sx, const float sy) {
	m_size.set(sx, sy);
	m_transform = Matrix4f::Translate(m_position[0], m_position[1], 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

void Widget::setSize(const Vector2f& size) {
	m_size = size;
	m_transform = Matrix4f::Translate(m_position[0], m_position[1], 0.0f) * Matrix4f::Scale(m_size[0] * 0.5f, m_size[1] * 0.5f, 0.0f);
}

const Vector2f& Widget::getPosition() const {
	return m_position;
}

const Vector2f& Widget::getSize() const {
	return m_size;
}

void Widget::Resize(unsigned int width, unsigned int height) {
	
	Orthographic.orthographic(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
	BatchShader->use();
	BatchShader->loadMatrix("u_transform", Orthographic);
	BatchShader->unuse();

	WidgetShader->use();
	WidgetShader->loadMatrix("u_transform", Orthographic);
	WidgetShader->unuse();
}

void Widget::Init(unsigned int width, unsigned int height) {
	
	WidgetShader = std::unique_ptr<Shader>(new Shader(WIDGET_VERTEX, WIDGET_FRGAMENT, false));
	BatchShader = std::unique_ptr<Shader>(new Shader(BATCH_VERTEX, BATCH_FRGAMENT, false));

	float data[] = {
		-1.0, -1.0, 0.0f, 0.0f, 0.0f,
		-1.0,  1.0, 0.0f, 0.0f, 1.0f,
		1.0,  1.0, 0.0f, 1.0f, 1.0f,
		1.0,-1.0, 0.0f, 1.0f, 0.0f
	};

	const GLushort index[] = {
		0, 2, 1,
		0, 3, 2
	};

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &Vbo);

	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	Resize(width, height);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Widget::CleanUp() {
	if (Vao) {
		glDeleteVertexArrays(1, &Vao);
		Vao = 0u;
	}

	if (Vbo) {
		glDeleteBuffers(1, &Vbo);
		Vbo = 0u;
	}
}

void Widget::SetWidgetShader(const Shader* shader) {
	WidgetShader.reset(const_cast<Shader*>(shader));
}

void Widget::SetBatchShader(const Shader* shader) {
	BatchShader.reset(const_cast<Shader*>(shader));
}

void Widget::DrawQuad() {
	glBindVertexArray(Vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}