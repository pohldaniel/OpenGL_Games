#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <soil2/SOIL2.h>
#include <GL/glew.h>
#include <vector>

class TextureVo
{
public:
	TextureVo();
	~TextureVo();

	void GenerateTexture(char* textureId);
	void GenerateMultipleTextures(std::vector<char*>& textureIds);
	void ActivateTexture(unsigned int unit = 0);
	void ActivateTextures(unsigned int unit = 0);
	void GenerateSkybox(unsigned short int startIndex = 0, unsigned short int lastIndex = 6);

private:
	GLuint m_texture;
	GLuint m_textures[32];
};

#endif // !__TEXTURE_H__