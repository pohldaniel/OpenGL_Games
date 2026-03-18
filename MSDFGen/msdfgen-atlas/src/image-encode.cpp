
#include "image-encode.h"

#include <pixel-conversion.hpp>

#ifdef MSDFGEN_USE_LIBPNG

#include <png.h>

namespace msdf_atlas {

class PngGuard {
    png_structp png;
    png_infop info;

public:
    inline PngGuard(png_structp png, png_infop info) : png(png), info(info) { }
    inline ~PngGuard() {
        png_destroy_write_struct(&png, &info);
    }

};

static void pngIgnoreError(png_structp, png_const_charp) { }

static void pngWrite(png_structp png, png_bytep data, png_size_t length) {
    std::vector<byte> &output = *reinterpret_cast<std::vector<byte> *>(png_get_io_ptr(png));
    output.insert(output.end(), data, data+length);
}

static void pngFlush(png_structp) { }

static bool pngEncode(std::vector<byte> &output, const byte *pixels, int width, int height, int rowStride, int colorType) {
    if (!(pixels && width && height))
        return false;
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, &pngIgnoreError, &pngIgnoreError);
    if (!png)
        return false;
    png_infop info = png_create_info_struct(png);
    PngGuard guard(png, info);
    if (!info)
        return false;
    std::vector<const byte *> rows(height);
    for (int y = 0; y < height; ++y)
        rows[y] = pixels+rowStride*y;
    if (setjmp(png_jmpbuf(png)))
        return false;
    png_set_write_fn(png, &output, &pngWrite, &pngFlush);
    png_set_IHDR(png, info, width, height, 8, colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_compression_level(png, 9);
    png_set_rows(png, info, const_cast<png_bytepp>(&rows[0]));
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    return true;
}

static bool pngEncode(std::vector<byte> &output, const float *pixels, int width, int height, int rowStride, int channels, int colorType) {
    if (!(pixels && width && height))
        return false;
    std::vector<byte> bytePixels(channels*width*height);
    byte *dst = bytePixels.data();
    const float *rowStart = pixels;
    for (int y = 0; y < height; ++y) {
        for (const float *src = rowStart, *end = rowStart+channels*width; src < end; ++src)
            *dst++ = msdfgen::pixelFloatToByte(*src);
        rowStart += rowStride;
    }
    return pngEncode(output, bytePixels.data(), width, height, channels*width, colorType);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<byte, 1> bitmap) {
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    return pngEncode(output, bitmap.pixels, bitmap.width, bitmap.height, bitmap.rowStride, PNG_COLOR_TYPE_GRAY);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<byte, 3> bitmap) {
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    return pngEncode(output, bitmap.pixels, bitmap.width, bitmap.height, bitmap.rowStride, PNG_COLOR_TYPE_RGB);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<byte, 4> bitmap) {
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    return pngEncode(output, bitmap.pixels, bitmap.width, bitmap.height, bitmap.rowStride, PNG_COLOR_TYPE_RGB_ALPHA);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<float, 1> bitmap) {
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    return pngEncode(output, bitmap.pixels, bitmap.width, bitmap.height, bitmap.rowStride, 1, PNG_COLOR_TYPE_GRAY);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<float, 3> bitmap) {
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    return pngEncode(output, bitmap.pixels, bitmap.width, bitmap.height, bitmap.rowStride, 3, PNG_COLOR_TYPE_RGB);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<float, 4> bitmap) {
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    return pngEncode(output, bitmap.pixels, bitmap.width, bitmap.height, bitmap.rowStride, 4, PNG_COLOR_TYPE_RGB_ALPHA);
}

}

#endif

#ifdef MSDFGEN_USE_LODEPNG

#include <lodepng.h>

namespace msdf_atlas {

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<byte, 1> bitmap) {
    std::vector<byte> pixels(bitmap.width*bitmap.height);
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    for (int y = 0; y < bitmap.height; ++y)
        memcpy(&pixels[bitmap.width*y], bitmap(0, y), bitmap.width);
    return !lodepng::encode(output, pixels, bitmap.width, bitmap.height, LCT_GREY);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<byte, 3> bitmap) {
    std::vector<byte> pixels(3*bitmap.width*bitmap.height);
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    for (int y = 0; y < bitmap.height; ++y)
        memcpy(&pixels[3*bitmap.width*y], bitmap(0, y), 3*bitmap.width);
    return !lodepng::encode(output, pixels, bitmap.width, bitmap.height, LCT_RGB);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<byte, 4> bitmap) {
    std::vector<byte> pixels(4*bitmap.width*bitmap.height);
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    for (int y = 0; y < bitmap.height; ++y)
        memcpy(&pixels[4*bitmap.width*y], bitmap(0, y), 4*bitmap.width);
    return !lodepng::encode(output, pixels, bitmap.width, bitmap.height, LCT_RGBA);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<float, 1> bitmap) {
    std::vector<byte> pixels(bitmap.width*bitmap.height);
    std::vector<byte>::iterator it = pixels.begin();
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    for (int y = 0; y < bitmap.height; ++y) {
        for (int x = 0; x < bitmap.width; ++x)
            *it++ = msdfgen::pixelFloatToByte(*bitmap(x, y));
    }
    return !lodepng::encode(output, pixels, bitmap.width, bitmap.height, LCT_GREY);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<float, 3> bitmap) {
    std::vector<byte> pixels(3*bitmap.width*bitmap.height);
    std::vector<byte>::iterator it = pixels.begin();
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    for (int y = 0; y < bitmap.height; ++y) {
        for (int x = 0; x < bitmap.width; ++x) {
            *it++ = msdfgen::pixelFloatToByte(bitmap(x, y)[0]);
            *it++ = msdfgen::pixelFloatToByte(bitmap(x, y)[1]);
            *it++ = msdfgen::pixelFloatToByte(bitmap(x, y)[2]);
        }
    }
    return !lodepng::encode(output, pixels, bitmap.width, bitmap.height, LCT_RGB);
}

bool encodePng(std::vector<byte> &output, msdfgen::BitmapConstSection<float, 4> bitmap) {
    std::vector<byte> pixels(4*bitmap.width*bitmap.height);
    std::vector<byte>::iterator it = pixels.begin();
    bitmap.reorient(msdfgen::Y_DOWNWARD);
    for (int y = 0; y < bitmap.height; ++y) {
        for (int x = 0; x < bitmap.width; ++x) {
            *it++ = msdfgen::pixelFloatToByte(bitmap(x, y)[0]);
            *it++ = msdfgen::pixelFloatToByte(bitmap(x, y)[1]);
            *it++ = msdfgen::pixelFloatToByte(bitmap(x, y)[2]);
            *it++ = msdfgen::pixelFloatToByte(bitmap(x, y)[3]);
        }
    }
    return !lodepng::encode(output, pixels, bitmap.width, bitmap.height, LCT_RGBA);
}

}

#endif
