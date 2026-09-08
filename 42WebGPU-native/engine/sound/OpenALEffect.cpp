#include "OpenALEffect.h"

CacheLRU<std::string, OpenALEffect::CacheEntry> OpenALEffect::Cache;

OpenALEffect::OpenALEffect() : m_next(0u){
    Cache.Init(5u);
}

OpenALEffect::~OpenALEffect() {
    Cache.Clear();
}

void OpenALEffect::init() {
    int maxChannels = 32;
    m_sources.resize(maxChannels);
    alGenSources(maxChannels, m_sources.data());
}

void OpenALEffect::play(const std::string& file) {
    const CacheEntry& entry = Cache.Get(file);

    ALuint sourceToUse = m_sources[m_next];
    for (ALuint source : m_sources) {
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            sourceToUse = source;
            break;
        }
    }

    alSourceStop(sourceToUse);
    alSourcei(sourceToUse, AL_BUFFER, entry.buffer);
    alSourcePlay(sourceToUse);
    m_next = (m_next + 1) % m_sources.size();
}

void OpenALEffect::setVolume(float volume) {
    for (ALuint source : m_sources) {
        alSourcef(source, AL_GAIN, std::clamp(volume, 0.0f, 1.0f));
    }
}

OpenALEffect::CacheEntry::CacheEntry(const std::string& file) : buffer(0u) {
    AVFormatContext* formatCtx = nullptr;
    avformat_open_input(&formatCtx, file.c_str(), nullptr, nullptr);
    avformat_find_stream_info(formatCtx, nullptr);

    int streamIdx = -1;
    for (unsigned int i = 0; i < formatCtx->nb_streams; i++) {
        if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            streamIdx = i;
            break;
        }
    }

    const AVCodec* codec = avcodec_find_decoder(formatCtx->streams[streamIdx]->codecpar->codec_id);
    AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecCtx, formatCtx->streams[streamIdx]->codecpar);
    avcodec_open2(codecCtx, codec, nullptr);

    SwrContext* swr = swr_alloc();
    av_opt_set_chlayout(swr, "in_chlayout", &codecCtx->ch_layout, 0);
    av_opt_set_int(swr, "in_sample_rate", codecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", codecCtx->sample_fmt, 0);

    AVChannelLayout outLayout;
    av_channel_layout_default(&outLayout, 2);
    av_opt_set_chlayout(swr, "out_chlayout", &outLayout, 0);
    av_opt_set_int(swr, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    swr_init(swr);

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    std::vector<float> pcmData;

    while (av_read_frame(formatCtx, packet) >= 0) {
        if (packet->stream_index == streamIdx) {
            int send_ret = avcodec_send_packet(codecCtx, packet);
            if (send_ret >= 0) {
                int ret = 0;
                while (true) {
                    ret = avcodec_receive_frame(codecCtx, frame);

                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0) {
                        break;
                    }

                    int outSamples = swr_get_out_samples(swr, frame->nb_samples);
                    if (outSamples <= 0) {
                        av_frame_unref(frame);
                        continue;
                    }

                    int maxSamplesNeeded = outSamples * 2;
                    size_t oldSize = pcmData.size();
                    pcmData.resize(oldSize + maxSamplesNeeded);
                    uint8_t* buffer = reinterpret_cast<uint8_t*>(pcmData.data() + oldSize);

                    int convertedSamples = swr_convert(swr, &buffer, outSamples, (const uint8_t**)frame->data, frame->nb_samples);
                    if (convertedSamples >= 0) {
                        int actualSamplesConverted = convertedSamples * 2;
                        pcmData.resize(oldSize + actualSamplesConverted);
                    }
                    else {
                        pcmData.resize(oldSize);
                    }
                    av_frame_unref(frame);
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
    alGenBuffers(1, &buffer);


    ALsizei bufferSizeInBytes = static_cast<ALsizei>(pcmData.size() * sizeof(float));
    alBufferData(buffer, AL_FORMAT_STEREO_FLOAT32, pcmData.data(), bufferSizeInBytes, 44100);
}

OpenALEffect::CacheEntry::~CacheEntry() {
    if (buffer != 0) {
        alDeleteBuffers(1, &buffer);
    }
}

OpenALEffect::CacheEntry::CacheEntry(OpenALEffect::CacheEntry&& other) noexcept : buffer(other.buffer) {
    other.buffer = 0;
}

OpenALEffect::CacheEntry& OpenALEffect::CacheEntry::operator=(CacheEntry&& other) noexcept {
    if (this != &other) {
        if (buffer != 0) {
            alDeleteBuffers(1, &buffer);
        }
        buffer = other.buffer;
        other.buffer = 0;
    }
    return *this;
}