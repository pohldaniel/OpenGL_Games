#include "Framebuffer.h"
#include <iostream>

unsigned int Framebuffer::Width = 1600;
unsigned int Framebuffer::Height = 800;

Framebuffer::Framebuffer() {

}

Framebuffer::~Framebuffer() {

}

void Framebuffer::create() {
	bind = std::bind(&Framebuffer::bindNoneVP, this);
	bindWrite = std::bind(&Framebuffer::bindWriteNoneVP, this);
	bindRead = std::bind(&Framebuffer::bindReadNoneVP, this);
}

void Framebuffer::create(unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;

	bind = std::function<void()>{ [&]() {bindVP(); } };
	bindWrite = std::bind(&Framebuffer::bindWriteVP, this);
	bindRead = std::bind(&Framebuffer::bindReadVP, this);
}

void Framebuffer::attachTexture(AttachmentTex::AttachmentTex attachments) {
	unsigned int internalFormat = GL_RGBA8;
	unsigned int format = GL_RGBA;
	unsigned int type = GL_UNSIGNED_BYTE;
	unsigned int attachment;
	unsigned int *texture = &m_stencilTexture;

	switch (attachments) {
		unsigned int tex;
		case AttachmentTex::SRGBA:
			internalFormat = GL_SRGB8_ALPHA8;
			format = GL_RGBA;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorTextureAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;
		case AttachmentTex::RGBA:
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorTextureAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;
		case AttachmentTex::RGBA32F:
			internalFormat = GL_RGBA32F;
			format = GL_RGBA;
			type = GL_FLOAT;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorTextureAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;
		case AttachmentTex::RGBA16F:
			internalFormat = GL_RGBA16F;
			format = GL_RGBA;
			type = GL_FLOAT;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorTextureAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;
		case AttachmentTex::RGB32F:
			internalFormat = GL_RGB32F;
			format = GL_RGB;
			type = GL_FLOAT;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorTextureAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;
		case AttachmentTex::RGB16F:
			internalFormat = GL_RGB16F;
			format = GL_RGB;
			type = GL_FLOAT;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorTextureAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;
		case AttachmentTex::RG16F:
			internalFormat = GL_RG16F;
			format = GL_RG;
			type = GL_FLOAT;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorTextureAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;
		case AttachmentTex::RED:
			internalFormat = GL_R8;
			format = GL_RED;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorAttachments - 1];
			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;
		case AttachmentTex::RED32F:
			internalFormat = GL_R32F;
			format = GL_RED;
			type = GL_FLOAT;
			m_colorAttachments++;
			m_colorTextureAttachments++;

			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorAttachments - 1];
			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			m_resizeTexture.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeTexture[m_colorTextureAttachments - 1]) = internalFormat;
			std::get<1>(m_resizeTexture[m_colorTextureAttachments - 1]) = format;
			std::get<2>(m_resizeTexture[m_colorTextureAttachments - 1]) = type;

			break;

		case AttachmentTex::DEPTH24:
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			attachment = GL_DEPTH_ATTACHMENT;
			texture = &m_depthTexture;
			std::get<0>(depth) = internalFormat;
			std::get<1>(depth) = format;
			std::get<2>(depth) = type;
			break;
		case AttachmentTex::DEPTH32:
			internalFormat = GL_DEPTH_COMPONENT32;
			format = GL_DEPTH_COMPONENT;
			attachment = GL_DEPTH_ATTACHMENT;
			texture = &m_depthTexture;
			std::get<0>(depth) = internalFormat;
			std::get<1>(depth) = format;
			std::get<2>(depth) = type;
			break;
		case AttachmentTex::DEPTH32F:
			internalFormat = GL_DEPTH_COMPONENT32F;
			format = GL_DEPTH_COMPONENT;
			type = GL_FLOAT;
			attachment = GL_DEPTH_ATTACHMENT;
			texture = &m_depthTexture;
			std::get<0>(depth) = internalFormat;
			std::get<1>(depth) = format;
			std::get<2>(depth) = type;
			break;
		case AttachmentTex::DEPTH16F:
			internalFormat = GL_DEPTH_COMPONENT16;
			format = GL_DEPTH_COMPONENT;
			type = GL_HALF_FLOAT;
			attachment = GL_DEPTH_ATTACHMENT;
			texture = &m_depthTexture;
			std::get<0>(depth) = internalFormat;
			std::get<1>(depth) = format;
			std::get<2>(depth) = type;
			break;
		case AttachmentTex::STENCIL:
			internalFormat = GL_STENCIL_INDEX8;
			format = GL_STENCIL_INDEX;
			attachment = GL_STENCIL_ATTACHMENT;
			texture = &m_stencilTexture;
			std::get<0>(stencil) = internalFormat;
			std::get<1>(stencil) = format;
			std::get<2>(stencil) = type;
			break;
		case AttachmentTex::DEPTH_STENCIL:
			internalFormat = GL_DEPTH24_STENCIL8;
			format = GL_DEPTH_STENCIL;
			type = GL_UNSIGNED_INT_24_8;
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			texture = &m_depthStencilTexture;
			std::get<0>(depthStencil) = internalFormat;
			std::get<1>(depthStencil) = format;
			std::get<2>(depthStencil) = type;
			break;
		default:
			break;
	}


	glGenTextures(1, &*texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (attachments == AttachmentTex::DEPTH24 || attachments == AttachmentTex::DEPTH32 || attachments == AttachmentTex::DEPTH32F || attachments == AttachmentTex::STENCIL || attachments == AttachmentTex::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (attachments == AttachmentTex::DEPTH24 || attachments == AttachmentTex::DEPTH32 || attachments == AttachmentTex::DEPTH32F || attachments == AttachmentTex::STENCIL || attachments == AttachmentTex::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}

	if (attachments == AttachmentTex::RGBA || attachments == AttachmentTex::RGBA32F || attachments == AttachmentTex::RGBA16F || attachments == AttachmentTex::RGB32F || attachments == AttachmentTex::RGB16F || attachments == AttachmentTex::RED || attachments == AttachmentTex::RED32F || attachments == AttachmentTex::RG16F) {

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

void Framebuffer::attachTexture(unsigned int& texture, Attachment::Attachment attachments, Target::Target target, unsigned short layer) {
	unsigned int attachment;

	switch (attachments) {
		case Attachment::COLOR:
			m_colorAttachments++;
			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));
			break;
		case Attachment::DEPTH:
			attachment = GL_DEPTH_ATTACHMENT;
			break;
		case Attachment::STENCIL:
			attachment = GL_STENCIL_ATTACHMENT;
			break;
		case Attachment::DEPTH_STENCIL:
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			break;
	}

	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}

	if (attachments == Attachment::COLOR) {
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, m_fbo);

		if (target == Target::TEXTURE3D) {
			glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), GL_TEXTURE_3D, texture, 0, layer);
		}else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), GL_TEXTURE_2D, texture, 0);
		}

		glDrawBuffers(m_colorAttachments, &m_attachments[0]);
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);

	}else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		if (target == Target::TEXTURE3D) {
			glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, texture, 0, layer);
		}else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
		}

		if (m_colorAttachments == 0) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Framebuffer::attachLayerdTexture(AttachmentTex::AttachmentTex attachments, unsigned short layer) {
	unsigned int internalFormat = GL_RGBA8;
	unsigned int format = GL_RGBA;
	unsigned int type = GL_UNSIGNED_BYTE;
	unsigned int attachment;
	unsigned int *texture = &m_stencilTexture;

	switch (attachments) {
		case AttachmentTex::RGBA:
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
			m_colorAttachments++;
			m_colorRBAttachments++;

			unsigned int tex;
			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));

			break;
		case AttachmentTex::DEPTH24:
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			attachment = GL_DEPTH_ATTACHMENT;
			texture = &m_depthTexture;
			break;
		case AttachmentTex::DEPTH32:
			internalFormat = GL_DEPTH_COMPONENT32;
			format = GL_DEPTH_COMPONENT;
			attachment = GL_DEPTH_ATTACHMENT;
			texture = &m_depthTexture;
			break;
		case AttachmentTex::DEPTH32F:
			internalFormat = GL_DEPTH_COMPONENT32F;
			format = GL_DEPTH_COMPONENT;
			type = GL_FLOAT;
			attachment = GL_DEPTH_ATTACHMENT;
			texture = &m_depthTexture;
			break;
		case AttachmentTex::STENCIL:
			internalFormat = GL_STENCIL_INDEX8;
			format = GL_STENCIL_INDEX;
			attachment = GL_STENCIL_ATTACHMENT;
			texture = &m_stencilTexture;
			break;
		case AttachmentTex::DEPTH_STENCIL:
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
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, (attachments == AttachmentTex::DEPTH24 || attachments == AttachmentTex::DEPTH32 || attachments == AttachmentTex::DEPTH32F || attachments == AttachmentTex::STENCIL || attachments == AttachmentTex::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, (attachments == AttachmentTex::DEPTH24 || attachments == AttachmentTex::DEPTH32 || attachments == AttachmentTex::DEPTH32F || attachments == AttachmentTex::STENCIL || attachments == AttachmentTex::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR);
	if (attachments == AttachmentTex::RGBA)
		glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, m_width, m_height, layer, 0, format, type, NULL);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}

	if (attachments == AttachmentTex::RGBA) {
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

void Framebuffer::attachRenderbuffer(AttachmentRB::AttachmentRB attachments) {
	unsigned int internalFormat = GL_RGBA8;
	unsigned int attachment;
	unsigned int *rb;
	switch (attachments) {
		unsigned int renderBuffer;
		case AttachmentRB::RGBA:
			internalFormat = GL_RGBA8;
			m_colorAttachments++;
			m_colorRBAttachments++;
			m_colorRB.push_back(renderBuffer);
			rb = &m_colorRB[m_colorRBAttachments - 1];
			m_resizeRB.push_back(internalFormat);
			break;
		case AttachmentRB::DEPTH16:
			internalFormat = GL_DEPTH_COMPONENT16;
			attachment = GL_DEPTH_ATTACHMENT;
			rb = &m_depthRB;
			m_depthRBFormat = internalFormat;
			break;
		case AttachmentRB::DEPTH24:
			internalFormat = GL_DEPTH_COMPONENT24;
			attachment = GL_DEPTH_ATTACHMENT;
			rb = &m_depthRB;
			m_depthRBFormat = internalFormat;
			break;
		case AttachmentRB::DEPTH32F:
			internalFormat = GL_DEPTH_COMPONENT32F;
			attachment = GL_DEPTH_ATTACHMENT;
			rb = &m_depthRB;
			m_depthRBFormat = internalFormat;
			break;
		case AttachmentRB::STENCIL:
			internalFormat = GL_STENCIL_INDEX8;
			attachment = GL_STENCIL_ATTACHMENT;
			rb = &m_stencilRB;
			m_stencilRBFormat = internalFormat;
			break;
		case AttachmentRB::DEPTH_STENCIL:
			internalFormat = GL_DEPTH24_STENCIL8;
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			rb = &m_depthStencilRB;
			m_depthStencilRBFormat = internalFormat;
			break;
		default:
			break;
	}

	glGenRenderbuffers(1, &*rb);
	glBindRenderbuffer(GL_RENDERBUFFER, *rb);
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_width, m_height);

	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}

	if (attachments == AttachmentRB::RGBA) {
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, m_fbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), GL_RENDERBUFFER, *rb);
		glBindFramebuffer(m_colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);

	}else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, *rb);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Framebuffer::resize(unsigned int width, unsigned int height) {

	if (m_depthTexture) {
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, std::get<0>(depth), width, height, 0, std::get<1>(depth), std::get<2>(depth), NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (m_stencilTexture) {
		glBindTexture(GL_TEXTURE_2D, m_stencilTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, std::get<0>(stencil), width, height, 0, std::get<1>(stencil), std::get<2>(stencil), NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (m_depthStencilTexture) {
		glBindTexture(GL_TEXTURE_2D, m_depthStencilTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, std::get<0>(depthStencil), width, height, 0, std::get<1>(depthStencil), std::get<2>(depthStencil), NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	for (unsigned short i = 0; i < m_colorTextureAttachments; i++) {
		glBindTexture(GL_TEXTURE_2D, m_colorTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, std::get<0>(m_resizeTexture[i]), width, height, 0, std::get<1>(m_resizeTexture[i]), std::get<2>(m_resizeTexture[i]), NULL);

	}
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_depthRB) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthRB);
		glRenderbufferStorage(GL_RENDERBUFFER, m_depthRBFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	if (m_stencilRB) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_stencilRB);
		glRenderbufferStorage(GL_RENDERBUFFER, m_stencilRBFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	if (m_depthStencilRB) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilRB);
		glRenderbufferStorage(GL_RENDERBUFFER, m_depthStencilRBFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	for (unsigned short i = 0; i < m_colorRBAttachments; i++) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_colorRB[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, m_resizeRB[i], width, height);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	m_width = width;
	m_height = height;
}

void Framebuffer::clear() {
	m_colorAttachments = 0;
	m_colorTextureAttachments = 0;
	m_colorRBAttachments = 0;

	m_attachments.clear();
	m_colorTextures.clear();
	m_resizeTexture.clear();

	m_colorRB.clear();
	m_resizeRB.clear();

	m_attachments.shrink_to_fit();
	m_colorTextures.shrink_to_fit();
	m_resizeTexture.shrink_to_fit();

	m_colorRB.shrink_to_fit();
	m_resizeRB.shrink_to_fit();
}

void Framebuffer::cleanup() {

}

void Framebuffer::bindVP(unsigned int width, unsigned int height) {
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::bindVP() {
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::bindWriteVP() {
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void Framebuffer::bindReadVP() {
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void Framebuffer::bindNoneVP() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::bindWriteNoneVP() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void Framebuffer::bindReadNoneVP() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Width, Height);
}

void Framebuffer::unbindWrite() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, Width, Height);
}

void Framebuffer::unbindRead() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glViewport(0, 0, Width, Height);
}

void Framebuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Width, Height);
}

void Framebuffer::UnbindWrite() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, Width, Height);
}

void Framebuffer::UnbindRead() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glViewport(0, 0, Width, Height);
}

void Framebuffer::SetDefaultSize(unsigned int width, unsigned int height) {
	Width = width;
	Height = height;
}

unsigned int& Framebuffer::getColorTexture(unsigned short attachment) {
	return m_colorTextures[attachment];
}

unsigned int& Framebuffer::getColorTexture() {
	return  m_colorTextures[0];
}

unsigned int& Framebuffer::getDepthTexture() {
	return m_depthTexture;
}

unsigned int& Framebuffer::getStencilTexture() {
	return m_stencilTexture;
}

unsigned int& Framebuffer::getDepthStencilTexture() {
	return  m_depthStencilTexture;
}

unsigned int& Framebuffer::getFramebuffer() {
	return  m_fbo;
}