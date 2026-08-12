#pragma once
#include <string>
#include <vector>
#include <cstdint>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class VideoDecoder {
public:
    VideoDecoder();
    ~VideoDecoder();

    bool open(const std::string& filename);
    void close();

    // Nimmt die deltaTime der Gameloop entgegen.
    // Gibt TRUE zurück, wenn ein brandneues Bild decodiert wurde (Textur muss aktualisiert werden).
    // Gibt FALSE zurück, wenn das Video-Frame noch aktuell ist (keine Arbeit für die GPU).
    bool update(double deltaTime, std::vector<uint8_t>& outRgbaBuffer);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    bool decodeNextFrame(); // Interne Hilfsfunktion

    AVFormatContext* m_formatContext = nullptr;
    AVCodecContext* m_codecContext = nullptr;
    SwsContext* m_swsContext = nullptr;
    int m_videoStreamIndex = -1;

    AVPacket* m_packet = nullptr;
    AVFrame* m_frame = nullptr;
    AVFrame* m_frameRgba = nullptr;
    uint8_t* m_rgbaBufferInternal = nullptr;

    int m_width = 0;
    int m_height = 0;

    // --- ZEIT-TRACKING NUN INTERN ---
    double m_timePerFrame = 0.0;
    double m_accumulator = 0.0;
    std::vector<uint8_t> m_currentFramePixels; // Cache für das aktuelle Bild
};