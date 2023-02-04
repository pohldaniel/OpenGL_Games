#pragma once
#include <GL/glew.h>
#include <vector>
#include <tuple>
#include <functional>

namespace Target {
	typedef enum Target {
		TEXTURE2D,
		TEXTURE3D,
		TEXTURE
	} Target;
}

namespace Attachment {
	typedef enum Attachment {
		COLOR,
		DEPTH,
		STENCIL,
		DEPTH_STENCIL
	} Attachment;
}

namespace AttachmentTex {
	typedef enum AttachmentTex {
		SRGBA,
		RGBA,
		RED,
		RED32F,
		RGBA32F,
		RGBA16F,
		RGB32F,
		RGB16F,
		RG16F,
		DEPTH24,
		DEPTH32,
		DEPTH32F,
		DEPTH16F,
		STENCIL,
		DEPTH_STENCIL
	} AttachmentTex;
}

namespace AttachmentRB {
	typedef enum AttachmentRB {
		RGBA,
		DEPTH16,
		DEPTH24,
		DEPTH32F,
		DEPTH_STENCIL,
		STENCIL
	}AttachmentRB;
}

class Framebuffer {

public:

	Framebuffer();
	~Framebuffer();

	void create();
	void create(unsigned int width, unsigned int height);
	void attachTexture(AttachmentTex::AttachmentTex attachments);
	void attachTexture(unsigned int& texture, Attachment::Attachment attachments = Attachment::Attachment::COLOR, Target::Target target = Target::Target::TEXTURE2D, unsigned short layer = 0);

	void attachLayerdTexture(AttachmentTex::AttachmentTex attachments, unsigned short layer);
	void attachRenderbuffer(AttachmentRB::AttachmentRB attachments);
	void bindVP(unsigned int width, unsigned int height);

	void unbind();
	void unbindWrite();
	void unbindRead();

	std::function<void()> bind = 0;
	std::function<void()> bindWrite = 0;
	std::function<void()> bindRead = 0;

	void resize(unsigned int width, unsigned int height);
	void clear();
	void cleanup();
	unsigned int getWidth();
	unsigned int getHeight();
	

	unsigned int& getColorTexture(unsigned short attachment);
	unsigned int& getColorTexture();
	unsigned int& getDepthTexture();
	unsigned int& getStencilTexture();
	unsigned int& getDepthStencilTexture();
	unsigned int& getFramebuffer();

	static void Unbind();
	static void UnbindWrite();
	static void UnbindRead();
	static void SetDefaultSize(unsigned int width, unsigned int height);

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
	unsigned int m_width = 0;
	unsigned int m_height = 0;

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

	void bindVP();
	void bindWriteVP();
	void bindReadVP();

	void bindNoneVP();
	void bindWriteNoneVP();
	void bindReadNoneVP();
};