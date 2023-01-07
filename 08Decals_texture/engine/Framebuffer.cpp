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
		unsigned int tex;
		case Framebuffer::COLOR:
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
			m_colorAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			break;
		case Framebuffer::RED:
			internalFormat = GL_R8;
			format = GL_RED;
			m_colorAttachments++;

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
		case Framebuffer::DEPTH16F:
			internalFormat = GL_DEPTH_COMPONENT16;
			format = GL_DEPTH_COMPONENT;
			type = GL_HALF_FLOAT;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (Framebuffer::DEPTH24 || Framebuffer::DEPTH32 || Framebuffer::DEPTH32F || Framebuffer::STENCIL || Framebuffer::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (Framebuffer::DEPTH24 || Framebuffer::DEPTH32 || Framebuffer::DEPTH32F || Framebuffer::STENCIL || Framebuffer::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR);
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
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);
	
	}else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, *texture, 0);
		if (m_colorAttachments == 0) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Framebuffer::attachLayerdTexture(Attachments attachments, unsigned short layer) {
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
	glBindTexture(GL_TEXTURE_2D_ARRAY, *texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, (Framebuffer::DEPTH24 || Framebuffer::DEPTH32 || Framebuffer::DEPTH32F || Framebuffer::STENCIL || Framebuffer::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, (Framebuffer::DEPTH24 || Framebuffer::DEPTH32 || Framebuffer::DEPTH32F || Framebuffer::STENCIL || Framebuffer::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR);
	if(Framebuffer::COLOR)
		glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, m_width, m_height, layer, 0, format, type, NULL);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}

	if (attachments == Framebuffer::COLOR) {
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, m_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), *texture, NULL);
		glDrawBuffers(m_colorAttachments, &m_attachments[0]);
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);		
	}else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, *texture, 0);
		if (m_colorAttachments == 0) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
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
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);

	}else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer);		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Framebuffer::bind() {
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::bindWrite() {
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void Framebuffer::bindRead() {
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void Framebuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void Framebuffer::UnbindWrite() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}


void Framebuffer::UnbindRead() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
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