#pragma once

#include <vector>
#include <cstdint>

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "IVideoDecoder.h"

class RGBADecoder : public IVideoDecoder {

public:

    RGBADecoder();
    ~RGBADecoder() override;

    void updateTexture(AVFrame* frame) override;
    void init(int width, int height) override;

private:

    void initWebGPUEntities();
    
    SwsContext* m_swsContext = nullptr;
    AVFrame* m_frameRgba = nullptr;
    uint8_t* m_rgbaBufferInternal = nullptr;
    std::vector<uint8_t> m_cpuUploadBuffer; 
};