#include <soil2\SOIL2.h>
#include <iostream>

#include "Texture.h"
#include "Extension.h"

Texture::Texture(std::string fileName, const bool _flipVertical, unsigned int _format) {
	
	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);
	unsigned int internalFormat = _format == -1 && numComponents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

	if(_flipVertical)
		flipVertical(imageData, numComponents * width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numComponents;
}

void Texture::flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
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

void Texture::FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
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

Texture::~Texture() {
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}

void Texture::loadFromFile(std::string fileName, const bool _flipVertical, unsigned int _format) {
	
	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);
	unsigned internalFormat = _format == -1 && numComponents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		flipVertical(imageData, numComponents * width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numComponents;
}

void Texture::loadFromFile(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int _posY, unsigned int _posX, const bool _flipVertical, unsigned int _format) {
	
	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);
	unsigned internalFormat = _format == -1 && numComponents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		flipVertical(imageData, numComponents * width, height);

	unsigned short tileCountY = height / (tileHeight + spacing);
	unsigned short posX = _posX;
	unsigned short posY = _flipVertical ? (tileCountY - 1) - _posY : _posY;

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

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tileWidth, tileHeight, 0, numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, subImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(subImage);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numComponents;
}

void Texture::loadFromFile(std::string pictureFile, unsigned int _offsetX, unsigned int _offsetY, unsigned int _width, unsigned int _height, const bool _flipVertical, unsigned int _format) {

	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);
	unsigned internalFormat = _format == -1 && numComponents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		flipVertical(imageData, numComponents * width, height);

	unsigned int offsetY = _flipVertical ? (height - (_offsetY + _height)) : _offsetY;

	unsigned char* subImage = (unsigned char*)malloc(_width * numComponents * _height);
	unsigned int subImageSize = _width * numComponents * _height;
	unsigned int count = 0, row = 0;
	unsigned int offset = width * numComponents * offsetY + numComponents * _offsetX;
	unsigned int x = offset;

	while (count < subImageSize) {
		if (count % (_width * numComponents) == 0 && count > 0) {
			row = row + width * numComponents;
			x = row + offset;
		}
		subImage[count] = imageData[x];
		x++;
		count++;
	}

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, numComponents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, subImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(subImage);
	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numComponents;
}

void Texture::createNullTexture(unsigned int width, unsigned int height, unsigned int color) {
	int pitch = ((width * 32 + 31) & ~31) >> 3; // align to 4-byte boundaries
	std::vector<unsigned char> pixels(pitch * height, color);
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

	glBindTexture(GL_TEXTURE_2D, 0);

	m_width = width;
	m_height = height;
	m_channels = 4;
}

//https://github.com/sol-prog/Perlin_Noise
void Texture::createPerlinTexture(unsigned int width, unsigned int height, unsigned int seed) {
	PerlinNoise pn = seed ? PerlinNoise(seed) : PerlinNoise();
	
	unsigned short numComponents = 4;
	unsigned char* pixels = (unsigned char*)malloc(width * numComponents * height);

	for (unsigned int i = 0; i < height; ++i) {     // y
		for (unsigned int j = 0; j < width; ++j) {  // x
			double x = (double)j / ((double)width);
			double y = (double)i / ((double)height);

			// Typical Perlin noise
			double n = pn.noise(10 * x, 10 * y, 0.8);

			// Wood like structure
			//n = 20 * pn.noise(x, y, 0.8);
			//n = n - floor(n);
		
			pixels[i * numComponents * width + j * numComponents + 0] = floor(255 * n);
			pixels[i * numComponents * width + j * numComponents + 1] = floor(255 * n);
			pixels[i * numComponents * width + j * numComponents + 2] = floor(255 * n);
			pixels[i * numComponents * width + j * numComponents + 3] = 255;
		}
	}
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

	glBindTexture(GL_TEXTURE_2D, 0);

	m_width = width;
	m_height = height;
	m_channels = 4;

	free(pixels);
}

void Texture::CutSubimage(std::string fileIn, std::string fileOut, unsigned int _offsetX, unsigned int _offsetY, unsigned int _width, unsigned int _height, const bool _flipVertical) {

	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(fileIn.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);

	if (_flipVertical)
		Texture::FlipVertical(imageData, numComponents * width, height);

	unsigned int offsetY = _flipVertical ? (height - (_offsetY + _height)) : _offsetY;

	unsigned char* subImage = (unsigned char*)malloc(_width * numComponents * _height);
	unsigned int subImageSize = _width * numComponents * _height;
	unsigned int count = 0, row = 0;
	unsigned int offset = width * numComponents * offsetY + numComponents * _offsetX;
	unsigned int x = offset;

	while (count < subImageSize) {
		if (count % (_width * numComponents) == 0 && count > 0) {
			row = row + width * numComponents;
			x = row + offset;
		}
		subImage[count] = imageData[x];
		x++;
		count++;
	}

	SOIL_save_image(fileOut.c_str(), SOIL_SAVE_TYPE_PNG, _width, _height, numComponents, subImage);

	free(subImage);

	SOIL_free_image_data(imageData);
}

void Texture::AddHorizontally(std::string fileIn1, std::string fileIn2, std::string fileOut, const bool _flipVertical) {
	
	int width1, height1, numComponents1;
	unsigned char* imageData1 = SOIL_load_image(fileIn1.c_str(), &width1, &height1, &numComponents1, SOIL_LOAD_AUTO);
	
	if (_flipVertical)
		Texture::FlipVertical(imageData1, numComponents1 * width1, height1);
	
	unsigned int imageSize1 = width1 * numComponents1 * height1;

	int width2, height2, numComponents2;
	unsigned char* imageData2 = SOIL_load_image(fileIn2.c_str(), &width2, &height2, &numComponents2, SOIL_LOAD_AUTO);

	if (_flipVertical)
		Texture::FlipVertical(imageData2, numComponents2 * width2, height2);

	unsigned char* image = (unsigned char*)malloc((width1 + width2) * numComponents1 * height1);
	unsigned int imageSize = (width1 + width2 ) * numComponents1 * (height1);

	unsigned char imageData;
	unsigned int count = 0, x = 0, y = 0, row = 0;
	unsigned int rowWidth = (width1 + width2) * numComponents1;
	bool toggle = false;

	while (count < imageSize) {
	
		if (row == rowWidth) {
			row = 0;
		}
		
		if (row < width1 * numComponents1) {
			toggle = false;
		}else {
			toggle = true;
		}

		if (!toggle) {
			imageData = imageData1[x];
			x++;
		}else {
			imageData = imageData2[y];
			y++;
		}
		image[count] = imageData;
		count++;
		row++;
	}

	SOIL_save_image(fileOut.c_str(), SOIL_SAVE_TYPE_PNG, width1 + width2, height1, numComponents1, image);

	free(image);
	SOIL_free_image_data(imageData1);
	SOIL_free_image_data(imageData2);
}

unsigned char* Texture::LoadFromFile(std::string pictureFile, const bool _flipVertical) {
	int width, height, numComponents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);
	
	if (_flipVertical)
		FlipVertical(imageData, numComponents * width, height);

	return imageData;
}

unsigned int Texture::getTexture(){
	return m_texture;
}

unsigned int Texture::getWidth(){
	return m_width;
}

unsigned int Texture::getHeight() {
	return m_height;
}

unsigned int Texture::getChannels() {
	return m_channels;
}

void Texture::bind(unsigned int unit){
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

void Texture::setRepeat() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setLinear() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}
