#pragma once

#include <vector>
#include <cstdint>

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "IVideoTextureBridge.h"

class SoftwareTextureBridge : public IVideoTextureBridge {
public:
    SoftwareTextureBridge(int width, int height, bool isPackedYuv);
    ~SoftwareTextureBridge() override;

    void updateTexture(AVFrame* frame) override;
    void release() override;
    void clearCache() override {}

private:
    void initWebGPUEntities();
    void handleRgbaPath(AVFrame* frame);
    void handleYuvPlanarPath(AVFrame* frame);

    int m_width;
    int m_height;
    bool m_isPackedYuv;


    SwsContext* m_swsContext = nullptr;
    AVFrame* m_frameRgba = nullptr;
    uint8_t* m_rgbaBufferInternal = nullptr;
    std::vector<uint8_t> m_cpuUploadBuffer; 
};