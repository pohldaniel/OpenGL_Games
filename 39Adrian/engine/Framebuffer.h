#pragma once

#include <GL/glew.h>
#include <vector>
#include <tuple>
#include <functional>

namespace Target {
	typedef enum Target {
		TEXTURE2D,
		TEXTURE3D,
		ARRAY,
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
		RGB,
		RED,
		RED32F,
		RGBA32F,
		RGBA16F,
		RGB32F,
		RGB16F,
		RG16F,
		R11FG11FB10F,
		DEPTH16,
		DEPTH24,
		DEPTH32,
		DEPTH32F,
		DEPTH16F,		
		DEPTH_STENCIL,
		STENCIL,
	} AttachmentTex;
}

//https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glRenderbufferStorage.xhtml
namespace AttachmentRB {
	typedef enum AttachmentRB {
		RGBA,
		RGB,
		RED,
		RGBA32F,
		RGBA16F,
		R11FG11FB10F,
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
	void attachTexture2D(AttachmentTex::AttachmentTex attachment);
	void attachTexture(const unsigned int& texture, Attachment::Attachment attachment = Attachment::Attachment::COLOR, Target::Target target = Target::Target::TEXTURE2D, unsigned short layer = 0);

	void attachLayerdTexture(AttachmentTex::AttachmentTex attachment, unsigned short layer);
	void attachRenderbuffer(AttachmentRB::AttachmentRB attachment, unsigned int samples = 0u, unsigned int coverageSamples = 0u);
	void bindVP(unsigned int width, unsigned int height);

	void unbind() const;
	void unbindWrite() const;
	void unbindRead() const;

	std::function<void()> bind = 0;
	std::function<void()> bindWrite = 0;
	std::function<void()> bindRead = 0;

	void resize(unsigned int width, unsigned int height);
	void cleanup();
	unsigned int getWidth();
	unsigned int getHeight();
	void bindColorTexture(unsigned short attachment = 0u, unsigned int unit = 0u, bool forceBind = false) const;
	void bindDepthTexture(unsigned int unit = 0u) const;
	void bindStencilTexture(unsigned int unit = 0u) const;
	void bindDepthStencilTexture(unsigned int unit = 0u) const;

	void unbindTexture(unsigned int unit = 0u) const;

	void blitBackBufferToDepthTexture() const;

	const unsigned int& getColorTexture(unsigned short attachment = 0) const;
	const unsigned int& getDepthTexture() const;
	const unsigned int& getStencilTexture() const;
	const unsigned int& getDepthStencilTexture() const;
	const unsigned int& getFramebuffer() const;

	static void Unbind();
	static void UnbindWrite();
	static void UnbindRead();
	static void SetDefaultSize(unsigned int width, unsigned int height);
	static unsigned int GetDefaultWidth();
	static unsigned int GetDefaultHeight();

private:

	unsigned int m_fbo = 0;

	unsigned int m_depthTexture = 0;
	unsigned int m_stencilTexture = 0;
	unsigned int m_depthStencilTexture = 0;

	unsigned int m_depthRB = 0;
	unsigned int m_stencilRB = 0;
	unsigned int m_depthStencilRB = 0;

	//format | sample | coverageSamples
	std::tuple<unsigned int, unsigned int, unsigned int> depthRB;
	std::tuple<unsigned int, unsigned int, unsigned int> stencilRB;
	std::tuple<unsigned int, unsigned int, unsigned int> depthStencilRB;

	unsigned short m_colorAttachments = 0;
	unsigned short m_colorTextureAttachments = 0;

	unsigned int m_width = 0;
	unsigned int m_height = 0;

	static unsigned int Width;
	static unsigned int Height;

	std::vector<unsigned int> m_attachments;

	std::vector<unsigned int> m_colorTextures;
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> m_resizeTexture;

	std::vector<unsigned int> m_colorRB;
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> m_resizeRB;

	//internalFormat | format | type
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