#pragma once

#include <unordered_map>
#include <d3d12.h>
#include <engine/Vector.h>
extern "C" {
#include <libavutil/hwcontext_d3d12va.h>
}

#include <WebGPU/WgpContext.h>
#include "IVideoDecoder.h"

typedef struct SharedTextureMemoryD3D12ResourceDescriptor {
    WGPUChainedStruct chain;
    ID3D12Resource* resource;
} SharedTextureMemoryD3D12ResourceDescriptor;

struct CachedWebGPUTexture {
    WGPUSharedTextureMemory sharedMemory;
    WGPUTexture texture;
    WGPUTextureView viewY;
    WGPUTextureView viewUV;
    WGPUBindGroup bindGroup;
};

class D3D12Decoder : public IVideoDecoder {

public:

    D3D12Decoder();
    ~D3D12Decoder() override;

    void configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) override;
    void init(int width, int height) override;
    void updateTexture(AVFrame* frame) override;
    void beginMemoryAccess() override;
    void endMemoryAccess() override;

    void setBindGroupLayout(const WGPUBindGroupLayout& bindGroupLayout);

private:

    void clearCache() override;
    std::unordered_map<ID3D12Resource*, CachedWebGPUTexture> m_textureCache;
};