#pragma once

#include <vector>
#include <cstdint>

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "IVideoTextureBridge.h"

class RGBATextureBridge : public IVideoTextureBridge {

public:

    RGBATextureBridge();
    ~RGBATextureBridge() override;

    void updateTexture(AVFrame* frame) override;
    void release() override;
    void clearCache() override {}
    void configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) override {}
    void init(int width, int height) override;

private:
    void initWebGPUEntities();
    
    int m_width;
    int m_height;

    SwsContext* m_swsContext = nullptr;
    AVFrame* m_frameRgba = nullptr;
    uint8_t* m_rgbaBufferInternal = nullptr;
    std::vector<uint8_t> m_cpuUploadBuffer; 
};