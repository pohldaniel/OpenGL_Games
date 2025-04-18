#include <GL/glew.h>
#include <soil2/SOIL2.h>
#include <iostream>

#include "Spritesheet.h"
#include "Texture.h"

Spritesheet::Spritesheet(std::string fileName, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, bool reverse, bool _flipVertical, int row, int minColumn, int maxColumn, unsigned int _format) {

	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);
	unsigned internalFormat = _format == -1 && numComponents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		Texture::FlipVertical(imageData, numComponents * width, height);

	m_tileCountX = width / (tileWidth + spacing);
	m_tileCountY = height / (tileHeight + spacing);
	m_totalFrames = maxColumn > -1 ? ((maxColumn - minColumn) + 1) : m_tileCountX * m_tileCountY;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, tileWidth, tileHeight, m_totalFrames, 0, numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, tileWidth, tileHeight, m_totalFrames);

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
		unsigned char* subImage = (unsigned char*)malloc((tileWidth)* numComponents * (tileHeight));
		unsigned int subImageSize = (tileWidth)* numComponents * tileHeight;
		unsigned int count = 0, row = 0;
		unsigned int offset = width * numComponents * ((tileHeight + spacing) * posY + spacing) + posX * (tileWidth + spacing) * numComponents;
		unsigned int x = offset;

		while (count < subImageSize) {
			if (count % (tileWidth * numComponents) == 0 && count > 0) {
				row = row + width * numComponents;
				x = row + offset;

			}
			subImage[count] = imageData[x];
			x++;
			count++;
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, image, tileWidth, tileHeight, 1, numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, subImage);
		posX++;
		image++;
		free(subImage);
	}

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	SOIL_free_image_data(imageData);
}

//https://stackoverflow.com/questions/34239049/how-to-grow-a-gl-texture-2d-array
void Spritesheet::addToSpritesheet(std::string fileName, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, bool reverse, bool _flipVertical, int row, int minColumn, int maxColumn, unsigned int _format) {

	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);
	unsigned internalFormat = _format == -1 && numComponents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		Texture::FlipVertical(imageData, numComponents * width, height);

	unsigned short tileCountX = width / (tileWidth + spacing);
	unsigned short tileCountY = height / (tileHeight + spacing);
	unsigned short totalFrames = maxColumn > -1 ? ((maxColumn - minColumn) + 1) : tileCountX * tileCountY;

	unsigned int fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	unsigned int texture_new;
	glGenTextures(1, &texture_new);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
	//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, tileWidth, tileHeight, m_totalFrames + totalFrames);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, tileWidth, tileHeight, m_totalFrames + totalFrames, 0, numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	for (int layer = 0; layer < m_totalFrames; ++layer) {
		glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0, layer);
		glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 0, 0, tileWidth, tileHeight);
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
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
		unsigned char* subImage = (unsigned char*)malloc((tileWidth)* numComponents * (tileHeight));
		unsigned int subImageSize = (tileWidth)* numComponents * tileHeight;
		unsigned int count = 0, row = 0;
		unsigned int offset = width * numComponents * ((tileHeight + spacing) * posY + spacing) + posX * (tileWidth + spacing) * numComponents;
		unsigned int x = offset;

		while (count < subImageSize) {
			if (count % (tileWidth * numComponents) == 0 && count > 0) {
				row = row + width * numComponents;
				x = row + offset;

			}
			subImage[count] = imageData[x];
			x++;
			count++;
		}
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_totalFrames + image, tileWidth, tileHeight, 1, numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, subImage);
		posX++;
		image++;
		free(subImage);
	}

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	SOIL_free_image_data(imageData);

	glDeleteTextures(1, &m_texture);
	m_totalFrames = m_totalFrames + totalFrames;
	m_texture = texture_new;
}

void Spritesheet::createNullSpritesheet(unsigned int width, unsigned int height, unsigned short layer) {
	int pitch = ((width * 32 + 31) & ~31) >> 3; // align to 4-byte boundaries
	std::vector<unsigned char> pixels(pitch * height, 255);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layer);
	for (unsigned short image = 0; image < layer; image++) {
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, image, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	}

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Spritesheet::createSpritesheet(unsigned int texture, unsigned int width, unsigned int height, unsigned int _format) {
	//It seems setLinear() isn't working when using glTexStorage3D so I go for glTexImage3D. 
	//Of curse you can increase the mipmaplevel (level) but it decreases the performance.
	//Unfortunately I wasn't able to use glCopyImageSubData with glTexImage3D. 

	unsigned internalFormat = _format == -1 ? GL_RGBA8 : _format;
	m_totalFrames++;

	//OpenGL 4.3
	/*glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 8, internalFormat, width, height, m_totalFrames);
	glCopyImageSubData(texture, GL_TEXTURE_2D, 0, 0, 0, 0, m_texture, GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_totalFrames - 1, width, height, 1);*/

	//OpenGL 3.0
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, width, height, m_totalFrames, 0, internalFormat == GL_RGBA8 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);


	unsigned int fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 0, 0, width, height);
	//glCopyTextureSubImage3D(m_texture, 0, 0, 0, 0, 0, 0, width, height);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

}

void Spritesheet::addToSpritesheet(unsigned int texture, unsigned int width, unsigned int height, unsigned int _format) {
	//It seems setLinear() isn't working when using glTexStorage3D so I go for glTexImage3D. 
	//Of curse you can increase the mipmaplevel (level) but it decreases the performance.
	//Unfortunately I wasn't able to use glCopyImageSubData with glTexImage3D. 

	unsigned internalFormat = _format == -1 ? GL_RGBA8 : _format;
	m_totalFrames++;

	//OpenGL 4.3
	/*unsigned int fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	unsigned int texture_new;
	glGenTextures(1, &texture_new);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 8, internalFormat, width, height, m_totalFrames);

	for (int layer = 0; layer < m_totalFrames - 1; ++layer) {
		glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0, layer);
		glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 0, 0, width, height);
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glCopyImageSubData(texture, GL_TEXTURE_2D, 0, 0, 0, 0, texture_new, GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_totalFrames - 1, width, height, 1);*/

	//OpenGL 3.0
	unsigned int fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	unsigned int texture_new;
	glGenTextures(1, &texture_new);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, width, height, m_totalFrames, 0, internalFormat == GL_RGBA8 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);

	for (int layer = 0; layer < m_totalFrames - 1; ++layer) {
		glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0, layer);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 0, 0, width, height);
	}

	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_totalFrames - 1, 0, 0, width, height);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	

	//OpenGL 4.5
	/*unsigned int texture_new;
	glGenTextures(1, &texture_new);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, width, height, m_totalFrames, 0, internalFormat == GL_RGBA8 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	unsigned int fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	for (int layer = 0; layer < m_totalFrames - 1; ++layer) {
		glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0, layer);
		glCopyTextureSubImage3D(texture_new, 0, 0, 0, layer, 0, 0, width, height);
	}
	
	//glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture, 0, 0);
	glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture, 0);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glCopyTextureSubImage3D(texture_new, 0, 0, 0, m_totalFrames - 1, 0, 0, width, height);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);*/

	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

void Spritesheet::bind(unsigned int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
}

void Spritesheet::Unbind() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glActiveTexture(GL_TEXTURE0);
}

void Spritesheet::setRepeat() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Spritesheet::setLinear() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}