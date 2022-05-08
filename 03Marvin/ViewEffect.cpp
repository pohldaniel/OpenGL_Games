#include "ViewEffect.h"

ViewEffect ViewEffect::s_instance;

ViewEffect& ViewEffect::get() {
	return s_instance;
}

const Matrix4f& ViewEffect::getView() {
	return m_view;
}

void ViewEffect::update() {
	m_view.lookAt(m_position - m_origin, m_target - m_origin, Vector3f(0.0f, 1.0f, 0.0f));
}

void ViewEffect::applyEffect() {
	
}

void ViewEffect::init() {
	m_view.lookAt(Vector3f(0.0f, -10.0f, -1.0f), Vector3f(0.0f, -10.0f, -2.0f), Vector3f(0.0f, 1.0f, 0.0f));
}

void ViewEffect::setPosition(const Vector2f &position) {
	m_position = Vector3f(position[0], position[1], -1.0f);
	m_target = Vector3f(position[0], position[1], -2.0f);

	m_view.lookAt(m_position - m_origin, m_target - m_origin, Vector3f(0.0f, 1.0f, 0.0f));
}

