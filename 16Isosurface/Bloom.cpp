#include "Bloom.h"
#include "Constants.h"

bloomFBO::bloomFBO() : mInit(false) {}
bloomFBO::~bloomFBO() {}

bool bloomFBO::Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength) {
	if (mInit) return true;

	Vector2f mipSize((float)windowWidth, (float)windowHeight);
	std::array<int, 2> mipIntSize = { (int)windowWidth, (int)windowHeight };

	if (windowWidth > (unsigned int)INT_MAX || windowHeight > (unsigned int)INT_MAX) {
		std::cerr << "Window size conversion overflow - cannot build bloom FBO!" << std::endl;
		return false;
	}

	for (GLuint i = 0; i < mipChainLength; i++) {
		bloomMip mip;

		mipSize *= 0.5f;
		mipIntSize[0] /= 2;
		mipIntSize[1] /= 2;
		mip.size = mipSize;
		mip.intSize[0] = mipIntSize[0];
		mip.intSize[1] = mipIntSize[1];
		Texture::CreateEmptyTexture(mip.texture, (int)mipSize[0], (int)mipSize[1], GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT);
	
		mMipChain.emplace_back(mip);
	}

	m_fbo.create();
	m_fbo.attachTexture(mMipChain[0].texture, Attachment::COLOR);

	mInit = true;
	return true;
}

void bloomFBO::Destroy() {
	m_fbo.cleanup();
	mInit = false;
}

void bloomFBO::BindForWriting() {
	m_fbo.bind();
}

std::vector<bloomMip>& bloomFBO::MipChain(){
	return mMipChain;
}

/////////////////////////////////////////////////////////////////////
BloomRenderer::BloomRenderer() : mInit(false) {}
BloomRenderer::~BloomRenderer() {}

bool BloomRenderer::Init(unsigned int windowWidth, unsigned int windowHeight) {
	if (mInit) return true;
	m_quad = new Quad(Vector2f(-1.0f, -1.0f), Vector2f(2.0f, 2.0f));
	
	mSrcViewportSize = { (int)windowWidth, (int)windowHeight };
	mSrcViewportSizeFloat = Vector2f((float)windowWidth, (float)windowHeight);

	// Framebuffer
	const unsigned int num_bloom_mips = 6; // TODO: Play around with this value
	bool status = mFBO.Init(windowWidth, windowHeight, num_bloom_mips);
	if (!status) {
		std::cerr << "Failed to initialize bloom FBO - cannot create bloom renderer!\n";
		return false;
	}

	// Shaders
	mDownsampleShader = new Shader("res/shader/compute/6.new_downsample.vs", "res/shader/compute/6.new_downsample.fs");
	mUpsampleShader = new Shader("res/shader/compute/6.new_upsample.vs", "res/shader/compute/6.new_upsample.fs");

	// Downsample
	glUseProgram(mDownsampleShader->m_program);
	mDownsampleShader->loadInt("srcTexture", 0);
	glUseProgram(0);

	// Upsample
	glUseProgram(mUpsampleShader->m_program);
	mUpsampleShader->loadInt("srcTexture", 0);
	glUseProgram(0);
	return true;
}

void BloomRenderer::Destroy() {
	mFBO.Destroy();
	delete mDownsampleShader;
	delete mUpsampleShader;
}

void BloomRenderer::RenderDownsamples(unsigned int srcTexture) {

	const std::vector<bloomMip>& mipChain = mFBO.MipChain();

	glUseProgram(mDownsampleShader->m_program);
	mDownsampleShader->loadVector("srcResolution", mSrcViewportSizeFloat);
	if (mKarisAverageOnDownsample) {
		mDownsampleShader->loadInt("mipLevel", 0);
	}

	// Bind srcTexture (HDR color buffer) as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	// Progressively downsample through the mip chain
	for (int i = 0; i < (int)mipChain.size(); i++){
		const bloomMip& mip = mipChain[i];

		// Set framebuffer render target (we write to this texture)
		glViewport(0, 0, mip.size[0], mip.size[1]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		m_quad->drawRaw();

		// Set current mip resolution as srcResolution for next iteration
		mDownsampleShader->loadVector("srcResolution", mip.size);
		// Set current mip as texture input for next iteration
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// Disable Karis average for consequent downsamples
		if (i == 0) { mDownsampleShader->loadInt("mipLevel", 1); }
	}

	glUseProgram(0);
}

void BloomRenderer::RenderUpsamples(float filterRadius) {

	const std::vector<bloomMip>& mipChain = mFBO.MipChain();

	glUseProgram(mUpsampleShader->m_program);
	mUpsampleShader->loadFloat("filterRadius", filterRadius);

	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = (int)mipChain.size() - 1; i > 0; i--) {
		const bloomMip& mip = mipChain[i];
		const bloomMip& nextMip = mipChain[i - 1];

		// Bind viewport and texture from where to read
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// Set framebuffer render target (we write to this texture)
		glViewport(0, 0, nextMip.size[0], nextMip.size[1]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		m_quad->drawRaw();
	}

	// Disable additive blending
	glDisable(GL_BLEND);

	glUseProgram(0);
}

void BloomRenderer::RenderBloomTexture(unsigned int srcTexture, float filterRadius) {
	mFBO.BindForWriting();

	this->RenderDownsamples(srcTexture);
	this->RenderUpsamples(filterRadius);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Restore viewport
	glViewport(0, 0, mSrcViewportSize[0], mSrcViewportSize[1]);
}

GLuint BloomRenderer::BloomTexture() {
	return mFBO.MipChain()[0].texture;
}

GLuint BloomRenderer::BloomMip_i(int index) {
	const std::vector<bloomMip>& mipChain = mFBO.MipChain();
	int size = (int)mipChain.size();
	return mipChain[(index > size - 1) ? size - 1 : (index < 0) ? 0 : index].texture;
}

void BloomRenderer::resize(unsigned int windowWidth, unsigned int windowHeight) {
	Vector2f mipSize((float)windowWidth, (float)windowHeight);
	std::array<int, 2> mipIntSize = { (int)windowWidth, (int)windowHeight };
	
	std::vector<bloomMip>& mipChain = mFBO.MipChain();

	for (GLuint i = 0; i < mipChain.size(); i++) {
		bloomMip& mip = mipChain[i];

		mipSize *= 0.5f;
		mipIntSize[0] /= 2;
		mipIntSize[1] /= 2;
		mip.size = mipSize;
		mip.intSize[0] = mipIntSize[0];
		mip.intSize[1] = mipIntSize[1];
		Texture::Resize(mip.texture, (int)mipSize[0], (int)mipSize[1], GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT);
	}

	mSrcViewportSize = { (int)windowWidth, (int)windowHeight };
	mSrcViewportSizeFloat = Vector2f((float)windowWidth, (float)windowHeight);
}