#pragma once

#include <vector>
#include <cstdint>

extern "C" {
#include <libavutil/imgutils.h>
}

#include "IVideoTextureBridge.h"

class YUVTextureBridge : public IVideoTextureBridge {

public:

    YUVTextureBridge();
    ~YUVTextureBridge() override;

    void updateTexture(AVFrame* frame) override;
    void release() override;
    void clearCache() override {}
    void configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) override {}
    void init(int width, int height) override;

private:

    void initWebGPUEntities();

    int m_width;
    int m_height;
    std::vector<uint8_t> m_cpuUploadBuffer; 
};