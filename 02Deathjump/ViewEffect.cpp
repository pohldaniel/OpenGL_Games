#include "ViewEffect.h"

ViewEffect ViewEffect::s_instance;

ViewEffect& ViewEffect::get() {
	return s_instance;
}

const Matrix4f& ViewEffect::getView() {
	return m_view;
}

void ViewEffect::update(const float& dt) {
	m_elapsedTime += dt;
	while (m_elapsedTime > m_updateTime) {
		applayEffect();
		m_elapsedTime -= m_updateTime;
	}
}

void ViewEffect::shake(const float& duration) {
	m_duration = duration;

	m_currentPosition = 0;

	for (int i = 0; i < 15; i++)
		m_positions[i] = Vector2f(Random::RandInt(-1, 1), Random::RandInt(-1, 1));
}

void ViewEffect::applayEffect() {
	if (m_duration > 0.0f) {
		m_duration -= 0.1f;

		
		Vector2f eye = m_positions[m_currentPosition] * 0.01f;
		m_view.lookAt(Vector3f(eye[0], eye[1], 1.0f), Vector3f(eye[0], eye[1], -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

		m_currentPosition++;
		if (m_currentPosition > 9)
			m_currentPosition = 0;

		if (m_duration <= 0.0f)
			m_duration = 0.0f;
	}

	if (m_duration == 0.0f) {
		m_view.lookAt(Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	}
}

void ViewEffect::init() {
	m_view.lookAt(Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
}

