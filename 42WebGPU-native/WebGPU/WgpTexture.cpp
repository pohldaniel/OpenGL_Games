#include <vector>
#include <FreeImage.h>
#include "WgpTexture.h"

WgpTexture::WgpTexture() :
    m_width(0u),
    m_height(0u),
    m_channels(0u) {

}

WgpTexture::WgpTexture(WgpTexture const& rhs) : m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels) {

}

WgpTexture::WgpTexture(WgpTexture&& rhs) noexcept : m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels) {

}

void WgpTexture::loadFromFile(std::string fileName, const bool flipVertical) {
    FreeImage_Initialise();
    FIBITMAP* sourceBitmap = FreeImage_Load(FIF_PNG, fileName.c_str(), PNG_DEFAULT);
    unsigned int bpp = FreeImage_GetBPP(sourceBitmap);
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

    data = (unsigned char*)malloc(width * height * bpp);
    memcpy(data, imageData, width * height * bpp);

    FreeImage_Unload(sourceBitmap);
    FreeImage_DeInitialise();
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