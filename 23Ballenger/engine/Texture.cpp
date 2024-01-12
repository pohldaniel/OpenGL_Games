#include <GL/glew.h>
#include <SOIL2/SOIL2.h>
#include <iostream>

#include "Texture.h"

#define DEFAULT_MIN_FILTER	GL_NEAREST
#define DEFAULT_MAG_FILTER	GL_NEAREST
#define DEFAULT_WRAP_S	GL_CLAMP_TO_EDGE
#define DEFAULT_WRAP_T	GL_CLAMP_TO_EDGE

bool operator==(const Texture& t1, const Texture& t2) {
	return t1.m_texture == t2.m_texture && t1.m_width == t2.m_width && t1.m_height == t2.m_height && t1.m_depth == t2.m_depth && t1.m_channels == t2.m_channels && t1.m_format == t2.m_format && t1.m_internalFormat == t2.m_internalFormat && t1.m_type == t2.m_type && t1.m_target == t2.m_target;
}

Texture::Texture(std::string fileName, const bool _flipVertical, unsigned int _internalFormat, unsigned int _format) {
	
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	m_internalFormat = _internalFormat == 0 && numCompontents == 1 ? GL_R8 : _internalFormat == 0 && numCompontents == 3 ? GL_RGB8 : _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
	m_format = _format == 0 && numCompontents == 1 ? GL_R : _format == 0 && numCompontents == 3 ? GL_RGB : _format == 0 ? GL_RGBA : _format;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_2D;

	if(_flipVertical) 
		flipVertical(imageData, numCompontents * width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(m_target, m_texture);
	glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(m_target, 0, m_internalFormat, width, height, 0, m_format, m_type, imageData);
	glBindTexture(m_target, 0);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;
}

Texture::Texture(Texture const& rhs){
	m_texture = rhs.m_texture;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_depth = rhs.m_depth;
	m_channels = rhs.m_channels;
	m_format = rhs.m_format;
	m_internalFormat = rhs.m_internalFormat;
	m_type = rhs.m_type;
	m_target = rhs.m_target;
}

void Texture::copy(const Texture& rhs) {

	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_depth = rhs.m_depth;
	m_channels = rhs.m_channels;
	m_format = rhs.m_format;
	m_internalFormat = rhs.m_internalFormat;
	m_type = rhs.m_type;
	m_target = rhs.m_target;

	if (m_target == GL_TEXTURE_2D) {
		unsigned char* bytes = (unsigned char*)malloc(m_width * m_height * m_channels);
		int magFilter, minFilter, wrapS, wrapT;

		glBindTexture(GL_TEXTURE_2D, rhs.m_texture);
		glGetTexImage(GL_TEXTURE_2D, 0, m_format, GL_UNSIGNED_BYTE, bytes);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, bytes);

		if (minFilter == 9984 || minFilter == 9985 || minFilter == 9986 || minFilter == 9987)
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		free(bytes);
	}
}

void Texture::flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
	std::vector<unsigned char> srcPixels(padWidth * height);
	memcpy(&srcPixels[0], data, padWidth * height);

	unsigned char *pSrcRow = 0;
	unsigned char *pDestRow = 0;

	for (int i = 0; i < height; ++i) {

		pSrcRow = &srcPixels[(height - 1 - i) * padWidth];
		pDestRow = &data[i * padWidth];
		memcpy(pDestRow, pSrcRow, padWidth);
	}
}

void Texture::flipHorizontal(unsigned char* data, unsigned int width, unsigned int height, int numCompontents) {
	unsigned char *pFront = 0;
	unsigned char *pBack = 0;
	unsigned char pixel[4] = { 0 };

	for (int i = 0; i < height; ++i) {
		pFront = &data[i * width * numCompontents];
		pBack = &pFront[(width -1) * numCompontents];

		while (pFront < pBack) {
			// Save current pixel at position pFront.
			for (int j = 0; j < numCompontents; j++) {
				pixel[j] = pFront[j];
			}

			// Copy new pixel from position pBack into pFront.
			for (int j = 0; j < numCompontents; j++) {
				pFront[j] = pBack[j];
			}

			// Copy old pixel at position pFront into pBack.
			for (int j = 0; j < numCompontents; j++) {
				pBack[j] = pixel[j];
			}

			pFront += numCompontents;
			pBack -= numCompontents;
		}
	}
}

void Texture::FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
	std::vector<unsigned char> srcPixels(padWidth * height);
	memcpy(&srcPixels[0], data, padWidth * height);

	unsigned char *pSrcRow = 0;
	unsigned char *pDestRow = 0;

	for (int i = 0; i < height; ++i) {

		pSrcRow = &srcPixels[(height - 1 - i) * padWidth];
		pDestRow = &data[i * padWidth];
		memcpy(pDestRow, pSrcRow, padWidth);
	}
}

void Texture::FlipHorizontal(unsigned char* data, unsigned int width, unsigned int height, int numCompontents) {
	unsigned char *pFront = 0;
	unsigned char *pBack = 0;
	unsigned char pixel[4] = { 0 };

	for (int i = 0; i < height; ++i) {
		pFront = &data[i * width * numCompontents];
		pBack = &pFront[(width - 1) * numCompontents];

		while (pFront < pBack) {
			// Save current pixel at position pFront.
			for (int j = 0; j < numCompontents; j++) {
				pixel[j] = pFront[j];
			}

			// Copy new pixel from position pBack into pFront.
			for (int j = 0; j < numCompontents; j++) {
				pFront[j] = pBack[j];
			}

			// Copy old pixel at position pFront into pBack.
			for (int j = 0; j < numCompontents; j++) {
				pBack[j] = pixel[j];
			}

			pFront += numCompontents;
			pBack -= numCompontents;
		}
	}
}

Texture::~Texture() {
	cleanup();
}

void Texture::cleanup() {
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}

void Texture::loadFromFile(std::string fileName, const bool _flipVertical, unsigned int _internalFormat, unsigned int _format, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom, unsigned int SOIL_FLAG) {
	
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_FLAG);
	if (numCompontents == 1 && (_format == GL_RGB || _format == GL_RGBA /*|| SOIL_FLAG == 1u*/)) {
		SOIL_free_image_data(imageData);
		SOIL_FLAG = _format == GL_RGB ? 3u : 4u;
		imageData = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_FLAG);
		numCompontents = 3;
	}

	m_internalFormat = _internalFormat == 0 && numCompontents == 1 ? GL_R8 : _internalFormat == 0 && numCompontents == 3 ? GL_RGB8 : _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
	m_format = _format == 0 && numCompontents == 1 ? GL_R : _format == 0 && numCompontents == 3 ? GL_RGB : _format == 0 ? GL_RGBA : _format;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_2D;

	if (_flipVertical)
		flipVertical(imageData, numCompontents * width, height);

	imageData = AddRemoveLeftPadding(imageData, width, height, numCompontents, paddingLeft);
	imageData = AddRemoveRightPadding(imageData, width, height, numCompontents, paddingRight);
	imageData = AddRemoveTopPadding(imageData, width, height, numCompontents, paddingTop);
	imageData = AddRemoveBottomPadding(imageData, width, height, numCompontents, paddingBottom);

	glGenTextures(1, &m_texture);
	glBindTexture(m_target, m_texture);
	glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if(SOIL_FLAG == 3 || numCompontents == 3 || numCompontents == 1)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(m_target, 0, m_internalFormat, width, height, 0, m_format, m_type, imageData);
	glBindTexture(m_target, 0);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void Texture::loadHDRIFromFile(std::string fileName, const bool _flipVertical, unsigned int internalFormat, unsigned int format, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	int width, height, numCompontents;
	unsigned char* imageData = reinterpret_cast<unsigned char *>(SOIL_load_image_f(fileName.c_str(), &width, &height, &numCompontents, 0));

	m_internalFormat = internalFormat == 0 && numCompontents == 3 ? GL_RGB32F : internalFormat == 0 ? GL_RGBA32F : internalFormat;
	m_format = format == 0 && numCompontents == 3 ? GL_RGB : format == 0 ? GL_RGBA : format;
	m_type = GL_FLOAT;
	m_target = GL_TEXTURE_2D;

	if (_flipVertical)
		FlipVertical(imageData, numCompontents * sizeof(float) * width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(m_target, m_texture);
	glTexParameterf(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(m_target, 0, m_internalFormat, width, height, 0, m_format, m_type, imageData);
	glGenerateMipmap(m_target);
	glBindTexture(m_target, 0);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;

	SOIL_free_image_data(imageData);
}

void Texture::loadCrossDDSFromFile(std::string fileName) {
	m_target = GL_TEXTURE_CUBE_MAP;
	m_texture = SOIL_load_OGL_single_cubemap(fileName.c_str(), SOIL_DDS_CUBEMAP_FACE_ORDER, SOIL_LOAD_AUTO, m_texture, SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT);
}

void Texture::loadDDSRawFromFile(std::string fileName, const int knownInternal) {
	m_texture = SOIL_direct_load_DDS(fileName.c_str(), m_texture, NULL, false);
	m_target = GL_TEXTURE_2D;

	int width, height;
	int miplevel = 0;

	glBindTexture(m_target, m_texture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &height);
	glBindTexture(m_target, 0);

	m_width = width;
	m_height = height;
	m_internalFormat = knownInternal;
}

void Texture::loadCubeFromFile(std::string* textureFiles, const bool _flipVertical, unsigned int _internalFormat, unsigned int _format) {
	int width, height, numCompontents;
	unsigned char* imageData;
	
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_CUBE_MAP;

	glGenTextures(1, &m_texture);
	glBindTexture(m_target, m_texture);

	for (unsigned short i = 0; i < 6; i++) {	
		imageData = SOIL_load_image(textureFiles[i].c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
		m_internalFormat = _internalFormat == 0 && numCompontents == 1 ? GL_R8 : _internalFormat == 0 && numCompontents == 3 ? GL_RGB8 : _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
		m_format = _format == 0 && numCompontents == 1 ? GL_R : _format == 0 && numCompontents == 3 ? GL_RGB : _format == 0 ? GL_RGBA : _format;
		unsigned int internalFormat = _format == 0 && numCompontents == 3 ? GL_RGB8 : _format == 0 ? GL_RGBA8 : _format;
		if (_flipVertical)
			flipVertical(imageData, numCompontents * width, height);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, width, height, 0, m_format, m_type, imageData);

		SOIL_free_image_data(imageData);
	}

	glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(m_target);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(m_target, 0);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;
}

void Texture::loadCrossCubeFromFile(std::string fileName, const bool _flipVertical, unsigned int _internalFormat, unsigned int _format) {
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);

	m_internalFormat = _internalFormat == 0 && numCompontents == 1 ? GL_R8 : _internalFormat == 0 && numCompontents == 3 ? GL_RGB8 : _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
	m_format = _format == 0 && numCompontents == 1 ? GL_R : _format == 0 && numCompontents == 3 ? GL_RGB : _format == 0 ? GL_RGBA : _format;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_CUBE_MAP;
	m_width = width;
	m_height = height;
	m_channels = numCompontents;

	std::vector<unsigned char*> facData;
	int fWidth = width / 4;
	int fHeight = height / 3;

	unsigned char *face = new unsigned char[fWidth * fHeight * m_channels];
	unsigned char *ptr;

	// positive X
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &imageData[(m_height - (fHeight + j + 1)) * m_width * m_channels + 2 * fWidth * m_channels], fWidth * m_channels);
		ptr += fWidth * m_channels;
	}
	facData.push_back(face);

	// negative X
	face = new unsigned char[fWidth * fHeight * m_channels];
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &imageData[(m_height - (fHeight + j + 1))*m_width * m_channels], fWidth * m_channels);
		ptr += fWidth * m_channels;
	}
	facData.push_back(face);

	// positive Y
	face = new unsigned char[fWidth * fHeight * m_channels];
	ptr = face;
	for (int j = 0; j < fHeight; j++) {
		memcpy(ptr, &imageData[j * m_width * m_channels + fWidth * m_channels], fWidth * m_channels);
		ptr += fWidth * m_channels;
	}
	facData.push_back(face);

	// negative Y
	face = new unsigned char[fWidth * fHeight * m_channels];
	ptr = face;
	for (int j = 0; j < fHeight; j++) {
		memcpy(ptr, &imageData[(m_height - (j + 1)) * m_width * m_channels + fWidth * m_channels], fWidth * m_channels);
		ptr += fWidth * m_channels;
	}
	facData.push_back(face);
	
	// positive Z
	face = new unsigned char[fWidth * fHeight * m_channels];
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &imageData[(m_height - (fHeight + j + 1)) * m_width * m_channels + fWidth * m_channels], fWidth * m_channels);
		ptr += fWidth * m_channels;
	}
	facData.push_back(face);

	// negative Z
	face = new unsigned char[fWidth * fHeight * m_channels];
	memset(face, 128, fWidth * fHeight * m_channels * sizeof(unsigned char));
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &imageData[(m_height - (fHeight + j + 1)) * m_width * m_channels + 3 * fWidth * m_channels], fWidth * m_channels);
		ptr += fWidth * m_channels;
	}
	facData.push_back(face);

	
	m_width = fWidth;
	m_height = fHeight;

	glGenTextures(1, &m_texture);
	glBindTexture(m_target, m_texture);

	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(m_target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	// load face data
	for (int i = 0; i < 6; i++) {
		if (_flipVertical && i != 2)
			flipVertical(facData[i], m_channels * m_width, m_height);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, facData[i]);
	}

	glBindTexture(m_target, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	for (int i = 0; i < 6; i++) {
		free(facData[i]);
	}

	SOIL_free_image_data(imageData);
}

void Texture::loadCrossHDRIFromFile(std::string fileName, const bool _flipVertical, unsigned int internalFormat, unsigned int format, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	int width, height, numCompontents;
	unsigned char* imageData = reinterpret_cast<unsigned char *>(SOIL_load_image_f(fileName.c_str(), &width, &height, &numCompontents, 0));
	
	m_internalFormat = GL_RGB16F;
	m_format = GL_RGB;
	m_type = GL_FLOAT;
	m_target = GL_TEXTURE_CUBE_MAP;

	if (_flipVertical)
		FlipVertical(imageData, numCompontents * sizeof(float) * width, height);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;

	//pointers to the levels
	std::vector<unsigned char*> facData;

	//get the source data
	unsigned char *data = imageData;

	int fWidth = m_width / 3;
	int fHeight = m_height / 4;
	int elementSize = 3 * sizeof(float);

	unsigned char *face = new unsigned char[fWidth * fHeight * elementSize];
	unsigned char *ptr;

	//extract the faces

	// positive X
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &data[((m_height - (fHeight + j + 1))*m_width + 2 * fWidth) * elementSize], fWidth * elementSize);
		ptr += fWidth * elementSize;
	}
	facData.push_back(face);

	// negative X
	face = new unsigned char[fWidth * fHeight * elementSize];
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &data[(m_height - (fHeight + j + 1))*m_width * elementSize], fWidth * elementSize);
		ptr += fWidth * elementSize;
	}
	facData.push_back(face);

	// positive Y
	face = new unsigned char[fWidth * fHeight * elementSize];
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &data[((4 * fHeight - j - 1)*m_width + fWidth) * elementSize], fWidth * elementSize);
		ptr += fWidth * elementSize;
	}
	facData.push_back(face);

	// negative Y
	face = new unsigned char[fWidth * fHeight * elementSize];
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &data[((2 * fHeight - j - 1)*m_width + fWidth) * elementSize], fWidth * elementSize);
		ptr += fWidth * elementSize;
	}
	facData.push_back(face);

	// positive Z
	face = new unsigned char[fWidth * fHeight * elementSize];
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		memcpy(ptr, &data[((m_height - (fHeight + j + 1))*m_width + fWidth) * elementSize], fWidth * elementSize);
		ptr += fWidth * elementSize;
	}
	facData.push_back(face);

	// negative Z
	face = new unsigned char[fWidth * fHeight * elementSize];
	ptr = face;
	for (int j = 0; j<fHeight; j++) {
		for (int i = 0; i<fWidth; i++) {
			memcpy(ptr, &data[(j*m_width + 2 * fWidth - (i + 1)) * elementSize], elementSize);
			ptr += elementSize;
		}
	}
	facData.push_back(face);

	//set the new # of faces, width and height
	m_width = fWidth;
	m_height = fHeight;

	glGenTextures(1, &m_texture);
	glBindTexture(m_target, m_texture);

	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(m_target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	// load face data
	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, facData[i]);
	}

	glBindTexture(m_target, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	for (int i = 0; i < 6; i++) {
		free(facData[i]);
	}

	SOIL_free_image_data(imageData);
}

void Texture::loadFromFile(std::string fileName, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int _posY, unsigned int _posX, const bool _flipVertical, unsigned int _internalFormat, unsigned int _format) {
	
	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	m_internalFormat = _internalFormat == 0 && numCompontents == 1 ? GL_R8 : _internalFormat == 0 && numCompontents == 3 ? GL_RGB8 : _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
	m_format = _format == 0 && numCompontents == 1 ? GL_R : _format == 0 && numCompontents == 3 ? GL_RGB : _format == 0 ? GL_RGBA : _format;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_2D;

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
	glBindTexture(m_target, m_texture);
	glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(m_target, 0, m_internalFormat, tileWidth, tileHeight, 0, m_format, m_type, subImage);
	glBindTexture(m_target, 0);

	free(subImage);

	SOIL_free_image_data(imageData);

	m_width = width;
	m_height = height;
	m_channels = numCompontents;
}

void Texture::loadFromFile(std::string fileName, unsigned int _offsetX, unsigned int _offsetY, unsigned int _width, unsigned int _height, const bool _flipVertical, unsigned int _internalFormat, unsigned int _format) {

	int width, height, numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	m_internalFormat = _internalFormat == 0 && numCompontents == 1 ? GL_R8 : _internalFormat == 0 && numCompontents == 3 ? GL_RGB8 : _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
	m_format = _format == 0 && numCompontents == 1 ? GL_R : _format == 0 && numCompontents == 3 ? GL_RGB : _format == 0 ? GL_RGBA : _format;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_2D;

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
	glBindTexture(m_target, m_texture);
	glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(m_target, 0, m_internalFormat, _width, _height, 0, m_format, GL_UNSIGNED_BYTE, subImage);
	glBindTexture(m_target, 0);

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

	m_internalFormat = GL_RGBA8;
	m_format = GL_RGBA;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_2D;
}

//https://github.com/sol-prog/Perlin_Noise
void Texture::createPerlinNoise(unsigned int width, unsigned int height, unsigned int seed) {
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
	m_internalFormat = GL_RGBA8;
	m_format = GL_RGBA;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_2D;
	free(pixels);
}

void Texture::createNoise(unsigned int width, unsigned int height) {
	
	unsigned char* pixels = (unsigned char*)malloc(width * height);

	unsigned char* pDest = pixels;
	for (int i = 0; i < width * height; i++) {
		*pDest++ = rand() % 256;
	}

	GLuint textureHandle;
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	m_width = width;
	m_height = height;
	m_channels = 1;
	m_internalFormat = GL_R8;
	m_format = GL_LUMINANCE;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_2D;
	free(pixels);
}

void Texture::createEmptyTexture(unsigned int width, unsigned int height, unsigned int _internalFormat, unsigned int _format, unsigned int _type ) {
	m_internalFormat = _internalFormat == 0 ? GL_RGBA8 : _internalFormat;
	m_format = _format == 0 ? GL_RGBA : _format;
	m_type = _type == 0 ? GL_UNSIGNED_BYTE : _type;
	m_target = GL_TEXTURE_2D;

	glGenTextures(1, &m_texture);
	glBindTexture(m_target, m_texture);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(m_target, 0, m_internalFormat, width, height, 0, m_format, m_type, NULL);
	glBindTexture(m_target, 0);

	m_width = width;
	m_height = height;
}

void Texture::createNullCubemap(unsigned int width, unsigned int height, unsigned int color) {
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

	m_internalFormat = GL_RGBA8;
	m_format = GL_RGBA;
	m_type = GL_UNSIGNED_BYTE;
	m_target = GL_TEXTURE_CUBE_MAP;
}

void Texture::addAlphaChannel(unsigned int value) {
	unsigned char* bytes = (unsigned char*)malloc(m_width * m_height * m_channels);
	int magFilter, minFilter, wrapS, wrapT;
	
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glGetTexImage(GL_TEXTURE_2D, 0, m_format, GL_UNSIGNED_BYTE, bytes);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
	glBindTexture(GL_TEXTURE_2D, 0);

	//unsigned char* data = (unsigned char*)malloc(m_width * m_height * m_channels);
	//unsigned int fbo = 0;
	//glGenFramebuffers(1, &fbo);
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	//glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
	//glReadPixels(0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, data);
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	//glDeleteFramebuffers(1, &fbo);

	m_channels = 4;
	m_format = GL_RGBA;
	m_internalFormat = GL_RGBA8;

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, bytesNew);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &m_texture);

	m_texture = texture_new;

	free(bytes);
	free(bytesNew);
}

unsigned char* Texture::readPixel() {
	unsigned char* bytes = (unsigned char*)malloc(m_width * m_height * m_channels);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glGetTexImage(GL_TEXTURE_2D, 0, m_format, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
	return bytes;
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

unsigned char* Texture::AddRemoveLeftPadding(unsigned char* imageData, int& width, int height, int numCompontents, int padding) {
	if (padding == 0) return imageData;

	unsigned char* bytes = (unsigned char*)malloc(numCompontents * height * (width + padding));

	if (padding < 0) {

		int row = 0, x = -padding * numCompontents;
		for (int i = 0; i < numCompontents * height * (width + padding); i++) {
			if (i % ((width + padding) * numCompontents) == 0 && i > 0) {
				row = row + width * numCompontents;
				x = row - padding * numCompontents;
			}

			bytes[i] = imageData[x];
			x++;
		}
	}

	if (padding > 0) {
		int row = 0, x = 0;
		for (int i = 0; i < numCompontents * height * (width + padding); i++) {
			if (i % ((width + padding) * numCompontents) == 0 && i > 0) {
				row = row + width * numCompontents;
				x = row;
			}

			bytes[i] = x - row < padding * numCompontents ? bytes[i] = 0 : bytes[i] = imageData[x - padding * numCompontents];
			x++;
		}
	}

	width = width + padding;
	free(imageData);
	return bytes;
}

unsigned char* Texture::AddRemoveRightPadding(unsigned char* imageData, int& width, int height, int numCompontents, int padding) {
	if (padding == 0) return imageData;

	unsigned char* bytes = (unsigned char*)malloc(numCompontents * height * (width + padding));

	if (padding < 0) {

		int row = 0, x = 0;
		for (int i = 0; i < numCompontents * height * (width + padding); i++) {
			if (i % ((width + padding) * numCompontents) == 0 && i > 0) {
				row = row + width * numCompontents;
				x = row;
			}

			bytes[i] = imageData[x];
			x++;
		}
	}

	if (padding > 0) {
		int row = 0, x = 0;
		for (int i = 0; i < numCompontents * height * (width + padding); i++) {
			if (i % ((width + padding) * numCompontents) == 0 && i > 0) {
				row = row + width * numCompontents;
				x = row;
			}

			bytes[i] = x - row > width * numCompontents ? bytes[i] = 0 : bytes[i] = imageData[x];
			x++;
		}

	}

	width = width + padding;
	free(imageData);
	return bytes;
}

unsigned char* Texture::AddRemoveTopPadding(unsigned char* imageData, int width, int& height, int numCompontents, int padding) {
	if (padding == 0) return imageData;

	unsigned char* bytes = (unsigned char*)malloc(numCompontents * (height + padding) * width);

	if (padding < 0) {
		int  row = 0, x = -(padding * width * numCompontents);
		for (int i = 0; i < numCompontents * width * (height + padding); i++) {

			if (i % (width  * numCompontents) == 0 && i > 0) {
				row = (row + width * numCompontents);
				x = row - (padding * width * numCompontents);
			}
			bytes[i] = imageData[x];
			x++;

		}
	}

	if (padding > 0) {
		int  row = 0, x = -(padding * width * numCompontents);
		for (int i = 0; i < numCompontents * width * (height + padding); i++) {
			if (i % (width  * numCompontents) == 0 && i > 0) {
				row = (row + width * numCompontents);
				x = row - (padding * width * numCompontents);
			}
			bytes[i] = x < 0 ? 0 : imageData[x];

			x++;
		}
	}

	height = height + padding;
	free(imageData);
	return bytes;
}

unsigned char* Texture::AddRemoveBottomPadding(unsigned char* imageData, int width, int& height, int numCompontents, int padding) {
	if (padding == 0) return imageData;

	unsigned char* bytes = (unsigned char*)malloc(numCompontents * (height + padding) * width);

	if (padding < 0) {
		int  row = 0, x = 0;
		for (int i = 0; i < numCompontents * width * (height + padding); i++) {

			if (i % (width  * numCompontents) == 0 && i > 0) {
				row = (row + width * numCompontents);
				x = row;
			}
			bytes[i] = imageData[x];
			x++;

		}
	}

	if (padding > 0) {
		int  row = 0, x = 0;
		for (int i = 0; i < numCompontents * width * (height + padding); i++) {
			if (i % (width  * numCompontents) == 0 && i > 0) {
				row = (row + width * numCompontents);
				x = row;
			}
			bytes[i] = x > numCompontents * width * height ? 0 : imageData[x];

			x++;
		}
	}

	height = height + padding;
	free(imageData);
	return bytes;
}

unsigned char* Texture::LoadFromFile(std::string pictureFile, const bool _flipVertical, short alphaChannel) {
	int width, height, numCompontents;
	unsigned char* bytes = SOIL_load_image(pictureFile.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);

	if (_flipVertical)
		FlipVertical(bytes, numCompontents * width, height);

	if (alphaChannel >= 0) {
		unsigned char* bytesNew = (unsigned char*)malloc(width *  width * (numCompontents + 1));

		for (unsigned int i = 0, k = 0; i < width * width * 4; i = i + 4, k = k + 3) {
			bytesNew[i] = bytes[k];
			bytesNew[i + 1] = bytes[k + 1];
			bytesNew[i + 2] = bytes[k + 2];
			bytesNew[i + 3] = alphaChannel;
		}
		free(bytes);
		return bytesNew;
	}

	return bytes;
}

unsigned char* Texture::LoadFromFile(std::string fileName, int& width, int& height, const bool _flipVertical, bool transparent, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	int numCompontents;
	unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numCompontents, SOIL_LOAD_AUTO);
	
	if (numCompontents == 3) {

		unsigned char* bytesNew = (unsigned char*)malloc(width * height * 4);

		for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4); i = i + 4, k = k + 3) {
			bytesNew[i] = imageData[k];
			bytesNew[i + 1] = imageData[k + 1];
			bytesNew[i + 2] = imageData[k + 2];
			bytesNew[i + 3] = transparent ? 0 : 255;
		}

		SOIL_free_image_data(imageData);
		imageData = bytesNew;
		numCompontents = 4;
	}

	if (_flipVertical)
		FlipVertical(imageData, numCompontents * width, height);

	if (paddingLeft != 0)
		imageData = AddRemoveLeftPadding(imageData, width, height, numCompontents, paddingLeft);

	if (paddingRight != 0)
		imageData = AddRemoveRightPadding(imageData, width, height, numCompontents, paddingRight);

	if (paddingTop != 0)
		imageData = AddRemoveTopPadding(imageData, width, height, numCompontents, paddingTop);

	if (paddingBottom != 0)
		imageData = AddRemoveBottomPadding(imageData, width, height, numCompontents, paddingBottom);

	return imageData;
}

unsigned char* Texture::LoadHDRIFromFile(std::string fileName, int& width, int& height, const bool flipVertical, unsigned int internalFormat, unsigned int format, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	int numCompontents;
	unsigned char* imageData = reinterpret_cast<unsigned char *>(SOIL_load_image_f(fileName.c_str(), &width, &height, &numCompontents, 0));

	if (flipVertical)
		FlipVertical(imageData, numCompontents * sizeof(float) * width, height);

	if (paddingLeft != 0)
		imageData = AddRemoveLeftPadding(imageData, width, height, numCompontents, paddingLeft);

	if (paddingRight != 0)
		imageData = AddRemoveRightPadding(imageData, width, height, numCompontents, paddingRight);

	if (paddingTop != 0)
		imageData = AddRemoveTopPadding(imageData, width, height, numCompontents, paddingTop);

	if (paddingBottom != 0)
		imageData = AddRemoveBottomPadding(imageData, width, height, numCompontents, paddingBottom);

	return imageData;
}


void Texture::Safe(std::string fileOut, unsigned int& texture) {
	int width, height, depth;
	int miplevel = 0;

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_DEPTH, &depth);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	unsigned char* bytes = (unsigned char*)malloc(width * 4 * height);

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_save_image(fileOut.c_str(), SOIL_SAVE_TYPE_PNG, width, height, 4, bytes);

	free(bytes);
}

void Texture::Safe(std::string fileOut, unsigned char* bytes, unsigned int width, unsigned int height, unsigned int channels) {
	SOIL_save_image(fileOut.c_str(), SOIL_SAVE_TYPE_PNG, width, height, channels, bytes);
}

void Texture::CreateTexture2D(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format, unsigned int type) {
	glGenTextures(1, &textureRef);
	glBindTexture(GL_TEXTURE_2D, textureRef);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::CreateEmptyCubemap(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format, unsigned int type) {
	glGenTextures(1, &textureRef);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureRef);
	for (unsigned short i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, type, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Texture::CreateTexture3D(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int depth, unsigned int internalFormat, unsigned int format, unsigned int type, unsigned char* data) {
	glGenTextures(1, &textureRef);
	glBindTexture(GL_TEXTURE_3D, textureRef);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, type, data);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture::CreateTextureArray(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int layer, unsigned int internalFormat, unsigned int format, unsigned int type, unsigned char* data) {
	glGenTextures(1, &textureRef);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureRef);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, width, height, layer, 0, format, type, data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Texture::CreateTextureCube(unsigned int& textureRef, unsigned int size, unsigned int internalFormat, unsigned int format, unsigned int type, unsigned char* data) {
	glGenTextures(1, &textureRef);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureRef);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, size, size, 0, format, type, data);
	}	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Texture::Resize(const unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format, unsigned int type, unsigned int target, unsigned int depth) {
	if (target == GL_TEXTURE_CUBE_MAP) {
		glBindTexture(target, textureRef);
		for (unsigned short i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, type, NULL);
		}
		glBindTexture(target, 0);
	}else if (target == GL_TEXTURE_3D) {
		glBindTexture(target, textureRef);
		glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, type, NULL);
		glBindTexture(target, 0);
	}else {
		glBindTexture(target, textureRef);
		glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, NULL);
		glBindTexture(target, 0);
	}
}

const unsigned int& Texture::getTexture() const{
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

unsigned int Texture::getInternalFormat() {
	return m_internalFormat;
}

unsigned int Texture::getFormat() {
	return m_format;
}

unsigned int Texture::getType() {
	return m_type;
}

unsigned int Texture::getTarget() {
	return m_target;
}

void Texture::bind(unsigned int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_target, m_texture);
}

void Texture::unbind(unsigned int unit) const {
	glBindTexture(m_target, 0);
	glActiveTexture(GL_TEXTURE0 + unit);
}

void Texture::setLinear(unsigned int mode) const {
	glBindTexture(m_target, m_texture);
	glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, mode);
	glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(mode == 9987)
		glGenerateMipmap(m_target);
	glBindTexture(m_target, 0);
}

void Texture::setNearest() const {
	glBindTexture(m_target, m_texture);
	glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(m_target, 0);
}

void Texture::setFilter(unsigned int minFilter, unsigned int magFilter) const {
	if (magFilter == 0)
		magFilter = minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_LINEAR_MIPMAP_LINEAR ? GL_LINEAR : minFilter == GL_NEAREST_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR ? GL_NEAREST : minFilter;

	glBindTexture(m_target, m_texture);
	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);
	if (minFilter == 9984 || minFilter == 9985 || minFilter == 9986 || minFilter == 9987)
		glGenerateMipmap(m_target);
	glBindTexture(m_target, 0);
}

void Texture::setWrapMode(unsigned int mode) const {
	if (m_target == GL_TEXTURE_CUBE_MAP || m_target == GL_TEXTURE_3D) {
		glBindTexture(m_target, m_texture);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_R, mode);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, mode);
		glBindTexture(m_target, 0);
	} else {
		glBindTexture(m_target, m_texture);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, mode);
		glBindTexture(m_target, 0);
	}
}

void Texture::setAnisotropy(float aniso) const {
	glBindTexture(m_target, m_texture);
	glTexParameterf(m_target, GL_TEXTURE_MAX_ANISOTROPY, aniso);
	glBindTexture(m_target, 0);
}

void Texture::Bind(unsigned int textureRef, unsigned int unit, unsigned int target) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(target, textureRef);
}

void Texture::Unbind(unsigned int unit, unsigned int target) {
	glBindTexture(target, 0);
	glActiveTexture(GL_TEXTURE0 + unit);
}

void Texture::SetFilter(const unsigned int& textureRef, unsigned int minFilter, unsigned int magFilter, unsigned int target) {
	
	if (magFilter == 0)
		magFilter = minFilter == 9985 || minFilter == 9987 ? GL_LINEAR : minFilter == 9984 || minFilter == 9986 ? GL_NEAREST : minFilter;

	glBindTexture(target, textureRef);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
	if (minFilter == 9984 || minFilter == 9985 || minFilter == 9986 || minFilter == 9987)
		glGenerateMipmap(target);
	glBindTexture(target, 0);
}

void Texture::SetWrapMode(const unsigned int& textureRef, unsigned int mode, unsigned int target) {
	if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_3D) {

		glBindTexture(target, textureRef);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, mode);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, mode);
		glBindTexture(target, 0);
	} else {
		glBindTexture(target, textureRef);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, mode);
		glBindTexture(target, 0);
	}
}

void Texture::ArrayTo3D(const unsigned int& textureRef1, unsigned int& textureRef2) {
	int width, height, depth;
	int miplevel = 0;

	glBindTexture(GL_TEXTURE_2D_ARRAY, textureRef1);
	glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, miplevel, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, miplevel, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, miplevel, GL_TEXTURE_DEPTH, &depth);

	unsigned char* bytes = (unsigned char*)malloc(width * height *depth * 4 * sizeof(unsigned char));

	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenTextures(1, &textureRef2);
	glBindTexture(GL_TEXTURE_3D, textureRef2);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

	glBindTexture(GL_TEXTURE_3D, 0);

	free(bytes);
}