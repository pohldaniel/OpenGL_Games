#include <vector>
#include <FreeImage.h>
#include "WgpContext.h"
#include "WgpTexture.h"

WgpTexture::WgpTexture() :
    m_width(0u),
    m_height(0u),
    m_channels(0u),
    m_texture(NULL),
    m_data(nullptr){

}

WgpTexture::WgpTexture(WgpTexture const& rhs) : m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels), m_texture(rhs.m_texture) {
    memcpy(rhs.m_data, m_data, m_width * m_height * m_channels);
}

WgpTexture::WgpTexture(WgpTexture&& rhs) noexcept : m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels), m_texture(rhs.m_texture) {
    m_data = rhs.m_data;
    rhs.m_data = nullptr;
}

void WgpTexture::loadFromFile(std::string fileName, const bool flipVertical) {
    FreeImage_Initialise();

    FIBITMAP* sourceBitmap = FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT);
    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData =  FreeImage_GetBits(sourceBitmap);

    if (bpp == 3) {
        unsigned char* bytesNew = (unsigned char*)malloc(width * height * 4);

        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4); i = i + 4, k = k + 3) {
            bytesNew[i] = imageData[k];
            bytesNew[i + 1] = imageData[k + 1];
            bytesNew[i + 2] = imageData[k + 2];
            bytesNew[i + 3] = 255;
        }
        FreeImage_Unload(sourceBitmap);
        imageData = bytesNew;
        bpp = 4;      
    }

    m_width = width;
    m_height = height;
    m_channels = bpp;

    m_data = (unsigned char*)malloc(width * height * bpp);
    memcpy(m_data, imageData, width * height * bpp);

    /*m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding);
    
    WGPUTexelCopyTextureInfo destination;
    destination.texture = m_texture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout source;
    source.offset = 0;
    source.bytesPerRow = m_channels * m_width;
    source.rowsPerImage = m_height;

    WGPUExtent3D extent3D = { m_width, m_height, 1 };
    wgpuQueueWriteTexture(wgpContext.queue, &destination, imageData, width * height * bpp, &source, &extent3D);*/

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();
}

void WgpTexture::copyToDestination(const WGPUTexture& destTesture) {
    WGPUTexelCopyTextureInfo destination;
    destination.texture = destTesture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout source;
    source.offset = 0;
    source.bytesPerRow = m_channels * m_width;
    source.rowsPerImage = m_height;

    WGPUExtent3D extent3D = { m_width, m_height, 1 };
    wgpuQueueWriteTexture(wgpContext.queue, &destination, m_data, m_width * m_height * m_channels, &source, &extent3D);
}

void WgpTexture::FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
    std::vector<unsigned char> srcPixels(padWidth * height);
    memcpy(&srcPixels[0], data, padWidth * height);
    unsigned char* pSrcRow = 0;
    unsigned char* pDestRow = 0;
    for (unsigned int i = 0; i < height; ++i) {
        pSrcRow = &srcPixels[(height - 1 - i) * padWidth];
        pDestRow = &data[i * padWidth];
        memcpy(pDestRow, pSrcRow, padWidth);
    }
}

unsigned char* WgpTexture::LoadFromFile(std::string fileName, const bool flipVertical, short alphaChannel) {
    FreeImage_Initialise();

    FIBITMAP* sourceBitmap = FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT);
    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);

    if (bpp == 3) {
        unsigned char* bytesNew = (unsigned char*)malloc(width * height * 4);

        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4); i = i + 4, k = k + 3) {
            bytesNew[i] = imageData[k];
            bytesNew[i + 1] = imageData[k + 1];
            bytesNew[i + 2] = imageData[k + 2];
            bytesNew[i + 3] = alphaChannel == -1 ? 255 : alphaChannel;
        }
        FreeImage_Unload(sourceBitmap);
        imageData = bytesNew;
        bpp = 4;
    }

    return imageData;
}