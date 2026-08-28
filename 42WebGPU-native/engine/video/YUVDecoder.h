#pragma once

#include <vector>
#include <cstdint>

extern "C" {
#include <libavutil/imgutils.h>
}

#include "IVideoDecoder.h"

class YUVDecoder : public IVideoDecoder {

public:

    YUVDecoder();
    ~YUVDecoder() override;

    void updateTexture(AVFrame* frame) override;
    void init(int width, int height) override;
    void setBindGroup(const WGPUBindGroup& bindgroup);

private:

    void initWebGPUEntities();

    std::vector<uint8_t> m_cpuUploadBuffer; 
};