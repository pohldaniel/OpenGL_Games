#include <iostream>
#include "ViewPort.h"

ViewPort ViewPort::s_instance;

ViewPort::ViewPort() : m_postition(m_camera.getPosition()) {}

ViewPort& ViewPort::get() {
	return s_instance;
}

void ViewPort::init(unsigned int width, unsigned int height) {
	if (!m_init) {
		m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
		m_init = true;
	}
	m_camera.orthographic(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
	m_camera.invOrthographic(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	m_width = width;
	m_height = height;
}

void ViewPort::update(float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();
	
	
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

	Mouse &mouse = Mouse::instance();

	float mouseXndc = static_cast<float>(2.0f * mouse.xPosAbsolute()) / static_cast<float>(m_width) - 1.0f;
	float mouseYndc = 1.0f - static_cast<float>(2.0f * mouse.yPosAbsolute()) / static_cast<float>(m_height);
	m_cursorPosEye = Vector2f(0.5f * (m_right * (mouseXndc + 1.0f) + m_left * (1.0f - mouseXndc)), 0.5f * (m_top * (mouseYndc + 1.0f) + m_bottom * (1.0f - mouseYndc)));
}

Camera& ViewPort::getCamera() {
	return m_camera;
}

Vector2f& ViewPort::getCursorPos() {	
	return m_cursorPosEye;	
}

const Vector3f& ViewPort::getPosition() {
	return m_postition;
}

const Vector2f ViewPort::getBottomLeft() {
	return m_postition + m_camera.getBottomLeft() ;
}

float ViewPort::getLeft() {
	return m_postition[0] + m_left + m_screeBorder;
}

float ViewPort::getRight() {
	return m_postition[0] + m_right - m_screeBorder;
}

float ViewPort::getBottom() {
	return m_postition[1] + m_bottom + m_screeBorder;
}

float ViewPort::getTop() {
	return m_postition[1] + m_top - m_screeBorder;
}

unsigned int ViewPort::getWidth() {
	return m_width;
}

unsigned int ViewPort::getHeight() {
	return m_height;
}