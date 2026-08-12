#pragma once
#include <string>
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

class AudioDecoder {
public:
    AudioDecoder();
    ~AudioDecoder();

    bool open(const std::string& filename);
    void close();

    // Liest den nächsten Schwung PCM-Daten für die Gameloop
    // Gibt fals zurück, wenn das Ende der Datei erreicht ist
    bool decodeFrame(std::vector<uint8_t>& outBuffer);

    int getSampleRate() const { return 44100; } // Ziel-Format fixiert für Einfachheit
    int getChannels() const { return 2; }

private:
    AVFormatContext* formatContext = nullptr;
    AVCodecContext* codecContext = nullptr;
    SwrContext* swrContext = nullptr;
    int audioStreamIndex = -1;

    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;
};