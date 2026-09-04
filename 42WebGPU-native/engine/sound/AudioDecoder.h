#pragma once
#include <string>
#include <vector>
#include <memory>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

#include "OpenALPlayer.h"

class AudioDecoder {

public:

    AudioDecoder();
    ~AudioDecoder();

    template <typename AudioImpl = OpenALPlayer>
    void open(const std::string& filename) {
        auto audio = std::make_unique<AudioImpl>();
        open(filename, std::move(audio));
    }
    void switchTrack(const std::string& filename);
    void close();
    void update();

private:

    void open(const std::string& filename, std::unique_ptr<IAudioOutput> audioOutput);
    void queryFirstFrame();
    bool decodeAudioFrame(std::vector<uint8_t>& outPcmData);

    AVFormatContext* m_formatContext = nullptr;
    AVCodecContext* m_codecContext = nullptr;
    SwrContext* m_swrContext = nullptr;
    int m_audioStreamIndex = -1;

    AVPacket* m_packet = nullptr;
    AVFrame* m_frame = nullptr;

    std::unique_ptr<IAudioOutput> m_audioOutput = nullptr;
};