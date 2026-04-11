#define _USE_MATH_DEFINES
#include <math.h>
#include <FreeImage.h>
#include <Utilities.h>
#include <iostream>

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
                             filePath.extension() == ".hdr" ? FreeImage_Load(FIF_HDR, fileName.c_str(), HDR_DEFAULT) :
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
        bpp = 4; 
    }

    m_width = width;
    m_height = height;
    m_channels = bpp;

    //bytesNew ? bytesNew = EquirectangularToCross(bytesNew, m_width, sizeof(unsigned char), m_height) : imageData = EquirectangularToCross(imageData, m_width, sizeof(unsigned char), m_height);

    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
    m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, m_format);
    
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
    wgpuQueueWriteTexture(wgpContext.queue, &destination, bytesNew ? bytesNew : imageData, m_width * m_height * m_channels, &source, &extent3D);
  
    if (bytesNew)
        free(bytesNew);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(m_format, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);
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
        bpp = 4;
    }

    m_width = width;
    m_height = height;
    m_channels = bpp;
    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
    m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, m_format);

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
    wgpuQueueWriteTexture(wgpContext.queue, &destination, bytesNew ? bytesNew : imageData, m_width * m_height * m_channels, &source, &extent3D);
  
    if (bytesNew)
        free(bytesNew);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(m_format, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);
}

void WgpTexture::loadHDRIFromFile(const std::string& fileName, const bool flipVertical) {
    std::filesystem::path filePath = fileName;

    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = filePath.extension() == ".png" ? FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT) :
                             filePath.extension() == ".jpg" ? FreeImage_Load(FIF_JPEG, fileName.c_str(), JPEG_DEFAULT) :
                             filePath.extension() == ".hdr" ? FreeImage_Load(FIF_HDR, fileName.c_str(), HDR_DEFAULT) :
                                                              FreeImage_Load(FIF_BMP, fileName.c_str(), BMP_DEFAULT);

    if (flipVertical)
        FreeImage_FlipVertical(sourceBitmap);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / (8u * sizeof(float));
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);

    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);
    unsigned char* bytesNew = nullptr;

    if (bpp == 3) {     
        bytesNew = (unsigned char*)malloc(sizeof(float) * 4u * width * height);
        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4u * sizeof(float)); i = i + 4u * sizeof(float), k = k + 3u * sizeof(float)) {
            bytesNew[i + 0] = imageData[k + 0]; bytesNew[i + 1] = imageData[k + 1]; bytesNew[i + 2]  = imageData[k + 2];  bytesNew[i + 3]  = imageData[k + 3];
            bytesNew[i + 4] = imageData[k + 4]; bytesNew[i + 5] = imageData[k + 5]; bytesNew[i + 6]  = imageData[k + 6];  bytesNew[i + 7]  = imageData[k + 7];
            bytesNew[i + 8] = imageData[k + 8]; bytesNew[i + 9] = imageData[k + 9]; bytesNew[i + 10] = imageData[k + 10]; bytesNew[i + 11] = imageData[k + 11];
            bytesNew[i + 12] = 255; bytesNew[i + 13] = 255; bytesNew[i + 14] = 255; bytesNew[i + 15] = 255;
        }
        bpp = 4;
    }

    m_width = width;
    m_height = height;
    m_channels = bpp;

    bytesNew ? bytesNew = EquirectangularToCross(bytesNew, m_width, sizeof(float), m_height) : imageData = EquirectangularToCross(imageData, m_width, sizeof(float), m_height);

    std::vector<unsigned char*> faces = CrossToFaces(bytesNew ? bytesNew : imageData, m_width, sizeof(float), m_height);

    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA32Float;

    uint32_t faceWidth = m_width > m_height ? m_width / 4u : m_width / 3u;
    uint32_t faceHeight = m_height > m_width ? m_height / 4u : m_height / 3u;

    const WGPUDevice& device = wgpContext.device;
    WGPUTextureDescriptor textureDescriptor = {};
    textureDescriptor.label = WGPU_STR("texture");
    textureDescriptor.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
    textureDescriptor.size = { faceWidth, faceHeight, 6u };
    textureDescriptor.format = m_format;
    textureDescriptor.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDescriptor.mipLevelCount = 1u;
    textureDescriptor.sampleCount = 1u;
    textureDescriptor.nextInChain = NULL;
    //if (viewFormat != WGPUTextureFormat_Undefined) {
    //    textureDescriptor.viewFormatCount = 1;
    //    textureDescriptor.viewFormats = &m_format;
    //}

    m_texture = wgpuDeviceCreateTexture(device, &textureDescriptor);

    WGPUTexelCopyBufferLayout source = {};
    source.offset = 0u;
    source.bytesPerRow = sizeof(float) * m_channels * faceWidth;
    source.rowsPerImage = faceHeight;

    for (uint32_t face = 0u; face < faces.size(); ++face) {
        WGPUTexelCopyTextureInfo destination = {};
        destination.texture = m_texture;
        destination.mipLevel = 0u;
        destination.origin = { 0u, 0u, face };
        destination.aspect = WGPUTextureAspect_All;

        WGPUExtent3D extent3D = { faceWidth, faceHeight, 1u };
        wgpuQueueWriteTexture(wgpContext.queue, &destination, faces[face], sizeof(float) * m_channels * faceWidth * faceWidth, &source, &extent3D);
    }
    if (bytesNew)
        free(bytesNew);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    WGPUTextureViewDescriptor textureViewDescriptor = {};
    textureViewDescriptor.label = WGPU_STR("texture_view");
    textureViewDescriptor.aspect = WGPUTextureAspect::WGPUTextureAspect_All;
    textureViewDescriptor.baseArrayLayer = 0u;
    textureViewDescriptor.arrayLayerCount = 6u;
    textureViewDescriptor.baseMipLevel = 0u;
    textureViewDescriptor.mipLevelCount = 1u;
    textureViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_Cube;
    textureViewDescriptor.format = m_format;
    textureViewDescriptor.nextInChain = NULL;

    m_textureView = wgpuTextureCreateView(m_texture, &textureViewDescriptor);  
    //m_textureView = wgpCreateTextureView(m_format, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);
}

void WgpTexture::createEmpty(uint32_t width, uint32_t height, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat) {
    m_width = width;
    m_height = height;
    m_channels = 4u;
    m_format = textureFormat;
    m_texture = wgpCreateTexture(m_width, m_height, textureUsage, m_format);
    m_textureView = wgpCreateTextureView(m_format, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);
}

unsigned char* WgpTexture::LoadFromFile(std::string fileName, const bool flipVertical, const short alphaChannel) {
    std::filesystem::path filePath = fileName;

    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = filePath.extension() == ".png" ? FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT) :
                             filePath.extension() == ".jpg" ? FreeImage_Load(FIF_JPEG, fileName.c_str(), JPEG_DEFAULT) :
                             filePath.extension() == ".hdr" ? FreeImage_Load(FIF_HDR, fileName.c_str(), HDR_DEFAULT) :
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
                             filePath.extension() == ".hdr" ? FreeImage_Load(FIF_HDR, fileName.c_str(), HDR_DEFAULT) :
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

void WgpTexture::SafeHDRI(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels) {
    unsigned char* bytesNew = nullptr;
    if (channels == 4u) {
        bytesNew = (unsigned char*)malloc(sizeof(float) * 3u * width * height);
        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4u * sizeof(float)); i = i + 4u * sizeof(float), k = k + 3u * sizeof(float)) {
            bytesNew[k + 0] = bytes[i + 0]; bytesNew[k + 1] = bytes[i + 1]; bytesNew[k + 2] = bytes[i + 2];  bytesNew[k + 3] = bytes[i + 3];
            bytesNew[k + 4] = bytes[i + 4]; bytesNew[k + 5] = bytes[i + 5]; bytesNew[k + 6] = bytes[i + 6];  bytesNew[k + 7] = bytes[i + 7];
            bytesNew[k + 8] = bytes[i + 8]; bytesNew[k + 9] = bytes[i + 9]; bytesNew[k + 10] = bytes[i + 10]; bytesNew[k + 11] = bytes[i + 11];  
        }
    }
  
    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = FreeImage_AllocateT(FIT_RGBF, width , height);
    memcpy(FreeImage_GetBits(sourceBitmap), bytesNew ? bytesNew : bytes, width * height * 3u * sizeof(float));
    FreeImage_Save(FIF_HDR, sourceBitmap, fileOut.c_str(), HDR_DEFAULT);
    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    if(bytesNew)
        free(bytesNew);
}

std::vector<unsigned char*> WgpTexture::CrossToFaces(unsigned char* source, uint32_t width, uint32_t bytesPerChannel, uint32_t height) {
    std::vector<unsigned char*> faces;

    uint32_t fWidth = width > height ?  width / 4u : width / 3u;
    uint32_t fHeight = height > width ?  height / 4u : height / 3u;
    uint32_t channels = 4u;

    unsigned char* face = new unsigned char[fWidth * fHeight * channels * bytesPerChannel];
    unsigned char* ptr;

    // positive X
    ptr = face;
    for (uint32_t j = 0; j < fHeight; j++) {
        //flip
        //memcpy(ptr, &source[((height - (fHeight + 1) - j) * width + 2u * fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
 
        memcpy(ptr, &source[((height - 2u * fHeight + j) * width + 2u * fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    faces.push_back(face);

    // negativ x
    ptr = face;
    for (uint32_t j = 0; j < fHeight; j++) {
        //flip
        //memcpy(ptr, &source[(height - (fHeight + 1) - j) * width * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);

        memcpy(ptr, &source[(height - 2u * fHeight + j) * width * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    faces.push_back(face);

    // positive z
    ptr = face;
    for (uint32_t j = 0; j < fHeight; j++) {
        //flip
        //memcpy(ptr, &source[((height - (fHeight + 1) - j) * width + 3u * fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);

        memcpy(ptr, &source[((height - 2u * fHeight + j) * width + 3u * fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    faces.push_back(face);

    // negativ z
    ptr = face;
    for (uint32_t j = 0; j < fHeight; j++) {
        //flip
        //memcpy(ptr, &source[((height - (fHeight + 1) - j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);

        memcpy(ptr, &source[((height - 2u * fHeight + j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    faces.push_back(face);

    // positive y
    ptr = face;
    for (uint32_t j = 0; j < fHeight; j++) {
        //flip
        //memcpy(ptr, &source[((height - (0u * fHeight + 1) - j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);

        memcpy(ptr, &source[((height - 1u * fHeight + j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    faces.push_back(face);

    // negative y
    ptr = face;
    for (uint32_t j = 0; j < fHeight; j++) {
        //flip
        //memcpy(ptr, &source[((height - (2u * fHeight + 1) - j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);

        memcpy(ptr, &source[((height - 3u * fHeight + j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    faces.push_back(face);

    return faces;
}

unsigned char* WgpTexture::EquirectangularToCross(unsigned char* source, uint32_t width, uint32_t bytesPerChannel, uint32_t& height) {
    uint32_t channels = 4u;
    uint32_t heightNew = (width * 3u) / 4u;

    unsigned char* bytesNew = (unsigned char*)malloc(bytesPerChannel * channels * width * heightNew);
    memset(bytesNew, 0, bytesPerChannel * channels * width * heightNew);

    uint32_t edge = width / 4u;
    for (uint32_t i = 0u; i < width; ++i) {
        int face = int(i / edge);
        uint32_t range = face == 2 ? edge * 3u : edge * 2u;
        uint32_t offset = face == 2 ? 0u : edge;

        for (uint32_t j = offset; j < range; ++j) {
            int face2 = j < edge ? 4 : j >= 2u * edge ? 5 : face;

            std::array<float, 3> coord = OutImgToXYZ(i, j, face2, static_cast<float>(edge));
            float theta = std::atan2f(coord[1], coord[0]);
            float radius = std::sqrtf((coord[0] * coord[0]) + (coord[1] * coord[1]));
            float phi = std::atan2(coord[2], radius);

            //source img coords
            float uf = (2.0f * edge * (theta + M_PI) / M_PI);
            float vf = (2.0f * edge * (M_PI_2 - phi) / M_PI);

            int ui = floor(uf); // coord of pixel to bottom left
            int vi = floor(vf);
            int u2 = ui + 1;    // coords of pixel to top right
            int v2 = vi + 1;
            float mu = uf - ui;   // fraction of way across pixel
            float nu = vf - vi;

            uint32_t pixelIndexA = (Clamp(vi, 0, height - 1) * width) * channels * bytesPerChannel + (ui % width) * channels * bytesPerChannel;
            uint32_t pixelIndexB = (Clamp(vi, 0, height - 1) * width) * channels * bytesPerChannel + (u2 % width) * channels * bytesPerChannel;
            uint32_t pixelIndexC = (Clamp(v2, 0, height - 1) * width) * channels * bytesPerChannel + (ui % width) * channels * bytesPerChannel;
            uint32_t pixelIndexD = (Clamp(v2, 0, height - 1) * width) * channels * bytesPerChannel + (u2 % width) * channels * bytesPerChannel;

            if (bytesPerChannel == sizeof(float)) {
                float Ar = BytesToFloatLE(source[pixelIndexA + 0], source[pixelIndexA + 1], source[pixelIndexA + 2], source[pixelIndexA + 3]);
                float Ag = BytesToFloatLE(source[pixelIndexA + 4], source[pixelIndexA + 5], source[pixelIndexA + 6], source[pixelIndexA + 7]);
                float Ab = BytesToFloatLE(source[pixelIndexA + 8], source[pixelIndexA + 9], source[pixelIndexA + 10], source[pixelIndexA + 11]);
                float Aa = BytesToFloatLE(source[pixelIndexA + 12], source[pixelIndexA + 13], source[pixelIndexA + 14], source[pixelIndexA + 15]);

                float Br = BytesToFloatLE(source[pixelIndexB + 0], source[pixelIndexB + 1], source[pixelIndexB + 2], source[pixelIndexB + 3]);
                float Bg = BytesToFloatLE(source[pixelIndexB + 4], source[pixelIndexB + 5], source[pixelIndexB + 6], source[pixelIndexB + 7]);
                float Bb = BytesToFloatLE(source[pixelIndexB + 8], source[pixelIndexB + 9], source[pixelIndexB + 10], source[pixelIndexB + 11]);
                float Ba = BytesToFloatLE(source[pixelIndexB + 12], source[pixelIndexB + 13], source[pixelIndexB + 14], source[pixelIndexB + 15]);

                float Cr = BytesToFloatLE(source[pixelIndexC + 0], source[pixelIndexC + 1], source[pixelIndexC + 2], source[pixelIndexC + 3]);
                float Cg = BytesToFloatLE(source[pixelIndexC + 4], source[pixelIndexC + 5], source[pixelIndexC + 6], source[pixelIndexC + 7]);
                float Cb = BytesToFloatLE(source[pixelIndexC + 8], source[pixelIndexC + 9], source[pixelIndexC + 10], source[pixelIndexC + 11]);
                float Ca = BytesToFloatLE(source[pixelIndexC + 12], source[pixelIndexC + 13], source[pixelIndexC + 14], source[pixelIndexC + 15]);

                float Dr = BytesToFloatLE(source[pixelIndexD + 0], source[pixelIndexD + 1], source[pixelIndexD + 2], source[pixelIndexD + 3]);
                float Dg = BytesToFloatLE(source[pixelIndexD + 4], source[pixelIndexD + 5], source[pixelIndexD + 6], source[pixelIndexD + 7]);
                float Db = BytesToFloatLE(source[pixelIndexD + 8], source[pixelIndexD + 9], source[pixelIndexD + 10], source[pixelIndexD + 11]);
                float Da = BytesToFloatLE(source[pixelIndexD + 12], source[pixelIndexD + 13], source[pixelIndexD + 14], source[pixelIndexD + 15]);

                float r = Ar * (1 - mu) * (1 - nu) + Br * (mu) * (1 - nu) + Cr * (1 - mu) * nu + Dr * mu * nu;
                float g = Ag * (1 - mu) * (1 - nu) + Bg * (mu) * (1 - nu) + Cg * (1 - mu) * nu + Dg * mu * nu;
                float b = Ab * (1 - mu) * (1 - nu) + Bb * (mu) * (1 - nu) + Cb * (1 - mu) * nu + Db * mu * nu;
                float a = Aa * (1 - mu) * (1 - nu) + Ba * (mu) * (1 - nu) + Ca * (1 - mu) * nu + Da * mu * nu;
  
                uint32_t offset = (j + (face == 0)) * width * channels * bytesPerChannel - (edge * channels * bytesPerChannel);                
                uint32_t pixelIndex = offset + i * channels * bytesPerChannel;

                UFloat R, G, B, A;
                R.flt = r; G.flt = g; B.flt = b;              
                bytesNew[pixelIndex + 0] = R.c[0]; bytesNew[pixelIndex + 1] = R.c[1]; bytesNew[pixelIndex + 2] = R.c[2]; bytesNew[pixelIndex + 3] = R.c[3];
                bytesNew[pixelIndex + 4] = G.c[0]; bytesNew[pixelIndex + 5] = G.c[1]; bytesNew[pixelIndex + 6] = G.c[2]; bytesNew[pixelIndex + 7] = G.c[3];
                bytesNew[pixelIndex + 8] = B.c[0]; bytesNew[pixelIndex + 9] = B.c[1]; bytesNew[pixelIndex + 10] = B.c[2]; bytesNew[pixelIndex + 11] = B.c[3];
                bytesNew[pixelIndex + 12] = 255; bytesNew[pixelIndex + 13] = 255; bytesNew[pixelIndex + 14] = 255; bytesNew[pixelIndex + 15] = 255;
                
            }else if (bytesPerChannel == sizeof(unsigned char)) {
                float Ar = static_cast<float>(source[pixelIndexA + 0]);
                float Ag = static_cast<float>(source[pixelIndexA + 1]);
                float Ab = static_cast<float>(source[pixelIndexA + 2]);
                float Aa = static_cast<float>(source[pixelIndexA + 3]);

                float Br = static_cast<float>(source[pixelIndexB + 0]);
                float Bg = static_cast<float>(source[pixelIndexB + 1]);
                float Bb = static_cast<float>(source[pixelIndexB + 2]);
                float Ba = static_cast<float>(source[pixelIndexB + 3]);

                float Cr = static_cast<float>(source[pixelIndexC + 0]);
                float Cg = static_cast<float>(source[pixelIndexC + 1]);
                float Cb = static_cast<float>(source[pixelIndexC + 2]);
                float Ca = static_cast<float>(source[pixelIndexC + 3]);

                float Dr = static_cast<float>(source[pixelIndexD + 0]);
                float Dg = static_cast<float>(source[pixelIndexD + 1]);
                float Db = static_cast<float>(source[pixelIndexD + 2]);
                float Da = static_cast<float>(source[pixelIndexD + 3]);

                float r = Ar * (1 - mu) * (1 - nu) + Br * (mu) * (1 - nu) + Cr * (1 - mu) * nu + Dr * mu * nu;
                float g = Ag * (1 - mu) * (1 - nu) + Bg * (mu) * (1 - nu) + Cg * (1 - mu) * nu + Dg * mu * nu;
                float b = Ab * (1 - mu) * (1 - nu) + Bb * (mu) * (1 - nu) + Cb * (1 - mu) * nu + Db * mu * nu;
                float a = Aa * (1 - mu) * (1 - nu) + Ba * (mu) * (1 - nu) + Ca * (1 - mu) * nu + Da * mu * nu;

                uint32_t offset = (j + (face == 0)) * width * channels * bytesPerChannel - (edge * channels * bytesPerChannel);
                uint32_t pixelIndex = offset + i * channels * bytesPerChannel;

                bytesNew[pixelIndex + 0] = round(r);
                bytesNew[pixelIndex + 1] = round(g);
                bytesNew[pixelIndex + 2] = round(b);
                bytesNew[pixelIndex + 3] = round(a);
            }
        }
    }
    height = heightNew;
    free(source);
    //bytesPerChannel == sizeof(float) ? SafeHDRI("res/sunset_cross.hdr", bytesNew, width, heightNew, 4u) : Safe("res/palace_cross.png", bytesNew, width, heightNew, 4u);
    return bytesNew;
}

float WgpTexture::Clamp(const float& n, const float& lower, const float& upper) {
    return std::max(lower, std::min(n, upper));
}

std::array<float, 3> WgpTexture::OutImgToXYZ(int i, int j, int face, float edge) {
    float a = 2.0 * static_cast<float>(i) / edge;
    float b = 2.0 * static_cast<float>(j) / edge;
    if (face == 0) {
        return { -1.0f, 1.0f - a, 3.0f - b };
    }else if (face == 1) {
        return { a - 3.0f, -1.0f, 3.0f - b };
    }else if (face == 2) {
        return { 1.0f, a - 5.0f, 3.0f - b };
    }else if (face == 3) {
        return { 7.0f - a, 1.0f, 3.0f - b };
    }else if (face == 4) {
        return { b - 1.0f, a - 5.0f, 1.0f };
    }else if (face == 5) {
        return { 5.0f - b, a - 5.0f, -1.0f };
    }
    return { 0.0f, 0.0f, 0.0f };
}

float WgpTexture::BytesToFloatLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3) {
    float f;
    unsigned char b[] = { b0, b1, b2, b3 };
    memcpy(&f, &b, sizeof(float));
    return f;
}

float WgpTexture::BytesToFloatBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3) {
    float f;
    unsigned char b[] = { b3, b2, b1, b0 };
    memcpy(&f, &b, sizeof(float));
    return f;
}
