#include "Texture.h"
#include "Extension.h"
#include "..\soil2\SOIL2.h"
#include <iostream>

Texture::Texture(std::string fileName, const bool _flipVertical, unsigned int _format) {
	
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	unsigned int internalFormat = _format == -1 && numCompontents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

	if(_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, numCompontents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;
}

void Texture::flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
	std::vector<BYTE> srcPixels(padWidth * height);
	memcpy(&srcPixels[0], data, padWidth * height);

	BYTE *pSrcRow = 0;
	BYTE *pDestRow = 0;
	
	for (unsigned int i = 0; i < height; ++i) {

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

	for (unsigned int i = 0; i < height; ++i) {

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

void Texture::loadFromFile(std::string fileName, const bool _flipVertical, unsigned int _internalFormat, unsigned int _format) {

	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	unsigned internalFormat = _internalFormat == 0 && numCompontents == 3 ? GL_RGB8 : _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
	m_format = _format == 0 && numCompontents == 3 ? GL_RGB : _format == 0 ? GL_RGBA : _format;

	if (_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, m_format, GL_UNSIGNED_BYTE, imageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;

}

void Texture::loadFromFile(std::string fileName, const bool _flipVertical, unsigned int _internalFormat, unsigned int _format, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {

	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	unsigned int internalFormat = _internalFormat == 0 && numCompontents == 3 ? GL_RGB8 : _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
	m_format = _format == 0 && numCompontents == 3 ? GL_RGB : _format == 0 ? GL_RGBA : _format;

	if (_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	unsigned char* bytes = (unsigned char*)malloc(numCompontents * (height - paddingTop) * width);

	int row = 0;
	for (int i = 0; i < width * numCompontents * (height - paddingTop); i= i + numCompontents) {
		if (i % (width * numCompontents) == 0 && i> 0) {
			row = row + width * numCompontents;
		}

		for (int x = 0; x < width * numCompontents; x++) {
			if (x < paddingLeft * numCompontents) continue;

			if (x > (width - (paddingRight + paddingLeft)) * numCompontents) continue;

			for (int j = 0; j < numCompontents; j++) {
				bytes[row + (x - paddingLeft * numCompontents) + j] = imageData[row + x + j];
			}
		}
		
	}
	height = height - paddingTop ;
	width = width - (paddingRight + paddingLeft);
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, m_format, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);
	free(bytes);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;

}

void Texture::loadFromFile(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int _posY, unsigned int _posX, const bool _flipVertical, unsigned int _format) {
	
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	unsigned internalFormat = _format == -1 && numCompontents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

	if (_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	unsigned short tileCountY = height / (tileHeight + spacing);
	unsigned short posX = _posX;
	unsigned short posY = _flipVertical ? (tileCountY - 1) - _posY : _posY;

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
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tileWidth, tileHeight, 0, numCompontents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, subImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(subImage);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;
}

void Texture::loadFromFile(std::string pictureFile, unsigned int _offsetX, unsigned int _offsetY, unsigned int _width, unsigned int _height, const bool _flipVertical, unsigned int _format) {

	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	unsigned internalFormat = _format == -1 && numCompontents == 3 ? GL_RGB8 : _format == -1 ? GL_RGBA8 : _format;

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
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, numCompontents == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, subImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(subImage);
	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;
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
		
			pixels[i * numComponents * width + j * numComponents + 0] = static_cast <int>(floor(255 * n));
			pixels[i * numComponents * width + j * numComponents + 1] = static_cast <int>(floor(255 * n));
			pixels[i * numComponents * width + j * numComponents + 2] = static_cast <int>(floor(255 * n));
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

unsigned char* Texture::readPixel() {
	unsigned char* bytes = (unsigned char*)malloc(m_width * m_height * m_channels);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glGetTexImage(GL_TEXTURE_2D, 0, m_format, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
	return bytes;
}

void Texture::addAlphaChannel(unsigned int value) {
	unsigned char* bytes = (unsigned char*)malloc(m_width * m_height * m_channels);
	
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glGetTexImage(GL_TEXTURE_2D, 0, m_format, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	m_channels = 4;
	m_format = GL_RGBA;
	
	unsigned char* bytesNew = (unsigned char*)malloc(m_width * m_height * m_channels);

	for (unsigned int i = 0, k = 0; i < m_width * m_height * 4; i = i + 4, k = k + 3) {
		bytesNew[i] = bytes[k];
		bytesNew[i + 1] = bytes[k + 1];
		bytesNew[i + 2] = bytes[k + 2];
		bytesNew[i + 3] = value;
	}

	unsigned int texture_new;
	glGenTextures(1, &texture_new);
	glBindTexture(GL_TEXTURE_2D, texture_new);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytesNew);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteTextures(1, &m_texture);
	m_texture = texture_new;

	free(bytes);
	free(bytesNew);
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
	unsigned int count = 0, x = 0, y = 0;
	int row = 0;
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

unsigned char* Texture::LoadFromFile(std::string pictureFile, const bool _flipVertical) {
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(pictureFile.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	
	if (_flipVertical)
		FlipVertical(imageData, numCompontents * width, height);

	return imageData;
}

void Texture::Safe(std::string fileOut, unsigned int& texture, unsigned int width, unsigned int height, unsigned int channels, unsigned int format) {
	unsigned char* bytes = (unsigned char*)malloc(width * channels * height);

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, bytes);

	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_save_image(fileOut.c_str(), SOIL_SAVE_TYPE_PNG, width, height, channels, bytes);

	free(bytes);
}

void Texture::Safe(std::string fileOut, unsigned char* bytes, unsigned int width, unsigned int height, unsigned int channels) {
	SOIL_save_image(fileOut.c_str(), SOIL_SAVE_TYPE_PNG, width, height, channels, bytes);
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
