#include "Label.h"

Label::Label(const CharacterSet& characterSet, std::string text) {
	m_characterSet = &characterSet;
	m_text = text;
}

Label &Label::operator=(const Label &rhs) {
	m_text = rhs.m_text;
	m_characterSet = rhs.m_characterSet;
}

Label::Label(Label const& rhs) : m_characterSet(rhs.m_characterSet) {
	m_text = rhs.m_text;
}

void Label::draw() {
	draw(m_text);
}

void Label::draw(std::string text) {

	glBindTexture(GL_TEXTURE_2D, m_characterSet->spriteSheet);

	Batchrenderer::get().setShader(Globals::shaderManager.getAssetPointer("font"));
	std::string::const_iterator c;
	float posX = getPosX();

	for (c = text.begin(); c != text.end(); c++) {

		const Char& ch = m_characterSet->getCharacter(*c);

		Batchrenderer::get().addQuad(Vector4f(posX, getPosY() , ch.size[0], ch.size[1]), Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]), activeColor);
		posX = posX + ch.advance[0];
	}
	Batchrenderer::get().drawBuffer(false);
	glBindTexture(GL_TEXTURE_2D, 0);
	//Widget::draw();
}

void Label::update(int mouseX, int mouseY) {
	
	if (mouseX > getPosX() && mouseX < getPosX() + getWidth() &&
		mouseY > getPosY() && mouseY < getPosY() + getHeight()) {

		activeColor = hoverColor;
		if (Globals::lMouseButton && m_fun) {
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
		sizeX = sizeX + ((ch.advance[0]));		
	}
	return  sizeX;
}

int Label::getHeight() const {
	return m_characterSet->lineHeight;
}

void Label::setFunction(std::function<void()> fun) {
	m_fun = fun;
}