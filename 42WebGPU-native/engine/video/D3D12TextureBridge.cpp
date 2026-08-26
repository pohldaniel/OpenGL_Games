#include "D3D12TextureBridge.h"

static enum AVPixelFormat get_hw_format_d3d12_(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
    const enum AVPixelFormat target = AV_PIX_FMT_D3D12;
    for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == target) {
            return target;
        }
    }
    return AV_PIX_FMT_NONE;
}

void D3D12TextureBridge::configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) {
    ctx->sw_pix_fmt = AV_PIX_FMT_NV12;
    ctx->hw_device_ctx = av_buffer_ref(hwDeviceCtx);
    ctx->get_format = get_hw_format_d3d12_;

    AVHWDeviceContext* device_ctx = reinterpret_cast<AVHWDeviceContext*>(ctx->hw_device_ctx->data);
    AVD3D12VADeviceContext* d3d12_ctx = reinterpret_cast<AVD3D12VADeviceContext*>(device_ctx->hwctx);
    d3d12_ctx->resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
}

void D3D12TextureBridge::init(int width, int height) {
    m_width = width;
    m_height = height;
}

void D3D12TextureBridge::updateTexture(AVFrame* frame) {

    if (!frame || frame->format != AV_PIX_FMT_D3D12) return;

    AVD3D12VAFrame* ffmpegFrame = reinterpret_cast<AVD3D12VAFrame*>(frame->data[0]);
    ID3D12Resource* d3d12Texture = ffmpegFrame->texture;

    auto it = m_textureCache.find(d3d12Texture);
    if (it != m_textureCache.end()) {
        m_videoTexture = it->second.texture;
        m_textureViewY = it->second.viewY;
        m_textureViewUV = it->second.viewUV;
        m_sharedTextureMemory = it->second.sharedMemory;
        return;
    }

    CachedWebGPUTexture newCacheItem = {};

    SharedTextureMemoryD3D12ResourceDescriptorNew d3d12Desc = {};
    d3d12Desc.chain.sType = WGPUSType_SharedTextureMemoryD3D12ResourceDescriptor;
    d3d12Desc.resource = d3d12Texture;

    WGPUSharedTextureMemoryDescriptor memoryDesc = {};
    memoryDesc.nextInChain = (WGPUChainedStruct*)&d3d12Desc;
    memoryDesc.label = WGPU_STR("FFmpeg_Direct_D3D12_ZeroCopy");
    newCacheItem.sharedMemory = wgpuDeviceImportSharedTextureMemory(wgpContext.device, &memoryDesc);

    WGPUTextureDescriptor textureDesc = {};
    textureDesc.usage = WGPUTextureUsage_TextureBinding;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1 };
    textureDesc.format = WGPUTextureFormat_R8BG8Biplanar420Unorm;
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;

    newCacheItem.texture = wgpuSharedTextureMemoryCreateTexture(newCacheItem.sharedMemory, &textureDesc);

    WGPUTextureViewDescriptor yViewDesc = {};
    yViewDesc.format = WGPUTextureFormat_R8Unorm;
    yViewDesc.dimension = WGPUTextureViewDimension_2D;
    yViewDesc.mipLevelCount = 1;
    yViewDesc.arrayLayerCount = 1;
    yViewDesc.aspect = WGPUTextureAspect_Plane0Only;
    newCacheItem.viewY = wgpuTextureCreateView(newCacheItem.texture, &yViewDesc);

    WGPUTextureViewDescriptor uvViewDesc = {};
    uvViewDesc.format = WGPUTextureFormat_RG8Unorm;
    uvViewDesc.dimension = WGPUTextureViewDimension_2D;
    uvViewDesc.mipLevelCount = 1;
    uvViewDesc.arrayLayerCount = 1;
    uvViewDesc.aspect = WGPUTextureAspect_Plane1Only;
    newCacheItem.viewUV = wgpuTextureCreateView(newCacheItem.texture, &uvViewDesc);

    m_textureCache[d3d12Texture] = newCacheItem;

    m_videoTexture = newCacheItem.texture;
    m_textureViewY = newCacheItem.viewY;
    m_textureViewUV = newCacheItem.viewUV;
    m_sharedTextureMemory = newCacheItem.sharedMemory;
}

void D3D12TextureBridge::clearCache() {

    for (auto& [key, item] : m_textureCache) {
        if (item.viewUV) wgpuTextureViewRelease(item.viewUV);
        if (item.viewY) wgpuTextureViewRelease(item.viewY);
        if (item.texture) wgpuTextureRelease(item.texture);
        if (item.sharedMemory) wgpuSharedTextureMemoryRelease(item.sharedMemory);
    }
    m_textureCache.clear();

    m_videoTexture = nullptr;
    m_textureViewY = nullptr;
    m_textureViewUV = nullptr;
    m_sharedTextureMemory = nullptr;
}

void D3D12TextureBridge::stopCurrentAccess() {

}