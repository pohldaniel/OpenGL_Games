#include <cstring>
#include <algorithm>
#include <iostream>

#include <WebGPU/WgpContext.h>
#include "RGBADecoder.h"

RGBADecoder::RGBADecoder(){
    
}

RGBADecoder::~RGBADecoder() {
    release();

    if (m_swsContext) 
        sws_freeContext(m_swsContext);

    if (m_frameRgba) 
        av_frame_free(&m_frameRgba);

    if (m_rgbaBufferInternal) 
        av_free(m_rgbaBufferInternal);
}

void RGBADecoder::init(int width, int height) {
    m_width = width;
    m_height = height;

    m_frameRgba = av_frame_alloc();
    m_swsContext = sws_getContext(m_width, m_height, AV_PIX_FMT_YUV420P, m_width, m_height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);

    int rgbaSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_width, m_height, 1);
    m_rgbaBufferInternal = (uint8_t*)av_malloc(rgbaSize * sizeof(uint8_t));
    m_cpuUploadBuffer.resize(rgbaSize, 0);
    av_image_fill_arrays(m_frameRgba->data, m_frameRgba->linesize, m_rgbaBufferInternal, AV_PIX_FMT_RGBA, m_width, m_height, 1);  
    initWebGPUEntities();
}

void RGBADecoder::initWebGPUEntities() {
    if (m_videoTexture) return;

    WGPUTextureDescriptor textureDesc = {};
    textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1u };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = WGPUTextureFormat_RGBA8Unorm;
    m_videoTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDesc);

    WGPUTextureViewDescriptor viewDesc = {};
    viewDesc.format = WGPUTextureFormat_RGBA8Unorm;
    viewDesc.dimension = WGPUTextureViewDimension_2D;
    viewDesc.mipLevelCount = 1;
    viewDesc.arrayLayerCount = 1;
    viewDesc.aspect = WGPUTextureAspect_All;
    m_textureViewY = wgpuTextureCreateView(m_videoTexture, &viewDesc);
}

void RGBADecoder::updateTexture(AVFrame* frame) {
    if (!frame) return;

    sws_scale(m_swsContext, (uint8_t const* const*)frame->data, frame->linesize,
        0, m_height, m_frameRgba->data, m_frameRgba->linesize);

    std::copy(m_rgbaBufferInternal, m_rgbaBufferInternal + m_cpuUploadBuffer.size(), m_cpuUploadBuffer.begin());

    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = m_videoTexture;
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout source = {};
    source.bytesPerRow = m_width * 4u;
    source.rowsPerImage = m_height;

    WGPUExtent3D writeSize = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1 };

    wgpuQueueWriteTexture(wgpContext.queue, &destination, m_cpuUploadBuffer.data(), m_cpuUploadBuffer.size(), &source, &writeSize);
}

void RGBADecoder::setBindGroup(const WGPUBindGroup& bindgroup) {
    m_bindGroup = bindgroup;
}