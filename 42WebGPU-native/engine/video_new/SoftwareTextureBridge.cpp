#include <cstring>
#include <algorithm>
#include <iostream>

#include <WebGPU/WgpContext.h>
#include "SoftwareTextureBridge.h"

SoftwareTextureBridge::SoftwareTextureBridge(int width, int height, bool isPackedYuv)
    : m_width(width), m_height(height), m_isPackedYuv(isPackedYuv)
{
    if (!m_isPackedYuv) {
        m_frameRgba = av_frame_alloc();
        m_swsContext = sws_getContext(m_width, m_height, AV_PIX_FMT_YUV420P, m_width, m_height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);

        int rgbaSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_width, m_height, 1);
        m_rgbaBufferInternal = (uint8_t*)av_malloc(rgbaSize * sizeof(uint8_t));
        m_cpuUploadBuffer.resize(rgbaSize, 0);
        av_image_fill_arrays(m_frameRgba->data, m_frameRgba->linesize, m_rgbaBufferInternal, AV_PIX_FMT_RGBA, m_width, m_height, 1);
    }else {
        int yuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_width, m_height, 1);
        m_cpuUploadBuffer.resize(yuvSize, 0);
    }
    initWebGPUEntities();
}

SoftwareTextureBridge::~SoftwareTextureBridge() {
    release();
    if (m_swsContext) sws_freeContext(m_swsContext);
    if (m_frameRgba) av_frame_free(&m_frameRgba);
    if (m_rgbaBufferInternal) av_free(m_rgbaBufferInternal);
}

void SoftwareTextureBridge::initWebGPUEntities() {
    if (m_videoTexture) return;

    WGPUTextureDescriptor textureDesc = {};
    textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size = { static_cast<uint32_t>(m_width), m_isPackedYuv ? static_cast<uint32_t>(m_height + m_height / 2) : static_cast<uint32_t>(m_height), 1u };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = m_isPackedYuv ? WGPUTextureFormat_R8Unorm : WGPUTextureFormat_RGBA8Unorm;
    m_videoTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDesc);

    WGPUTextureViewDescriptor viewDesc = {};
    viewDesc.format = m_isPackedYuv ? WGPUTextureFormat_R8Unorm : WGPUTextureFormat_RGBA8Unorm;
    viewDesc.dimension = WGPUTextureViewDimension_2D;
    viewDesc.mipLevelCount = 1;
    viewDesc.arrayLayerCount = 1;
    viewDesc.aspect = WGPUTextureAspect_All;
    m_textureViewY = wgpuTextureCreateView(m_videoTexture, &viewDesc);
}

void SoftwareTextureBridge::updateTexture(AVFrame* frame) {
    if (!frame) return;

    if (!m_isPackedYuv) {
        handleRgbaPath(frame);
    }else {
        handleYuvPlanarPath(frame);
    }   
}

void SoftwareTextureBridge::handleRgbaPath(AVFrame* frame) {
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

void SoftwareTextureBridge::handleYuvPlanarPath(AVFrame* frame) {
    uint8_t* dst = m_cpuUploadBuffer.data();
    int w = m_width;
    int h = m_height;
    int uvW = w / 2;
    int uvH = h / 2;

    uint8_t* srcY = frame->data[0];
    int linesizeY = frame->linesize[0];
    for (int y = 0; y < h; y++) {
        std::memcpy(dst + (y * w), srcY + (y * linesizeY), w);
    }

    uint8_t* uDstStart = dst + (w * h);
    uint8_t* vDstStart = uDstStart + (uvW * uvH);

    uint8_t* srcU = frame->data[1];
    int linesizeU = frame->linesize[1];
    for (int y = 0; y < uvH; y++) {
        std::memcpy(uDstStart + (y * uvW), srcU + (y * linesizeU), uvW);
    }

    uint8_t* srcV = frame->data[2];
    int linesizeV = frame->linesize[2];
    for (int y = 0; y < uvH; y++) {
        std::memcpy(vDstStart + (y * uvW), srcV + (y * linesizeV), uvW);
    }

    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = m_videoTexture;
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout dataLayout = {};
    dataLayout.bytesPerRow = m_width;
    dataLayout.rowsPerImage = m_isPackedYuv ? static_cast<uint32_t>(m_height + m_height / 2) : static_cast<uint32_t>(m_height);

    WGPUExtent3D writeSize = { static_cast<uint32_t>(m_width), dataLayout.rowsPerImage, 1u };

    wgpuQueueWriteTexture(wgpContext.queue, &destination, m_cpuUploadBuffer.data(), m_cpuUploadBuffer.size(), &dataLayout, &writeSize);
}

void SoftwareTextureBridge::release() {
    if (m_textureViewY) { wgpuTextureViewRelease(m_textureViewY); m_textureViewY = nullptr; }
    if (m_textureViewUV) { wgpuTextureViewRelease(m_textureViewUV); m_textureViewUV = nullptr; }
    if (m_videoTexture) { wgpuTextureRelease(m_videoTexture); m_videoTexture = nullptr; }
}