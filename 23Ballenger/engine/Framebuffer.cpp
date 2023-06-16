#include "Framebuffer.h"
#include <iostream>

unsigned int Framebuffer::Width = 1600;
unsigned int Framebuffer::Height = 800;

Framebuffer::Framebuffer() {
}

Framebuffer::~Framebuffer() {
	cleanup();
}

void Framebuffer::create() {
	bind = std::bind(&Framebuffer::bindNoneVP, this);
	bindWrite = std::bind(&Framebuffer::bindWriteNoneVP, this);
	bindRead = std::bind(&Framebuffer::bindReadNoneVP, this);

	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}
}

void Framebuffer::create(unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;

	bind = std::function<void()>{ [&]() {bindVP(); } };
	bindWrite = std::bind(&Framebuffer::bindWriteVP, this);
	bindRead = std::bind(&Framebuffer::bindReadVP, this);

	if (!m_fbo) {
		glGenFramebuffers(1, &m_fbo);
	}
}

void Framebuffer::attachTexture2D(AttachmentTex::AttachmentTex _attachment) {
	unsigned int internalFormat = GL_RGBA8;
	unsigned int format = GL_RGBA;
	unsigned int type = GL_UNSIGNED_BYTE;
	unsigned int attachment;
	unsigned int *texture = &m_stencilTexture;

	switch (_attachment) {
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
		case AttachmentTex::RGB:
			internalFormat = GL_RGB8;
			format = GL_RGB;
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
			type = GL_HALF_FLOAT;
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
			type = GL_HALF_FLOAT;
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
			type = GL_HALF_FLOAT;
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
		case AttachmentTex::R11FG11FB10F:
			internalFormat = GL_R11F_G11F_B10F;
			format = GL_RGB;
			type = GL_UNSIGNED_INT_10F_11F_11F_REV;
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
			texture = &m_colorTextures[m_colorTextureAttachments - 1];
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
			texture = &m_colorTextures[m_colorTextureAttachments - 1];
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (_attachment == AttachmentTex::DEPTH24 || _attachment == AttachmentTex::DEPTH32 || _attachment == AttachmentTex::DEPTH32F || _attachment == AttachmentTex::STENCIL || _attachment == AttachmentTex::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (_attachment == AttachmentTex::DEPTH24 || _attachment == AttachmentTex::DEPTH32 || _attachment == AttachmentTex::DEPTH32F || _attachment == AttachmentTex::STENCIL || _attachment == AttachmentTex::DEPTH_STENCIL) ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (_attachment == AttachmentTex::SRGBA || _attachment == AttachmentTex::RGBA || _attachment == AttachmentTex::RGB || _attachment == AttachmentTex::RGBA32F || _attachment == AttachmentTex::RGBA16F || _attachment == AttachmentTex::RGB32F || _attachment == AttachmentTex::RGB16F || _attachment == AttachmentTex::RED || _attachment == AttachmentTex::RED32F || _attachment == AttachmentTex::RG16F || _attachment == AttachmentTex::R11FG11FB10F) {

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

void Framebuffer::attachTexture(unsigned int& texture, Attachment::Attachment _attachment, Target::Target target, unsigned short layer) {
	unsigned int attachment;

	switch (_attachment) {
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

	if (_attachment == Attachment::COLOR) {
		int colorAttachments = m_colorAttachments;
		glBindFramebuffer(colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, m_fbo);

		if (target == Target::TEXTURE3D) {
			glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), GL_TEXTURE_3D, texture, 0, layer);
		}else if (target == Target::TEXTURE2D){
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), GL_TEXTURE_2D, texture, 0);
		}else if (target == Target::ARRAY){

			for (unsigned int i = 0; i < layer; i++) {
				if (i != 0) {
					m_colorAttachments++;
					m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1));
				}
				glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), texture, 0, i);
			}

		} else{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_colorAttachments - 1), texture, 0);
		}

		glDrawBuffers(m_colorAttachments, &m_attachments[0]);
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_FRONT);
		glBindFramebuffer(colorAttachments == 1 ? GL_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);

	}else {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		if (target == Target::TEXTURE3D) {
			glFramebufferTexture3D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_3D, texture, 0, layer);
		}else if (target == Target::TEXTURE2D) {
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
		}else if (target == Target::ARRAY) {
			glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, attachment, texture, 0, layer);			
		}else {
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, attachment, texture, 0);
		}

		if (m_colorAttachments == 0) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
}

/*void Framebuffer::attachLayerdTexture(AttachmentTex::AttachmentTex attachments, unsigned short layer) {
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
			m_colorTextureAttachments++;

			unsigned int tex;
			m_colorTextures.push_back(tex);
			texture = &m_colorTextures[m_colorAttachments - 1];

			m_attachments.push_back(GL_COLOR_ATTACHMENT0 + (m_colorTextureAttachments - 1));

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
}*/


void Framebuffer::attachRenderbuffer(AttachmentRB::AttachmentRB attachments, unsigned int samples, unsigned int coverageSamples) {
	unsigned int internalFormat = GL_RGBA8;
	unsigned int attachment;
	unsigned int *rb;
	switch (attachments) {
		unsigned int renderBuffer;
		case AttachmentRB::RGBA:
			internalFormat = GL_RGBA8;
			m_colorAttachments++;
			m_colorRB.push_back(renderBuffer);
			rb = &m_colorRB[m_colorRB.size() - 1];

			m_resizeRB.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeRB[m_resizeRB.size() - 1]) = internalFormat;
			std::get<1>(m_resizeRB[m_resizeRB.size() - 1]) = samples;
			std::get<2>(m_resizeRB[m_resizeRB.size() - 1]) = coverageSamples;

			break;
		case AttachmentRB::RGB:
			internalFormat = GL_RGB8;
			m_colorAttachments++;
			m_colorRB.push_back(renderBuffer);
			rb = &m_colorRB[m_colorRB.size() - 1];

			m_resizeRB.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeRB[m_resizeRB.size() - 1]) = internalFormat;
			std::get<1>(m_resizeRB[m_resizeRB.size() - 1]) = samples;
			std::get<2>(m_resizeRB[m_resizeRB.size() - 1]) = coverageSamples;

			break;
		case AttachmentRB::RGBA32F:
			internalFormat = GL_RGBA32F;
			m_colorAttachments++;
			m_colorRB.push_back(renderBuffer);
			rb = &m_colorRB[m_colorRB.size() - 1];

			m_resizeRB.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeRB[m_resizeRB.size() - 1]) = internalFormat;
			std::get<1>(m_resizeRB[m_resizeRB.size() - 1]) = samples;
			std::get<2>(m_resizeRB[m_resizeRB.size() - 1]) = coverageSamples;

			break;
		case AttachmentRB::RGBA16F:
			internalFormat = GL_RGBA16F;
			m_colorAttachments++;
			m_colorRB.push_back(renderBuffer);
			rb = &m_colorRB[m_colorRB.size() - 1];

			m_resizeRB.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeRB[m_resizeRB.size() - 1]) = internalFormat;
			std::get<1>(m_resizeRB[m_resizeRB.size() - 1]) = samples;
			std::get<2>(m_resizeRB[m_resizeRB.size() - 1]) = coverageSamples;

			break;
		case AttachmentRB::R11FG11FB10F:
			internalFormat = GL_R11F_G11F_B10F;
			m_colorAttachments++;
			m_colorRB.push_back(renderBuffer);
			rb = &m_colorRB[m_colorRB.size() - 1];

			m_resizeRB.push_back(std::tuple<unsigned int, unsigned int, unsigned int>());
			std::get<0>(m_resizeRB[m_resizeRB.size() - 1]) = internalFormat;
			std::get<1>(m_resizeRB[m_resizeRB.size() - 1]) = samples;
			std::get<2>(m_resizeRB[m_resizeRB.size() - 1]) = coverageSamples;

			break;
		case AttachmentRB::DEPTH16:
			internalFormat = GL_DEPTH_COMPONENT16;
			attachment = GL_DEPTH_ATTACHMENT;
			rb = &m_depthRB;
			std::get<0>(depthRB) = internalFormat;
			std::get<1>(depthRB) = samples;
			std::get<2>(depthRB) = coverageSamples;
			break;
		case AttachmentRB::DEPTH24:
			internalFormat = GL_DEPTH_COMPONENT24;
			attachment = GL_DEPTH_ATTACHMENT;
			rb = &m_depthRB;
			std::get<0>(depthRB) = internalFormat;
			std::get<1>(depthRB) = samples;
			std::get<2>(depthRB) = coverageSamples;
			break;
		case AttachmentRB::DEPTH32F:
			internalFormat = GL_DEPTH_COMPONENT32F;
			attachment = GL_DEPTH_ATTACHMENT;
			rb = &m_depthRB;
			std::get<0>(depthRB) = internalFormat;
			std::get<1>(depthRB) = samples;
			std::get<2>(depthRB) = coverageSamples;
			break;
		case AttachmentRB::STENCIL:
			internalFormat = GL_STENCIL_INDEX8;
			attachment = GL_STENCIL_ATTACHMENT;
			rb = &m_stencilRB;
			std::get<0>(stencilRB) = internalFormat;
			std::get<1>(stencilRB) = samples;
			std::get<2>(stencilRB) = coverageSamples;

			break;
		case AttachmentRB::DEPTH_STENCIL:
			internalFormat = GL_DEPTH24_STENCIL8;
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			rb = &m_depthStencilRB;
			std::get<0>(depthStencilRB) = internalFormat;
			std::get<1>(depthStencilRB) = samples;
			std::get<2>(depthStencilRB) = coverageSamples;
			break;
		default:
			break;
	}

	glGenRenderbuffers(1, &*rb);
	glBindRenderbuffer(GL_RENDERBUFFER, *rb);

	if (samples > 0) {
		if ((coverageSamples > 0) && glRenderbufferStorageMultisampleCoverageNV) {
			glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, coverageSamples, samples, internalFormat, m_width, m_height);
		}else {
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, m_width, m_height);
		}
	}else {
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_width, m_height);
	}

	if (attachments == AttachmentRB::RGBA || attachments == AttachmentRB::RGB || attachments == AttachmentRB::RGBA32F || attachments == AttachmentRB::RGBA16F || attachments == AttachmentRB::R11FG11FB10F) {
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

	for (unsigned short i = 0; i < m_colorTextures.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, m_colorTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, std::get<0>(m_resizeTexture[i]), width, height, 0, std::get<1>(m_resizeTexture[i]), std::get<2>(m_resizeTexture[i]), NULL);

	}
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_depthRB) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthRB);
		if (std::get<1>(depthRB) > 0) {
			if ((std::get<2>(depthRB) > 0) && glRenderbufferStorageMultisampleCoverageNV) {
				glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, std::get<2>(depthRB), std::get<1>(depthRB), std::get<0>(depthRB), width, height);
			}else {
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, std::get<1>(depthRB), std::get<0>(depthRB), width, height);
			}
		}else {
			glRenderbufferStorage(GL_RENDERBUFFER, std::get<0>(depthRB), width, height);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	if (m_stencilRB) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_stencilRB);
		if (std::get<1>(stencilRB) > 0) {
			if ((std::get<2>(depthRB) > 0) && glRenderbufferStorageMultisampleCoverageNV) {
				glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, std::get<2>(stencilRB), std::get<1>(stencilRB), std::get<0>(stencilRB), width, height);
			}else {
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, std::get<1>(stencilRB), std::get<0>(stencilRB), width, height);
			}
		}else {
			glRenderbufferStorage(GL_RENDERBUFFER, std::get<0>(stencilRB), width, height);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	if (m_depthStencilRB) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilRB);
		if (std::get<1>(depthStencilRB) > 0) {
			if ((std::get<2>(depthRB) > 0) && glRenderbufferStorageMultisampleCoverageNV) {
				glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, std::get<2>(depthStencilRB), std::get<1>(depthStencilRB), std::get<0>(depthStencilRB), width, height);
			}else {
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, std::get<1>(depthStencilRB), std::get<0>(depthStencilRB), width, height);
			}
		}else {
			glRenderbufferStorage(GL_RENDERBUFFER, std::get<0>(depthStencilRB), width, height);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	for (unsigned short i = 0; i < m_colorRB.size(); i++) {
		glBindRenderbuffer(GL_RENDERBUFFER, m_colorRB[i]);

		if (std::get<1>(m_resizeRB[i]) > 0) {
			if ((std::get<2>(m_resizeRB[i]) > 0) && glRenderbufferStorageMultisampleCoverageNV) {
				glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, std::get<2>(m_resizeRB[i]), std::get<1>(m_resizeRB[i]), std::get<0>(m_resizeRB[i]), width, height);
			} else {
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, std::get<1>(m_resizeRB[i]), std::get<0>(m_resizeRB[i]), width, height);
			}
		} else {
			glRenderbufferStorage(GL_RENDERBUFFER, std::get<0>(m_resizeRB[i]), width, height);
		}
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	m_width = width;
	m_height = height;
}

void Framebuffer::cleanup() {

	if (m_depthTexture) {
		glDeleteTextures(1, &m_depthTexture);
		m_depthTexture = 0;
	}

	if (m_stencilTexture) {
		glDeleteTextures(1, &m_stencilTexture);
		m_stencilTexture = 0;
	}

	if (m_depthStencilTexture) {
		glDeleteTextures(1, &m_depthStencilTexture);
		m_depthStencilTexture = 0;
	}

	for (unsigned short i = 0; i < m_colorTextures.size(); i++) {
		glDeleteTextures(1, &m_colorTextures[i]);
		m_colorTextures[i] = 0;
	}

	if (m_depthRB) {
		glDeleteRenderbuffers(0, &m_depthRB);
		m_depthRB = 0;
	}

	if (m_stencilRB) {
		glDeleteRenderbuffers(0, &m_stencilRB);
		m_stencilRB = 0;
	}

	if (m_depthStencilRB) {
		glDeleteRenderbuffers(0, &m_depthStencilRB);
		m_depthStencilRB = 0;
	}

	for (unsigned short i = 0; i < m_colorRB.size(); i++) {
		glDeleteRenderbuffers(0, &m_colorRB[i]);
		m_colorRB[i] = 0;
	}

	if (m_fbo) {
		glDeleteFramebuffers(1, &m_fbo);
		m_fbo = 0;
	}

	m_colorAttachments = 0;
	m_colorTextureAttachments = 0;

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

void Framebuffer::cleanupWithoutTexture() {
	if (m_fbo) {
		glDeleteFramebuffers(1, &m_fbo);
		m_fbo = 0;
	}

	m_colorAttachments = 0;
	m_colorTextureAttachments = 0;

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

unsigned int Framebuffer::getWidth() {
	return m_width;
}

unsigned int Framebuffer::getHeight() {
	return m_height;
}

const unsigned int& Framebuffer::getColorTexture(unsigned short attachment) const {
	return m_colorTextures[attachment];
}

const unsigned int& Framebuffer::getColorTexture() const {
	return  m_colorTextures[0];
}

const unsigned int& Framebuffer::getDepthTexture() const {
	return m_depthTexture;
}

const unsigned int& Framebuffer::getStencilTexture() const {
	return m_stencilTexture;
}

const unsigned int& Framebuffer::getDepthStencilTexture() const {
	return  m_depthStencilTexture;
}

const unsigned int& Framebuffer::getFramebuffer() const {
	return  m_fbo;
}

void Framebuffer::bindColorTexture(unsigned int unit, unsigned int attachment) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_colorTextures[attachment]);
}

void Framebuffer::bindDepthTexture(unsigned int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
}

void Framebuffer::bindStencilTexture(unsigned int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_stencilTexture);
}

void Framebuffer::bindDepthStencilTexture(unsigned int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_depthStencilTexture);
}
