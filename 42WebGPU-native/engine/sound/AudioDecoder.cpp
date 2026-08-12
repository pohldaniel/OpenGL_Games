#include <iostream>
#include "AudioDecoder.h"

AudioDecoder::AudioDecoder() {
    packet = av_packet_alloc();
    frame = av_frame_alloc();
}

AudioDecoder::~AudioDecoder() {
    close();
    av_packet_free(&packet);
    av_frame_free(&frame);
}

bool AudioDecoder::open(const std::string& filename) {
    if (avformat_open_input(&formatContext, filename.c_str(), nullptr, nullptr) < 0) return false;
    if (avformat_find_stream_info(formatContext, nullptr) < 0) return false;

    // Audio-Stream finden
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }
    if (audioStreamIndex == -1) return false;

    // Decoder finden und öffnen
    const AVCodec* codec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);
    if (!codec) return false;

    codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, formatContext->streams[audioStreamIndex]->codecpar);
    if (avcodec_open2(codecContext, codec, nullptr) < 0) return false;

    // Resampler einrichten (Konvertiert MP3-Format stur in Standard Stereo i16 44.1kHz)
    swrContext = swr_alloc();
    av_opt_set_chlayout(swrContext, "in_chlayout", &codecContext->ch_layout, 0);
    av_opt_set_int(swrContext, "in_sample_rate", codecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", codecContext->sample_fmt, 0);

    AVChannelLayout outLayout;
    av_channel_layout_default(&outLayout, 2);
    av_opt_set_chlayout(swrContext, "out_chlayout", &outLayout, 0);
    av_opt_set_int(swrContext, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

    swr_init(swrContext);
    return true;
}

bool AudioDecoder::decodeFrame(std::vector<uint8_t>& outBuffer) {
    outBuffer.clear();

    while (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == audioStreamIndex) {
            int response = avcodec_send_packet(codecContext, packet);
            if (response < 0) {
                av_packet_unref(packet);
                return false;
            }

            while (response >= 0) {
                response = avcodec_receive_frame(codecContext, frame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                }
                else if (response < 0) {
                    av_packet_unref(packet);
                    return false;
                }

                // Buffer-Größe für konvertierte Daten berechnen
                int maxOutSamples = swr_get_out_samples(swrContext, frame->nb_samples);
                std::vector<uint8_t> tempBuf(maxOutSamples * 2 * sizeof(int16_t)); // 2 Kanäle, 16-Bit

                uint8_t* outData[1] = { tempBuf.data() };
                int convertedSamples = swr_convert(swrContext, outData, maxOutSamples, (const uint8_t**)frame->data, frame->nb_samples);

                if (convertedSamples > 0) {
                    size_t actualSize = convertedSamples * 2 * sizeof(int16_t);
                    outBuffer.insert(outBuffer.end(), tempBuf.begin(), tempBuf.begin() + actualSize);
                    av_packet_unref(packet);
                    return true; // Frame erfolgreich decodiert!
                }
            }
        }
        av_packet_unref(packet);
    }
    return false; // EOF (End of file)
}

void AudioDecoder::close() {
    if (swrContext) swr_free(&swrContext);
    if (codecContext) avcodec_free_context(&codecContext);
    if (formatContext) avformat_close_input(&formatContext);
}