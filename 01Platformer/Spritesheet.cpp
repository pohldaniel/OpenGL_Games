#include "stb\stb_image.h"
#include "Spritesheet.h"
#include "Extension.h"

Spritesheet::Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, bool reverse, bool flipVertical) {

	stbi_set_flip_vertically_on_load(flipVertical);
	int width, height, numComponents;
	unsigned char* imageData = stbi_load(pictureFile.c_str(), &width, &height, &numComponents, NULL);

	unsigned short tileCountX = width / tileWidth;
	unsigned short tileCountY = height / tileHeight;
	unsigned short totalFrames = tileCountX * tileCountY;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, tileWidth, tileHeight, totalFrames, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileWidth, tileHeight, totalFrames);

	unsigned short image = 0;
	unsigned short posX = 0;
	unsigned short posY = reverse ? tileCountY - 1 : 0;

	while (image < totalFrames) {
		if (image % tileCountX == 0 && image > 0) {
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
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
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