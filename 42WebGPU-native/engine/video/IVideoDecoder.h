#pragma once
#include <webgpu.h>

extern "C" {
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
}

struct IVideoDecoder {

    virtual ~IVideoDecoder() = default;
    virtual void updateTexture(AVFrame* frame) = 0;
    virtual void init(int width, int height) = 0;

    virtual void configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) {};
    virtual void beginMemoryAccess() {}
    virtual void endMemoryAccess() {}
    virtual void clearCache() {}
   
    WGPUSharedTextureMemory m_sharedTextureMemory = nullptr;
    WGPUTexture m_videoTexture = nullptr;
    WGPUTexture m_yTexture = nullptr;
    WGPUTexture m_uvTexture = nullptr;
    WGPUTextureView m_textureViewY = nullptr;
    WGPUTextureView m_textureViewUV = nullptr;
    WGPUBindGroup m_bindGroup = nullptr;
    WGPUBindGroupLayout m_bindGroupLayout = nullptr;
    WGPUBuffer m_buffer = nullptr;
    bool m_hasActiveAccess = false;
    int m_width = 0;
    int m_height = 0;

    void release() {

        if (m_sharedTextureMemory) {
            wgpuSharedTextureMemoryRelease(m_sharedTextureMemory);
            m_sharedTextureMemory = nullptr;
        }

        if (m_videoTexture) {
            wgpuTextureDestroy(m_videoTexture);
            wgpuTextureRelease(m_videoTexture); 
            m_videoTexture = nullptr;
        }

        if (m_yTexture) {
            wgpuTextureDestroy(m_yTexture);
            wgpuTextureRelease(m_yTexture);
            m_yTexture = nullptr;
        }

        if (m_uvTexture) {
            wgpuTextureDestroy(m_uvTexture);
            wgpuTextureRelease(m_uvTexture);
            m_uvTexture = nullptr;
        }

        if (m_textureViewY) {
            wgpuTextureViewRelease(m_textureViewY);
            m_textureViewY = nullptr;
        }

        if (m_textureViewUV) {
            wgpuTextureViewRelease(m_textureViewUV);
            m_textureViewUV = nullptr;
        }

        if (m_bindGroup) {
            wgpuBindGroupRelease(m_bindGroup);
            m_bindGroup = nullptr;
        }     
    }   
};