#include <vector>
#include <FreeImage.h>
#include <Utilities.h>
#include "WgpContext.h"
#include "WgpTexture.h"

WgpTexture::WgpTexture() :
    m_texture(NULL),
    m_width(0u),
    m_height(0u),
    m_channels(0u),
    m_data(nullptr),
    m_markForDelete(false) {

}

WgpTexture::WgpTexture(WgpTexture const& rhs) : m_texture(rhs.m_texture), m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels), m_data(rhs.m_data), m_markForDelete(false) {

}

WgpTexture::WgpTexture(WgpTexture&& rhs) noexcept : m_texture(rhs.m_texture), m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels), m_data(std::move(rhs.m_data)), m_markForDelete(rhs.m_markForDelete) {

}

WgpTexture::~WgpTexture() {
    if (m_markForDelete) {
        cleanup();
    }
}

void WgpTexture::cleanup() {
    wgpuTextureDestroy(m_texture);
    wgpuTextureRelease(m_texture);
    free(m_data);
    m_texture = nullptr;
}

void WgpTexture::markForDelete() {
    m_markForDelete = true;
}

void WgpTexture::loadFromFile(std::string fileName, const bool flipVertical, short alphaChannel) { 
    std::filesystem::path filePath = fileName;
    
    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = filePath.extension() == ".png" ? FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT) : 
                             filePath.extension() == ".jpg" ? FreeImage_Load(FIF_JPEG, fileName.c_str(), JPEG_DEFAULT) :
                                                              FreeImage_Load(FIF_BMP, fileName.c_str(), BMP_DEFAULT);
    SwapRedBlue32(sourceBitmap);
    
    if(flipVertical)
        FreeImage_FlipVertical(sourceBitmap);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData =  FreeImage_GetBits(sourceBitmap);
    unsigned char* bytesNew = nullptr;

    if (bpp == 3) {
        bytesNew  = (unsigned char*)malloc(width * height * 4);

        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4); i = i + 4, k = k + 3) {
            bytesNew[i] = imageData[k];
            bytesNew[i + 1] = imageData[k + 1];
            bytesNew[i + 2] = imageData[k + 2];
            bytesNew[i + 3] = alphaChannel == -1 ? 255 : alphaChannel;
        }
        imageData = bytesNew;
        bpp = 4; 
    }

    m_width = width;
    m_height = height;
    m_channels = bpp;
     
    m_data = (unsigned char*)malloc(width * height * bpp);
    memcpy(m_data, imageData, width * height * bpp);

    m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm);
    
    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = m_texture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout source = {};
    source.offset = 0;
    source.bytesPerRow = m_channels * m_width;
    source.rowsPerImage = m_height;

    WGPUExtent3D extent3D = { m_width, m_height, 1 };
    wgpuQueueWriteTexture(wgpContext.queue, &destination, imageData, width * height * bpp, &source, &extent3D);

    FreeImage_Unload(sourceBitmap);
    if (bytesNew)
        free(bytesNew);

    FreeImage_DeInitialise();
}

void WgpTexture::copyToDestination(const WGPUTexture& destTesture) {
    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = destTesture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout source = {};
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
    std::filesystem::path filePath = fileName;

    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = filePath.extension() == ".png" ? FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT) :
                             filePath.extension() == ".jpg" ? FreeImage_Load(FIF_JPEG, fileName.c_str(), JPEG_DEFAULT) :
                                                              FreeImage_Load(FIF_BMP, fileName.c_str(), BMP_DEFAULT);
    SwapRedBlue32(sourceBitmap);

    if (flipVertical)
        FreeImage_FlipVertical(sourceBitmap);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);
    unsigned char* bytesNew = nullptr;

    if (bpp == 3) {
        bytesNew = (unsigned char*)malloc(width * height * 4);

        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4); i = i + 4, k = k + 3) {
            bytesNew[i] = imageData[k];
            bytesNew[i + 1] = imageData[k + 1];
            bytesNew[i + 2] = imageData[k + 2];
            bytesNew[i + 3] = alphaChannel == -1 ? 255 : alphaChannel;
        }
        imageData = bytesNew;
        bpp = 4;
    }

   

    if (bytesNew) {
        FreeImage_Unload(sourceBitmap);
        return bytesNew;
    }

    return imageData;
}

unsigned char* WgpTexture::LoadFromFile(std::string fileName, uint32_t& width, uint32_t& height, const bool flipVertical, short alphaChannel) {
    std::filesystem::path filePath = fileName;

    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = filePath.extension() == ".png" ? FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT) :
                             filePath.extension() == ".jpg" ? FreeImage_Load(FIF_JPEG, fileName.c_str(), JPEG_DEFAULT) :
                                                              FreeImage_Load(FIF_BMP, fileName.c_str(), BMP_DEFAULT);
    SwapRedBlue32(sourceBitmap);

    if (flipVertical)
        FreeImage_FlipVertical(sourceBitmap);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    width = FreeImage_GetWidth(sourceBitmap);
    height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);
    unsigned char* bytesNew = nullptr;

    if (bpp == 3) {
        bytesNew = (unsigned char*)malloc(width * height * 4);

        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4); i = i + 4, k = k + 3) {
            bytesNew[i] = imageData[k];
            bytesNew[i + 1] = imageData[k + 1];
            bytesNew[i + 2] = imageData[k + 2];
            bytesNew[i + 3] = alphaChannel == -1 ? 255 : alphaChannel;
        }
        imageData = bytesNew;
        bpp = 4;
    }

    if (bytesNew) {
        FreeImage_Unload(sourceBitmap);
        return bytesNew;
    }

    return imageData;
}