#include <FreeImage.h>
#include <Utilities.h>

#include "WgpContext.h"
#include "WgpTexture.h"

WgpTexture::WgpTexture() :
    m_texture(NULL),
    m_format(WGPUTextureFormat::WGPUTextureFormat_Undefined),
    m_textureView(NULL),
    m_width(0u),
    m_height(0u),
    m_channels(0u),
    m_markForDelete(false) {

}

WgpTexture::WgpTexture(WgpTexture const& rhs) : m_texture(rhs.m_texture), m_format(rhs.m_format), m_textureView(rhs.m_textureView), m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels), m_markForDelete(false) {

}

WgpTexture::WgpTexture(WgpTexture&& rhs) noexcept : m_texture(rhs.m_texture), m_format(rhs.m_format), m_textureView(rhs.m_textureView), m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels), m_markForDelete(rhs.m_markForDelete) {

}

WgpTexture::~WgpTexture() {
    if (m_markForDelete) {
        cleanup();
    }
}

void WgpTexture::cleanup() {
    if (m_texture) {
        wgpuTextureDestroy(m_texture);
        wgpuTextureRelease(m_texture);
        m_texture = NULL;
    }

    if (m_textureView) {
        wgpuTextureViewRelease(m_textureView);
        m_textureView = NULL;
    }
}

void WgpTexture::markForDelete() {
    m_markForDelete = true;
}

const WGPUTexture& WgpTexture::getTexture() const {
    return m_texture;
}

const WGPUTextureView& WgpTexture::getTextureView() const {
    return m_textureView;
}

const unsigned int WgpTexture::getWidth() const {
    return m_width;
}

const unsigned int WgpTexture::getHeight() const {
    return m_height;
}

const WGPUTextureFormat WgpTexture::getFormat() const {
    return m_format;
}

void WgpTexture::loadFromFile(const std::string& fileName, const bool flipVertical, const short alphaChannel) {
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
    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
    m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm);
    
    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = m_texture;
    destination.mipLevel = 0u;
    destination.origin = { 0u, 0u, 0u };
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout source = {};
    source.offset = 0u;
    source.bytesPerRow = m_channels * m_width;
    source.rowsPerImage = m_height;

    WGPUExtent3D extent3D = { m_width, m_height, 1u };
    wgpuQueueWriteTexture(wgpContext.queue, &destination, imageData, width * height * bpp, &source, &extent3D);

    FreeImage_Unload(sourceBitmap);
    if (bytesNew)
        free(bytesNew);

    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);
}

void WgpTexture::loadFromMemory(unsigned char* data, uint32_t size, const bool flipVertical, const short alphaChannel) {
    FreeImage_Initialise();
    FIMEMORY* hmem = FreeImage_OpenMemory(data, size);

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem);
    FIBITMAP* sourceBitmap = FreeImage_LoadFromMemory(fif, hmem);

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

    m_width = width;
    m_height = height;
    m_channels = bpp;
    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
    m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm);

    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = m_texture;
    destination.mipLevel = 0u;
    destination.origin = { 0u, 0u, 0u };
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout source = {};
    source.offset = 0u;
    source.bytesPerRow = m_channels * m_width;
    source.rowsPerImage = m_height;

    WGPUExtent3D extent3D = { m_width, m_height, 1u };
    wgpuQueueWriteTexture(wgpContext.queue, &destination, imageData, width * height * bpp, &source, &extent3D);

    FreeImage_Unload(sourceBitmap);
    if (bytesNew)
        free(bytesNew);

    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);
}

void WgpTexture::createEmpty(uint32_t width, uint32_t height, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat) {
    m_width = width;
    m_height = height;
    m_channels = 4u;
    m_format = textureFormat;
    m_texture = wgpCreateTexture(m_width, m_height, textureUsage, textureFormat);

}

unsigned char* WgpTexture::LoadFromFile(std::string fileName, const bool flipVertical, const short alphaChannel) {
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
        FreeImage_DeInitialise();
        return bytesNew;
    }
    FreeImage_DeInitialise();
    return imageData;
}

unsigned char* WgpTexture::LoadFromFile(std::string fileName, uint32_t& width, uint32_t& height, const bool flipVertical, const short alphaChannel) {
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
        FreeImage_DeInitialise();
        return bytesNew;
    }
    FreeImage_DeInitialise();
    return imageData;
}

unsigned char* WgpTexture::LoadFromMemory(unsigned char* data, uint32_t size, uint32_t& width, uint32_t& height, const bool flipVertical, const short alphaChannel) {
    FreeImage_Initialise();
    FIMEMORY* hmem = FreeImage_OpenMemory(data, size);

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem);
    FIBITMAP* sourceBitmap = FreeImage_LoadFromMemory(fif, hmem);

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
        FreeImage_CloseMemory(hmem);
        FreeImage_DeInitialise();
        return bytesNew;
    }
    FreeImage_CloseMemory(hmem);
    FreeImage_DeInitialise();
    return imageData;
}

void WgpTexture::Safe(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels) {
    FreeImage_Initialise();    
    FIBITMAP* sourceBitmap = FreeImage_Allocate(width, height, channels * 8u, 0u, 0u, 0u);
    memcpy(FreeImage_GetBits(sourceBitmap), bytes, width * height * channels);
    FreeImage_Save(FIF_PNG, sourceBitmap, fileOut.c_str(), PNG_DEFAULT);
    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();
}

void WgpTexture::Safe(const std::string& fileOut, unsigned char* bytes, uint32_t size) {
    FreeImage_Initialise();
    FIMEMORY* hmem = FreeImage_OpenMemory(bytes, size);
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem);
    FIBITMAP* sourceBitmap = FreeImage_LoadFromMemory(fif, hmem);
    FreeImage_Save(FIF_PNG, sourceBitmap, fileOut.c_str(), PNG_DEFAULT);
    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();
}