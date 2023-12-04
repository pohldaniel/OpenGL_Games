#include "textureRo.hpp"

#include <engine/Texture.h>
#include <iostream>
#include <GL/glew.h>
#include <SOIL2/SOIL2.h>

TextureRo::TextureRo(const std::string& path)
	: m_rendererID(0), m_filePath(path), m_localBuffer(nullptr),
	m_width(0), m_height(0), m_bpp(0)
{
	// Because 0,0 is bottom left in OpenGL
	//stbi_set_flip_vertically_on_load(true);
	m_localBuffer = SOIL_load_image(path.c_str(), &m_width, &m_height, &m_bpp, 4);
	if (!m_localBuffer) {
		
	}
	Texture::FlipVertical(m_localBuffer, m_width * m_bpp, m_height);

	//stbi_set_flip_vertically_on_load(false);

	glGenTextures(1, &m_rendererID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer);
	unbind();

	if (m_localBuffer) {
		SOIL_free_image_data(m_localBuffer);
	}
}

TextureRo::~TextureRo() {
	// GLCall(glDeleteTextures(1, &m_rendererID));  // Deleted in sprite factory
}

void TextureRo::bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void TextureRo::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}
