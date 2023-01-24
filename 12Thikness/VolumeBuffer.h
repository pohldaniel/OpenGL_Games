#pragma once

#include "engine/Texture.h"
#include "engine/Framebuffer.h"
#include "engine/Shader.h"

class VolumeBuffer {
public:
	VolumeBuffer(unsigned int internalFormat, int width, int height, int depth);
	~VolumeBuffer();

	enum BlendMode { BLEND_NONE = 0, BLEND_ADDITIVE };

	void setFiltering(unsigned int mode);
	void setBlendMode(BlendMode mode);
	void setShader(Shader* shader);
	void draw();

	unsigned int& getTexture();

	int getWidth();
	int getHeight();
	int getDepth();
	void createSlicedCube();

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

private:
	BlendMode m_blendMode;
	Shader* m_shader;
	Framebuffer m_fbo;
	int m_width, m_height, m_depth;
	unsigned int m_texture;
};