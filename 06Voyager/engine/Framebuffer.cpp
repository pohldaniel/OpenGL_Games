#include "Framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer() {

}

Framebuffer::~Framebuffer() {

}

void Framebuffer::create(unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;
}

void Framebuffer::attachTexture(Attachments attachments) {
	unsigned int internalFormat = GL_RGBA8;
	unsigned int format = GL_RGBA;
	unsigned int type = GL_UNSIGNED_BYTE;
	unsigned int attachment;
	unsigned int *texture = &m_stencilTexture;

	switch (attachments) {
	case Framebuffer::COLOR:
		internalFormat = GL_RGBA8;
		format = GL_RGBA;
		m_colorAttachments++;

		unsigned int tex;
		m_colorTextures.push_back(tex);
		texture = &m_colorTextures[m_colorAttachments - 1];

		m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

		break;
	case Framebuffer::DEPTH24:
		internalFormat = GL_DEPTH_COMPONENT24;
		format = GL_DEPTH_COMPONENT;
		attachment = GL_DEPTH_ATTACHMENT;
		texture = &m_depthTexture;
		break;
	case Framebuffer::DEPTH32:
		internalFormat = GL_DEPTH_COMPONENT32;
		format = GL_DEPTH_COMPONENT;
		attachment = GL_DEPTH_ATTACHMENT;
		texture = &m_depthTexture;
		break;
	case Framebuffer::DEPTH32F:
		internalFormat = GL_DEPTH_COMPONENT32F;
		format = GL_DEPTH_COMPONENT;
		type = GL_FLOAT;
		attachment = GL_DEPTH_ATTACHMENT;
		texture = &m_depthTexture;
		break;
	case Framebuffer::STENCIL:
		internalFormat = GL_STENCIL_INDEX8;
		format = GL_STENCIL_INDEX;
		attachment = GL_STENCIL_ATTACHMENT;
		texture = &m_stencilTexture;
		break;
	case Framebuffer::DEPTH_STENCIL:
		internalFormat = GL_DEPTH24_STENCIL8;
		format = GL_DEPTH_STENCIL;
		type = GL_UNSIGNED_INT_24_8;
		attachment = GL_DEPTH_STENCIL_ATTACHMENT;
		texture = &m_depthStencilTexture;
		break;
	default:
		break;
	}

	
	glGenTextures(1, &*texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Framebuffer::DEPTH24 ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Framebuffer::DEPTH24 ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}

	if (attachments == Framebuffer::COLOR) {

		/*glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), GL_TEXTURE_2D, *texture, 0);
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(m_colorAttachments, attachments);
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);*/

		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), GL_TEXTURE_2D, *texture, 0);
		glDrawBuffers(m_colorAttachments, &m_attachments[0]);
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);
	}else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, *texture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Framebuffer::attachRenderbuffer(Attachments attachments) {
	unsigned int internalFormat = GL_RGBA8;
	unsigned int attachment;
	
	switch (attachments) {
	case Framebuffer::COLOR:
		internalFormat = GL_RGBA8;
		m_colorAttachments++;
		//m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));
		break;
	case Framebuffer::DEPTH24:
		internalFormat = GL_DEPTH_COMPONENT24;
		attachment = GL_DEPTH_ATTACHMENT;
		break;
	case Framebuffer::DEPTH32:
		internalFormat = GL_DEPTH_COMPONENT32;
		attachment = GL_DEPTH_ATTACHMENT;
		break;
	case Framebuffer::DEPTH32F:
		internalFormat = GL_DEPTH_COMPONENT32F;
		attachment = GL_DEPTH_ATTACHMENT;
		break;
	case Framebuffer::STENCIL:
		internalFormat = GL_STENCIL_INDEX8;
		attachment = GL_STENCIL_ATTACHMENT;
		break;
	case Framebuffer::DEPTH_STENCIL:
		internalFormat = GL_DEPTH24_STENCIL8;
		attachment = GL_DEPTH_STENCIL_ATTACHMENT;
		break;
	default:
		break;
	}


	unsigned int renderbuffer;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_width, m_height);

	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}

	if (attachments == Framebuffer::COLOR) {
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, m_fbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), GL_RENDERBUFFER, renderbuffer);
		//glDrawBuffers(m_colorAttachments, &m_attachments[0]);
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);

	}else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Framebuffer::getColorTexture(unsigned short attachment) {
	return m_colorTextures[attachment];
}

unsigned int Framebuffer::getColorTexture() const {
	return  m_colorTextures[0];
}

unsigned int Framebuffer::getDepthTexture() {
	return m_depthTexture;
}

unsigned int Framebuffer::getStencilTexture() {
	return m_stencilTexture;
}

unsigned int Framebuffer::getDepthStencilTexture() {
	return  m_depthStencilTexture;
}

unsigned int Framebuffer::getFramebuffer() {
	return  m_fbo;
}