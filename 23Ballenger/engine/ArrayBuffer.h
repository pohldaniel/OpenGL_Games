#pragma once

#include "engine/Texture.h"
#include "engine/Framebuffer.h"
#include "engine/Shader.h"

class ArrayBuffer {

public:

	ArrayBuffer(unsigned int internalFormat, int width, int height, int layer);
	~ArrayBuffer();

	void setFiltering(unsigned int minFilter);
	void setWrapMode(unsigned int mode);
	void setShader(Shader* shader);
	void draw();
	void rebind(int offset);

	int getWidth();
	int getHeight();
	int getLayer();

	void safe(const char* fileName);
	void resize(int width, int height);
	void getArray(unsigned int& texture);
	void writeArrayToRaw(const char* fileName);
	void setDrawFunction(std::function<void()> fun);
	Framebuffer& getFramebuffer();
	Shader* getShader();
	unsigned int& getVao();
	unsigned int& getTexture();

	bool static LoadArrayFromRaw(const char* fileName, unsigned int& texture, int width, int height, int layer);

private:

	void createBuffer();
	
	Shader* m_shader;
	Framebuffer m_fbo;

	unsigned int m_texture;
	unsigned int m_internalFormat;

	unsigned int m_minFilter;
	unsigned int m_magFilter;
	unsigned int m_mode;

	int m_width, m_height, m_layer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	std::function<void()> m_draw = 0;
};