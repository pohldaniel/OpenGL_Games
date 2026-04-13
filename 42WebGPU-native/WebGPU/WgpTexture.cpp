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

    sourceBitmap = AddAlphaChannel(sourceBitmap, alphaChannel);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData =  FreeImage_GetBits(sourceBitmap);

    m_width = width;
    m_height = height;
    m_channels = bpp;
    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;

    uint32_t mipLevelCount = BitWidth(std::max(m_width, m_height));
    m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, m_format, mipLevelCount);
    WriteMipMaps(m_texture, { m_width, m_height, 1u }, mipLevelCount, imageData);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(m_format, WGPUTextureAspect::WGPUTextureAspect_All, mipLevelCount, m_texture);
}

void WgpTexture::loadFromMemory(unsigned char* data, uint32_t size, const bool flipVertical, const short alphaChannel) {
    FreeImage_Initialise();
    FIMEMORY* hmem = FreeImage_OpenMemory(data, size);

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem);
    FIBITMAP* sourceBitmap = FreeImage_LoadFromMemory(fif, hmem);

    SwapRedBlue32(sourceBitmap);

    if (flipVertical)
        FreeImage_FlipVertical(sourceBitmap);

    sourceBitmap = AddAlphaChannel(sourceBitmap, alphaChannel);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);
    
    m_width = width;
    m_height = height;
    m_channels = bpp;
    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;

    uint32_t mipLevelCount = BitWidth(std::max(m_width, m_height));
    m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, m_format, mipLevelCount);
    WriteMipMaps(m_texture, { m_width, m_height, 1u }, mipLevelCount, imageData);


    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(m_format, WGPUTextureAspect::WGPUTextureAspect_All, mipLevelCount, m_texture);
}

void WgpTexture::loadHDRICubeFromFile(const std::string& fileName, const bool flipVertical, const bool halfBPP) {
    std::filesystem::path filePath = fileName;

    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = filePath.extension() == ".png" ? FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT) :
                             filePath.extension() == ".jpg" ? FreeImage_Load(FIF_JPEG, fileName.c_str(), JPEG_DEFAULT) :
                             filePath.extension() == ".hdr" ? FreeImage_Load(FIF_HDR, fileName.c_str(), HDR_DEFAULT) :
                                                              FreeImage_Load(FIF_BMP, fileName.c_str(), BMP_DEFAULT);

    if (flipVertical)
        FreeImage_FlipVertical(sourceBitmap);

    sourceBitmap = AddAlphaChannel(sourceBitmap);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / (8u * sizeof(float));
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);

    m_width = width;
    m_height = height;
    m_channels = bpp; 

    unsigned char* pixels = (unsigned char*)malloc(sizeof(float) * m_channels * m_width * m_height);
    memcpy(pixels, imageData, sizeof(float) * m_channels * m_width * m_height);
    EquirectangularToCross(pixels, m_width, sizeof(float), m_height);
    std::vector<unsigned char*> faces = CrossToFaces(pixels, m_width, sizeof(float), m_height);

    m_format = halfBPP ? WGPUTextureFormat::WGPUTextureFormat_RGBA16Float : WGPUTextureFormat::WGPUTextureFormat_RGBA32Float;

    uint32_t faceWidth = m_width > m_height ? m_width / 4u : m_width / 3u;
    uint32_t faceHeight = m_height > m_width ? m_height / 4u : m_height / 3u;
    uint32_t mipLevelCount = BitWidth(std::max(faceWidth, faceHeight));

    WGPUTextureDescriptor textureDescriptor = {};
    textureDescriptor.label = WGPU_STR("texture");
    textureDescriptor.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
    textureDescriptor.size = { faceWidth, faceHeight, 6u };
    textureDescriptor.format = m_format;
    textureDescriptor.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDescriptor.mipLevelCount = mipLevelCount;
    textureDescriptor.sampleCount = 1u;
    textureDescriptor.nextInChain = NULL;

    m_texture = wgpuDeviceCreateTexture(wgpContext.device, &textureDescriptor);

    for (uint32_t face = 0u; face < faces.size(); ++face) {
        WriteMipMaps(m_texture, { faceWidth, faceHeight, 1u }, mipLevelCount, reinterpret_cast<float*>(faces[face]), face, halfBPP);
        free(faces[face]);
    }

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    WGPUTextureViewDescriptor textureViewDescriptor = {};
    textureViewDescriptor.label = WGPU_STR("texture_view");
    textureViewDescriptor.aspect = WGPUTextureAspect::WGPUTextureAspect_All;
    textureViewDescriptor.baseArrayLayer = 0u;
    textureViewDescriptor.arrayLayerCount = 6u;
    textureViewDescriptor.baseMipLevel = 0u;
    textureViewDescriptor.mipLevelCount = mipLevelCount;
    textureViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_Cube;
    textureViewDescriptor.format = m_format;
    textureViewDescriptor.nextInChain = NULL;

    m_textureView = wgpuTextureCreateView(m_texture, &textureViewDescriptor);  
}

void WgpTexture::loadHDRIFromFile(const std::string& fileName, const bool flipVertical, const bool halfBPP) {
    std::filesystem::path filePath = fileName;

    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = filePath.extension() == ".png" ? FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT) :
        filePath.extension() == ".jpg" ? FreeImage_Load(FIF_JPEG, fileName.c_str(), JPEG_DEFAULT) :
        filePath.extension() == ".hdr" ? FreeImage_Load(FIF_HDR, fileName.c_str(), HDR_DEFAULT) :
        FreeImage_Load(FIF_BMP, fileName.c_str(), BMP_DEFAULT);

    if (flipVertical)
        FreeImage_FlipVertical(sourceBitmap);

    sourceBitmap = AddAlphaChannel(sourceBitmap);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / (8u * sizeof(float));
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);
   
    m_width = width;
    m_height = height;
    m_channels = bpp;
    m_format = halfBPP ? WGPUTextureFormat::WGPUTextureFormat_RGBA16Float : WGPUTextureFormat::WGPUTextureFormat_RGBA32Float;

    WGPUTextureDescriptor textureDescriptor = {};
    textureDescriptor.label = WGPU_STR("texture");
    textureDescriptor.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
    textureDescriptor.size = { m_width, m_height, 1u };
    textureDescriptor.format = m_format;
    textureDescriptor.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDescriptor.mipLevelCount = 1u;
    textureDescriptor.sampleCount = 1u;
    textureDescriptor.nextInChain = NULL;

    m_texture = wgpuDeviceCreateTexture(wgpContext.device, &textureDescriptor);

    uint32_t mipLevelCount = BitWidth(std::max(m_width, m_height));
    m_texture = wgpCreateTexture(m_width, m_height, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, m_format, mipLevelCount);
    WriteMipMaps(m_texture, { m_width, m_height, 1u }, mipLevelCount, reinterpret_cast<float*>(imageData), 0u, halfBPP);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    WGPUTextureViewDescriptor textureViewDescriptor = {};
    textureViewDescriptor.label = WGPU_STR("texture_view");
    textureViewDescriptor.aspect = WGPUTextureAspect::WGPUTextureAspect_All;
    textureViewDescriptor.baseArrayLayer = 0u;
    textureViewDescriptor.arrayLayerCount = 1u;
    textureViewDescriptor.baseMipLevel = 0u;
    textureViewDescriptor.mipLevelCount = mipLevelCount;
    textureViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
    textureViewDescriptor.format = m_format;
    textureViewDescriptor.nextInChain = NULL;

    m_textureView = wgpuTextureCreateView(m_texture, &textureViewDescriptor);
}

void WgpTexture::createEmpty(uint32_t width, uint32_t height, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat) {
    m_width = width;
    m_height = height;
    m_channels = 4u;
    m_format = textureFormat;
    m_texture = wgpCreateTexture(m_width, m_height, textureUsage, m_format);
    m_textureView = wgpCreateTextureView(m_format, WGPUTextureAspect::WGPUTextureAspect_All, 1u, m_texture);
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

    sourceBitmap = AddAlphaChannel(sourceBitmap, alphaChannel);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);

    unsigned char* pixels = (unsigned char*)malloc(bpp * width * height);
    memcpy(pixels, FreeImage_GetBits(sourceBitmap), bpp * width * height);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    return pixels;
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

    sourceBitmap = AddAlphaChannel(sourceBitmap, alphaChannel);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);


    unsigned char* pixels = (unsigned char*)malloc(bpp * width * height);
    memcpy(pixels, FreeImage_GetBits(sourceBitmap), bpp * width * height);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    return pixels;
}

unsigned char* WgpTexture::LoadFromMemory(unsigned char* data, uint32_t size, uint32_t& width, uint32_t& height, const bool flipVertical, const short alphaChannel) {
    FreeImage_Initialise();
    FIMEMORY* hmem = FreeImage_OpenMemory(data, size);

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem);
    FIBITMAP* sourceBitmap = FreeImage_LoadFromMemory(fif, hmem);

    SwapRedBlue32(sourceBitmap);

    if (flipVertical)
        FreeImage_FlipVertical(sourceBitmap);

    sourceBitmap = AddAlphaChannel(sourceBitmap, alphaChannel);

    unsigned int bpp = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);


    unsigned char* pixels = (unsigned char*)malloc(bpp * width * height);
    memcpy(pixels, FreeImage_GetBits(sourceBitmap), bpp * width * height);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    return pixels;
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
    FreeImage_CloseMemory(hmem);
    FreeImage_DeInitialise();
}

void WgpTexture::SafeHDRI(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels) {    
    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = FreeImage_Allocate(width, height, 128u, 0u, 0u, 0u);
    memcpy(FreeImage_GetBits(sourceBitmap), bytes, width * height * 4u * sizeof(float));
    sourceBitmap = RemoveAlphaChannel(sourceBitmap);
    FreeImage_Save(FIF_HDR, sourceBitmap, fileOut.c_str(), HDR_DEFAULT);
    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();
}

FIBITMAP* WgpTexture::AddAlphaChannel(FIBITMAP* bitmap, const short alphaChannel) {
    unsigned int bpp = FreeImage_GetBPP(bitmap);

    if (bpp == 24) {
        unsigned int width = FreeImage_GetWidth(bitmap);
        unsigned int height = FreeImage_GetHeight(bitmap);
        unsigned char* imageData = FreeImage_GetBits(bitmap);
        unsigned char* pixels = (unsigned char*)malloc(width * height * 4u);

        for (unsigned int i = 0, k = 0; i < width * height * 4u; i = i + 4u, k = k + 3u) {
            pixels[i] = imageData[k];
            pixels[i + 1] = imageData[k + 1];
            pixels[i + 2] = imageData[k + 2];
            pixels[i + 3] = alphaChannel == -1 ? 255 : alphaChannel;
        }     
        FreeImage_Unload(bitmap);

        bitmap = FreeImage_Allocate(width, height, 32u, 0u, 0u, 0u);
        memcpy(FreeImage_GetBits(bitmap), pixels, width * height * 4u);

        free(pixels);
    }else if (bpp == 96) {
        unsigned int width = FreeImage_GetWidth(bitmap);
        unsigned int height = FreeImage_GetHeight(bitmap);
        unsigned char* imageData = FreeImage_GetBits(bitmap);
        unsigned char* pixels = (unsigned char*)malloc(sizeof(float) * width * height * 4u);

        UFloat one;
        one.flt = 1.0f;
        for (unsigned int i = 0, k = 0; i < width * height * 4u * sizeof(float); i = i + 4u * sizeof(float), k = k + 3u * sizeof(float)) {
            pixels[i + 0] = imageData[k + 0]; pixels[i + 1] = imageData[k + 1]; pixels[i + 2] = imageData[k + 2];  pixels[i + 3] = imageData[k + 3];
            pixels[i + 4] = imageData[k + 4]; pixels[i + 5] = imageData[k + 5]; pixels[i + 6] = imageData[k + 6];  pixels[i + 7] = imageData[k + 7];
            pixels[i + 8] = imageData[k + 8]; pixels[i + 9] = imageData[k + 9]; pixels[i + 10] = imageData[k + 10]; pixels[i + 11] = imageData[k + 11];
            pixels[i + 12] = one.c[0]; pixels[i + 13] = one.c[1]; pixels[i + 14] = one.c[2]; pixels[i + 15] = one.c[3];
        }
        FreeImage_Unload(bitmap);

        bitmap = FreeImage_Allocate(width, height, 128u, 0u, 0u, 0u);
        memcpy(FreeImage_GetBits(bitmap), pixels, sizeof(float) * width * height * 4u);

        free(pixels);
    }
    return bitmap;
}

FIBITMAP* WgpTexture::RemoveAlphaChannel(FIBITMAP* bitmap) {
    unsigned int bpp = FreeImage_GetBPP(bitmap);

    if(bpp == 32u) {
        unsigned int width = FreeImage_GetWidth(bitmap);
        unsigned int height = FreeImage_GetHeight(bitmap);
        unsigned char* imageData = FreeImage_GetBits(bitmap);
        unsigned char* pixels = (unsigned char*)malloc(width * height * 4u);

        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4u); i = i + 4u, k = k + 3u) {
            pixels[k + 0] = imageData[i + 0];
            pixels[k + 1] = imageData[i + 4];
            pixels[k + 2] = imageData[i + 8];
        }
        FreeImage_Unload(bitmap);
        bitmap = FreeImage_Allocate(width, height, 24u, 0u, 0u, 0u);
        memcpy(FreeImage_GetBits(bitmap), pixels, sizeof(float) * width * height * 3u);

        free(pixels);
    }else if (bpp == 128u) {
        unsigned int width = FreeImage_GetWidth(bitmap);
        unsigned int height = FreeImage_GetHeight(bitmap);
        unsigned char* imageData = FreeImage_GetBits(bitmap);
        unsigned char* pixels = (unsigned char*)malloc(width * height * 16u);

        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4u * sizeof(float)); i = i + 4u * sizeof(float), k = k + 3u * sizeof(float)) {
            pixels[k + 0] = imageData[i + 0]; pixels[k + 1] = imageData[i + 1]; pixels[k + 2] = imageData[i + 2];  pixels[k + 3] = imageData[i + 3];
            pixels[k + 4] = imageData[i + 4]; pixels[k + 5] = imageData[i + 5]; pixels[k + 6] = imageData[i + 6];  pixels[k + 7] = imageData[i + 7];
            pixels[k + 8] = imageData[i + 8]; pixels[k + 9] = imageData[i + 9]; pixels[k + 10] = imageData[i + 10]; pixels[k + 11] = imageData[i + 11];
        }
        FreeImage_Unload(bitmap);
        bitmap = FreeImage_AllocateT(FIT_RGBF, width, height);
        memcpy(FreeImage_GetBits(bitmap), pixels, sizeof(float) * width * height * 3u);

        free(pixels);
    }
    return bitmap;
}

void WgpTexture::FlipVertical(unsigned char* data, uint32_t width, uint32_t bytesPerChannel, uint32_t height) {
    uint32_t channels = 4u;
    
    std::vector<unsigned char> srcPixels(bytesPerChannel * channels * width * height);
    memcpy(&srcPixels[0], data, bytesPerChannel * channels * width * height);

    unsigned char* pSrcRow = 0;
    unsigned char* pDestRow = 0;

    for (unsigned int i = 0; i < height; ++i) {
        pSrcRow = &srcPixels[(height - 1 - i) * bytesPerChannel * channels * width];
        pDestRow = &data[i * bytesPerChannel * channels * width];
        memcpy(pDestRow, pSrcRow, bytesPerChannel * channels * width);
    }
}

void WgpTexture::FlipHorizontal(unsigned char* data, uint32_t width, uint32_t bytesPerChannel, uint32_t height) {
    uint32_t channels = 4u;

    unsigned char* pFront = 0;
    unsigned char* pBack = 0;
    std::vector<unsigned char> pixel;
    pixel.resize(bytesPerChannel * channels);
    for (uint32_t i = 0; i < height; ++i) {
        pFront = &data[i * width * bytesPerChannel * channels];
        pBack = &pFront[(width - 1) * bytesPerChannel * channels];
        while (pFront < pBack) {
            for (uint32_t j = 0; j < channels * bytesPerChannel; j++) {
                pixel[j] = pFront[j];
            }

            for (uint32_t j = 0; j < channels * bytesPerChannel; j++) {
                pFront[j] = pBack[j];
            }

            for (uint32_t j = 0; j < channels * bytesPerChannel; j++) {
                pBack[j] = pixel[j];
            }

            pFront += bytesPerChannel * channels;
            pBack -= bytesPerChannel * channels;
        }
    }
}

void WgpTexture::Rotate90(unsigned char*& sourceInOut, uint32_t width, uint32_t bytesPerChannel, uint32_t height, bool ccw) {
    uint32_t channels = 4u;

    unsigned char* bytesNew = (unsigned char*)malloc(bytesPerChannel * channels * width * height);   
    for (uint32_t y = 0u; y < height; ++y) {
        for (uint32_t x = 0u; x < width; ++x) {
            uint32_t oldPixelPos = (y * width + x) * channels * bytesPerChannel;
            uint32_t newX = ccw ? y : height - 1u - y;
            uint32_t newY = ccw ? (width - 1u) - x : x;

            uint32_t newPixelPos = (newY * height + newX) * channels * bytesPerChannel;
            for (uint32_t j = 0u; j < channels * bytesPerChannel; j++) {
                bytesNew[newPixelPos + j] = sourceInOut[oldPixelPos + j];
            }
        }
    }
    free(sourceInOut);
    sourceInOut = bytesNew;
}

std::vector<unsigned char*> WgpTexture::CrossToFaces(unsigned char* source, uint32_t width, uint32_t bytesPerChannel, uint32_t height) {   
    uint32_t fWidth = width > height ?  width / 4u : width / 3u;
    uint32_t fHeight = height > width ?  height / 4u : height / 3u;
    uint32_t channels = 4u;

    std::vector<unsigned char*> faces;
    faces.resize(6u);
    for (size_t face = 0u; face < faces.size(); ++face) {
        faces[face] = new unsigned char[fWidth * fHeight * channels * bytesPerChannel];
    }

    unsigned char* ptr;   
    // positive X
    ptr = faces[0];
    for (uint32_t j = 0; j < fHeight; j++) {
        memcpy(ptr, &source[((height - (fHeight + 1) - j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    } 
    FlipHorizontal(faces[0], fWidth, bytesPerChannel, fHeight);

    // negativ x
    ptr = faces[1];
    for (uint32_t j = 0; j < fHeight; j++) {
        memcpy(ptr, &source[((height - (fHeight + 1) - j) * width + 3u * fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }   
    FlipHorizontal(faces[1], fWidth, bytesPerChannel, fHeight);

    // positive y
    ptr = faces[2];
    for (uint32_t j = 0; j < fHeight; j++) {
        memcpy(ptr, &source[((height - (0u * fHeight + 1) - j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    Rotate90(faces[2], fWidth, bytesPerChannel, fHeight, true);
    FlipVertical(faces[2], fWidth, bytesPerChannel, fHeight);

    // negativ y
    ptr = faces[3];
    for (uint32_t j = 0; j < fHeight; j++) {
        memcpy(ptr, &source[((height - (2u * fHeight + 1) - j) * width + fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    Rotate90(faces[3], fWidth, bytesPerChannel, fHeight);
    FlipVertical(faces[3], fWidth, bytesPerChannel, fHeight);

    // positive z
    ptr = faces[4];
    for (uint32_t j = 0; j < fHeight; j++) {
        memcpy(ptr, &source[((height - (fHeight + 1) - j) * width + 2u * fWidth) * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    FlipHorizontal(faces[4], fWidth, bytesPerChannel, fHeight);

    // negative z
    ptr = faces[5];
    for (uint32_t j = 0; j < fHeight; j++) {
        memcpy(ptr, &source[(height - (fHeight + 1) - j) * width * channels * bytesPerChannel], fWidth * channels * bytesPerChannel);
        ptr += fWidth * channels * bytesPerChannel;
    }
    FlipHorizontal(faces[5], fWidth, bytesPerChannel, fHeight);
    free(source);
    return faces;
}

void WgpTexture::EquirectangularToCross(unsigned char*& sourceInOut, uint32_t width, uint32_t bytesPerChannel, uint32_t& height) {
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
                float Ar = BytesToFloatLE(sourceInOut[pixelIndexA + 0], sourceInOut[pixelIndexA + 1], sourceInOut[pixelIndexA + 2], sourceInOut[pixelIndexA + 3]);
                float Ag = BytesToFloatLE(sourceInOut[pixelIndexA + 4], sourceInOut[pixelIndexA + 5], sourceInOut[pixelIndexA + 6], sourceInOut[pixelIndexA + 7]);
                float Ab = BytesToFloatLE(sourceInOut[pixelIndexA + 8], sourceInOut[pixelIndexA + 9], sourceInOut[pixelIndexA + 10], sourceInOut[pixelIndexA + 11]);
                float Aa = BytesToFloatLE(sourceInOut[pixelIndexA + 12], sourceInOut[pixelIndexA + 13], sourceInOut[pixelIndexA + 14], sourceInOut[pixelIndexA + 15]);

                float Br = BytesToFloatLE(sourceInOut[pixelIndexB + 0], sourceInOut[pixelIndexB + 1], sourceInOut[pixelIndexB + 2], sourceInOut[pixelIndexB + 3]);
                float Bg = BytesToFloatLE(sourceInOut[pixelIndexB + 4], sourceInOut[pixelIndexB + 5], sourceInOut[pixelIndexB + 6], sourceInOut[pixelIndexB + 7]);
                float Bb = BytesToFloatLE(sourceInOut[pixelIndexB + 8], sourceInOut[pixelIndexB + 9], sourceInOut[pixelIndexB + 10], sourceInOut[pixelIndexB + 11]);
                float Ba = BytesToFloatLE(sourceInOut[pixelIndexB + 12], sourceInOut[pixelIndexB + 13], sourceInOut[pixelIndexB + 14], sourceInOut[pixelIndexB + 15]);

                float Cr = BytesToFloatLE(sourceInOut[pixelIndexC + 0], sourceInOut[pixelIndexC + 1], sourceInOut[pixelIndexC + 2], sourceInOut[pixelIndexC + 3]);
                float Cg = BytesToFloatLE(sourceInOut[pixelIndexC + 4], sourceInOut[pixelIndexC + 5], sourceInOut[pixelIndexC + 6], sourceInOut[pixelIndexC + 7]);
                float Cb = BytesToFloatLE(sourceInOut[pixelIndexC + 8], sourceInOut[pixelIndexC + 9], sourceInOut[pixelIndexC + 10], sourceInOut[pixelIndexC + 11]);
                float Ca = BytesToFloatLE(sourceInOut[pixelIndexC + 12], sourceInOut[pixelIndexC + 13], sourceInOut[pixelIndexC + 14], sourceInOut[pixelIndexC + 15]);

                float Dr = BytesToFloatLE(sourceInOut[pixelIndexD + 0], sourceInOut[pixelIndexD + 1], sourceInOut[pixelIndexD + 2], sourceInOut[pixelIndexD + 3]);
                float Dg = BytesToFloatLE(sourceInOut[pixelIndexD + 4], sourceInOut[pixelIndexD + 5], sourceInOut[pixelIndexD + 6], sourceInOut[pixelIndexD + 7]);
                float Db = BytesToFloatLE(sourceInOut[pixelIndexD + 8], sourceInOut[pixelIndexD + 9], sourceInOut[pixelIndexD + 10], sourceInOut[pixelIndexD + 11]);
                float Da = BytesToFloatLE(sourceInOut[pixelIndexD + 12], sourceInOut[pixelIndexD + 13], sourceInOut[pixelIndexD + 14], sourceInOut[pixelIndexD + 15]);

                float r = Ar * (1 - mu) * (1 - nu) + Br * (mu) * (1 - nu) + Cr * (1 - mu) * nu + Dr * mu * nu;
                float g = Ag * (1 - mu) * (1 - nu) + Bg * (mu) * (1 - nu) + Cg * (1 - mu) * nu + Dg * mu * nu;
                float b = Ab * (1 - mu) * (1 - nu) + Bb * (mu) * (1 - nu) + Cb * (1 - mu) * nu + Db * mu * nu;
                float a = Aa * (1 - mu) * (1 - nu) + Ba * (mu) * (1 - nu) + Ca * (1 - mu) * nu + Da * mu * nu;
  
                uint32_t offset = (j + (face == 0)) * width * channels * bytesPerChannel - (edge * channels * bytesPerChannel);                
                uint32_t pixelIndex = offset + i * channels * bytesPerChannel;

                UFloat R, G, B, A;
                R.flt = r; G.flt = g; B.flt = b; A.flt = a;
                bytesNew[pixelIndex + 0] = R.c[0]; bytesNew[pixelIndex + 1] = R.c[1]; bytesNew[pixelIndex + 2] = R.c[2]; bytesNew[pixelIndex + 3] = R.c[3];
                bytesNew[pixelIndex + 4] = G.c[0]; bytesNew[pixelIndex + 5] = G.c[1]; bytesNew[pixelIndex + 6] = G.c[2]; bytesNew[pixelIndex + 7] = G.c[3];
                bytesNew[pixelIndex + 8] = B.c[0]; bytesNew[pixelIndex + 9] = B.c[1]; bytesNew[pixelIndex + 10] = B.c[2]; bytesNew[pixelIndex + 11] = B.c[3];
                bytesNew[pixelIndex + 12] = A.c[0]; bytesNew[pixelIndex + 13] = A.c[1]; bytesNew[pixelIndex + 14] = A.c[2]; bytesNew[pixelIndex + 15] = A.c[3];
                
            }else if (bytesPerChannel == sizeof(unsigned char)) {
                float Ar = static_cast<float>(sourceInOut[pixelIndexA + 0]);
                float Ag = static_cast<float>(sourceInOut[pixelIndexA + 1]);
                float Ab = static_cast<float>(sourceInOut[pixelIndexA + 2]);
                float Aa = static_cast<float>(sourceInOut[pixelIndexA + 3]);

                float Br = static_cast<float>(sourceInOut[pixelIndexB + 0]);
                float Bg = static_cast<float>(sourceInOut[pixelIndexB + 1]);
                float Bb = static_cast<float>(sourceInOut[pixelIndexB + 2]);
                float Ba = static_cast<float>(sourceInOut[pixelIndexB + 3]);

                float Cr = static_cast<float>(sourceInOut[pixelIndexC + 0]);
                float Cg = static_cast<float>(sourceInOut[pixelIndexC + 1]);
                float Cb = static_cast<float>(sourceInOut[pixelIndexC + 2]);
                float Ca = static_cast<float>(sourceInOut[pixelIndexC + 3]);

                float Dr = static_cast<float>(sourceInOut[pixelIndexD + 0]);
                float Dg = static_cast<float>(sourceInOut[pixelIndexD + 1]);
                float Db = static_cast<float>(sourceInOut[pixelIndexD + 2]);
                float Da = static_cast<float>(sourceInOut[pixelIndexD + 3]);

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

    
    free(sourceInOut);
    sourceInOut = bytesNew;
    //bytesPerChannel == sizeof(float) ? SafeHDRI("res/sunset_cross.hdr", bytesNew, width, heightNew, 4u) : Safe("res/palace_cross.png", bytesNew, width, heightNew, 4u);
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

uint32_t WgpTexture::BitWidth(uint32_t m) {
    if (m == 0) return 0;
    else { uint32_t w = 0; while (m >>= 1) ++w; return w; }
}

uint16_t WgpTexture::Float32Tofloat16(float value) {
    union {
        float f;
        uint32_t i;
    } v;
    v.f = value;
    uint32_t i = v.i;

    uint32_t sign = (i >> 16) & 0x8000;
    int32_t exponent = ((i >> 23) & 0xFF) - 127 + 15;
    uint32_t mantissa = i & 0x007FFFFF;

    //Handle special cases 
    if (exponent <= 0) {
        ///nderflow or zero
        if (exponent < -10)
            return sign; //Too small, flush to zero
        mantissa = (mantissa | 0x00800000) >> (1 - exponent);
        return sign | (mantissa >> 13);
    } else if (exponent >= 0x1F) {
        //Overflow or infinity
        return sign | 0x7C00 | (mantissa ? 0x0200 : 0);
    }
    //Normalized value
    return sign | (exponent << 10) | (mantissa >> 13);
}

uint16_t* WgpTexture::GetFloat16(float* data, uint32_t width, uint32_t height, uint32_t channels) {
    const size_t count = width * height * channels;
    uint16_t* float16 = (uint16_t*)malloc(count * sizeof(uint16_t));

    for (size_t i = 0; i < count; i++) {
        float16[i] = Float32Tofloat16(data[i]);
    }

    return float16;
}
