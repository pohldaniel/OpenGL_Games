#include "ViewPort.h"
#include "Application.h"

ViewPort ViewPort::s_instance;
float ViewPort::ViewLeft = 0.0f;
float ViewPort::ViewRight = 0.0f;
float ViewPort::ViewBottom = 0.0f;
float ViewPort::ViewTop = 0.0f;
bool ViewPort::Update = false;

ViewPort::ViewPort() : postition(m_camera.getPosition()) {
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(1000.0f);
	m_screeBorder = -200.0f;
}

ViewPort& ViewPort::Get() {
	return s_instance;
}

void ViewPort::resize() {

	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	ViewLeft = getLeft();
	ViewRight = getRight();
	ViewBottom = getBottom();
	ViewTop = getTop();
}

Camera& ViewPort::getCamera() {
	return m_camera;
}

void ViewPort::setPosition(const Vector3f& position) {
	Update = true;
	m_camera.setPosition(position);
}

void ViewPort::setPosition(float x, float y, float z) {
	Update = true;
	m_camera.setPosition(x, y, z);
}

void ViewPort::move(const Vector3f& delta) {
	Update = true;
	m_camera.move(delta);
}

void ViewPort::setScreenBorder(float screeBorder) {
	Update = true;
	m_screeBorder = screeBorder;
}

const Vector3f& ViewPort::getPosition() {
	return postition;
}

float ViewPort::getLeft() {
	return postition[0] + m_left + m_screeBorder;
}

float ViewPort::getRight() {
	return postition[0] + m_right - m_screeBorder;
}

float ViewPort::getBottom() {
	return postition[1] + m_bottom +  m_screeBorder;
}

float ViewPort::getTop() {
	return postition[1] + m_top - m_screeBorder;
}

bool ViewPort::IsRectOnScreen(float posX, float posY, float width,  float height) {
	
	if (Update) {
		ViewPort& viewPort = ViewPort::Get();
		ViewLeft = viewPort.getLeft();
		ViewRight = viewPort.getRight();
		ViewBottom = viewPort.getBottom();
		ViewTop = viewPort.getTop();
		Update = false;
	}

	if (posX >= (ViewRight) || posY >= (ViewTop) || (posX + width) <= ViewLeft ||  (posY - height) <= ViewBottom) {
		return false;
	}
	return true;
}