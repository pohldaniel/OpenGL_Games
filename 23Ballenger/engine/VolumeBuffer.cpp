#include "VolumeBuffer.h"

VolumeBuffer::VolumeBuffer(unsigned int internalFormat, int width, int height, int depth) {
	m_internalFormat = internalFormat;
	m_slicedCube.create(width, height, depth);
	Texture::CreateTexture3D(m_texture, width, height, depth , internalFormat, GL_RGBA, GL_FLOAT);

	m_fbo.create(width, height);
	m_fbo.attachTexture(m_texture, Attachment::COLOR, Target::TEXTURE3D, 0);
}

VolumeBuffer::~VolumeBuffer() {
	glDeleteTextures(1, &m_texture);
}

void VolumeBuffer::setFiltering(unsigned int minFilter) {
	m_minFilter = minFilter;
	m_magFilter = minFilter == 9985 || minFilter == 9987 ? GL_LINEAR : minFilter == 9984 || minFilter == 9986 ? GL_NEAREST : minFilter;
	Texture::SetFilter(m_texture, m_minFilter, m_magFilter, GL_TEXTURE_3D);
}

void VolumeBuffer::setWrapMode(unsigned int mode) {
	m_mode = mode;
	Texture::SetWrapMode(m_texture, mode, GL_TEXTURE_3D);
}

void VolumeBuffer::resize(int width, int height, int depth) {

	m_slicedCube.create(width, height, depth);
	m_fbo.resize(width, height);

	glBindTexture(GL_TEXTURE_3D, m_texture);
	glTexImage3D(GL_TEXTURE_3D, 0, m_internalFormat, width, height, depth, 0, GL_RGBA, GL_FLOAT, NULL);
	if (m_minFilter == 9984 || m_minFilter == 9985 || m_minFilter == 9986 || m_minFilter == 9987)
		glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
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

void VolumeBuffer::writeVolumeToRaw(const char* fileName) {
	
	float* bytes = (float*)malloc(getWidth() * getHeight() * getDepth() * 4 * sizeof(float));
	glBindTexture(GL_TEXTURE_3D, m_texture);
	glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, bytes);
	glBindTexture(GL_TEXTURE_3D, 0);

	
	FILE* pFile = fopen(fileName, "wb");
	if (pFile != NULL) {
		fwrite(bytes, sizeof(float) * 4, getWidth() * getHeight() * getDepth(), pFile);
		fclose(pFile);
	}
	free(bytes);
}

void VolumeBuffer::getVolume(unsigned int& texture) {
	
	float* bytes = (float*)malloc(getWidth() * getHeight() * getDepth() * 4 * sizeof(float));
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

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, getWidth(), getHeight(), getDepth(), 0, GL_RGBA, GL_FLOAT, bytes);
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