#include "ArrayBuffer.h"

ArrayBuffer::ArrayBuffer(unsigned int internalFormat, int width, int height, int textureLayer, int attachements) {
	m_internalFormat = internalFormat;
	m_minFilter = GL_NEAREST;
	m_magFilter = GL_NEAREST;
	m_mode = GL_CLAMP_TO_EDGE;
	m_width = width;
	m_height = height;
	m_textureLayer = textureLayer;

	Texture::CreateTextureArray(m_texture, width, height, textureLayer, internalFormat, GL_RGBA, GL_UNSIGNED_BYTE);
	m_fbo.create(width, height);
	m_fbo.attachTexture(m_texture, Attachment::COLOR, Target::ARRAY, attachements);

	createBuffer();
}

ArrayBuffer::~ArrayBuffer() {
	glDeleteTextures(1, &m_texture);

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
}

void ArrayBuffer::setFiltering(unsigned int minFilter) {
	m_minFilter = minFilter;
	m_magFilter = minFilter == 9985 || minFilter == 9987 ? GL_LINEAR : minFilter == 9984 || minFilter == 9986 ? GL_NEAREST : minFilter;
	Texture::SetFilter(m_texture, m_minFilter, m_magFilter, GL_TEXTURE_2D_ARRAY);
}

void ArrayBuffer::setWrapMode(unsigned int mode) {
	m_mode = mode;
	Texture::SetWrapMode(m_texture, mode, GL_TEXTURE_2D_ARRAY);
}

void ArrayBuffer::resize(int width, int height) {
	m_width = width;
	m_height = height;
	
	m_fbo.resize(width, height);

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, m_internalFormat, width, height, m_textureLayer, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	if (m_minFilter == 9984 || m_minFilter == 9985 || m_minFilter == 9986 || m_minFilter == 9987)
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ArrayBuffer::setShader(Shader* shader) {
	m_shader = shader;
}

unsigned int& ArrayBuffer::getTexture() {
	return m_texture;
}

int ArrayBuffer::getWidth() {
	return m_width;
}

int ArrayBuffer::getHeight() {
	return m_height;
}

int ArrayBuffer::getTextureLayer() {
	return m_textureLayer;
}

void ArrayBuffer::draw() {

	if (m_draw) {
		m_draw();
	}else if (m_innerDraw) {
		m_fbo.bind();
		glUseProgram(m_shader->m_program);
		glBindVertexArray(m_vao);
		m_innerDraw();
		glBindVertexArray(0);
		glUseProgram(0);
		m_fbo.unbind();
	}else {
		m_fbo.bind();
		glUseProgram(m_shader->m_program);
		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		m_fbo.unbind();
	}
}

void ArrayBuffer::getArray(unsigned int& texture) {
	unsigned char* bytes = (unsigned char*)malloc(getWidth() * getHeight() * getTextureLayer() * 4 * sizeof(unsigned char));
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, m_magFilter);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, m_mode);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, m_mode);


	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, getWidth(), getHeight(), getTextureLayer(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	if (m_minFilter == 9984 || m_minFilter == 9985 || m_minFilter == 9986 || m_minFilter == 9987)
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	free(bytes);
}

void ArrayBuffer::getVolume(unsigned int& texture) {
	unsigned char* bytes = (unsigned char*)malloc(getWidth() * getHeight() * getTextureLayer() * 4 * sizeof(unsigned char));
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_3D, texture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, m_magFilter);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, m_mode);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, m_mode);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, m_mode);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, getWidth(), getHeight(), getTextureLayer(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	if (m_minFilter == 9984 || m_minFilter == 9985 || m_minFilter == 9986 || m_minFilter == 9987)
		glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
	free(bytes);
}

void ArrayBuffer::createBuffer() {
	
	std::vector<float> vertex;
	vertex.push_back(-1.0), vertex.push_back(-1.0), vertex.push_back(0.0f);
	vertex.push_back(0.0), vertex.push_back(0.0);

	vertex.push_back(1.0), vertex.push_back(-1.0), vertex.push_back(0.0f);
	vertex.push_back(1.0), vertex.push_back(0.0);

	vertex.push_back(1.0), vertex.push_back(1.0), vertex.push_back(0.0f);
	vertex.push_back(1.0), vertex.push_back(1.0);

	vertex.push_back(-1.0), vertex.push_back(1.0), vertex.push_back(0.0f);
	vertex.push_back(0.0), vertex.push_back(1.0);

	const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};

	if (m_vbo) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}else {

		short stride = 5;
		short offset = 3;

		unsigned int ibo;
		glGenBuffers(1, &ibo);
		glGenBuffers(1, &m_vbo);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

		//Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

		//Texture Coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glDeleteBuffers(1, &ibo);		
	}
	vertex.clear();
	vertex.shrink_to_fit();
}

void ArrayBuffer::safe(const char* fileName) {
	Spritesheet::Safe(fileName, m_texture);
}

void ArrayBuffer::writeArrayToRaw(const char* fileName) {
	unsigned char* bytes = (unsigned char*)malloc(getWidth() * getHeight() * getTextureLayer() * 4 * sizeof(unsigned char));
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);


	FILE* pFile = fopen(fileName, "wb");
	if (pFile != NULL) {
		fwrite(bytes, sizeof(unsigned char) * 4, getWidth() * getHeight() * getTextureLayer(), pFile);
		fclose(pFile);
	}
	free(bytes);
}

bool ArrayBuffer::LoadArrayFromRaw(const char* fileName, unsigned int& texture, int width, int height, int layer) {
	FILE* pFile = fopen(fileName, "rb");
	if (NULL == pFile) {
		return false;
	}

	unsigned char* pArray = new unsigned char[width * height * layer * 4 * sizeof(unsigned char)];
	fread(pArray, sizeof(unsigned char) * 4, width * height * layer, pFile);
	fclose(pFile);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layer, 0, GL_RGBA, GL_UNSIGNED_BYTE, pArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	delete[] pArray;
	return true;
}

void ArrayBuffer::rebind(unsigned short attachements, unsigned int offset) {
	for (unsigned short i = 0; i < attachements; i++) {
		glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, m_texture, 0, offset + i);
	}
}

void ArrayBuffer::setDrawFunction(std::function<void()> fun) {
	m_draw = fun;
}

void ArrayBuffer::setInnerDrawFunction(std::function<void()> fun) {
	m_innerDraw = fun;
}

Framebuffer& ArrayBuffer::getFramebuffer() {
	return m_fbo;
}

Shader* ArrayBuffer::getShader() {
	return m_shader;
}

unsigned int& ArrayBuffer::getVao() {
	return m_vao;
}