#include "Label.h"

Label::Label(const CharacterSet& characterSet, std::string text) {
	m_characterSet = &characterSet;
	m_text = text;
}

Label &Label::operator=(const Label &rhs) {
	m_text = rhs.m_text;
	m_characterSet = rhs.m_characterSet;
	return *this;
}

Label::Label(Label const& rhs) : m_characterSet(rhs.m_characterSet) {
	m_text = rhs.m_text;
}

void Label::draw() {
	draw(m_text);
}

void Label::draw(std::string text) {
	Fontrenderer::Get().addText(*m_characterSet, m_parentWidget->getPosX() + getPosX(), m_parentWidget->getPosY() + getPosY(), text, activeColor);
}

void Label::processInput() {

	if (ViewPort::Get().getCursorPosRelX() > m_parentWidget->getPosX() + getPosX() + 5 && ViewPort::Get().getCursorPosRelX() < m_parentWidget->getPosX() + getPosX() + getWidth() &&
		ViewPort::Get().getCursorPosRelY() > m_parentWidget->getPosY() + getPosY() && ViewPort::Get().getCursorPosRelY() < m_parentWidget->getPosY() + getPosY() + getHeight()) {

		activeColor = hoverColor;
		if (Mouse::instance().buttonPressed(Mouse::MouseButton::BUTTON_LEFT) && m_fun) {
			m_fun();
		}

	}else {
		activeColor = defaultColor;

	}	
}

void Label::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {
	if (mouseX > m_parentWidget->getPosX() + getPosX() + 5 && mouseX < m_parentWidget->getPosX() + getPosX() + getWidth() &&
		mouseY > m_parentWidget->getPosY() + getPosY() && mouseY < m_parentWidget->getPosY() + getPosY() + getHeight()) {

		activeColor = hoverColor;
		if (button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT && m_fun) {
			m_fun();
		}

	} else {
		activeColor = defaultColor;

	}
}

int Label::getWidth() const {

	int sizeX = 0;
	std::string::const_iterator c;
	for (c = m_text.begin(); c != m_text.end(); c++) {
		const Char ch = m_characterSet->getCharacter(*c);
		sizeX = sizeX + ((ch.advance));		
	}
	return  sizeX;
}

int Label::getHeight() const {
	return m_characterSet->lineHeight;
}

void Label::setFunction(std::function<void()> fun) {
	m_fun = fun;
}

void Label::setDefaultColor(const Vector4f &color) {
	defaultColor = color;
	activeColor = defaultColor;
}

void Label::setHoverColor(const Vector4f &color) {
	hoverColor = color;
}