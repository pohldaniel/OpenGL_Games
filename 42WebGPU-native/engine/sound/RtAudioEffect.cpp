#include "RtAudioEffect.h"

CacheLRU<std::string, RtAudioEffect::CacheEntry> RtAudioEffect::Cache;

int RtAudioEffect::RtAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* userData) {
    return static_cast<RtAudioEffect*>(userData)->audioCallback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status);
}

RtAudioEffect::RtAudioEffect() {
    m_ringBuffer.init(65536);
    Cache.Init(5u);
}

RtAudioEffect::~RtAudioEffect() {
    if(m_dac.isStreamOpen())
        m_dac.closeStream();
    Cache.Clear();
}

void RtAudioEffect::init() {
    if (m_dac.getDeviceCount() < 1) {
        return;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = m_dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;

    unsigned int bufferFrames = 256;
    m_dac.openStream(&parameters, nullptr, RTAUDIO_SINT16, 44100, &bufferFrames, &RtAudioCallback, this);
    return;
}

void RtAudioEffect::play(const std::string& file) {
    const CacheEntry& entry = Cache.Get(file);
    if (entry.m_samples.empty()) 
        return;

    bool channelFound = false;
    for (auto& channel : m_softwareMixer.m_channels) {
        int expected = 0;
        if (channel.status.compare_exchange_strong(expected, 1)) {
            channel.pcmData = &entry.m_samples;
            channel.progress = 0;
            channel.status.store(1);
            channel.pitchFactor = 1.0f;
            channelFound = true;
            break;
        }
    }

    if (!channelFound) {
        size_t maxProgress = 0;
        ActiveSound* oldestChannel = nullptr;
        for (auto& channel : m_softwareMixer.m_channels) {
            if (channel.progress > maxProgress) {
                maxProgress = channel.progress;
                oldestChannel = &channel;
            }
        }

        if (oldestChannel) {
            oldestChannel->status.store(0);
            oldestChannel->pcmData = &entry.m_samples;
            oldestChannel->progress = 0;
            oldestChannel->status.store(1);
        }
    }

    resume();
}

int RtAudioEffect::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status) {
    size_t samplesNeeded = nBufferFrames * 2;
    size_t bytesNeeded = samplesNeeded * sizeof(int16_t);
    int16_t* out = static_cast<int16_t*>(outputBuffer);
    size_t bytesRead = m_ringBuffer.read(reinterpret_cast<uint8_t*>(out), bytesNeeded);

    if (bytesRead < bytesNeeded) {
        std::fill_n(reinterpret_cast<uint8_t*>(out) + bytesRead, bytesNeeded - bytesRead, 0);
    }

    m_softwareMixer.mixAudio(out, static_cast<int32_t>(samplesNeeded));

    return 0;
}

void RtAudioEffect::resume() {
    if (!m_dac.isStreamRunning())
        m_dac.startStream();
}

RtAudioEffect::CacheEntry::CacheEntry(const std::string& file) {
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
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr);

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    std::vector<uint8_t> pcmData;

    while (av_read_frame(formatCtx, packet) >= 0) {
        if (packet->stream_index == streamIdx) {
            int send_ret = avcodec_send_packet(codecCtx, packet);
            if (send_ret >= 0) {
                int ret = 0;
                while (true) {
                    ret = avcodec_receive_frame(codecCtx, frame);

                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    }else if (ret < 0) {
                        break;
                    }

                    int outSamples = swr_get_out_samples(swr, frame->nb_samples);
                    if (outSamples <= 0) {
                        av_frame_unref(frame);
                        continue;
                    }

                    int maxFrameSize = outSamples * 2 * sizeof(int16_t);
                    size_t oldSize = pcmData.size();
                    pcmData.resize(oldSize + maxFrameSize);
                    uint8_t* buffer = pcmData.data() + oldSize;

                    int convertedSamples = swr_convert(swr, &buffer, outSamples, (const uint8_t**)frame->data, frame->nb_samples);
                    if (convertedSamples >= 0) {
                        int actualFrameSize = convertedSamples * 2 * sizeof(int16_t);
                        pcmData.resize(oldSize + actualFrameSize);
                    }else {
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

    m_samples.resize(pcmData.size() / sizeof(int16_t));
    std::memcpy(m_samples.data(), pcmData.data(), pcmData.size());
    m_totalSamples = m_samples.size();
}

RtAudioEffect::CacheEntry::~CacheEntry() {
    
}

RtAudioEffect::CacheEntry::CacheEntry(CacheEntry&& other) noexcept : m_samples(std::move(other.m_samples)), m_totalSamples(other.m_totalSamples){
    other.m_totalSamples = 0u;
}

RtAudioEffect::CacheEntry& RtAudioEffect::CacheEntry::operator=(CacheEntry&& other) noexcept {
    if (this != &other) {
        m_samples = std::move(other.m_samples);
        m_totalSamples = other.m_totalSamples;
        other.m_totalSamples = 0u;
    }
    return *this;
}