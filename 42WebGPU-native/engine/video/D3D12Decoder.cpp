#include "D3D12Decoder.h"

static enum AVPixelFormat get_hw_format_d3d12_(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
    const enum AVPixelFormat target = AV_PIX_FMT_D3D12;
    for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == target) {
            return target;
        }
    }
    return AV_PIX_FMT_NONE;
}

D3D12Decoder::D3D12Decoder() {

}

D3D12Decoder::~D3D12Decoder() {
    clearCache();
}

void D3D12Decoder::configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) {
    ctx->sw_pix_fmt = AV_PIX_FMT_NV12;
    ctx->hw_device_ctx = av_buffer_ref(hwDeviceCtx);
    ctx->get_format = get_hw_format_d3d12_;

    AVHWDeviceContext* device_ctx = reinterpret_cast<AVHWDeviceContext*>(ctx->hw_device_ctx->data);
    AVD3D12VADeviceContext* d3d12_ctx = reinterpret_cast<AVD3D12VADeviceContext*>(device_ctx->hwctx);
    d3d12_ctx->resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
}

void D3D12Decoder::init(int width, int height) {
    m_width = width;
    m_height = height;
}

void D3D12Decoder::updateTexture(AVFrame* frame) {

    if (!frame || frame->format != AV_PIX_FMT_D3D12) return;

    AVD3D12VAFrame* ffmpegFrame = reinterpret_cast<AVD3D12VAFrame*>(frame->data[0]);
    ID3D12Resource* d3d12Texture = ffmpegFrame->texture;

    auto it = m_textureCache.find(d3d12Texture);
    if (it != m_textureCache.end()) {
        m_videoTexture = it->second.texture;
        m_textureViewY = it->second.viewY;
        m_textureViewUV = it->second.viewUV;
        m_sharedTextureMemory = it->second.sharedMemory;
        m_bindGroup = it->second.bindGroup;
        return;
    }

    CachedWebGPUTexture newCacheItem = {};

    SharedTextureMemoryD3D12ResourceDescriptor d3d12Desc = {};
    d3d12Desc.chain.sType = WGPUSType_SharedTextureMemoryD3D12ResourceDescriptor;
    d3d12Desc.resource = d3d12Texture;

    WGPUSharedTextureMemoryDescriptor memoryDesc = {};
    memoryDesc.nextInChain = (WGPUChainedStruct*)&d3d12Desc;
    memoryDesc.label = WGPU_STR("FFmpeg_Direct_D3D12_ZeroCopy");
    newCacheItem.sharedMemory = wgpuDeviceImportSharedTextureMemory(wgpContext.device, &memoryDesc);

    WGPUTextureDescriptor textureDesc = {};
    textureDesc.usage = WGPUTextureUsage_TextureBinding;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1u };
    textureDesc.format = WGPUTextureFormat_R8BG8Biplanar420Unorm;
    textureDesc.mipLevelCount = 1u;
    textureDesc.sampleCount = 1u;

    newCacheItem.texture = wgpuSharedTextureMemoryCreateTexture(newCacheItem.sharedMemory, &textureDesc);

    WGPUTextureViewDescriptor yViewDesc = {};
    yViewDesc.format = WGPUTextureFormat_R8Unorm;
    yViewDesc.dimension = WGPUTextureViewDimension_2D;
    yViewDesc.mipLevelCount = 1u;
    yViewDesc.arrayLayerCount = 1u;
    yViewDesc.aspect = WGPUTextureAspect_Plane0Only;
    newCacheItem.viewY = wgpuTextureCreateView(newCacheItem.texture, &yViewDesc);

    WGPUTextureViewDescriptor uvViewDesc = {};
    uvViewDesc.format = WGPUTextureFormat_RG8Unorm;
    uvViewDesc.dimension = WGPUTextureViewDimension_2D;
    uvViewDesc.mipLevelCount = 1u;
    uvViewDesc.arrayLayerCount = 1u;
    uvViewDesc.aspect = WGPUTextureAspect_Plane1Only;
    newCacheItem.viewUV = wgpuTextureCreateView(newCacheItem.texture, &uvViewDesc);

    std::vector<WGPUBindGroupEntry> entries(4u);
    entries[0].binding = 0u;
    entries[0].buffer = m_buffer;
    entries[0].offset = 0u;
    entries[0].size = wgpuBufferGetSize(m_buffer);

    entries[1].binding = 1u;
    entries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

    entries[2].binding = 2u;
    entries[2].textureView = newCacheItem.viewY;

    entries[3].binding = 3u;
    entries[3].textureView = newCacheItem.viewUV;

    WGPUBindGroupDescriptor bindGroupDesc = {};
    bindGroupDesc.layout = m_bindGroupLayout;
    bindGroupDesc.entryCount = (uint32_t)entries.size();
    bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
    newCacheItem.bindGroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

    m_textureCache[d3d12Texture] = newCacheItem;

    m_videoTexture = newCacheItem.texture;
    m_textureViewY = newCacheItem.viewY;
    m_textureViewUV = newCacheItem.viewUV;
    m_sharedTextureMemory = newCacheItem.sharedMemory;
    m_bindGroup = newCacheItem.bindGroup;
}

void D3D12Decoder::beginMemoryAccess() {
    if (!m_hasActiveAccess && m_sharedTextureMemory) {
        WGPUSharedTextureMemoryBeginAccessDescriptor accessDesc = {};
        accessDesc.nextInChain = NULL;
        accessDesc.initialized = true;
        accessDesc.fenceCount = 0;
        accessDesc.fences = NULL;

        WGPUStatus status = wgpuSharedTextureMemoryBeginAccess(m_sharedTextureMemory, m_videoTexture, &accessDesc);
        m_hasActiveAccess = status & WGPUStatus_Success;
    } 
}

void D3D12Decoder::endMemoryAccess() {
    if (m_hasActiveAccess) {
        WGPUSharedTextureMemoryEndAccessState endState = {};
        endState.nextInChain = NULL;
        wgpuSharedTextureMemoryEndAccess(m_sharedTextureMemory, m_videoTexture, &endState);
        m_hasActiveAccess = false;
    }
}

void D3D12Decoder::clearCache() {

    for (auto& it : m_textureCache) {
        wgpuTextureViewRelease(it.second.viewY);
        wgpuTextureViewRelease(it.second.viewUV);
        wgpuTextureRelease(it.second.texture);
        wgpuSharedTextureMemoryRelease(it.second.sharedMemory);
        wgpuBindGroupRelease(it.second.bindGroup);
    }

    m_textureCache.clear();
    m_videoTexture = nullptr;
    m_textureViewY = nullptr;
    m_textureViewUV = nullptr;
    m_sharedTextureMemory = nullptr;
    m_hasActiveAccess = false;
}

void D3D12Decoder::setBindGroupLayout(const WGPUBindGroupLayout& bindGroupLayout) {
    m_bindGroupLayout = bindGroupLayout;
}