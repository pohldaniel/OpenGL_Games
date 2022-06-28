#include "Texture.h"
#include "Extension.h"
#include "..\soil2\SOIL2.h"
#include <iostream>

Texture::Texture(std::string fileName, const bool _flipVertical, unsigned int _format) {
	unsigned format = _format == -1 ? GL_RGBA8 : _format;
	
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);

	if(_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);
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

void Texture::loadFromFile(std::string pictureFile, const bool _flipVertical, const bool linear, unsigned int _format) {
	
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	unsigned format = _format == -1 && numCompontents == 3 ? GL_RGB : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, numCompontents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	if (linear) glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);
}

void Texture::loadFromFile(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int _row, unsigned int column, const bool _flipVertical, bool reverse, unsigned int _format, const bool linear) {
	
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	unsigned format = _format == -1 && numCompontents == 3 ? GL_RGB : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	unsigned short tileCountY = height / (tileHeight + spacing);
	unsigned short posX = column;
	unsigned short posY = reverse ? (tileCountY - 1) - _row : _row;

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

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, format, tileWidth, tileHeight, 0, numCompontents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, subImage);
	if (linear) glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(subImage);

	SOIL_free_image_data(imageData);
}

void Texture::loadFromFile(std::string pictureFile, unsigned int _offsetX, unsigned int _offsetY, unsigned int _width, unsigned int _height, const bool _flipVertical, unsigned int _format, const bool linear) {

	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	unsigned format = _format == -1 && numCompontents == 3 ? GL_RGB : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	unsigned int offsetY = _flipVertical ? (height - (_offsetY + _height)) : _offsetY;

	unsigned char* subImage = (unsigned char*)malloc(_width * numCompontents * _height);
	unsigned int subImageSize = _width * numCompontents * _height;
	unsigned int count = 0, row = 0;
	unsigned int offset = width * numCompontents * offsetY + numCompontents * _offsetX;
	unsigned int x = offset;

	while (count < subImageSize) {
		if (count % (_width * numCompontents) == 0 && count > 0) {
			row = row + width * numCompontents;
			x = row + offset;
		}
		subImage[count] = imageData[x];
		x++;
		count++;
	}

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, numCompontents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, subImage);
	if (linear) glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(subImage);
	SOIL_free_image_data(imageData);
}

void Texture::CutSubimage(std::string fileIn, std::string fileOut, unsigned int _offsetX, unsigned int _offsetY, unsigned int _width, unsigned int _height, const bool _flipVertical) {

	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileIn.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);

	if (_flipVertical)
		Texture::FlipVertical(imageData, numCompontents * width, height);

	unsigned int offsetY = _flipVertical ? (height - (_offsetY + _height)) : _offsetY;

	unsigned char* subImage = (unsigned char*)malloc(_width * numCompontents * _height);
	unsigned int subImageSize = _width * numCompontents * _height;
	unsigned int count = 0, row = 0;
	unsigned int offset = width * numCompontents * offsetY + numCompontents * _offsetX;
	unsigned int x = offset;

	while (count < subImageSize) {
		if (count % (_width * numCompontents) == 0 && count > 0) {
			row = row + width * numCompontents;
			x = row + offset;
		}
		subImage[count] = imageData[x];
		x++;
		count++;
	}

	SOIL_save_image(fileOut.c_str(), SOIL_SAVE_TYPE_PNG, _width, _height, numCompontents, subImage);

	free(subImage);

	SOIL_free_image_data(imageData);
}

void Texture::AddHorizontally(std::string fileIn1, std::string fileIn2, std::string fileOut, const bool _flipVertical) {
	
	int width1, height1, numCompontents1;
	unsigned char* imageData1 = SOIL_load_image(fileIn1.c_str(), &width1, &height1, &numCompontents1, SOIL_LOAD_AUTO);
	
	if (_flipVertical)
		Texture::FlipVertical(imageData1, numCompontents1 * width1, height1);
	
	unsigned int imageSize1 = width1 * numCompontents1 * height1;

	int width2, height2, numCompontents2;
	unsigned char* imageData2 = SOIL_load_image(fileIn2.c_str(), &width2, &height2, &numCompontents2, SOIL_LOAD_AUTO);

	if (_flipVertical)
		Texture::FlipVertical(imageData2, numCompontents2 * width2, height2);

	unsigned char* image = (unsigned char*)malloc((width1 + width2) * numCompontents1 * height1);
	unsigned int imageSize = (width1 + width2 ) * numCompontents1 * (height1);

	unsigned char imageData;
	unsigned int count = 0, x = 0, y = 0, row = 0;
	unsigned int rowWidth = (width1 + width2) * numCompontents1;
	bool toggle = false;

	while (count < imageSize) {
	
		if (row == rowWidth) {
			row = 0;
		}
		
		if (row < width1 * numCompontents1) {
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

	SOIL_save_image(fileOut.c_str(), SOIL_SAVE_TYPE_PNG, width1 + width2, height1, numCompontents1, image);

	free(image);
	SOIL_free_image_data(imageData1);
	SOIL_free_image_data(imageData2);
}

unsigned int Texture::getTexture(){
	return m_texture;
}

void Texture::bind(unsigned int unit){
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::setRepeat() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}
