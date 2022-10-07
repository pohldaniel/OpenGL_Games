#include "Fontrenderer.h"

Fontrenderer Fontrenderer::s_instance;

Fontrenderer& Fontrenderer::get() {
	return s_instance;
}

Fontrenderer::Fontrenderer() : m_batchrenderer(new Batchrenderer()) {}

Fontrenderer::~Fontrenderer() {
	delete m_batchrenderer;
}

void Fontrenderer::init(size_t size)  {
	m_batchrenderer->init(size);
}

void Fontrenderer::setShader(Shader* shader) {
	m_batchrenderer->setShader(shader);
}

void Fontrenderer::setCamera(const Camera& camera) {
	m_batchrenderer->setCamera(camera);
}

void Fontrenderer::updateModelMtx(const Matrix4f& mtx) {
	m_batchrenderer->updateModelMtx(mtx);
}

void Fontrenderer::drawText(const CharacterSet& characterSet, int posX, int posY, std::string text, Vector4f color, bool updateView) {
	glBindTexture(GL_TEXTURE_2D, characterSet.spriteSheet);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {

		const Char& ch = characterSet.getCharacter(*c);

		m_batchrenderer->addQuad(Vector4f(static_cast<float>(posX), static_cast<float>(posY), static_cast<float>(ch.size[0]), static_cast<float>(ch.size[1])), Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]), color, 0 , updateView);
		posX = posX + ch.advance;
	}
	m_batchrenderer->drawBuffer(updateView);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Fontrenderer::addText(const CharacterSet& characterSet, int posX, int posY, std::string text, Vector4f color, bool updateView) {
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {

		const Char& ch = characterSet.getCharacter(*c);

		m_batchrenderer->addQuad(Vector4f(static_cast<float>(posX), static_cast<float>(posY), static_cast<float>(ch.size[0]), static_cast<float>(ch.size[1])), Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]), color, 0, updateView);
		posX = posX + ch.advance;
	}
}

void Fontrenderer::drawBuffer(bool updateView) {
	m_batchrenderer->drawBuffer(updateView);
}

void Fontrenderer::bindTexture(const CharacterSet& characterSet) {
	glBindTexture(GL_TEXTURE_2D, characterSet.spriteSheet);
}

void Fontrenderer::unbindTexture() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::string Fontrenderer::FloatToString(float val, int precision) {
	static const int bufSize = 100;
	static char buffer[bufSize];

	std::snprintf(buffer, bufSize, "%.*f", precision, val);

	return std::string(buffer);
}
