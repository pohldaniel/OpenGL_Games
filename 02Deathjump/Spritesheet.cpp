#include <iostream>
#include "Spritesheet.h"
#include "Extension.h"
#include "stb\stb_image.h"

Spritesheet::Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, bool reverse, bool flipVertical, int row, int maxColumn) {
	stbi_set_flip_vertically_on_load(flipVertical);
	int width, height, numCompontents;
	unsigned char* imageData = stbi_load(pictureFile.c_str(), &width, &height, &numCompontents, NULL);

	unsigned short tileCountX = width / tileWidth;
	unsigned short tileCountY = height / tileHeight;
	unsigned short totalFrames = maxColumn > -1 ? maxColumn : tileCountX * tileCountY;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, tileWidth, tileHeight, totalFrames, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileWidth, tileHeight, totalFrames);

	//default row = 0
	unsigned short image = 0;
	unsigned short posX = 0;
	unsigned short posY = reverse ? (tileCountY - 1) - row : row;

	while (image < totalFrames) {
		if (image % tileCountX == 0 && image > 0) {
			if (reverse) posY--; else posY++;
			posX = 0;
		}
		unsigned char* subImage = (unsigned char*)malloc(tileWidth * numCompontents * tileHeight);
		unsigned int subImageSize = tileWidth * numCompontents * tileHeight;
		unsigned int count = 0, row = 0;
		unsigned int x = width * numCompontents * tileHeight * posY + posX * tileWidth * numCompontents;

		while (count < subImageSize) {
			if (count % (tileWidth * numCompontents) == 0 && count > 0) {
				row = row + width * numCompontents;
				x = row + width * numCompontents * tileHeight * posY + posX * tileWidth * numCompontents;
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
	return tileCountX;
}

unsigned short Spritesheet::getTileCountY() {
	return tileCountX;
}

Spritesheet::~Spritesheet(){
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}