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
	Fontrenderer::Get().drawText(*m_characterSet, m_parentX + getPosX(), m_parentY + getPosY(), text, activeColor);
}

void Label::processInput() {

	if (ViewPort::get().getCursorPosRelX() > m_parentX + getPosX() + 5 && ViewPort::get().getCursorPosRelX() < m_parentX + getPosX() + getWidth() &&
		ViewPort::get().getCursorPosRelY() > m_parentY + getPosY() && ViewPort::get().getCursorPosRelY() < m_parentY + getPosY() + getHeight()) {

		activeColor = hoverColor;
		if (Mouse::instance().buttonPressed(Mouse::MouseButton::BUTTON_LEFT) && m_fun) {
			m_fun();
		}

	}else {
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