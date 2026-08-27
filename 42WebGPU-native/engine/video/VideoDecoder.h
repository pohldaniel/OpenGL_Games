#pragma once
#include <vector>
#include <string>
#include <memory>

#include "../sound/IAudioOutput.h"
#include "IVideoDecoder.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

enum HardwareAcceleration {
    HW_D3D12,
    HW_D3D11,
    HW_VULKAN,
    SW_RGBA,
    SW_YUV,
    HW_NONE
};

class VideoDecoder {

public:
   
    VideoDecoder();
    ~VideoDecoder();

    void open(const std::string& filename);
    void close();
    void update(float deltaTime);
    void OnPostDraw();

    double getFps() const { return m_fps; }
    void togglePause() { m_isPaused = !m_isPaused; }
    bool isPaused() const { return m_isPaused; }
    float getCurrentTime() const { return m_currentTime; }
    float getDuration() const { return m_duration; }
    void seekTo(float seconds);

    const WGPUBindGroup& getBindGroup();
    const WGPUBuffer& getBuffer();
    const WGPUTextureView& getTextureViewY();
    const WGPUTextureView& getTextureViewUV();

    void setBindGroup(const WGPUBindGroup& bindgroup);
    void setBuffer(const WGPUBuffer& buffer);
    void setBindGroupLayout(const WGPUBindGroupLayout& bindGroupLayout);
    void queryFirstFrame();

    std::unique_ptr<IAudioOutput> m_audioOutput;
    std::unique_ptr<IVideoDecoder> m_decoder = nullptr;
    HardwareAcceleration m_hardwareAcceleration = HW_NONE;

private:

    bool decodeVideoFrame();
    bool decodeAudioFrame(std::vector<uint8_t>& outPcmData);
   
    AVBufferRef* m_hwDeviceContext = nullptr;
    AVDictionary* options = nullptr;

    AVPacket* m_packet = nullptr;
    AVFrame* m_videoFrame = nullptr;
    AVFrame* m_audioFrame = nullptr;

    AVFormatContext* m_formatContext = nullptr;
    AVCodecContext* m_audioCodecContext = nullptr;
    AVCodecContext* m_videoCodecContext = nullptr;
    SwrContext* m_swrContext = nullptr;

    bool m_isPaused = false;
    float m_accumulator = 0.0f;
    float m_timePerFrame = 0.0f;
    float m_currentTime = 0.0f;
    float m_videoTimebase = 0.0f;
    float m_fps = 30.0f;
    float m_duration = 0.0f;
   
    int m_videoStreamIndex = -1;
    int m_audioStreamIndex = -1;
    bool m_wantSeek = false;
};