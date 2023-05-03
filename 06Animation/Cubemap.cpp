#include "Cubemap.h"
#include "engine\Extension.h"
#include "soil2\SOIL2.h"
#include <iostream>
Cubemap::~Cubemap() {
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}

void Cubemap::flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
	std::vector<BYTE> srcPixels(padWidth * height);
	memcpy(&srcPixels[0], data, padWidth * height);

	BYTE *pSrcRow = 0;
	BYTE *pDestRow = 0;

	for (int i = 0; i < height; ++i) {

		pSrcRow = &srcPixels[(height - 1 - i) * padWidth];
		pDestRow = &data[i * padWidth];
		memcpy(pDestRow, pSrcRow, padWidth);
	}
}

void Cubemap::FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
	std::vector<BYTE> srcPixels(padWidth * height);
	memcpy(&srcPixels[0], data, padWidth * height);

	BYTE *pSrcRow = 0;
	BYTE *pDestRow = 0;

	for (int i = 0; i < height; ++i) {

		pSrcRow = &srcPixels[(height - 1 - i) * padWidth];
		pDestRow = &data[i * padWidth];
		memcpy(pDestRow, pSrcRow, padWidth);
	}
}

void Cubemap::loadFromFile(std::string* textureFiles, const bool _flipVertical, unsigned int _format) {
	int width, height, numCompontents;
	unsigned char* imageData;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

	for (unsigned short i = 0; i < 6; i++) {
		imageData = SOIL_load_image(textureFiles[i].c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
		unsigned internalFormat = _format == -1 && numCompontents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;
		
		if (_flipVertical)
			flipVertical(imageData, numCompontents * width, height);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, numCompontents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, imageData);
			
		SOIL_free_image_data(imageData);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;

}

void Cubemap::createNullCubemap(unsigned int width, unsigned int height, unsigned int color) {
	int pitch = ((width * 32 + 31) & ~31) >> 3; // align to 4-byte boundaries
	std::vector<unsigned char> pixels(pitch * height, color);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

	for (unsigned short i = 0; i < 6; i++) {
	
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	m_width = width;
	m_height = height;
	m_channels = 4;
}

unsigned int Cubemap::getCubemap() {
	return m_texture;
}

unsigned int Cubemap::getWidth() {
	return m_width;
}

unsigned int Cubemap::getHeight() {
	return m_height;
}

unsigned int Cubemap::getChannels() {
	return m_channels;
}

void Cubemap::bind(unsigned int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}

void Cubemap::Unbind() {
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE0);
}

void Cubemap::setRepeat() {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::setNearest() {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
