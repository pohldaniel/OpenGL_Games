#include "texture-array.hpp"

#include <iostream>
#include <SOIL2/SOIL2.h>

TextureArray::TextureArray(const std::string& path, GLsizei tileWidth, GLsizei tileHeight)
	: m_rendererID(0), m_filePath(path), m_localBuffer(nullptr),
	m_width(0), m_height(0), m_bpp(0)
{
	// Because the UV map is inverted for texture arrays
	//SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_FLAG);
	m_localBuffer = SOIL_load_image(path.c_str(), &m_width, &m_height, &m_bpp, 4u);
	if (!m_localBuffer) {
		
	}

	GLsizei tilesX = m_width / tileWidth;
	GLsizei tilesY = m_height / tileHeight;
	GLsizei tileCount = tilesX * tilesY;
	
	glGenTextures(1, &m_rendererID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_rendererID);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileWidth, tileHeight, tileCount);
	
	{
		GLuint tempTexture = 0;
		glGenTextures(1, &tempTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tempTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer);

		for (GLsizei i = 0; i < tileCount; ++i) {
			GLint x = (i % tilesX) * tileWidth, y = (i / tilesX) * tileHeight;
			glCopyImageSubData(tempTexture, GL_TEXTURE_2D, 0, x, y, 0, m_rendererID, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tileWidth, tileHeight, 1);
		}
		glDeleteTextures(1, &tempTexture);
	}

	unbind();

	if (m_localBuffer) {
		SOIL_free_image_data(m_localBuffer);
	}
}

TextureArray::~TextureArray() {
	// GLCall(glDeleteTextures(1, &m_rendererID)); // Deleted in sprite factory
}

void TextureArray::bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_rendererID);
}

void TextureArray::unbind() const {
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
