#pragma once
#include <iostream>
#include <vector>
#include <array>

#include "engine/Vector.h"
#include "engine/Shader.h"
#include "engine/Quad.h"
#include "engine/Framebuffer.h"

struct bloomMip {
	Vector2f size;
	std::array<int, 2> intSize;
	unsigned int texture;
};

class bloomFBO {

public:
	bloomFBO();
	~bloomFBO();
	bool Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength);
	void Destroy();
	void BindForWriting();
	std::vector<bloomMip>& MipChain();

private:
	bool mInit;
	std::vector<bloomMip> mMipChain;
	Framebuffer m_fbo;
};

class BloomRenderer {

public:

    BloomRenderer();
    ~BloomRenderer();
    bool Init(unsigned int windowWidth, unsigned int windowHeight);
    void Destroy();
    void RenderBloomTexture(unsigned int srcTexture, float filterRadius);
    unsigned int BloomTexture();
    unsigned int BloomMip_i(int index);
	void resize(unsigned int windowWidth, unsigned int windowHeight);

private:

    void RenderDownsamples(unsigned int srcTexture);
    void RenderUpsamples(float filterRadius);

    bool mInit;
    bloomFBO mFBO;
    std::array<int, 2> mSrcViewportSize;
    Vector2f mSrcViewportSizeFloat;
    Shader* mDownsampleShader;
    Shader* mUpsampleShader;
	Quad *m_quad;
    bool mKarisAverageOnDownsample = true;
};

