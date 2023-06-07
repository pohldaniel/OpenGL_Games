#include "ArrayBuffer.h"

ArrayBuffer::ArrayBuffer(unsigned int internalFormat, int width, int height, int layer) {
	m_internalFormat = internalFormat;
	createBuffer();
	Texture::CreateTextureArray(m_texture, width, height, layer, internalFormat, GL_RGBA, GL_UNSIGNED_BYTE);

	m_fbo.create(width, height);
	m_fbo.attachTexture(m_texture, Attachment::COLOR, Target::ARRAY, layer);
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
	Texture::SetFilter(m_texture, m_minFilter, m_magFilter, GL_TEXTURE_3D);
}

void ArrayBuffer::setWrapMode(unsigned int mode) {
	m_mode = mode;
	Texture::SetWrapMode(m_texture, mode, GL_TEXTURE_3D);
}

void ArrayBuffer::resize(int width, int height, int depth) {

	//m_slicedCube.create(width, height, depth);
	m_fbo.resize(width, height);

	glBindTexture(GL_TEXTURE_3D, m_texture);
	glTexImage3D(GL_TEXTURE_3D, 0, m_internalFormat, width, height, depth, 0, GL_RGBA, GL_FLOAT, NULL);
	if (m_minFilter == 9984 || m_minFilter == 9985 || m_minFilter == 9986 || m_minFilter == 9987)
		glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
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

int ArrayBuffer::getLayer() {
	return m_layer;
}

void ArrayBuffer::draw() {

	m_fbo.bind();
	glUseProgram(m_shader->m_program);

	glBindVertexArray(m_vao);
	//for (int z = 0; z < m_layer; z++) {
		//glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, m_texture, 0, z);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	//}
	glBindVertexArray(0);

	glUseProgram(0);
	m_fbo.unbind();
}

void ArrayBuffer::getArray(unsigned int& texture) {

	float* bytes = (float*)malloc(getWidth() * getHeight() * getLayer() * 4 * sizeof(float));
	glBindTexture(GL_TEXTURE_3D, m_texture);
	glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, bytes);
	glBindTexture(GL_TEXTURE_3D, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_3D, texture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, m_magFilter);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, m_mode);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, m_mode);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, m_mode);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, getWidth(), getHeight(), getLayer(), 0, GL_RGBA, GL_FLOAT, bytes);
	if (m_minFilter == 9984 || m_minFilter == 9985 || m_minFilter == 9986 || m_minFilter == 9987)
		glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);

	free(bytes);

	//glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_3D, texture);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, m_magFilter);

	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, m_mode);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, m_mode);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, m_mode);
	//if (m_minFilter == 9984 || m_minFilter == 9985 || m_minFilter == 9986 || m_minFilter == 9987)
	//	glGenerateMipmap(GL_TEXTURE_3D);
	//glBindTexture(GL_TEXTURE_3D, 0);

	//glCopyImageSubData(m_texture, GL_TEXTURE_3D, 0, 0, 0, 0, texture, GL_TEXTURE_3D, 0, 0, 0, 0, getWidth(), getHeight(), getDepth());
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
		0, 2, 1,
		0, 3, 2
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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glDeleteBuffers(1, &ibo);
	}
	vertex.clear();
	vertex.shrink_to_fit();
}