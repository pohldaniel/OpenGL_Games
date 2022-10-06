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

void Label::draw(int posX, int posY) {
	draw(posX, posY, m_text);
}

void Label::draw(int posX, int posY, std::string text) {
	Fontrenderer::get().drawText(*m_characterSet, posX + getPosX(), posY + getPosY(), text, activeColor);
	//Widget::draw();
}

void Label::update(int mouseX, int mouseY) {
	
	if (mouseX > getPosX() && mouseX < getPosX() + getWidth() &&
		mouseY > getPosY() && mouseY < getPosY() + getHeight()) {

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