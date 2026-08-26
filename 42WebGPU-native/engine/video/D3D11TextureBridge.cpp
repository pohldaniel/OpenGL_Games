
#include <WebGPU/WgpContext.h>

#include "D3D11TextureBridge.h"

static enum AVPixelFormat get_hw_format_d3d11_(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
    const enum AVPixelFormat target = AV_PIX_FMT_D3D11;
    for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == target) {
            return target;
        }
    }
    return AV_PIX_FMT_NONE;
}

void D3D11TextureBridge::configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) {
    ctx->sw_pix_fmt = AV_PIX_FMT_NV12;
    ctx->hw_device_ctx = av_buffer_ref(hwDeviceCtx);
    ctx->get_format = get_hw_format_d3d11_;

    AVHWDeviceContext* device_ctx = reinterpret_cast<AVHWDeviceContext*>(ctx->hw_device_ctx->data);
    AVD3D11VADeviceContext* d3d11_device_ctx = reinterpret_cast<AVD3D11VADeviceContext*>(device_ctx->hwctx);
    d3d11_device_ctx->MiscFlags |= D3D11_RESOURCE_MISC_SHARED;

    m_d3d11_device = d3d11_device_ctx->device;
    m_d3d11_context = d3d11_device_ctx->device_context;
}

void D3D11TextureBridge::init(int width, int height) {
    m_width = width;
    m_height = height;
    initWebGPUEntities();
}

void D3D11TextureBridge::initWebGPUEntities() {
    D3D11_TEXTURE2D_DESC single_desc = {};
    single_desc.Width = m_width;
    single_desc.Height = m_height;
    single_desc.MipLevels = 1;
    single_desc.ArraySize = 1;
    single_desc.Format = DXGI_FORMAT_NV12;
    single_desc.SampleDesc.Count = 1;
    single_desc.SampleDesc.Quality = 0;
    single_desc.Usage = D3D11_USAGE_DEFAULT;
    single_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    single_desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_SHARED_NTHANDLE;

    HRESULT hr = m_d3d11_device->CreateTexture2D(&single_desc, nullptr, &m_single_texture);

    IDXGIResource1* dxgi_res1 = nullptr;
    hr = m_single_texture->QueryInterface(__uuidof(IDXGIResource1), (void**)&dxgi_res1);

    HANDLE webgpu_compatible_handle = nullptr;
    hr = dxgi_res1->CreateSharedHandle(nullptr, DXGI_SHARED_RESOURCE_READ, nullptr, &webgpu_compatible_handle);
    dxgi_res1->Release();


    WGPUSharedTextureMemoryDXGISharedHandleDescriptor desc = {};
    desc.chain.next = nullptr;
    desc.chain.sType = WGPUSType_SharedTextureMemoryDXGISharedHandleDescriptor;
    desc.handle = webgpu_compatible_handle;

   
    WGPUSharedTextureMemoryDescriptor memoryDesc = {};
    memoryDesc.nextInChain = (WGPUChainedStruct*)&desc;
    memoryDesc.label = WGPU_STR("FFmpeg Shared Frame Memory");
    m_sharedTextureMemory = wgpuDeviceImportSharedTextureMemory(wgpContext.device, &memoryDesc);

    WGPUTextureDescriptor textureDesc = {};
    textureDesc.nextInChain = NULL;
    textureDesc.label = WGPU_STR("FFmpeg_Hardware_Video_Texture");
    textureDesc.usage = WGPUTextureUsage_TextureBinding;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size.width = m_width;
    textureDesc.size.height = m_height;
    textureDesc.size.depthOrArrayLayers = 1;
    textureDesc.format = WGPUTextureFormat_R8BG8Biplanar420Unorm;
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;

    m_videoTexture = wgpuSharedTextureMemoryCreateTexture(m_sharedTextureMemory, &textureDesc);

    WGPUTextureViewDescriptor yViewDesc = {};
    yViewDesc.format = WGPUTextureFormat_R8Unorm;
    yViewDesc.dimension = WGPUTextureViewDimension_2D;
    yViewDesc.baseMipLevel = 0u;
    yViewDesc.mipLevelCount = 1u;
    yViewDesc.baseArrayLayer = 0u;
    yViewDesc.arrayLayerCount = 1u;
    yViewDesc.aspect = WGPUTextureAspect_Plane0Only;
    m_textureViewY = wgpuTextureCreateView(m_videoTexture, &yViewDesc);

    WGPUTextureViewDescriptor uvViewDesc = {};
    uvViewDesc.format = WGPUTextureFormat_RG8Unorm;
    uvViewDesc.dimension = WGPUTextureViewDimension_2D;
    uvViewDesc.baseMipLevel = 0u;
    uvViewDesc.mipLevelCount = 1u;
    uvViewDesc.baseArrayLayer = 0u;
    uvViewDesc.arrayLayerCount = 1u;
    uvViewDesc.aspect = WGPUTextureAspect_Plane1Only;
    m_textureViewUV = wgpuTextureCreateView(m_videoTexture, &uvViewDesc);
}

void D3D11TextureBridge::updateTexture(AVFrame* frame) {
    ID3D11Texture2D* d3d11Texture = reinterpret_cast<ID3D11Texture2D*>(frame->data[0]);
    IDXGIResource1* dxgiResource = nullptr;
    HRESULT hr = d3d11Texture->QueryInterface(__uuidof(IDXGIResource1), (void**)&dxgiResource);

    HANDLE sharedHandle = nullptr;
    hr = dxgiResource->GetSharedHandle(&sharedHandle);
    dxgiResource->Release();

    ID3D11Texture2D* ffmpeg_texture_array = nullptr;
    hr = m_d3d11_device->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), (void**)&ffmpeg_texture_array);

    int current_slice_index = (int)(intptr_t)frame->data[1];
    UINT src_subresource = D3D11CalcSubresource(0, current_slice_index, 1);

    m_d3d11_context->CopySubresourceRegion(m_single_texture, 0, 0, 0, 0, ffmpeg_texture_array, src_subresource, nullptr);
    m_d3d11_context->Flush();

    ffmpeg_texture_array->Release();
}