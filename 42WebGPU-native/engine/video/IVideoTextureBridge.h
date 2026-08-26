#pragma once
#include <webgpu.h>

extern "C" {
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
}

class IVideoTextureBridge {

public:

    virtual ~IVideoTextureBridge() = default;
    virtual void updateTexture(AVFrame* frame) = 0;
    virtual void release() = 0;
    virtual void clearCache() = 0;

    virtual void configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) = 0;
    virtual void init(int width, int height) = 0;

    WGPUSharedTextureMemory m_sharedTextureMemory = nullptr;
    WGPUTexture m_videoTexture = nullptr;
    WGPUTexture m_yTexture = nullptr;
    WGPUTexture m_uvTexture = nullptr;
    WGPUTextureView m_textureViewY = nullptr;
    WGPUTextureView m_textureViewUV = nullptr;
};