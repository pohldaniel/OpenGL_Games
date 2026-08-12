#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

struct SoundEffect {
    std::vector<int16_t> pcmData;
    int sampleRate = 44100;
    int channels = 2;
};

class SFXCache {
public:
    static SoundEffect& get(const std::string& file) {
        static std::unordered_map<std::string, SoundEffect> cache;

        // Falls noch nicht geladen, jetzt decodieren
        if (cache.find(file) == cache.end()) {
            SoundEffect sfx;
            if (loadWavWithFFmpeg(file, sfx)) {
                cache[file] = std::move(sfx);
            }
            else {
                std::cerr << "Fehler beim Vorab-Laden des Sounds: " << file << std::endl;
                cache[file] = SoundEffect(); // Leeres Fallback
            }
        }
        return cache[file];
    }

private:

    static void exportPcmToWav(const std::string& filename, const SoundEffect& sfx) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) return;

        int32_t sampleRate = sfx.sampleRate; // 44100
        int16_t numChannels = sfx.channels;  // 2
        int32_t dataSize = static_cast<int32_t>(sfx.pcmData.size() * sizeof(int16_t));
        int32_t chunkSize = 36 + dataSize;
        int32_t byteRate = sampleRate * numChannels * sizeof(int16_t);
        int16_t blockAlign = numChannels * sizeof(int16_t);
        int16_t bitsPerSample = 16;

        // RIFF-Header schreiben
        file.write("RIFF", 4);
        file.write(reinterpret_cast<char*>(&chunkSize), 4);
        file.write("WAVE", 4);
        file.write("fmt ", 4);

        int32_t subChunk1Size = 16;
        int16_t audioFormat = 1; // PCM
        file.write(reinterpret_cast<char*>(&subChunk1Size), 4);
        file.write(reinterpret_cast<char*>(&audioFormat), 2);
        file.write(reinterpret_cast<char*>(&numChannels), 2);
        file.write(reinterpret_cast<char*>(&sampleRate), 4);
        file.write(reinterpret_cast<char*>(&byteRate), 4);
        file.write(reinterpret_cast<char*>(&blockAlign), 2);
        file.write(reinterpret_cast<char*>(&bitsPerSample), 2);

        file.write("data", 4);
        file.write(reinterpret_cast<char*>(&dataSize), 4);

        // Die rohen FFmpeg-Daten schreiben
        file.write(reinterpret_cast<const char*>(sfx.pcmData.data()), dataSize);
        file.close();
    }

    static bool loadWavWithFFmpeg(const std::string& filename, SoundEffect& outSfx) {
        AVFormatContext* formatCtx = nullptr;
        if (avformat_open_input(&formatCtx, filename.c_str(), nullptr, nullptr) < 0) return false;
        if (avformat_find_stream_info(formatCtx, nullptr) < 0) return false;

        int streamIdx = -1;
        for (unsigned int i = 0; i < formatCtx->nb_streams; i++) {
            if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                streamIdx = i;
                break;
            }
        }
        if (streamIdx == -1) return false;

        const AVCodec* codec = avcodec_find_decoder(formatCtx->streams[streamIdx]->codecpar->codec_id);
        AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codecCtx, formatCtx->streams[streamIdx]->codecpar);
        if (avcodec_open2(codecCtx, codec, nullptr) < 0) return false;

        // --- ENTSCHEIDENDE ÄNDERUNG: RESAMPLER AUF MONO FIXIEREN ---
        SwrContext* swr = swr_alloc();
        av_opt_set_chlayout(swr, "in_chlayout", &codecCtx->ch_layout, 0);
        av_opt_set_int(swr, "in_sample_rate", codecCtx->sample_rate, 0);
        av_opt_set_sample_fmt(swr, "in_sample_fmt", codecCtx->sample_fmt, 0);

        // Wir erzwingen hier knallhart 1 Kanal (Mono) für den Ausgang
        AVChannelLayout outLayout;
        av_channel_layout_default(&outLayout, 1); // 1 = MONO SINGLETON-SPUR
        av_opt_set_chlayout(swr, "out_chlayout", &outLayout, 0);
        av_opt_set_int(swr, "out_sample_rate", 44100, 0);
        av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
        swr_init(swr);

        AVPacket* packet = av_packet_alloc();
        AVFrame* frame = av_frame_alloc();

        while (av_read_frame(formatCtx, packet) >= 0) {
            if (packet->stream_index == streamIdx) {
                if (avcodec_send_packet(codecCtx, packet) >= 0) {
                    while (avcodec_receive_frame(codecCtx, frame) >= 0) {
                        int maxOutSamples = swr_get_out_samples(swr, frame->nb_samples);

                        // Da wir oben Mono (1 Kanal) erzwingen, fällt das "* inputChannels" weg.
                        // Jedes konvertierte Sample entspricht genau einem Element im Vektor.
                        std::vector<int16_t> tempBuf(maxOutSamples);
                        uint8_t* outData = reinterpret_cast<uint8_t*>(tempBuf.data());

                        int convertedPerChannel = swr_convert(swr, &outData, maxOutSamples, (const uint8_t**)frame->data, frame->nb_samples);
                        if (convertedPerChannel > 0) {
                            outSfx.pcmData.insert(outSfx.pcmData.end(), tempBuf.begin(), tempBuf.begin() + convertedPerChannel);
                        }
                    }
                }
            }
            av_packet_unref(packet);
        }

        av_frame_free(&frame);
        av_packet_free(&packet);
        swr_free(&swr);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&formatCtx);

        // Wichtig für deine Player: Explizit als Mono markieren
        outSfx.channels = 1;
        outSfx.sampleRate = 44100;
        exportPcmToWav("tmp.wav", outSfx);
        return !outSfx.pcmData.empty();
    }

    
};