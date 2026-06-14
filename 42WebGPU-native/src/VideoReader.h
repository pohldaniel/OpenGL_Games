#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <inttypes.h>
}

struct VideoReaderState {
    // Public things for other parts of the program to read from
    int width, height;
    AVRational time_base;
    int frameRate = 24;
    // Private internal state
    AVFormatContext* av_format_ctx = nullptr;
    AVCodecContext* av_codec_ctx = nullptr;
    int video_stream_index = 0;
    AVFrame* av_frame = nullptr;
    AVPacket* av_packet = nullptr;
    SwsContext* sws_scaler_ctx = nullptr;
};

bool video_reader_open(VideoReaderState* state, const char* filename);
bool video_reader_read_frame(VideoReaderState* state, uint8_t* frame_buffer, int64_t* pts);
bool video_reader_seek_frame(VideoReaderState* state, int64_t ts);
void video_reader_close(VideoReaderState* state);