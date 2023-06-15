#include "Widget.h"

Matrix4f Widget::Orthographic;

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
}