#pragma once

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
}

#include <vector>

class AudioDecoderNew {

public:
    AudioDecoderNew();
    ~AudioDecoderNew();

	bool decodeAudioFrame(AVCodecContext* audioCtx, SwrContext* swrCtx, AVPacket* packet, std::vector<uint8_t>& outPcmData);

private:

    AVFrame* m_audioFrame;
};