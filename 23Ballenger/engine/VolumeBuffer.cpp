#include "VolumeBuffer.h"

VolumeBuffer::VolumeBuffer(unsigned int internalFormat, int width, int height, int depth) {

	m_slicedCube.create(width, height, depth);

	Texture::CreateTexture3D(m_texture, width, height, depth, internalFormat, GL_RGBA, GL_FLOAT);

	m_fbo.create(width, height);
	m_fbo.attachTexture(m_texture, Attachment::COLOR, Target::TEXTURE3D, 0);
}

VolumeBuffer::~VolumeBuffer() {
	glDeleteTextures(1, &m_texture);
}

void VolumeBuffer::setFiltering(unsigned int mode) {
	Texture::SetFilter(m_texture, mode);
}

void VolumeBuffer::setShader(Shader* shader) {
	m_shader = shader;
}

unsigned int& VolumeBuffer::getTexture() {
	return m_texture; 
}

int VolumeBuffer::getWidth() {
	return m_slicedCube.getWidth();
}

int VolumeBuffer::getHeight() { 
	return m_slicedCube.getHeight();
}

int VolumeBuffer::getDepth() { 
	return m_slicedCube.getDepth();
}

void VolumeBuffer::draw() {

	m_fbo.bind();
	glUseProgram(m_shader->m_program);

	glBindVertexArray(m_slicedCube.getVao());
	for (int z = 0; z < m_slicedCube.getDepth(); z++) {
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, m_texture, 0, z);
		glDrawArrays(GL_QUADS, z * 4, 4);
	}
	glBindVertexArray(0);

	glUseProgram(0);
	m_fbo.unbind();
}

void VolumeBuffer::drawRaw() {
	m_slicedCube.drawRaw();
}