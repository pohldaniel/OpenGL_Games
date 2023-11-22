#pragma once
#include <vector>
#include <tuple>
#include "Extension.h"

class Framebuffer {

public:
	enum Attachments {
		RGBA,
		RED,
		RGBA32F,
		RGBA16F,
		RGB32F,
		RGB16F,
		DEPTH24,
		DEPTH32,
		DEPTH32F,
		DEPTH16F,
		STENCIL,
		DEPTH_STENCIL
	};

	Framebuffer();
	~Framebuffer();

	void create(unsigned int width, unsigned int height);
	void attachTexture(Attachments attachments);
	void attachTexture(Attachments attachments, unsigned int& texture);
	void attachLayerdTexture(Attachments attachments, unsigned short layer);
	void attachRenderbuffer(Attachments attachments);
	void bind();
	void bindWrite();
	void bindRead();
	void unbind();
	void resize(unsigned int width, unsigned int height);
	void clear();

	static void Unbind();
	static void UnbindWrite();
	static void UnbindRead();
	static void SetDefaultSize(unsigned int width, unsigned int height);

	unsigned int& getColorTexture(unsigned short attachment);
	unsigned int& getColorTexture();
	unsigned int& getDepthTexture();
	unsigned int& getStencilTexture();
	unsigned int& getDepthStencilTexture();
	unsigned int& getFramebuffer();

private:

	unsigned int m_fbo = 0;

	unsigned int m_depthTexture = 0;
	unsigned int m_stencilTexture = 0;
	unsigned int m_depthStencilTexture = 0;

	unsigned int m_depthRB = 0;
	unsigned int m_stencilRB = 0;
	unsigned int m_depthStencilRB = 0;

	unsigned int m_depthRBFormat = 0;
	unsigned int m_stencilRBFormat = 0;
	unsigned int m_depthStencilRBFormat = 0;

	unsigned short m_colorAttachments = 0;
	unsigned short m_colorTextureAttachments = 0;
	unsigned short m_colorRBAttachments = 0;
	unsigned int m_width;
	unsigned int m_height;

	static unsigned int Width;
	static unsigned int Height;

	std::vector<unsigned int> m_attachments;
	std::vector<unsigned int> m_colorTextures;
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> m_resizeTexture;

	std::vector<unsigned int> m_colorRB;
	std::vector<unsigned int> m_resizeRB;

	std::tuple<unsigned int, unsigned int, unsigned int> depth;
	std::tuple<unsigned int, unsigned int, unsigned int> stencil;
	std::tuple<unsigned int, unsigned int, unsigned int> depthStencil;
};