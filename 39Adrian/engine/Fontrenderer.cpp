#include "Fontrenderer.h"
#include "Texture.h"

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

	renderTarget.create(1024u, 768u);
	renderTarget.attachTexture2D(AttachmentTex::RGBA);

	blitTarget.create();
	blitTarget.attachTexture2D(AttachmentTex::RGBA);
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

void Fontrenderer::drawText(const CharacterSet& characterSet, float posX, float posY, std::string text, Vector4f color, float size, bool flipGlyph) {
	glBindTexture(GL_TEXTURE_2D_ARRAY, characterSet.spriteSheet);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {

		const Char& ch = characterSet.getCharacter(*c);

		m_batchrenderer->addQuadAA(Vector4f(posX, posY, static_cast<float>(ch.size[0]) * size, static_cast<float>(ch.size[1])) * size, flipGlyph ? Vector4f(ch.textureOffset[0], ch.textureOffset[1] + ch.textureSize[1], ch.textureSize[0], -ch.textureSize[1]) : Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]), color, characterSet.frame);
		posX = posX + ch.advance * size;
	}
	m_batchrenderer->drawBuffer();
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Fontrenderer::addText(const CharacterSet& characterSet, float posX, float posY, std::string text, Vector4f color, float size, bool flipGlyph) {
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		const Char& ch = characterSet.getCharacter(*c);
		m_batchrenderer->addQuadAA(Vector4f(posX, posY, static_cast<float>(ch.size[0]) * size, static_cast<float>(ch.size[1]) * size), flipGlyph ? Vector4f(ch.textureOffset[0], ch.textureOffset[1] + ch.textureSize[1], ch.textureSize[0], -ch.textureSize[1]) : Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]), color, characterSet.frame);
		posX = posX + ch.advance * size;
	}
}

void Fontrenderer::addTextTransformed(const CharacterSet& characterSet, const Matrix4f& transformation, std::string text, Vector4f color, float size) {
	Vector2f verices[4];
	const Vector3f scale = transformation.getScale();
	float sx = (1.0f / scale[0]) * size;
	float sy = (1.0f / scale[1]) * size;

	std::string::const_iterator c;
	float offset = 0.0f;
	for (c = text.begin(); c != text.end(); c++) {
		const Char& ch = characterSet.getCharacter(*c);
		
		verices[0] = transformation ^ Vector3f(offset, 0.0f, 0.0f);
		verices[1] = transformation ^ Vector3f(offset + sx * static_cast<float>(ch.size[0]), 0.0f,                                0.0f);
		verices[2] = transformation ^ Vector3f(offset + sx * static_cast<float>(ch.size[0]), sy * static_cast<float>(ch.size[1]), 0.0f);
		verices[3] = transformation ^ Vector3f(offset,                                       sy * static_cast<float>(ch.size[1]), 0.0f);			
		m_batchrenderer->addQuad(verices, Vector4f(ch.textureOffset[0], ch.textureOffset[1], ch.textureSize[0], ch.textureSize[1]), color, characterSet.frame);
		offset = offset + ch.advance * sx;
	}
}

void Fontrenderer::drawBuffer() {
	m_batchrenderer->drawBuffer();
}

void Fontrenderer::blitText(int widthDst, int heightDst, int paddingX, int paddingY) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	Rect blitRect;
	m_batchrenderer->getBlitRect(blitRect);
	renderTarget.bind();
	glClear(GL_COLOR_BUFFER_BIT);
	m_batchrenderer->drawBuffer();
	renderTarget.unbind();


	blitTarget.resize(widthDst, heightDst);
	renderTarget.bindRead();
	blitTarget.bindWrite();
	glBlitFramebuffer(blitRect.posX - paddingX / 2, blitRect.posY - paddingY / 2, blitRect.width + paddingX / 2, blitRect.height + paddingY / 2, 0, 0, widthDst, heightDst, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	Framebuffer::Unbind();

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
}

const unsigned int& Fontrenderer::getColorTexture(unsigned short attachment) const {
	return blitTarget.getColorTexture(attachment);
}

void Fontrenderer::bindColorTexture(unsigned short attachment, unsigned int unit, bool forceBind) const {
	if (Texture::ActiveTextures[unit] != getColorTexture(attachment) || forceBind) {
		Texture::ActiveTextures[unit] = getColorTexture(attachment);
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, getColorTexture(attachment));
	}
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