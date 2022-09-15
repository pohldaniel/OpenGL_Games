#include "Label.h"

LabelNew::LabelNew(const CharacterSet& characterSet, std::string text) {
	m_characterSet = &characterSet;
	m_text = text;
}

LabelNew &LabelNew::operator=(const LabelNew &rhs) {
	m_text = rhs.m_text;
	m_characterSet = rhs.m_characterSet;
}

LabelNew::LabelNew(LabelNew const& rhs) : m_characterSet(rhs.m_characterSet) {
	m_text = rhs.m_text;
}

void LabelNew::draw() {
	draw(m_text);
}

void LabelNew::draw(std::string text) {

	glBindTexture(GL_TEXTURE_2D, m_characterSet->spriteSheet);

	Batchrenderer::get().setShader(Globals::shaderManager.getAssetPointer("font"));
	std::string::const_iterator c;
	float width = getPosX();

	for (c = text.begin(); c != text.end(); c++) {

		const Character& _c = m_characterSet->getCharacter(*c);

		Batchrenderer::get().addQuad(Vector4f(width, getPosY() , _c.size[0], _c.size[1]), Vector4f(_c.textureOffset[0], _c.textureOffset[1], _c.textureSize[0], _c.textureSize[1]), 0);
		width = width + _c.advance[0];
	}
	Batchrenderer::get().drawBuffer(false);
	glBindTexture(GL_TEXTURE_2D, 0);
	//Widget::draw();
}


int LabelNew::getWidth() const {

	int sizeX = 0;
	std::string::const_iterator c;
	for (c = m_text.begin(); c != m_text.end(); c++) {
		const Character ch = m_characterSet->getCharacter(*c);
		sizeX = sizeX + ((ch.advance[0]));		
	}
	return  sizeX;
}

int LabelNew::getHeight() const {
	//std::cout << "Label New Height: " << m_characterSet->lineHeight << std::endl;
	return m_characterSet->lineHeight;
}