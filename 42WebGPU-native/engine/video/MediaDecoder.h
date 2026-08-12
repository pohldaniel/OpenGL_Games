#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <atomic>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

// Der Ringbuffer aus unserem Audiosystem, den wir mit Audiodaten befüllen
class AudioRingBuffer; 

class MediaDecoder {
public:
    MediaDecoder();
    ~MediaDecoder();

    bool open(const std::string& filename);
    void close();

    // Das stupide Update für die Gameloop.
    // Liest Pakete, konvertiert Audio live in den targetBuffer und 
    // gibt TRUE zurück, sobald ein neues Videoframe für WebGPU bereitsteht.
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
private:
    bool decodeVideoFrame();
    bool decodeAudioFrame(std::vector<uint8_t>& outPcmData);

    // FFmpeg Basis-Kontext
    AVFormatContext* m_formatContext = nullptr;
    AVPacket* m_packet = nullptr;

    // Video-Kontext & Rescaler
    AVCodecContext* m_videoCodecContext = nullptr;
    SwsContext* m_swsContext = nullptr;
    AVFrame* m_videoFrame = nullptr;
    AVFrame* m_frameRgba = nullptr;
    uint8_t* m_rgbaBufferInternal = nullptr;
    int m_videoStreamIndex = -1;

    // Audio-Kontext & Resampler
    AVCodecContext* m_audioCodecContext = nullptr;
    SwrContext* m_swrContext = nullptr;
    AVFrame* m_audioFrame = nullptr;
    int m_audioStreamIndex = -1;

    // Video-Metadaten & Zeit-Tracking
    int m_width = 0;
    int m_height = 0;
    double m_timePerFrame = 0.0;
    double m_accumulator = 0.0;
    double m_fps = 30.0;

    std::vector<uint8_t> m_currentFramePixels;

    bool m_isPaused = false;
    double m_currentTime = 0.0;
    double m_duration = 0.0;

    // Wird benötigt, um die Zeitbasis (Timebase) von FFmpeg korrekt umzurechnen
    double m_videoTimebase = 0.0;
};