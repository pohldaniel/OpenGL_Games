#pragma once
#include <vector>
#include <string>
#include <memory>

#include "../sound/IAudioOutput.h"
#include "../sound/OpenALPlayer.h"
#include "../sound/RtAudioPlayer.h"

#include "IVideoDecoder.h"
#include "VulkanDecoder.h"
#include "D3D12Decoder.h"
#include "D3D11Decoder.h"
#include "YUVDecoder.h"
#include "RGBADecoder.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

class VideoDecoder {

public:
   
    VideoDecoder();
    ~VideoDecoder();

    template <typename VideoImpl = VulkanDecoder, typename AudioImpl = OpenALPlayer>
    void open(const std::string& filename) {
        auto decoder = std::make_unique<VideoImpl>();
        auto audio = std::make_unique<AudioImpl>();
        open(filename, std::move(decoder), std::move(audio));
    }

    template <class VideoImpl = IVideoDecoder>
    VideoImpl* getDecoder() {
        return static_cast<VideoImpl*>(m_decoder.get());
    }

    template <class AudioImpl = IAudioOutput>
    AudioImpl* getAudioOutput() {
        return static_cast<AudioImpl*>(m_audioOutput.get());
    }

    void close();
    void update(float deltaTime);
    void OnPostDraw();

    double getFps() const { return m_fps; }
    void togglePause() { m_isPaused = !m_isPaused; }
    bool isPaused() const { return m_isPaused; }
    float getCurrentTime() const { return m_currentTime; }
    float getDuration() const { return m_duration; }
    void seekTo(float seconds);
    void setVolume(float volume);
    float getVolume();
    void queryFirstFrame();

private:

    void open(const std::string& filename, std::unique_ptr<IVideoDecoder> videoDecoder, std::unique_ptr<IAudioOutput> audioOutput);

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

    std::unique_ptr<IAudioOutput> m_audioOutput = nullptr;
    std::unique_ptr<IVideoDecoder> m_decoder = nullptr;
};