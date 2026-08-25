#include <iostream>
#include <algorithm>
#include <thread>

#include <WebGPU/WgpContext.h>

#include "MediaDecoder.h"
#include "../sound/AudioRingBuffer.h"
#include <dxgi.h>
#include <dxgi1_2.h>

MediaDecoder::MediaDecoder() : m_audioOutput(nullptr){
    m_packet = av_packet_alloc();
    m_videoFrame = av_frame_alloc();
    m_frameRgba = av_frame_alloc();
    m_audioFrame = av_frame_alloc();  
    //av_log_set_level(AV_LOG_DEBUG);
    av_log_set_level(AV_LOG_ERROR);
}

MediaDecoder::~MediaDecoder() {
    close();
    av_packet_free(&m_packet);
    av_frame_free(&m_videoFrame);
    av_frame_free(&m_frameRgba);
    av_frame_free(&m_audioFrame);
}

static enum AVPixelFormat get_hw_format_d3d12(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {  
    const enum AVPixelFormat target = AV_PIX_FMT_D3D12;
    for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == target) {          
            return target;
        }
    }
    return AV_PIX_FMT_NONE;
}

static enum AVPixelFormat get_hw_format_d3d11(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
    const enum AVPixelFormat target = AV_PIX_FMT_D3D11;
    for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == target) {
            return target;
        }
    }
    return AV_PIX_FMT_NONE;
}

static enum AVPixelFormat get_hw_format_vulkan(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
    for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == AV_PIX_FMT_VULKAN) {

            if (!ctx->hw_frames_ctx) {
                int err = avcodec_get_hw_frames_parameters(ctx, ctx->hw_device_ctx, AV_PIX_FMT_VULKAN, &ctx->hw_frames_ctx);
                if (err < 0) {
                    std::cerr << "[Vulkan-HW] Parameter-Abruf fehlgeschlagen." << std::endl;
                    break;
                }

                AVHWFramesContext* frames_ctx = (AVHWFramesContext*)ctx->hw_frames_ctx->data;
                AVVulkanFramesContext* vk_frames_ctx = (AVVulkanFramesContext*)frames_ctx->hwctx;

                vk_frames_ctx->img_flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
                vk_frames_ctx->usage = static_cast<VkImageUsageFlagBits>(vk_frames_ctx->usage | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
                vk_frames_ctx->tiling = VK_IMAGE_TILING_OPTIMAL;

                static VkExternalMemoryImageCreateInfoKHR win32_ext_image_info = {};
                win32_ext_image_info.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR;
                win32_ext_image_info.pNext = vk_frames_ctx->create_pnext;
                win32_ext_image_info.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

                vk_frames_ctx->create_pnext = &win32_ext_image_info;


                 static VkExportMemoryWin32HandleInfoKHR win32_handle_info = {};
                win32_handle_info.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
                win32_handle_info.dwAccess = DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE;
                win32_handle_info.pNext = nullptr;

                static VkExportMemoryAllocateInfoKHR win32_export_alloc_info = {};
                win32_export_alloc_info.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_KHR;
                win32_export_alloc_info.pNext = &win32_handle_info;
                win32_export_alloc_info.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
            
                for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) {
                    win32_export_alloc_info.pNext = (vk_frames_ctx->alloc_pnext[i]) ? vk_frames_ctx->alloc_pnext[i] : &win32_handle_info;
                    vk_frames_ctx->alloc_pnext[i] = &win32_export_alloc_info;
                }

                err = av_hwframe_ctx_init(ctx->hw_frames_ctx);
                if (err < 0) {
                    std::cerr << "[Vulkan-HW] av_hwframe_ctx_init fehlgeschlagen: " << err << std::endl;
                    av_buffer_unref(&ctx->hw_frames_ctx); // Bereinigen bei Fehler
                    break;
                }
              
                std::cout << "[Vulkan-HW] Hardware-Frames erfolgreich initialisiert." << std::endl;
                return AV_PIX_FMT_VULKAN;
            }    
        }
    }
    std::cout << "[Vulkan-HW] Nutze Software-Fallback: " << av_get_pix_fmt_name(ctx->sw_pix_fmt) << std::endl;
    return ctx->sw_pix_fmt;
}

bool MediaDecoder::open(const std::string& filename) {
    if (avformat_open_input(&m_formatContext, filename.c_str(), nullptr, nullptr) < 0) return false;
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) return false;

    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && m_videoStreamIndex == -1) {
            m_videoStreamIndex = i;
        } else if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStreamIndex == -1) {
            m_audioStreamIndex = i;
        }
    }

    if (m_videoStreamIndex == -1) return false;

    const AVCodec* videoCodec = avcodec_find_decoder(m_formatContext->streams[m_videoStreamIndex]->codecpar->codec_id);
    m_videoCodecContext = avcodec_alloc_context3(videoCodec);
    avcodec_parameters_to_context(m_videoCodecContext, m_formatContext->streams[m_videoStreamIndex]->codecpar);

    int numCores = std::min( std::thread::hardware_concurrency(), 16u);
    m_videoCodecContext->thread_count = numCores;
    m_videoCodecContext->thread_type = FF_THREAD_FRAME;
    //m_videoCodecContext->active_thread_type = 0;

    if (m_hardwareAcceleration == HW_VULKAN){
        av_dict_set(&options, "extensions", "VK_KHR_external_memory_win32", 0);
        int err =  av_hwdevice_ctx_create(&m_hwDeviceContext, AV_HWDEVICE_TYPE_VULKAN, NULL, options, 0);
        m_videoCodecContext->get_format = get_hw_format_vulkan;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);

        avcodec_open2(m_videoCodecContext, videoCodec, nullptr);
        m_textureBridge = std::make_unique<VulkanTextureBridge>(m_videoCodecContext->width, m_videoCodecContext->height);

    }else if (m_hardwareAcceleration == HW_D3D12) {
        int err = av_hwdevice_ctx_create(&m_hwDeviceContext, AV_HWDEVICE_TYPE_D3D12VA, NULL, NULL, 0);
        m_videoCodecContext->sw_pix_fmt = AV_PIX_FMT_NV12;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);
        m_videoCodecContext->get_format = get_hw_format_d3d12;

        AVHWDeviceContext* device_ctx = (AVHWDeviceContext*)m_videoCodecContext->hw_device_ctx->data;
        AVD3D12VADeviceContext* d3d12_ctx = (AVD3D12VADeviceContext*)device_ctx->hwctx;
        d3d12_ctx->resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);

        avcodec_open2(m_videoCodecContext, videoCodec, nullptr);
        m_textureBridge = std::make_unique<D3D12TextureBridge>(m_videoCodecContext->width, m_videoCodecContext->height);

    }else if (m_hardwareAcceleration == HW_D3D11) {
        int err = av_hwdevice_ctx_create(&m_hwDeviceContext, AV_HWDEVICE_TYPE_D3D11VA, NULL, NULL, 0);

        AVHWDeviceContext* device_ctx = (AVHWDeviceContext*)(m_hwDeviceContext->data);
        AVD3D11VADeviceContext* d3d11_device_ctx = (AVD3D11VADeviceContext*)device_ctx->hwctx;
        d3d11_device_ctx->MiscFlags |= D3D11_RESOURCE_MISC_SHARED;

        m_videoCodecContext->sw_pix_fmt = AV_PIX_FMT_NV12;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);
        m_videoCodecContext->get_format = get_hw_format_d3d11;

        avcodec_open2(m_videoCodecContext, videoCodec, nullptr);
        m_textureBridge = std::make_unique<D3D11TextureBridge>(m_videoCodecContext->width, m_videoCodecContext->height, d3d11_device_ctx->device, d3d11_device_ctx->device_context);
    }else {
        avcodec_open2(m_videoCodecContext, videoCodec, nullptr);
        m_textureBridge = std::make_unique<SoftwareTextureBridge>(m_videoCodecContext->width, m_videoCodecContext->height, m_isPackedYuv);
    }

    //if (avcodec_open2(m_videoCodecContext, videoCodec, nullptr) < 0) return false;

    m_width = m_videoCodecContext->width;
    m_height = m_videoCodecContext->height;
    AVRational streamFps = m_formatContext->streams[m_videoStreamIndex]->r_frame_rate;
    m_fps = (streamFps.den > 0) ? av_q2d(streamFps) : 30.0;
    m_timePerFrame = 1.0 / m_fps;

    m_swsContext = sws_getContext(m_width, m_height, m_videoCodecContext->pix_fmt, m_width, m_height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);

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
    m_swFrame = av_frame_alloc();
    decodeVideoFrame();

    if (m_formatContext->duration != AV_NOPTS_VALUE) {
        m_duration = static_cast<double>(m_formatContext->duration) / AV_TIME_BASE;
    }

    AVStream* videoStream = m_formatContext->streams[m_videoStreamIndex];
    m_videoTimebase = av_q2d(videoStream->time_base);
    m_isPaused = false;
    m_currentTime = 0.0;

    return true;
}

bool MediaDecoder::update(double deltaTime) {
    if (m_isPaused) return false;

    m_accumulator += deltaTime;
    bool newFrameUploaded = false;

    while (m_accumulator >= m_timePerFrame && !newFrameUploaded) {
        if (av_read_frame(m_formatContext, m_packet) < 0) {
            av_seek_frame(m_formatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
            if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
            if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);
            continue;
        }

        if (m_packet->stream_index == m_videoStreamIndex) {
            avcodec_send_packet(m_videoCodecContext, m_packet);
            
            if (decodeVideoFrame()) {
                newFrameUploaded = true;
                m_accumulator -= m_timePerFrame; // Ein Frame-Zeitfenster abziehen
            }
        }else if (m_packet->stream_index == m_audioStreamIndex && m_swrContext) {
            std::vector<uint8_t> pcmData;
            if (m_audioDecoder.decodeAudioFrame(m_audioCodecContext, m_swrContext, m_packet, pcmData)) {            
                 m_audioOutput->enqueueData(pcmData);
               
            }
        }
        av_packet_unref(m_packet);
    }

    if (m_accumulator > m_timePerFrame * 2.0) {
        m_accumulator = m_timePerFrame;
    }

    return newFrameUploaded;
}

bool MediaDecoder::decodeVideoFrame() {
    int response = avcodec_receive_frame(m_videoCodecContext, m_videoFrame);

    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
        return false;
    }else if (response < 0) {
        return false;
    }

    if (m_videoFrame->pts != AV_NOPTS_VALUE) {
        m_currentTime = m_videoFrame->pts * m_videoTimebase;
    }else if (m_videoFrame->pkt_dts != AV_NOPTS_VALUE) {
        m_currentTime = m_videoFrame->pkt_dts * m_videoTimebase;
    }
    
   
    m_textureBridge->updateTexture(m_videoFrame);
    av_frame_unref(m_videoFrame);
    return true;    
}

void MediaDecoder::close() {
    if (m_swrContext) { swr_free(&m_swrContext); m_swrContext = nullptr; }
    if (m_swsContext) { sws_freeContext(m_swsContext); m_swsContext = nullptr; }
    if (m_rgbaBufferInternal) { av_free(m_rgbaBufferInternal); m_rgbaBufferInternal = nullptr; }
    if (m_videoCodecContext) { avcodec_free_context(&m_videoCodecContext); m_videoCodecContext = nullptr; }
    if (m_audioCodecContext) { avcodec_free_context(&m_audioCodecContext); m_audioCodecContext = nullptr; }
    if (m_formatContext) { avformat_close_input(&m_formatContext); m_formatContext = nullptr; }
}

void MediaDecoder::seekTo(double seconds) {
    if (!m_formatContext || !m_videoCodecContext) return;

    m_currentTime = std::clamp(seconds, 0.0, m_duration);
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