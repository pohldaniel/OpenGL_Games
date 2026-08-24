#pragma once

#include <d3d12.h>

extern "C" {
#include <libavutil/hwcontext_d3d12va.h>
}

#include <WebGPU/WgpContext.h>
#include "IVideoTextureBridge.h"

struct SharedTextureMemoryD3D12ResourceDescriptorNew {
    WGPUChainedStruct chain;
    ID3D12Resource* resource;
};

class D3D12TextureBridge : public IVideoTextureBridge {
public:
    D3D12TextureBridge(int width, int height) : m_width(width), m_height(height) {}
    ~D3D12TextureBridge() override { release(); }

    void updateTexture(AVFrame* frame) override {
        // Ressourcen des vorherigen Frames freigeben
        release();

        if (!frame || frame->format != AV_PIX_FMT_D3D12) return;

        AVD3D12VAFrame* ffmpegFrame = reinterpret_cast<AVD3D12VAFrame*>(frame->data[0]);
        ID3D12Resource* d3d12Texture = ffmpegFrame->texture;

        SharedTextureMemoryD3D12ResourceDescriptorNew d3d12Desc = {};
        d3d12Desc.chain.sType = WGPUSType_SharedTextureMemoryD3D12ResourceDescriptor;
        d3d12Desc.resource = d3d12Texture;

        WGPUSharedTextureMemoryDescriptor memoryDesc = {};
        memoryDesc.nextInChain = (WGPUChainedStruct*)&d3d12Desc;
        memoryDesc.label = WGPU_STR("FFmpeg_Direct_D3D12_ZeroCopy");
        m_sharedTextureMemory = wgpuDeviceImportSharedTextureMemory(wgpContext.device, &memoryDesc);

        WGPUTextureDescriptor textureDesc = {};
        textureDesc.usage = WGPUTextureUsage_TextureBinding;
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.size = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1 };
        textureDesc.format = WGPUTextureFormat_R8BG8Biplanar420Unorm;
        textureDesc.mipLevelCount = 1;
        textureDesc.sampleCount = 1;

        m_videoTexture = wgpuSharedTextureMemoryCreateTexture(m_sharedTextureMemory, &textureDesc);

        WGPUTextureViewDescriptor yViewDesc = {};
        yViewDesc.format = WGPUTextureFormat_R8Unorm;
        yViewDesc.dimension = WGPUTextureViewDimension_2D;
        yViewDesc.mipLevelCount = 1;
        yViewDesc.arrayLayerCount = 1;
        yViewDesc.aspect = WGPUTextureAspect_Plane0Only;
        m_textureViewY = wgpuTextureCreateView(m_videoTexture, &yViewDesc);

        WGPUTextureViewDescriptor uvViewDesc = {};
        uvViewDesc.format = WGPUTextureFormat_RG8Unorm;
        uvViewDesc.dimension = WGPUTextureViewDimension_2D;
        uvViewDesc.mipLevelCount = 1;
        uvViewDesc.arrayLayerCount = 1;
        uvViewDesc.aspect = WGPUTextureAspect_Plane1Only;
        m_textureViewUV = wgpuTextureCreateView(m_videoTexture, &uvViewDesc);

        // Daten zurückgeben
       //  return { m_videoTexture, m_textureViewY, m_textureViewUV, false };
    }

    void release() override {
        if (m_textureViewY) { wgpuTextureViewRelease(m_textureViewY); m_textureViewY = nullptr; }
        if (m_textureViewUV) { wgpuTextureViewRelease(m_textureViewUV); m_textureViewUV = nullptr; }
        if (m_videoTexture) { wgpuTextureRelease(m_videoTexture); m_videoTexture = nullptr; }
        if (m_sharedTextureMemory) { wgpuSharedTextureMemoryRelease(m_sharedTextureMemory); m_sharedTextureMemory = nullptr; }
    }

private:
    int m_width;
    int m_height;
   
};