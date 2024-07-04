#include "Fontrenderer.h"

Fontrenderer Fontrenderer::s_instance;

Fontrenderer& Fontrenderer::Get() {
	return s_instance;
}

Fontrenderer::Fontrenderer() : m_batchrenderer(new Batchrenderer()) {
	batchrenderer = m_batchrenderer;
}

Fontrenderer::~Fontrenderer() {
	if (batchrenderer == m_batchrenderer) {
		delete m_batchrenderer;
		m_batchrenderer = batchrenderer = nullptr;
	}else {
		delete batchrenderer;
		batchrenderer = nullptr;
	}
}

void Fontrenderer::shutdown() {
	m_batchrenderer->shutdown();
}

void Fontrenderer::init(size_t size, bool drawSingle)  {
	m_batchrenderer->init(size, drawSingle);
}

void Fontrenderer::setShader(Shader* shader) {
	m_batchrenderer->setShader(shader);
}

void Fontrenderer::setRenderer(Batchrenderer* renderer) {
	batchrenderer = m_batchrenderer;
	m_batchrenderer = renderer;
}

void Fontrenderer::resetRenderer() {
	m_batchrenderer = batchrenderer;
	batchrenderer = nullptr;
}

void Fontrenderer::drawText(const CharacterSet& characterSet, float posX, float posY, std::string text, Vector4f color, float size) {
	glBindTexture(GL_TEXTURE_2D_ARRAY, characterSet.spriteSheet);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {

		const Char& ch = characterSet.getCharacter(*c);

		m_batchrenderer->addQuadAA(Vector4f(posX, posY, static_cast<float>(ch.size[0]) * size, static_cast<float>(ch.size[1])) * size, Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]), color, characterSet.frame);
		posX = posX + ch.advance * size;
	}
	m_batchrenderer->drawBuffer();
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Fontrenderer::addText(const CharacterSet& characterSet, float posX, float posY, std::string text, Vector4f color, float size) {
	std::string::const_iterator c;

	for (c = text.begin(); c != text.end(); c++) {

		const Char& ch = characterSet.getCharacter(*c);

		m_batchrenderer->addQuadAA(Vector4f(posX, posY, static_cast<float>(ch.size[0]) * size, static_cast<float>(ch.size[1]) * size), Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]), color, characterSet.frame);
		posX = posX + ch.advance * size;
	}
}

void Fontrenderer::drawBuffer() {
	m_batchrenderer->drawBuffer();
}

void Fontrenderer::bindTexture(const CharacterSet& characterSet) {
	glBindTexture(GL_TEXTURE_2D_ARRAY, characterSet.spriteSheet);
}

void Fontrenderer::unbindTexture() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

std::string Fontrenderer::floatToString(float val, int precision) {
	static const int bufSize = 100;
	static char buffer[bufSize];
	std::snprintf(buffer, bufSize, "%.*f", precision, val);
	return std::string(buffer);
}

std::string Fontrenderer::FloatToString(float val, int precision) {
	static const int bufSize = 100;
	static char buffer[bufSize];
	std::snprintf(buffer, bufSize, "%.*f", precision, val);
	return std::string(buffer);
}

Batchrenderer* Fontrenderer::getBatchRenderer() {
	return m_batchrenderer;
}