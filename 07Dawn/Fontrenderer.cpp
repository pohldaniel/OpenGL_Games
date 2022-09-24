#include "Fontrenderer.h"

Fontrenderer Fontrenderer::s_instance;

void Fontrenderer::setCharacterSet(const CharacterSet& characterSet) {
	m_characterSet = &characterSet;
}

void Fontrenderer::setShader(Shader* shader) {
	m_shader = shader;
}

Fontrenderer& Fontrenderer::get() {
	return s_instance;
}

Fontrenderer::~Fontrenderer() {

}

void Fontrenderer::drawText(int posX, int posY, std::string text) {

	
	glBindTexture(GL_TEXTURE_2D, m_characterSet->spriteSheet);

	Batchrenderer::get().setShader(m_shader);
	std::string::const_iterator c;
	

	for (c = text.begin(); c != text.end(); c++) {

		const Char& ch = m_characterSet->getCharacter(*c);

		Batchrenderer::get().addQuad(Vector4f(posX, posY, ch.size[0], ch.size[1]), Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]));
		posX = posX + ch.advance[0];
	}
	Batchrenderer::get().drawBuffer(false);
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Fontrenderer::getFontHeight() {
	return m_characterSet->lineHeight;
}

std::string Fontrenderer::FloatToString(float val, int precision) {
	static const int bufSize = 100;
	static char buffer[bufSize];

	std::snprintf(buffer, bufSize, "%.*f", precision, val);

	return std::string(buffer);
}
