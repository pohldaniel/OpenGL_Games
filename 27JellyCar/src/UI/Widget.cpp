#include "Widget.h"

std::unique_ptr<Shader> Widget::WidgetShader = nullptr;
std::unique_ptr<Shader> Widget::BatchShader = nullptr;
Matrix4f Widget::Orthographic;

Widget::Widget() {
	m_size.set(1.0f, 1.0f);
	m_position.set(0.0f, 0.0f);
	m_transform.identity();

	if (!WidgetShader) {
		WidgetShader = std::unique_ptr<Shader>(new Shader(WIDGET_VERTEX, WIDGET_FRGAMENT, false));
	}

	if (!BatchShader) {
		BatchShader = std::unique_ptr<Shader>(new Shader(BATCH_VERTEX, BATCH_FRGAMENT, false));
	}
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

	if (!BatchShader) {
		BatchShader = std::unique_ptr<Shader>(new Shader(BATCH_VERTEX, BATCH_FRGAMENT, false));
	}

	BatchShader->use();
	BatchShader->loadMatrix("u_transform", Orthographic);
	BatchShader->unuse();
}

void Widget::SetWidgetShader(const Shader* shader) {
	WidgetShader.reset(const_cast<Shader*>(shader));
}

void Widget::SetBatchShader(const Shader* shader) {
	BatchShader.reset(const_cast<Shader*>(shader));
}