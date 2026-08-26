#include <cstring>
#include <algorithm>
#include <iostream>

#include <WebGPU/WgpContext.h>
#include "YUVTextureBridge.h"

YUVTextureBridge::YUVTextureBridge() : m_width(0), m_height(0) {
    
}

YUVTextureBridge::~YUVTextureBridge() {
    release();
}

void YUVTextureBridge::init(int width, int height) {
    m_width = width;
    m_height = height;
    int yuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_width, m_height, 1);
    m_cpuUploadBuffer.resize(yuvSize, 0);   
    initWebGPUEntities();
}

void YUVTextureBridge::initWebGPUEntities() {
    if (m_videoTexture) return;

    WGPUTextureDescriptor textureDesc = {};
    textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height + m_height / 2), 1u };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = WGPUTextureFormat_R8Unorm;
    m_videoTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDesc);

    WGPUTextureViewDescriptor viewDesc = {};
    viewDesc.format = WGPUTextureFormat_R8Unorm;
    viewDesc.dimension = WGPUTextureViewDimension_2D;
    viewDesc.mipLevelCount = 1;
    viewDesc.arrayLayerCount = 1;
    viewDesc.aspect = WGPUTextureAspect_All;
    m_textureViewY = wgpuTextureCreateView(m_videoTexture, &viewDesc);
}

void YUVTextureBridge::updateTexture(AVFrame* frame) {
    if (!frame) return;

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
    dataLayout.rowsPerImage = static_cast<uint32_t>(m_height + m_height / 2);

    WGPUExtent3D writeSize = { static_cast<uint32_t>(m_width), dataLayout.rowsPerImage, 1u };

    wgpuQueueWriteTexture(wgpContext.queue, &destination, m_cpuUploadBuffer.data(), m_cpuUploadBuffer.size(), &dataLayout, &writeSize);
}

void YUVTextureBridge::release() {
    if (m_textureViewY) { wgpuTextureViewRelease(m_textureViewY); m_textureViewY = nullptr; }
    if (m_videoTexture) { wgpuTextureRelease(m_videoTexture); m_videoTexture = nullptr; }
}