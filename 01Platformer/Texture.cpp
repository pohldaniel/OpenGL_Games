#include "Texture.h"
#include "Extension.h"
#include "stb\stb_image.h"

Texture::Texture(std::string pictureFile, bool flipVertical) {

	stbi_set_flip_vertically_on_load(flipVertical);
	int width, height, numComponents;
	unsigned char* imageData = stbi_load(pictureFile.c_str(), &width, &height, &numComponents, NULL);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(imageData);
}

unsigned int Texture::getTexture() {
	return m_texture;
}

Texture::~Texture() {
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}