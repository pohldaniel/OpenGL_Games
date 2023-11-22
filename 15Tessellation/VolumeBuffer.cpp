#include "VolumeBuffer.h"

VolumeBuffer::VolumeBuffer(unsigned int internalFormat, int width, int height, int depth) : m_width(width), m_height(height), m_depth(depth), m_blendMode(BLEND_NONE) {

	Texture::CreateTexture3D(m_texture, m_width, m_height, m_depth, internalFormat, GL_RGBA, GL_FLOAT);

	m_fbo.create(width, height);
	//m_fbo.attachTexture(m_texture, Attachment::COLOR, Target::TEXTURE3D, 0);
	createSlicedCube();
}

VolumeBuffer::~VolumeBuffer() {
	glDeleteTextures(1, &m_texture);
}

void VolumeBuffer::setFiltering(unsigned int mode) {
	Texture::SetFilter3D(m_texture, mode);
}

void VolumeBuffer::setBlendMode(BlendMode mode) { 
	m_blendMode = mode; 
}

void VolumeBuffer::setShader(Shader* shader) {
	m_shader = shader;
}

unsigned int& VolumeBuffer::getTexture() {
	return m_texture; 
}

int VolumeBuffer::getWidth() {
	return m_width; 
}

int VolumeBuffer::getHeight() { 
	return m_height; 
}

int VolumeBuffer::getDepth() { 
	return m_depth; 
}

void VolumeBuffer::draw() {

	m_fbo.bind();
	glUseProgram(m_shader->m_program);

	glBindVertexArray(m_vao);
	for (int z = 0; z < m_depth; z++) {
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, m_texture, 0, z);
		glDrawArrays(GL_QUADS, z * 4, 4);
	}
	glBindVertexArray(0);

	glUseProgram(0);
	m_fbo.unbind();
}

void VolumeBuffer::createSlicedCube() {

	std::vector <float> vertex;
	for (int z = 0; z < m_depth; z++) {

		vertex.push_back(-1.0), vertex.push_back(-1.0), vertex.push_back((2.0f * z / (float)(m_depth - 1)) - 1.0f);
		vertex.push_back(0.0), vertex.push_back(0.0), vertex.push_back(z / (float)(m_depth - 1));

		vertex.push_back(1.0), vertex.push_back(-1.0), vertex.push_back((2.0f * z / (float)(m_depth - 1)) - 1.0f);
		vertex.push_back(1.0), vertex.push_back(0.0), vertex.push_back(z / (float)(m_depth - 1));

		vertex.push_back(1.0), vertex.push_back(1.0), vertex.push_back((2.0f * z / (float)(m_depth - 1)) - 1.0f);
		vertex.push_back(1.0), vertex.push_back(1.0), vertex.push_back(z / (float)(m_depth - 1));

		vertex.push_back(-1.0), vertex.push_back(1.0), vertex.push_back((2.0f * z / (float)(m_depth - 1)) - 1.0f);
		vertex.push_back(0.0), vertex.push_back(1.0), vertex.push_back(z / (float)(m_depth - 1));
	}

	short stride = 6;
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//TexutreCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	vertex.clear();
	vertex.shrink_to_fit();
}