#pragma once
#include <libavutil/frame.h>



class IVideoTextureBridge {
public:
    virtual ~IVideoTextureBridge() = default;
    virtual void updateTexture(AVFrame* frame) = 0;
    virtual void release() = 0;

    WGPUSharedTextureMemory m_sharedTextureMemory = nullptr;
    WGPUTexture m_videoTexture = nullptr;
    WGPUTextureView m_textureViewY = nullptr;
    WGPUTextureView m_textureViewUV = nullptr;
};