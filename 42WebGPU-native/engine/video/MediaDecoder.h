#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <atomic>
#include <webgpu.h>
#include <d3d11.h>
#include <d3d12.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/log.h>
#include <libavutil/hwcontext_d3d11va.h>
#include <libavutil/hwcontext_d3d12va.h>

#include <libavutil/hwcontext_vulkan.h >
}

typedef struct SharedTextureMemoryD3D12ResourceDescriptor {
    WGPUChainedStruct chain;
    ID3D12Resource* resource;
} SharedTextureMemoryD3D12ResourceDescriptor;

typedef struct SharedTextureMemoryD3D11Texture2DDescriptor {
    WGPUChainedStruct chain;
    ID3D11Texture2D* texture;
} SharedTextureMemoryD3D11Texture2DDescriptor;

class AudioRingBuffer; 

enum HardwareAcceleration {
    HW_D3D12,
    HW_D3D11,
    HW_VULKAN,
    HW_NONE
};

class MediaDecoder {

public:
    WGPUTexture m_yTexture = nullptr;
    WGPUTexture m_uvTexture = nullptr;

    WGPUTexture m_videoTexture = nullptr;

    WGPUBindGroup m_yBindgroup = nullptr;
    WGPUBindGroup m_uvBindgroup = nullptr;

    WGPUSharedTextureMemory m_sharedTextureMemory = nullptr;
    WGPUTextureView m_textureViewY = nullptr;
    WGPUTextureView m_textureViewUV = nullptr;

    MediaDecoder();
    ~MediaDecoder();

    bool open(const std::string& filename);
    void close();
    bool update(double deltaTime, std::vector<uint8_t>& outRgbaBuffer, AudioRingBuffer& targetBuffer);
    bool updateOpenAL(double deltaTime, std::vector<uint8_t>& outRgbaBuffer, std::vector<uint8_t>& outPcmAudio);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    double getFps() const { return m_fps; }
    void togglePause() { m_isPaused = !m_isPaused; }
    bool isPaused() const { return m_isPaused; }
    double getCurrentTime() const { return m_currentTime; }
    double getDuration() const { return m_duration; }
    void seekTo(double seconds);
    bool m_isPackedYuv = false;
    HardwareAcceleration m_hardwareAcceleration = HW_NONE;

private:

    AVBufferRef* m_hwDeviceContext = nullptr;
    AVFrame* m_swFrame = nullptr;
    AVDictionary* options = nullptr;

    std::function<void(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor)> OnDraw = NULL;
    bool decodeVideoFrame();
    bool decodeAudioFrame(std::vector<uint8_t>& outPcmData);

    AVFormatContext* m_formatContext = nullptr;
    AVPacket* m_packet = nullptr;

    AVCodecContext* m_videoCodecContext = nullptr;
    SwsContext* m_swsContext = nullptr;
    AVFrame* m_videoFrame = nullptr;
    AVFrame* m_frameRgba = nullptr;
    uint8_t* m_rgbaBufferInternal = nullptr;
    int m_videoStreamIndex = -1;

    AVCodecContext* m_audioCodecContext = nullptr;
    SwrContext* m_swrContext = nullptr;
    AVFrame* m_audioFrame = nullptr;
    int m_audioStreamIndex = -1;

    int m_width = 0;
    int m_height = 0;
    double m_timePerFrame = 0.0;
    double m_accumulator = 0.0;
    double m_fps = 30.0;

    std::vector<uint8_t> m_currentFramePixels;

    bool m_isPaused = false;
    double m_currentTime = 0.0;
    double m_duration = 0.0;
   
    double m_videoTimebase = 0.0;

    ID3D11Device* m_d3d11_device = nullptr;
    ID3D11DeviceContext* m_d3d11_context = nullptr;
    ID3D11Texture2D* m_single_texture = nullptr;
};