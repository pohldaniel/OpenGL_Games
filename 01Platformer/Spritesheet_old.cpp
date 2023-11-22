#include "stb\stb_image.h"
#include "Spritesheet_old.h"
#include "Extension.h"

SpritesheetOld::SpritesheetOld(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, bool reverse, bool flipVertical) {

	stbi_set_flip_vertically_on_load(flipVertical);
	int width, height, numCompontents;
	unsigned char* imageData = stbi_load(pictureFile.c_str(), &width, &height, &numCompontents, NULL);

	tileCountX = width / tileWidth;
	tileCountY = height / tileHeight;
	unsigned short totalFrames = tileCountX * tileCountY;
	float tileScaleX = 1.0 / (float)(tileCountX);
	float tileScaleY = 1.0 / (float)(tileCountY);

	
	if (reverse) {
		for (short i = totalFrames - 1, j = tileCountX - 1; i >= 0; i--, j -= 2) {
			tileFrameTransforms.push_back(calcTileFrameTransform(i - j, tileScaleX, tileScaleY, tileCountX));
			if (i % tileCountX == 0 && i < (totalFrames - 1)) {
				j = (tileCountX + 1);
			}
		}
	}else {
		for (unsigned short i = 0; i < totalFrames; i++) {
			tileFrameTransforms.push_back(calcTileFrameTransform(i, tileScaleX, tileScaleY, tileCountX));
		}
	}
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(imageData);
}

Matrix4f SpritesheetOld::calcTileFrameTransform(unsigned short id, float tileScaleX, float tileScaleY, unsigned short tileCountX) {
	Matrix4f scale = Matrix4f::Scale(tileScaleX, tileScaleY, 0.0f);
	Matrix4f trans = Matrix4f::Translate((float)(id % tileCountX), (float)(id / tileCountX), 0.0f);
	return scale * trans;
}

unsigned int SpritesheetOld::getTexture() {
	return m_texture;
}

const Matrix4f &SpritesheetOld::getFrameTransform(unsigned short index) const {
	return tileFrameTransforms[index];
}

unsigned short SpritesheetOld::getTileCountX() {
	return tileCountX;
}

unsigned short SpritesheetOld::getTileCountY() {
	return tileCountX;
}

SpritesheetOld::~SpritesheetOld() {
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}