#include "ViewPort.h"


ViewPort ViewPort::s_instance;

ViewPort::ViewPort() : m_postition(m_camera.getPosition()) {}

ViewPort& ViewPort::get() {
	return s_instance;
}

void ViewPort::init(unsigned int width, unsigned int height) {
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.orthographic(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);

	m_left = m_camera.getLeftOrthographic() + m_screeBorder;
	m_right = m_camera.getRightOrthographic() - m_screeBorder;
	m_bottom = m_camera.getBottomOrthographic() + m_screeBorder;
	m_top = m_camera.getTopOrthographic() - m_screeBorder;
}

void ViewPort::update(float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	float moveSpeed = 500.0f;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
	}

	m_camera.move(directrion[0] * dt * moveSpeed, directrion[1] * dt * moveSpeed, directrion[2] * dt * moveSpeed);
}

Camera& ViewPort::getCamera() {
	return m_camera;
}

float ViewPort::getLeft() {
	return m_postition[0] + m_left;
}

float ViewPort::getRight() {
	return m_postition[0] + m_right;
}

float ViewPort::getBottom() {
	return m_postition[1] + m_bottom;
}

float ViewPort::getTop() {
	return m_postition[1] + m_top;
}