#include <iostream>
#include "Extension.h"
#include "Spritesheet.h"

#include "..\stb\stb_image.h"

Spritesheet::Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, bool reverse, bool flipVertical, int row, int minColumn, int maxColumn, unsigned int _format) {
	unsigned format = _format == -1 ? GL_RGBA8 : _format;

	stbi_set_flip_vertically_on_load(flipVertical);
	int width, height, numCompontents;
	unsigned char* imageData = stbi_load(pictureFile.c_str(), &width, &height, &numCompontents, NULL);

	m_tileCountX = width / (tileWidth + spacing);
	m_tileCountY = height / (tileHeight + spacing);
	m_totalFrames = maxColumn > -1 ? ((maxColumn - minColumn) + 1) : m_tileCountX * m_tileCountY;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, tileWidth, tileHeight, totalFrames, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, format, tileWidth, tileHeight, m_totalFrames);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, tileWidth, tileHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//default row = 0
	row = row > 0 ? row - 1 : row;
	unsigned short image = 0;
	unsigned short posX = minColumn > 0 ? minColumn - 1 : 0;
	unsigned short posY = reverse ? (m_tileCountY - 1) - row : row;

	while (image < m_totalFrames) {
		if (image % m_tileCountX == 0 && image > 0) {
			if (reverse) posY--; else posY++;
			posX = minColumn > 0 ? minColumn - 1 : 0;
		}
		unsigned char* subImage = (unsigned char*)malloc((tileWidth)* numCompontents * (tileHeight));
		unsigned int subImageSize = (tileWidth)* numCompontents * tileHeight;
		unsigned int count = 0, row = 0;
		unsigned int offset = width * numCompontents * ((tileHeight + spacing) * posY + spacing) + posX * (tileWidth + spacing) * numCompontents;
		unsigned int x = offset;

		while (count < subImageSize) {
			if (count % (tileWidth * numCompontents) == 0 && count > 0) {
				row = row + width * numCompontents;
				x = row + offset;

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
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	stbi_image_free(imageData);
}

//https://stackoverflow.com/questions/34239049/how-to-grow-a-gl-texture-2d-array
void Spritesheet::addToSpritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, bool reverse, bool flipVertical, int row, int minColumn, int maxColumn, unsigned int _format) {
	unsigned format = _format == -1 ? GL_RGBA8 : _format;

	stbi_set_flip_vertically_on_load(flipVertical);
	int width, height, numCompontents;
	unsigned char* imageData = stbi_load(pictureFile.c_str(), &width, &height, &numCompontents, NULL);

	unsigned short tileCountX = width / (tileWidth + spacing);
	unsigned short tileCountY = height / (tileHeight + spacing);
	unsigned short totalFrames = maxColumn > -1 ? ((maxColumn - minColumn) + 1) : tileCountX * tileCountY;

	unsigned int fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	unsigned int texture_new;
	glGenTextures(1, &texture_new);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, format, tileWidth, tileHeight, m_totalFrames + totalFrames);

	for (int layer = 0; layer < m_totalFrames; ++layer) {
		glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0, layer);
		glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 0, 0, tileWidth, tileHeight);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

	row = row > 0 ? row - 1 : row;
	unsigned short image = 0;
	unsigned short posX = minColumn > 0 ? minColumn - 1 : 0;
	unsigned short posY = reverse ? (tileCountY - 1) - row : row;

	while (image < totalFrames) {
		if (image % tileCountX == 0 && image > 0) {
			if (reverse) posY--; else posY++;
			posX = minColumn > 0 ? minColumn - 1 : 0;
		}
		unsigned char* subImage = (unsigned char*)malloc((tileWidth)* numCompontents * (tileHeight));
		unsigned int subImageSize = (tileWidth)* numCompontents * tileHeight;
		unsigned int count = 0, row = 0;
		unsigned int offset = width * numCompontents * ((tileHeight + spacing) * posY + spacing) + posX * (tileWidth + spacing) * numCompontents;
		unsigned int x = offset;

		while (count < subImageSize) {
			if (count % (tileWidth * numCompontents) == 0 && count > 0) {
				row = row + width * numCompontents;
				x = row + offset;

			}
			subImage[count] = imageData[x];
			x++;
			count++;
		}
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_totalFrames + image, tileWidth, tileHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, subImage);
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

	glDeleteTextures(1, &m_texture);
	m_totalFrames = m_totalFrames + totalFrames;
	m_texture = texture_new;
}

void Spritesheet::createSpritesheet(unsigned int texture, unsigned int width, unsigned int height, unsigned int _format) {
	unsigned format = _format == -1 ? GL_RGBA8 : _format;
	m_totalFrames++;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, format, width, height, m_totalFrames);
	glCopyImageSubData(texture, GL_TEXTURE_2D, 0, 0, 0, 0, m_texture, GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_totalFrames - 1, width, height, 1);

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Spritesheet::addToSpritesheet(unsigned int texture, unsigned int width, unsigned int height, unsigned int _format) {
	unsigned format = _format == -1 ? GL_RGBA8 : _format;
	m_totalFrames++;

	unsigned int fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	unsigned int texture_new;
	glGenTextures(1, &texture_new);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, format, width, height, m_totalFrames);

	for (int layer = 0; layer < m_totalFrames - 1; ++layer) {
		glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0, layer);
		glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 0, 0, width, height);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

	glCopyImageSubData(texture, GL_TEXTURE_2D, 0, 0, 0, 0, texture_new, GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_totalFrames - 1, width, height, 1);

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glDeleteTextures(1, &m_texture);
	m_texture = texture_new;
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

Spritesheet::~Spritesheet() {
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}