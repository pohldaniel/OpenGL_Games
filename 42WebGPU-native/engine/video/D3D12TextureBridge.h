#pragma once

#include <unordered_map>
#include <d3d12.h>

extern "C" {
#include <libavutil/hwcontext_d3d12va.h>
}

#include <WebGPU/WgpContext.h>
#include "IVideoTextureBridge.h"

typedef struct SharedTextureMemoryD3D12ResourceDescriptorNew {
    WGPUChainedStruct chain;
    ID3D12Resource* resource;
} SharedTextureMemoryD3D12ResourceDescriptorNew;

struct CachedWebGPUTexture {
    WGPUSharedTextureMemory sharedMemory;
    WGPUTexture texture;
    WGPUTextureView viewY;
    WGPUTextureView viewUV;
};

class D3D12TextureBridge : public IVideoTextureBridge {

public:

    D3D12TextureBridge() : m_width(0), m_height(0) {

    }

    ~D3D12TextureBridge() override { 
        release(); 
        //clearCache(); 
    }

    void configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) override;
    void init(int width, int height) override;
    void updateTexture(AVFrame* frame) override;
   
    void release() override {
        if (m_textureViewY) { wgpuTextureViewRelease(m_textureViewY); m_textureViewY = nullptr; }
        if (m_textureViewUV) { wgpuTextureViewRelease(m_textureViewUV); m_textureViewUV = nullptr; }
        if (m_videoTexture) { wgpuTextureRelease(m_videoTexture); m_videoTexture = nullptr; }
        if (m_sharedTextureMemory) { wgpuSharedTextureMemoryRelease(m_sharedTextureMemory); m_sharedTextureMemory = nullptr; }
    }

private:

    std::unordered_map<ID3D12Resource*, CachedWebGPUTexture> m_textureCache;
    int m_width;
    int m_height; 
    void stopCurrentAccess();
    void clearCache() override;
};