#include <iostream>
#include "Extension.h"
#include "Spritesheet.h"

#include "..\stb\stb_image.h"

Spritesheet::Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, bool reverse, bool flipVertical, int row, int maxColumn, unsigned int _format) {
	unsigned format = _format == -1 ? GL_RGBA8 : _format;

	stbi_set_flip_vertically_on_load(flipVertical);
	int width, height, numComponents;
	unsigned char* imageData = stbi_load(pictureFile.c_str(), &width, &height, &numComponents, NULL);

	m_tileCountX = width / tileWidth;
	m_tileCountY = height / tileHeight;
	m_totalFrames = maxColumn > -1 ? (maxColumn + 1) : m_tileCountX * m_tileCountY;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, tileWidth, tileHeight, totalFrames, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, format, tileWidth, tileHeight, m_totalFrames);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, tileWidth, tileHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//default row = 0
	unsigned short image = 0;
	unsigned short posX = 0;
	unsigned short posY = reverse ? (m_tileCountY - 1) - row : row;

	while (image < m_totalFrames) {
		if (image % m_tileCountX == 0 && image > 0) {
			if (reverse) posY--; else posY++;
			posX = 0;
		}
		unsigned char* subImage = (unsigned char*)malloc(tileWidth * numComponents * tileHeight);
		unsigned int subImageSize = tileWidth * numComponents * tileHeight;
		unsigned int count = 0, row = 0;
		unsigned int x = width * numComponents * tileHeight * posY + posX * tileWidth * numComponents;

		while (count < subImageSize) {
			if (count % (tileWidth * numComponents) == 0 && count > 0) {
				row = row + width * numComponents;
				x = row + width * numComponents * tileHeight * posY + posX * tileWidth * numComponents;
			}
			subImage[count] = imageData[x];
			x++;
			count++;
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, image, tileWidth, tileHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, subImage);
		posX++;
		image++;		
		free(subImage);
	}
	//glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	stbi_image_free(imageData);
}

unsigned int Spritesheet::getAtlas() {
	return m_texture;
}

unsigned short Spritesheet::getTileCountX() {
	return m_tileCountX;
}

unsigned short Spritesheet::getTileCountY() {
	return m_tileCountX;
}

unsigned short Spritesheet::getTotalFrames() {
	return m_totalFrames;
}

Spritesheet::~Spritesheet(){
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}