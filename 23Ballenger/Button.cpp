#include "Button.h"

Button::Button(const CharacterSet& charset) : TextField(charset) {
	m_fun = 0;
}

Button::Button(Button const& rhs) : TextField(rhs) {
	m_fun = std::function<void()>(rhs.m_fun);	
}

Button& Button::operator=(const Button& rhs) {
	TextField::operator=(rhs);
	m_fun = std::function<void()>(rhs.m_fun);
	return *this;
}

Button::~Button() { }

void Button::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {
	if (mouseX > m_position[0] + m_thickness && mouseX < m_position[0] + m_size[0] + m_thickness  &&
		mouseY > m_position[1] + m_thickness && mouseY < m_position[1] + m_size[1] + m_thickness) {
		m_outlineColor = m_outlineColorHover;
		
		if (button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT && m_fun) {
			m_fun();
		}

	}else {
		m_outlineColor = m_outlineColorDefault;

	}
}

void Button::setFunction(std::function<void()> fun) {
	m_fun = fun;
}