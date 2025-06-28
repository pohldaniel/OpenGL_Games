#include "CameraController.h"

constexpr float DEF_SPEED = 1.0f;
constexpr int SCROLL_L = -1;
constexpr int SCROLL_R = 1;
constexpr int SCROLL_U = -1;
constexpr int SCROLL_D = 1;
constexpr int NO_SCROLL = 0;
constexpr int SCROLL_MARGIN = 10;

CameraController::CameraController(IsometricCamera& camera, const int& width, const int& height) : camera(camera), width(width), height(height), m_speed(DEF_SPEED){

}

void CameraController::update(float dt) {
	if (m_dirX == SCROLL_L) {	
		camera.moveLeft(dt * m_speed);
	}else if (m_dirX == SCROLL_R) {
		camera.moveRight(dt * m_speed);
	}

	if (m_dirY == SCROLL_U) {
		camera.moveDown(dt * m_speed);
	}else if (m_dirY == SCROLL_D) {
		camera.moveUp(dt * m_speed);
	}
}

void CameraController::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_screenX = event.x;
	m_screenY = event.y;
	OnMouseMotion();	
}

void CameraController::OnMouseMotion() {
	if (m_screenX < SCROLL_MARGIN) {
		if (!m_keyScrollX) {
			m_dirX = SCROLL_R;
			m_mouseScrollX = true;
		}
	}else if (m_screenX > (width - SCROLL_MARGIN)) {
		if (!m_keyScrollX) {
			m_dirX = SCROLL_L;
			m_mouseScrollX = true;
		}
	}else if (!m_keyScrollX) {
		m_dirX = NO_SCROLL;
		m_mouseScrollX = false;
	}

	if (m_screenY < SCROLL_MARGIN) {
		if (!m_keyScrollY) {
			m_dirY = SCROLL_U;
			m_mouseScrollY = true;
		}
	}else if (m_screenY > (height - SCROLL_MARGIN)) {
		if (!m_keyScrollY) {
			m_dirY = SCROLL_D;
			m_mouseScrollY = true;
		}
	}else if (!m_keyScrollY) {
		m_dirY = NO_SCROLL;
		m_mouseScrollY = false;
	}
}

void CameraController::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == 'A' || event.keyCode == 37) {
		m_dirX = SCROLL_R;
		m_keyScrollX = true;
		m_mouseScrollX = false;
	}else if (event.keyCode == 'D' || event.keyCode == 39) {
		m_dirX = SCROLL_L;
		m_keyScrollX = true;
		m_mouseScrollX = false;
	}
	
	if (event.keyCode == 'W' || event.keyCode == 38) {
		m_dirY = SCROLL_U;
		m_keyScrollY = true;
		m_mouseScrollY = false;	
	}else if (event.keyCode == 'S' || event.keyCode == 40) {
		m_dirY = SCROLL_D;
		m_keyScrollY = true;
		m_mouseScrollY = false;
	}
}

void CameraController::OnKeyUp(Event::KeyboardEvent& event) {
	m_keyScrollX = !(event.keyCode == 'A' || event.keyCode == 'D' || event.keyCode == 37 || event.keyCode == 39) && m_keyScrollX;
	m_keyScrollY = !(event.keyCode == 'W' || event.keyCode == 'S' || event.keyCode == 38 || event.keyCode == 40) && m_keyScrollY;

	m_dirX = !m_keyScrollX ? NO_SCROLL : m_dirX;
	m_dirY = !m_keyScrollY ? NO_SCROLL : m_dirY;

	OnMouseMotion();
}

void CameraController::setSpeed(float speed) { 
	m_speed = speed; 
}