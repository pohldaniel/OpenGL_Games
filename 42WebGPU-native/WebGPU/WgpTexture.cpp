#define _USE_MATH_DEFINES
#include <math.h>
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
    m_markForDelete(false),
    m_textureUsage(WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst) {

}

WgpTexture::WgpTexture(WgpTexture const& rhs) : m_texture(rhs.m_texture), m_format(rhs.m_format), m_textureView(rhs.m_textureView), m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels), m_markForDelete(false), m_textureUsage(rhs.m_textureUsage) {

}

WgpTexture::WgpTexture(WgpTexture&& rhs) noexcept : m_texture(rhs.m_texture), m_format(rhs.m_format), m_textureView(rhs.m_textureView), m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels), m_markForDelete(false), m_textureUsage(rhs.m_textureUsage) {

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

void WgpTexture::setTextureUsage(WGPUTextureUsage textureUsage) {
    m_textureUsage = textureUsage;
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

const uint32_t  WgpTexture::getMipLevelCount() const {
    return wgpuTextureGetMipLevelCount(m_texture);
}

const unsigned int& WgpTexture::width() const {
    return m_width;
}

const unsigned int& WgpTexture::height() const {
    return m_width;
}

static uint16_t Float32Tofloat16(float value) {
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
    }else if (exponent >= 0x1F) {
        //Overflow or infinity
        return sign | 0x7C00 | (mantissa ? 0x0200 : 0);
    }
    //Normalized value
    return sign | (exponent << 10) | (mantissa >> 13);
}

static uint16_t* GetFloat16(float* data, uint32_t width, uint32_t height, uint32_t channels) {
    const size_t count = width * height * channels;
    uint16_t* float16 = (uint16_t*)malloc(count * sizeof(uint16_t));

    for (size_t i = 0; i < count; i++) {
        float16[i] = Float32Tofloat16(data[i]);
    }

    return float16;
}

template<typename component_t>
static void WriteMipMaps(WGPUTexture& texture, WGPUExtent3D textureSize, uint32_t mipLevelCount, component_t* pixelData, uint32_t layer = 0u, const bool halfBPP = false) {
    uint32_t channels = 4u;

    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = texture;
    destination.mipLevel = 0u;
    destination.origin = { 0u, 0u, layer };
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout source = {};
    source.offset = 0u;

    // Create image data
    WGPUExtent3D mipLevelSize = textureSize;
    std::vector<component_t> previousLevelPixels;
    WGPUExtent3D previousMipLevelSize;
    for (uint32_t level = 0; level < mipLevelCount; ++level) {
        std::vector<component_t> pixels(channels * mipLevelSize.width * mipLevelSize.height);
        if (level == 0) {
            memcpy(pixels.data(), pixelData, pixels.size() * sizeof(component_t));
        }else {
            for (uint32_t i = 0; i < mipLevelSize.width; ++i) {
                for (uint32_t j = 0; j < mipLevelSize.height; ++j) {
                    component_t* p = &pixels[channels * (j * mipLevelSize.width + i)];
                    // Get the corresponding 4 pixels from the previous level
                    component_t* p00 = &previousLevelPixels[channels * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 0))];
                    component_t* p01 = &previousLevelPixels[channels * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 1))];
                    component_t* p10 = &previousLevelPixels[channels * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 0))];
                    component_t* p11 = &previousLevelPixels[channels * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 1))];
                    // Average
                    p[0] = (p00[0] + p01[0] + p10[0] + p11[0]) / (component_t)4;
                    p[1] = (p00[1] + p01[1] + p10[1] + p11[1]) / (component_t)4;
                    p[2] = (p00[2] + p01[2] + p10[2] + p11[2]) / (component_t)4;
                    p[3] = (p00[3] + p01[3] + p10[3] + p11[3]) / (component_t)4;
                }
            }
        }

        // Upload data to the GPU texture
        destination.mipLevel = level;
        source.bytesPerRow = channels * mipLevelSize.width * sizeof(component_t);
        source.rowsPerImage = mipLevelSize.height;
        if (halfBPP) {
            source.bytesPerRow = channels * mipLevelSize.width * sizeof(uint16_t);
            uint16_t* float16 = GetFloat16(reinterpret_cast<float*>(pixels.data()), mipLevelSize.width, mipLevelSize.height, channels);
            wgpuQueueWriteTexture(wgpContext.queue, &destination, float16, mipLevelSize.width * mipLevelSize.height * channels * sizeof(uint16_t), &source, &mipLevelSize);
            free(float16);
        }else
            wgpuQueueWriteTexture(wgpContext.queue, &destination, pixels.data(), pixels.size() * sizeof(component_t), &source, &mipLevelSize);

        previousLevelPixels = std::move(pixels);
        previousMipLevelSize = mipLevelSize;
        mipLevelSize.width /= 2;
        mipLevelSize.height /= 2;
    }
}

static int Clamp(const int& n, const int& lower, const int& upper) {
    return std::max(lower, std::min(n, upper));
}

static std::array<float, 3> OutImgToXYZ(int i, int j, int face, float edge) {
    float a = 2.0 * static_cast<float>(i) / edge;
    float b = 2.0 * static_cast<float>(j) / edge;
    if (face == 0) {
        return { -1.0f, 1.0f - a, 3.0f - b };
    }
    else if (face == 1) {
        return { a - 3.0f, -1.0f, 3.0f - b };
    }
    else if (face == 2) {
        return { 1.0f, a - 5.0f, 3.0f - b };
    }
    else if (face == 3) {
        return { 7.0f - a, 1.0f, 3.0f - b };
    }
    else if (face == 4) {
        return { b - 1.0f, a - 5.0f, 1.0f };
    }
    else if (face == 5) {
        return { 5.0f - b, a - 5.0f, -1.0f };
    }
    return { 0.0f, 0.0f, 0.0f };
}

template<typename component_t>
static unsigned char* EquirectangularToCross(component_t* sourceInOut, uint32_t width, uint32_t& height) {
    uint32_t channels = 4u;
    uint32_t heightNew = (width * 3u) / 4u;

    component_t* bytesNew = (component_t*)malloc(sizeof(component_t) * channels * width * heightNew);
    memset(bytesNew, 0, sizeof(component_t) * channels * width * heightNew);

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

            component_t* p00 = &sourceInOut[(Clamp(vi, 0, height - 1) * width) * channels + (ui % width) * channels];
            component_t* p01 = &sourceInOut[(Clamp(vi, 0, height - 1) * width) * channels + (u2 % width) * channels];
            component_t* p10 = &sourceInOut[(Clamp(v2, 0, height - 1) * width) * channels + (ui % width) * channels];
            component_t* p11 = &sourceInOut[(Clamp(v2, 0, height - 1) * width) * channels + (u2 % width) * channels];

            uint32_t offset = (j + (face == 0)) * width * channels - (edge * channels);
            uint32_t pixelIndex = offset + i * channels;

            bytesNew[pixelIndex]     = p00[0] * (1 - mu) * (1 - nu) + p01[0] * (mu) * (1 - nu) + p10[0] * (1 - mu) * nu + p11[0] * mu * nu;
            bytesNew[pixelIndex + 1] = p00[1] * (1 - mu) * (1 - nu) + p01[1] * (mu) * (1 - nu) + p10[1] * (1 - mu) * nu + p11[1] * mu * nu;
            bytesNew[pixelIndex + 2] = p00[2] * (1 - mu) * (1 - nu) + p01[2] * (mu) * (1 - nu) + p10[2] * (1 - mu) * nu + p11[2] * mu * nu;
            bytesNew[pixelIndex + 3] = p00[3] * (1 - mu) * (1 - nu) + p01[3] * (mu) * (1 - nu) + p10[3] * (1 - mu) * nu + p11[3] * mu * nu;
        }
    }

    height = heightNew;
    free(sourceInOut);
    return reinterpret_cast<unsigned char*>(bytesNew);
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

    unsigned int channels = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData =  FreeImage_GetBits(sourceBitmap);

    m_width = width;
    m_height = height;
    m_channels = channels;
    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;

    uint32_t mipLevelCount = BitWidth(std::max(m_width, m_height));
    m_texture = wgpCreateTexture(m_width, m_height, 1u, m_textureUsage, m_format, mipLevelCount);
    WriteMipMaps(m_texture, { m_width, m_height, 1u }, mipLevelCount, imageData);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(m_texture, WGPUTextureAspect::WGPUTextureAspect_All);
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

    unsigned int channels = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);
    
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;

    uint32_t mipLevelCount = BitWidth(std::max(m_width, m_height));
    m_texture = wgpCreateTexture(m_width, m_height, 1u, m_textureUsage, m_format, mipLevelCount);
    WriteMipMaps(m_texture, { m_width, m_height, 1u }, mipLevelCount, imageData);


    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(m_texture, WGPUTextureAspect::WGPUTextureAspect_All);
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

    unsigned int channels = FreeImage_GetBPP(sourceBitmap) / (8u * sizeof(float));
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);

    m_width = width;
    m_height = height;
    m_channels = channels;

    unsigned char* pixels = (unsigned char*)malloc(sizeof(float) * m_channels * m_width * m_height);
    memcpy(pixels, imageData, sizeof(float) * m_channels * m_width * m_height);
    pixels = EquirectangularToCross(reinterpret_cast<float*>(pixels), m_width, m_height);
    std::vector<unsigned char*> faces = CrossToFaces(pixels, m_width, sizeof(float), m_height);

    m_format = halfBPP ? WGPUTextureFormat::WGPUTextureFormat_RGBA16Float : WGPUTextureFormat::WGPUTextureFormat_RGBA32Float;

    uint32_t faceWidth = m_width > m_height ? m_width / 4u : m_width / 3u;
    uint32_t faceHeight = m_height > m_width ? m_height / 4u : m_height / 3u;
    uint32_t mipLevelCount = BitWidth(std::max(faceWidth, faceHeight));

    m_texture = wgpCreateTexture(faceWidth, faceHeight, 6u, m_textureUsage, m_format, mipLevelCount);
    for (uint32_t face = 0u; face < faces.size(); ++face) {
        WriteMipMaps(m_texture, { faceWidth, faceHeight, 1u }, mipLevelCount, reinterpret_cast<float*>(faces[face]), face, halfBPP);
        free(faces[face]);
    }

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(m_texture, WGPUTextureAspect::WGPUTextureAspect_All);
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

    unsigned int channels = FreeImage_GetBPP(sourceBitmap) / (8u * sizeof(float));
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);
    unsigned char* imageData = FreeImage_GetBits(sourceBitmap);
   
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_format = halfBPP ? WGPUTextureFormat::WGPUTextureFormat_RGBA16Float : WGPUTextureFormat::WGPUTextureFormat_RGBA32Float;

    uint32_t mipLevelCount = BitWidth(std::max(m_width, m_height));

    m_texture = wgpCreateTexture(m_width, m_height, 1u, m_textureUsage, m_format, mipLevelCount);
    WriteMipMaps(m_texture, { m_width, m_height, 1u }, mipLevelCount, reinterpret_cast<float*>(imageData), 0u, halfBPP);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();

    m_textureView = wgpCreateTextureView(m_texture, WGPUTextureAspect::WGPUTextureAspect_All);
}

void WgpTexture::loadCubeFromFiles(std::string* fileNames, const bool flipVertical) {
    FreeImage_Initialise();

    uint32_t mipLevelCount = 1u;
    for (unsigned short face = 0u; face < 6; face++) {
        std::filesystem::path filePath = fileNames[face];
        FIBITMAP* sourceBitmap = filePath.extension() == ".png" ? FreeImage_Load(FIF_PNG, fileNames[face].c_str(), PNG_DEFAULT) :
            filePath.extension() == ".jpg" ? FreeImage_Load(FIF_JPEG, fileNames[face].c_str(), JPEG_DEFAULT) :
            filePath.extension() == ".hdr" ? FreeImage_Load(FIF_HDR, fileNames[face].c_str(), HDR_DEFAULT) :
            FreeImage_Load(FIF_BMP, fileNames[face].c_str(), BMP_DEFAULT);

        SwapRedBlue32(sourceBitmap);

        if (flipVertical)
            FreeImage_FlipVertical(sourceBitmap);

        sourceBitmap = AddAlphaChannel(sourceBitmap);

        unsigned int channels = FreeImage_GetBPP(sourceBitmap) / 8;
        unsigned int width = FreeImage_GetWidth(sourceBitmap);
        unsigned int height = FreeImage_GetHeight(sourceBitmap);
        unsigned char* imageData = FreeImage_GetBits(sourceBitmap);
       
        if (face == 0) {
            m_width = width;
            m_height = height;
            m_channels = channels;
            m_format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
            mipLevelCount  = BitWidth(std::max(m_width, m_height));
            m_texture = wgpCreateTexture(m_width, m_height, 6u, m_textureUsage, m_format, mipLevelCount);
        }

        WriteMipMaps(m_texture, { m_width, m_height, 1u }, mipLevelCount, imageData, face);
        FreeImage_Unload(sourceBitmap);
    }

    FreeImage_DeInitialise();
    m_textureView = wgpCreateTextureView(m_texture, WGPUTextureAspect::WGPUTextureAspect_All);
}

void WgpTexture::createEmpty(uint32_t width, uint32_t height, uint32_t depth, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat, uint32_t mipLevelCount) {
    m_width = width;
    m_height = height;
    m_channels = 4u;
    m_format = textureFormat;
    m_texture = wgpCreateTexture(m_width, m_height, depth, textureUsage, m_format, mipLevelCount);
    m_textureView = wgpCreateTextureView(m_texture, WGPUTextureAspect::WGPUTextureAspect_All);
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

    unsigned int channels = FreeImage_GetBPP(sourceBitmap) / 8;
    unsigned int width = FreeImage_GetWidth(sourceBitmap);
    unsigned int height = FreeImage_GetHeight(sourceBitmap);

    unsigned char* pixels = (unsigned char*)malloc(channels * width * height);
    memcpy(pixels, FreeImage_GetBits(sourceBitmap), channels * width * height);

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

    unsigned int channels = FreeImage_GetBPP(sourceBitmap) / 8;
    width = FreeImage_GetWidth(sourceBitmap);
    height = FreeImage_GetHeight(sourceBitmap);

    unsigned char* pixels = (unsigned char*)malloc(channels * width * height);
    memcpy(pixels, FreeImage_GetBits(sourceBitmap), channels * width * height);

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

    unsigned int channels = FreeImage_GetBPP(sourceBitmap) / 8;
    width = FreeImage_GetWidth(sourceBitmap);
    height = FreeImage_GetHeight(sourceBitmap);

    unsigned char* pixels = (unsigned char*)malloc(channels * width * height);
    memcpy(pixels, FreeImage_GetBits(sourceBitmap), channels * width * height);

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
        
        union {
            float flt;
            unsigned char c[4];
        } one;

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

uint32_t WgpTexture::BitWidth(uint32_t m) {
    if (m == 0u) return 0u;
    else { uint32_t w = 0u; while (m >>= 1) ++w; return w + 1u; }
}