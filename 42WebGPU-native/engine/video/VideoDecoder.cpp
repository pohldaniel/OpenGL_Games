#include <algorithm>
#include <thread>

#include "VideoDecoder.h"
#include "VulkanTextureBridge.h"
#include "D3D12TextureBridge.h"
#include "D3D11TextureBridge.h"
#include "YUVTextureBridge.h"
#include "RGBATextureBridge.h"

VideoDecoder::VideoDecoder() : m_audioOutput(nullptr){
    m_packet = av_packet_alloc();
    m_videoFrame = av_frame_alloc();
    m_audioFrame = av_frame_alloc();  
    //av_log_set_level(AV_LOG_DEBUG);
    av_log_set_level(AV_LOG_ERROR);
}

VideoDecoder::~VideoDecoder() {
    close();
    av_packet_free(&m_packet);
    av_frame_free(&m_videoFrame);
    av_frame_free(&m_audioFrame);
}

static std::unique_ptr<IVideoTextureBridge> create(HardwareAcceleration type) {
    switch (type) {
    case HW_VULKAN:
        return std::make_unique<VulkanTextureBridge>();
    case HW_D3D12:
        return std::make_unique<D3D12TextureBridge>();
    case HW_D3D11:
        return std::make_unique<D3D11TextureBridge>();
    case SW_YUV:
        return std::make_unique<YUVTextureBridge>();
    case SW_RGBA:
        return std::make_unique<RGBATextureBridge>();
    default:
        return nullptr;
    }
}

void VideoDecoder::open(const std::string& filename) {
    if (avformat_open_input(&m_formatContext, filename.c_str(), nullptr, nullptr) < 0) return;
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) return;

    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && m_videoStreamIndex == -1) {
            m_videoStreamIndex = i;
        } else if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStreamIndex == -1) {
            m_audioStreamIndex = i;
        }
    }

    if (m_videoStreamIndex == -1) return;

    const AVCodec* videoCodec = avcodec_find_decoder(m_formatContext->streams[m_videoStreamIndex]->codecpar->codec_id);
    m_videoCodecContext = avcodec_alloc_context3(videoCodec);
    avcodec_parameters_to_context(m_videoCodecContext, m_formatContext->streams[m_videoStreamIndex]->codecpar);

    int numCores = std::min( std::thread::hardware_concurrency(), 16u);
    m_videoCodecContext->thread_count = numCores;
    m_videoCodecContext->thread_type = FF_THREAD_FRAME;

    AVDictionary* options = nullptr;
    if (m_hardwareAcceleration == HW_VULKAN) {
        av_dict_set(&options, "extensions", "VK_KHR_external_memory_win32", 0);
    }
    AVHWDeviceType avType = m_hardwareAcceleration == HW_VULKAN ? AV_HWDEVICE_TYPE_VULKAN : 
                            m_hardwareAcceleration == HW_D3D12 ? AV_HWDEVICE_TYPE_D3D12VA :
                            m_hardwareAcceleration == HW_D3D11 ? AV_HWDEVICE_TYPE_D3D11VA : 
                            AV_HWDEVICE_TYPE_NONE;

    int err = av_hwdevice_ctx_create(&m_hwDeviceContext, avType, nullptr, options, 0);
    if (options) av_dict_free(&options);

    m_textureBridge = create(m_hardwareAcceleration);
    m_textureBridge->configureContext(m_videoCodecContext, m_hwDeviceContext);
    avcodec_open2(m_videoCodecContext, videoCodec, nullptr);
    m_textureBridge->init(m_videoCodecContext->width, m_videoCodecContext->height);

    AVRational streamFps = m_formatContext->streams[m_videoStreamIndex]->r_frame_rate;
    m_fps = (streamFps.den > 0) ? av_q2d(streamFps) : 30.0;
    m_timePerFrame = 1.0 / m_fps;

    if (m_audioStreamIndex != -1) {
        const AVCodec* audioCodec = avcodec_find_decoder(m_formatContext->streams[m_audioStreamIndex]->codecpar->codec_id);
        if (audioCodec) {
            m_audioCodecContext = avcodec_alloc_context3(audioCodec);
            avcodec_parameters_to_context(m_audioCodecContext, m_formatContext->streams[m_audioStreamIndex]->codecpar);
            if (avcodec_open2(m_audioCodecContext, audioCodec, nullptr) >= 0) {
                
                // Resampler auf Standard Stereo i16 44.1kHz fixieren
                m_swrContext = swr_alloc();
                av_opt_set_chlayout(m_swrContext, "in_chlayout", &m_audioCodecContext->ch_layout, 0);
                av_opt_set_int(m_swrContext, "in_sample_rate", m_audioCodecContext->sample_rate, 0);
                av_opt_set_sample_fmt(m_swrContext, "in_sample_fmt", m_audioCodecContext->sample_fmt, 0);

                AVChannelLayout outLayout;
                av_channel_layout_default(&outLayout, 2); // Stereo
                av_opt_set_chlayout(m_swrContext, "out_chlayout", &outLayout, 0);
                av_opt_set_int(m_swrContext, "out_sample_rate", 44100, 0);
                av_opt_set_sample_fmt(m_swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
                swr_init(m_swrContext);
            }
        }
    }

    decodeVideoFrame();

    if (m_formatContext->duration != AV_NOPTS_VALUE) {
        m_duration = static_cast<double>(m_formatContext->duration) / AV_TIME_BASE;
    }

    AVStream* videoStream = m_formatContext->streams[m_videoStreamIndex];
    m_videoTimebase = av_q2d(videoStream->time_base);
    m_isPaused = false;
    m_currentTime = 0.0f;
}

void VideoDecoder::update(float deltaTime) {
    if (m_isPaused) {
        m_audioOutput->pause();
        return;
    }
   
    m_accumulator += deltaTime;
    bool newFrameUploaded = false;

    while(m_accumulator >= m_timePerFrame && !newFrameUploaded) {
        if(av_read_frame(m_formatContext, m_packet) < 0) {
            av_seek_frame(m_formatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
            if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
            if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);
            continue;
        }

        if(m_packet->stream_index == m_videoStreamIndex) {
                
            if(decodeVideoFrame()) {
                newFrameUploaded = true;
                m_accumulator -= m_timePerFrame;
            }
        }else if(m_packet->stream_index == m_audioStreamIndex && m_swrContext) {
            std::vector<uint8_t> pcmData;
            if(decodeAudioFrame(pcmData)) {   
                 m_audioOutput->enqueueData(pcmData);              
            }
        }
        av_packet_unref(m_packet);
    }

    if (m_accumulator > m_timePerFrame * 2.0) {
        m_accumulator = m_timePerFrame;
    }
}

bool VideoDecoder::decodeVideoFrame() {
    if(avcodec_send_packet(m_videoCodecContext, m_packet) < 0)
        return false;

    if(avcodec_receive_frame(m_videoCodecContext, m_videoFrame) < 0)
        return false;

    if(m_videoFrame->pts != AV_NOPTS_VALUE) {
        m_currentTime = m_videoFrame->pts * m_videoTimebase;
    }else if(m_videoFrame->pkt_dts != AV_NOPTS_VALUE) {
        m_currentTime = m_videoFrame->pkt_dts * m_videoTimebase;
    }
    
    m_textureBridge->updateTexture(m_videoFrame);
    av_frame_unref(m_videoFrame);
    return true;    
}

bool VideoDecoder::decodeAudioFrame(std::vector<uint8_t>& outPcmData) {
    if (avcodec_send_packet(m_audioCodecContext, m_packet) < 0)
        return false;

    if (avcodec_receive_frame(m_audioCodecContext, m_audioFrame) < 0)
        return false;

    int outSamples = swr_get_out_samples(m_swrContext, m_audioFrame->nb_samples);

    outPcmData.resize(outSamples * 2 * sizeof(int16_t));
    uint8_t* outputBuffer = outPcmData.data();

    int translated = swr_convert(m_swrContext, &outputBuffer, outSamples,
        (const uint8_t**)m_audioFrame->data, m_audioFrame->nb_samples);

    if (translated < 0)
        return false;

    outPcmData.resize(translated * 2 * sizeof(int16_t));

    av_frame_unref(m_audioFrame);
    return true;
}

void VideoDecoder::close() {
    if (m_swrContext) { swr_free(&m_swrContext); m_swrContext = nullptr; }
    if (m_videoCodecContext) { avcodec_free_context(&m_videoCodecContext); m_videoCodecContext = nullptr; }
    if (m_audioCodecContext) { avcodec_free_context(&m_audioCodecContext); m_audioCodecContext = nullptr; }
    if (m_formatContext) { avformat_close_input(&m_formatContext); m_formatContext = nullptr; }
}

void VideoDecoder::seekTo(float seconds) {
    if (!m_formatContext || !m_videoCodecContext) return;

    m_currentTime = std::clamp(seconds, 0.0f, m_duration);
    int64_t targetVideoPts = static_cast<int64_t>(m_currentTime / m_videoTimebase);

    int seek_res = avformat_seek_file(
        m_formatContext,
        m_videoStreamIndex,
        INT64_MIN,
        targetVideoPts,
        targetVideoPts,
        AVSEEK_FLAG_BACKWARD
    );

    if (seek_res < 0) {
        av_seek_frame(m_formatContext, m_videoStreamIndex, targetVideoPts, AVSEEK_FLAG_BACKWARD);
    }

   
    avcodec_flush_buffers(m_videoCodecContext);
    if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);

    if (m_textureBridge) m_textureBridge->clearCache();

    m_videoCodecContext->skip_frame = AVDISCARD_NONREF;

    bool seekDone = false;
    AVFrame* finalFrame = av_frame_alloc();

    while (av_read_frame(m_formatContext, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {

            if (avcodec_send_packet(m_videoCodecContext, m_packet) >= 0) {
                while (avcodec_receive_frame(m_videoCodecContext, m_videoFrame) >= 0) {

                    int64_t currentPts = (m_videoFrame->pts != AV_NOPTS_VALUE)
                        ? m_videoFrame->pts
                        : m_videoFrame->pkt_dts;

                    if (currentPts >= targetVideoPts) {
                        m_currentTime = currentPts * m_videoTimebase;
                        av_frame_move_ref(finalFrame, m_videoFrame);
                        seekDone = true;
                        break;
                    }

                    av_frame_unref(m_videoFrame);
                }
            }
        }
        av_packet_unref(m_packet);
        if (seekDone) break;
    }

    m_videoCodecContext->skip_frame = AVDISCARD_DEFAULT;
    if (!seekDone && m_videoFrame->format != -1) {
        av_frame_move_ref(finalFrame, m_videoFrame);
        seekDone = true;
    }

    if (seekDone && finalFrame->format != -1) {
        m_textureBridge->updateTexture(finalFrame);
    }

    av_frame_free(&finalFrame);
    m_accumulator = 0.0;
}