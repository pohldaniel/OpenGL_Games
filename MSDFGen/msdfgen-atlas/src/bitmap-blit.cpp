
#include "bitmap-blit.h"

#include <cstring>
#include <algorithm>

namespace msdf_atlas {

#define BOUND_SECTION() { \
    if (dx < 0) w += dx, sx -= dx, dx = 0; \
    if (dy < 0) h += dy, sy -= dy, dy = 0; \
    if (sx < 0) w += sx, dx -= sx, sx = 0; \
    if (sy < 0) h += sy, dy -= sy, sy = 0; \
    w = std::max(0, std::min(w, std::min(dst.width-dx, src.width-sx))); \
    h = std::max(0, std::min(h, std::min(dst.height-dy, src.height-sy))); \
}

template <typename T, int N>
static void blitSameType(const msdfgen::BitmapSection<T, N> &dst, const msdfgen::BitmapConstSection<T, N> &src) {
    int width = std::min(dst.width, src.width), height = std::min(dst.height, src.height);
    size_t rowSize = sizeof(T)*N*width;
    for (int y = 0; y < height; ++y)
        memcpy(dst(0, y), src(0, y), rowSize);
}

template <typename T, int N>
static void blitSameType(const msdfgen::BitmapSection<T, N> &dst, const msdfgen::BitmapConstSection<T, N> &src, int dx, int dy, int sx, int sy, int w, int h) {
    BOUND_SECTION();
    size_t rowSize = sizeof(T)*N*w;
    for (int y = 0; y < h; ++y)
        memcpy(dst(dx, dy+y), src(sx, sy+y), rowSize);
}

#define BLIT_SAME_TYPE_IMPL(T, N) void blit(const msdfgen::BitmapSection<T, N> &dst, const msdfgen::BitmapConstSection<T, N> &src) { blitSameType(dst, src); }
#define BLIT_SAME_TYPE_PART_IMPL(T, N) void blit(const msdfgen::BitmapSection<T, N> &dst, const msdfgen::BitmapConstSection<T, N> &src, int dx, int dy, int sx, int sy, int w, int h) { blitSameType(dst, src, dx, dy, sx, sy, w, h); }

BLIT_SAME_TYPE_IMPL(byte, 1)
BLIT_SAME_TYPE_IMPL(byte, 3)
BLIT_SAME_TYPE_IMPL(byte, 4)
BLIT_SAME_TYPE_IMPL(float, 1)
BLIT_SAME_TYPE_IMPL(float, 3)
BLIT_SAME_TYPE_IMPL(float, 4)

void blit(const msdfgen::BitmapSection<byte, 1> &dst, const msdfgen::BitmapConstSection<float, 1> &src) {
    int width = std::min(dst.width, src.width), height = std::min(dst.height, src.height);
    for (int y = 0; y < height; ++y) {
        byte *dstPixel = dst(0, y);
        const float *srcPixel = src(0, y);
        for (int x = 0; x < width; ++x)
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
    }
}

void blit(const msdfgen::BitmapSection<byte, 3> &dst, const msdfgen::BitmapConstSection<float, 3> &src) {
    int width = std::min(dst.width, src.width), height = std::min(dst.height, src.height);
    for (int y = 0; y < height; ++y) {
        byte *dstPixel = dst(0, y);
        const float *srcPixel = src(0, y);
        for (int x = 0; x < width; ++x) {
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
        }
    }
}

void blit(const msdfgen::BitmapSection<byte, 4> &dst, const msdfgen::BitmapConstSection<float, 4> &src) {
    int width = std::min(dst.width, src.width), height = std::min(dst.height, src.height);
    for (int y = 0; y < height; ++y) {
        byte *dstPixel = dst(0, y);
        const float *srcPixel = src(0, y);
        for (int x = 0; x < width; ++x) {
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
        }
    }
}

BLIT_SAME_TYPE_PART_IMPL(byte, 1)
BLIT_SAME_TYPE_PART_IMPL(byte, 3)
BLIT_SAME_TYPE_PART_IMPL(byte, 4)
BLIT_SAME_TYPE_PART_IMPL(float, 1)
BLIT_SAME_TYPE_PART_IMPL(float, 3)
BLIT_SAME_TYPE_PART_IMPL(float, 4)

void blit(const msdfgen::BitmapSection<byte, 1> &dst, const msdfgen::BitmapConstSection<float, 1> &src, int dx, int dy, int sx, int sy, int w, int h) {
    BOUND_SECTION();
    for (int y = 0; y < h; ++y) {
        byte *dstPixel = dst(dx, dy+y);
        const float *srcPixel = src(sx, sy+y);
        for (int x = 0; x < w; ++x)
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
    }
}

void blit(const msdfgen::BitmapSection<byte, 3> &dst, const msdfgen::BitmapConstSection<float, 3> &src, int dx, int dy, int sx, int sy, int w, int h) {
    BOUND_SECTION();
    for (int y = 0; y < h; ++y) {
        byte *dstPixel = dst(dx, dy+y);
        const float *srcPixel = src(sx, sy+y);
        for (int x = 0; x < w; ++x) {
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
        }
    }
}

void blit(const msdfgen::BitmapSection<byte, 4> &dst, const msdfgen::BitmapConstSection<float, 4> &src, int dx, int dy, int sx, int sy, int w, int h) {
    BOUND_SECTION();
    for (int y = 0; y < h; ++y) {
        byte *dstPixel = dst(dx, dy+y);
        const float *srcPixel = src(sx, sy+y);
        for (int x = 0; x < w; ++x) {
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
            *dstPixel++ = msdfgen::pixelFloatToByte(*srcPixel++);
        }
    }
}

}
