#pragma once

#include "engine/Texture.h"
#include "engine/Framebuffer.h"
#include "engine/Shader.h"
#include "engine/SlicedCube.h"

class VolumeBuffer {

public:

	VolumeBuffer(unsigned int internalFormat, int width, int height, int depth);
	~VolumeBuffer();

	void setFiltering(unsigned int minFilter);
	void setWrapMode(unsigned int mode);
	void setShader(Shader* shader);
	void draw();
	void drawRaw();
	unsigned int& getTexture();

	int getWidth();
	int getHeight();
	int getDepth();
	void writeVolumeToRaw(const char* fileName);
	void resize(int width, int height, int depth);
	void getVolume(unsigned int& texture);

private:

	Shader* m_shader;
	Framebuffer m_fbo;
	SlicedCube m_slicedCube;
	unsigned int m_texture;
	unsigned int m_internalFormat;

	unsigned int m_minFilter;
	unsigned int m_magFilter;
	unsigned int m_mode;
};