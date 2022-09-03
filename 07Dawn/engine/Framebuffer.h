#pragma once
#include <vector>
#include "Extension.h"
#include "../Constants.h"

class Framebuffer {

public:
	enum Attachments {
		COLOR,
		DEPTH24,
		DEPTH32,
		DEPTH32F,
		STENCIL,
		DEPTH_STENCIL
	};

	Framebuffer();
	~Framebuffer();

	void create(unsigned int width, unsigned int height);
	void attachTexture(Attachments attachments);
	void attachLayerdTexture(Attachments attachments, unsigned short layer);
	void attachRenderbuffer(Attachments attachments);
	void bind();
	void bindWrite();
	void bindRead();
	static void Unbind();

	static void UnbindWrite() ;
	static void UnbindRead();

	unsigned int getColorTexture(unsigned short attachment);
	unsigned int getColorTexture() const;
	unsigned int getDepthTexture();
	unsigned int getStencilTexture();
	unsigned int getDepthStencilTexture();
	unsigned int getFramebuffer();
private:

	unsigned int m_fbo = 0;

	unsigned int m_colorTexture = 0;
	unsigned int m_depthTexture = 0;
	unsigned int m_stencilTexture = 0;
	unsigned int m_depthStencilTexture = 0;

	unsigned short m_colorAttachments = 0;
	unsigned int m_width;
	unsigned int m_height;

	std::vector<unsigned int> m_attachments;
	std::vector<unsigned int> m_colorTextures;
};