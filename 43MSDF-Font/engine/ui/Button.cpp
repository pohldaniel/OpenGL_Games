#include "Button.h"

Button::Button() : TextField() {
	m_isPressed = false;
}

Button::Button(Button const& rhs) : TextField(rhs) {
	m_fun = std::function<void()>(rhs.m_fun);
	m_isPressed = rhs.m_isPressed;
}

Button& Button::operator=(const Button& rhs) {
	TextField::operator=(rhs);
	m_fun = std::function<void()>(rhs.m_fun);
	m_isPressed = rhs.m_isPressed;
	return *this;
}

Button::~Button() { }

void Button::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {
	if (mouseX > m_position[0] + m_thickness && mouseX < m_position[0] + m_size[0] + m_thickness  &&
		mouseY > m_position[1] + m_thickness && mouseY < m_position[1] + m_size[1] + m_thickness) {
		m_outlineColor = m_outlineColorHover;
		m_isPressed = button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT;
	}else {
		m_outlineColor = m_outlineColorDefault;
	}

	if (m_isPressed && m_fun) {	
		m_fun();
	}
}

void Button::setFunction(std::function<void()> fun) {
	m_fun = fun;
}

const bool Button::isPressed() {
	return m_isPressed;
}