#include <iostream>
#include <algorithm>
#include <thread>

#include <WebGPU/WgpContext.h>

#include "MediaDecoder.h"
#include "../sound/AudioRingBuffer.h"
#include <dxgi.h>

MediaDecoder::MediaDecoder() {
    m_packet = av_packet_alloc();
    m_videoFrame = av_frame_alloc();
    m_frameRgba = av_frame_alloc();
    m_audioFrame = av_frame_alloc();
    //av_log_set_level(AV_LOG_DEBUG);
}

MediaDecoder::~MediaDecoder() {
    close();
    av_packet_free(&m_packet);
    av_frame_free(&m_videoFrame);
    av_frame_free(&m_frameRgba);
    av_frame_free(&m_audioFrame);
}



static enum AVPixelFormat get_hw_format_d3d12(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
    /* const enum AVPixelFormat target = AV_PIX_FMT_D3D12;
    for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == target) {
            if (!ctx->hw_frames_ctx) {
                AVBufferRef* frames_ref = av_hwframe_ctx_alloc(ctx->hw_device_ctx);
                if (!frames_ref) return AV_PIX_FMT_NONE;

                AVHWFramesContext* frames_ctx = (AVHWFramesContext*)frames_ref->data;
                frames_ctx->format = target;
                frames_ctx->sw_format = ctx->sw_pix_fmt; // Meist NV12 oder YUV420P
                frames_ctx->width = ctx->width;
                frames_ctx->height = ctx->height;
                frames_ctx->initial_pool_size = 20;

                if (av_hwframe_ctx_init(frames_ref) < 0) {
                    av_buffer_unref(&frames_ref);
                    return AV_PIX_FMT_NONE;
                }
                ctx->hw_frames_ctx = frames_ref;
                AVBufferRef* frames_ctx_ref = av_hwframe_ctx_alloc(ctx->hw_device_ctx);
                AVHWFramesContext* frames_ctx = (AVHWFramesContext*)frames_ctx_ref->data;

                // 2. Access the D3D12 specific context
                AVD3D12VAFramesContext* d3d12_frames_ctx = (AVD3D12VAFramesContext*)frames_ctx->hwctx;
                d3d12_frames_ctx->resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
                ctx->hw_frames_ctx = frames_ctx_ref;
            }
            return target;
        }
    }
    return AV_PIX_FMT_NONE; */
    while (*pix_fmts != AV_PIX_FMT_NONE) {
        if (*pix_fmts == AV_PIX_FMT_D3D12) {
            return AV_PIX_FMT_D3D12; // Erzwinge D3D12 Ausgabe
        }
        pix_fmts++;
    }
    return AV_PIX_FMT_NONE;
}

static enum AVPixelFormat get_hw_format_vulkan(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
    const enum AVPixelFormat target = AV_PIX_FMT_VULKAN;
    for (const enum AVPixelFormat* p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == target) {
            if (!ctx->hw_frames_ctx) {
                AVBufferRef* frames_ref = av_hwframe_ctx_alloc(ctx->hw_device_ctx);
                if (!frames_ref) return AV_PIX_FMT_NONE;

                AVHWFramesContext* frames_ctx = (AVHWFramesContext*)frames_ref->data;
                frames_ctx->format = target;
                frames_ctx->sw_format = ctx->sw_pix_fmt;
                frames_ctx->width = ctx->width;
                frames_ctx->height = ctx->height;
                frames_ctx->initial_pool_size = 20;

                if (av_hwframe_ctx_init(frames_ref) < 0) {
                    av_buffer_unref(&frames_ref);
                    return AV_PIX_FMT_NONE;
                }
                ctx->hw_frames_ctx = frames_ref;
            }
            return target;
        }
    }
    return AV_PIX_FMT_NONE;
}

bool MediaDecoder::open(const std::string& filename) {
    if (avformat_open_input(&m_formatContext, filename.c_str(), nullptr, nullptr) < 0) return false;
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) return false;

    // 1. STREAMS SUCKEN (Video und Audio parallel)
    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && m_videoStreamIndex == -1) {
            m_videoStreamIndex = i;
        } else if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStreamIndex == -1) {
            m_audioStreamIndex = i;
        }
    }

    // Mindestens eine Videospur ist Pflicht
    if (m_videoStreamIndex == -1) return false;

    // 2. VIDEO-CODEC CONFIG
    const AVCodec* videoCodec = avcodec_find_decoder(m_formatContext->streams[m_videoStreamIndex]->codecpar->codec_id);
    m_videoCodecContext = avcodec_alloc_context3(videoCodec);
    avcodec_parameters_to_context(m_videoCodecContext, m_formatContext->streams[m_videoStreamIndex]->codecpar);

    //int numCores = std::min( std::thread::hardware_concurrency(), 16u);
    //m_videoCodecContext->thread_count = numCores;
    //m_videoCodecContext->thread_type = FF_THREAD_FRAME;
    int err = av_hwdevice_ctx_create(&m_hwDeviceContext, AV_HWDEVICE_TYPE_D3D12VA, NULL, NULL, 0);
    m_videoCodecContext->sw_pix_fmt = AV_PIX_FMT_NV12;
    m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);
    m_videoCodecContext->get_format = get_hw_format_d3d12;


    AVHWDeviceContext* device_ctx = (AVHWDeviceContext*)m_videoCodecContext->hw_device_ctx->data;
    AVD3D12VADeviceContext* d3d12_ctx = (AVD3D12VADeviceContext*)device_ctx->hwctx;

    d3d12_ctx->resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
    m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);

    if (avcodec_open2(m_videoCodecContext, videoCodec, nullptr) < 0) return false;

    m_width = m_videoCodecContext->width;
    m_height = m_videoCodecContext->height;
    AVRational streamFps = m_formatContext->streams[m_videoStreamIndex]->r_frame_rate;
    m_fps = (streamFps.den > 0) ? av_q2d(streamFps) : 30.0;
    m_timePerFrame = 1.0 / m_fps;

    if (m_isPackedYuv) {
        int yuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_width, m_height, 1);
        m_currentFramePixels.resize(yuvSize, 0);
        m_rgbaBufferInternal = nullptr;
    }else {
        // Internen RGBA-Pixelbuffer allokieren
        int rgbaSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_width, m_height, 1);
        m_rgbaBufferInternal = (uint8_t*)av_malloc(rgbaSize * sizeof(uint8_t));
        m_currentFramePixels.resize(rgbaSize, 0);
        av_image_fill_arrays(m_frameRgba->data, m_frameRgba->linesize, m_rgbaBufferInternal, AV_PIX_FMT_RGBA, m_width, m_height, 1);
    }
    
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

    
    WGPUTextureDescriptor textureDesc = {};
    textureDesc.nextInChain = NULL;
    textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size.width = m_width;
    textureDesc.size.height = m_height;
    textureDesc.size.depthOrArrayLayers = 1u;
    textureDesc.format = WGPUTextureFormat_R8BG8Biplanar420Unorm;
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;

    videoTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDesc);

    WGPUTextureDescriptor textureDescY = {};
    textureDescY.nextInChain = NULL;
    textureDescY.label = WGPU_STR("uv texture");
    textureDescY.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDescY.dimension = WGPUTextureDimension_2D;
    textureDescY.size.width = m_width;
    textureDescY.size.height = m_height;
    textureDescY.size.depthOrArrayLayers = 1u;
    textureDescY.format = WGPUTextureFormat_R8Unorm;
    textureDescY.mipLevelCount = 1;
    textureDescY.sampleCount = 1;

    m_yTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDescY);

    WGPUTextureDescriptor textureDescUV = {};
    textureDescUV.nextInChain = NULL;
    textureDescUV.label = WGPU_STR("uv texture");
    textureDescUV.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDescUV.dimension = WGPUTextureDimension_2D;
    textureDescUV.size.width = m_width / 2;
    textureDescUV.size.height = m_height / 2;
    textureDescUV.size.depthOrArrayLayers = 1u;
    textureDescUV.format = WGPUTextureFormat_R8Unorm;
    textureDescUV.mipLevelCount = 1;
    textureDescUV.sampleCount = 1;

    m_uvTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDescUV);

    return true;
}

bool MediaDecoder::update(double deltaTime, std::vector<uint8_t>& outRgbaBuffer, AudioRingBuffer& targetBuffer) {
    m_hasNewHwFrame = false;
    m_accumulator += deltaTime;
    bool newFrameUploaded = false;
    if (m_isPaused) {
        outRgbaBuffer = m_currentFramePixels;
        return false;
    }
    // --- DIE KORREKTE WEICHE ---
    // Wir lesen so lange ununterbrochen Pakete aus der Datei, 
    // bis wir das für diesen Frame fällige Video-Bild ERFOLGREICH aus dem Decoder extrahiert haben.
    // Das bricht den Teufelskreis aus EAGAIN und vollem Audiobuffer!
    while (m_accumulator >= m_timePerFrame && !newFrameUploaded) {

        // Versuche, das nächste Paket aus der Datei zu lesen
        if (av_read_frame(m_formatContext, m_packet) < 0) {
            av_seek_frame(m_formatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
            // Den internen Codec-Speicher leeren, um Bildartefakte beim Sprung zu verhindern
            if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
            if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);
            continue; // Schleife direkt mit dem ersten Paket von vorne fortsetzen
        }

        // FALL A: Es ist ein VIDEO-Paket
        if (m_packet->stream_index == m_videoStreamIndex) {
            // JEDES Paket wandert sofort in den Codec (kein POC Abreißen!)
            avcodec_send_packet(m_videoCodecContext, m_packet);

            // Direkt versuchen, das Bild abzurufen
            if (decodeVideoFrame()) {
                newFrameUploaded = true;
                m_accumulator -= m_timePerFrame; // Ein Frame-Zeitfenster abziehen
            }
        }
        // FALL B: Es ist ein AUDIO-Paket
        else if (m_packet->stream_index == m_audioStreamIndex && m_swrContext) {
            std::vector<uint8_t> pcmData;
            if (decodeAudioFrame(pcmData)) {
                // Nur schreiben, wenn Platz ist, um Überläufe im RAM zu verhindern
                if (targetBuffer.getAvailableWrite() >= pcmData.size()) {
                    targetBuffer.write(pcmData.data(), pcmData.size());
                }
            }
        }

        av_packet_unref(m_packet);
    }

    // Extrem wichtiger Schutz vor unendlichem Frame-Drop (z.B. nach harten Rucklern/Ladebildschirmen):
    // Wenn der Akkumulator weiter als 2 Frames in der Zukunft liegt, bremsen wir ihn ein,
    // sonst würde die Schleife versuchen, 100 Frames im selben Durchlauf zu decodieren.
    if (m_accumulator > m_timePerFrame * 2.0) {
        m_accumulator = m_timePerFrame;
    }

    outRgbaBuffer = m_currentFramePixels;
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

    AVFrame* workingFrame = m_videoFrame;
  
    if (workingFrame->format == AV_PIX_FMT_D3D12) {
        if (!workingFrame->hw_frames_ctx) {
            std::cout << "Fehler: Kein Hardware-Frame-Kontext vorhanden!" << std::endl;
            return false;
        }

        /* AVD3D12VAFrame* ffmpegFrameStruct = reinterpret_cast<AVD3D12VAFrame*>(m_videoFrame->data[0]);
        ID3D12Resource* ffmpegTextureResource = ffmpegFrameStruct->texture;
        UINT64 ffmpegSubresourceIndex = ffmpegFrameStruct->subresource_index;

        m_d3d12Resource = reinterpret_cast<ID3D12Resource*>(workingFrame->data[0]);
        m_subresourceIndex = static_cast<UINT64>(reinterpret_cast<uintptr_t>(workingFrame->data[1]));

        if (!m_d3d12Resource) {
            std::cout << "Fehler: D3D12 Resource-Pointer ist null!" << std::endl;
            return false;
        }*/

        /*UINT ySourceSubresource = (UINT)ffmpegSubresourceIndex;
        UINT uvSourceSubresource = (UINT)ffmpegSubresourceIndex + (1 * 1 * 1); // Nächste Plane-Ebene


        D3D12_TEXTURE_COPY_LOCATION dstY = {};
        dstY.pResource = ffmpegTextureResource;
        dstY.Type = D3D12_TEXTURE_COPY_LOCATION_TYPE_SUBRESOURCE_INDEX;
        dstY.SubresourceIndex = 0; // Ziel-Textur Plane 0

        D3D12_TEXTURE_COPY_LOCATION srcY = {};
        srcY.pResource = ffmpegTextureResource;
        srcY.Type = D3D12_TEXTURE_COPY_LOCATION_TYPE_SUBRESOURCE_INDEX;
        srcY.SubresourceIndex = ySourceSubresource;

        d3d12CommandList->CopySubresourceRegion(&dstY, 0, 0, 0, &srcY, nullptr);

        D3D12_TEXTURE_COPY_LOCATION dstUV = {};
        dstUV.pResource = ffmpegTextureResource;
        dstUV.Type = D3D12_TEXTURE_COPY_LOCATION_TYPE_SUBRESOURCE_INDEX;
        dstUV.SubresourceIndex = 1;

        D3D12_TEXTURE_COPY_LOCATION srcUV = {};
        srcUV.pResource = ffmpegTextureResource;
        srcUV.Type = D3D12_TEXTURE_COPY_LOCATION_TYPE_SUBRESOURCE_INDEX;
        srcUV.SubresourceIndex = uvSourceSubresource;

        d3d12CommandList->CopySubresourceRegion(&dstUV, 0, 0, 0, &srcUV, nullptr);*/

        // Setzen Sie ein Flag für Ihren Renderer, dass ein neuer HW-Frame bereitsteht

        /*AVFrame* swFrame = av_frame_alloc();
        int err = av_hwframe_transfer_data(swFrame, workingFrame, 0);
        if (err < 0) {
            av_frame_free(&swFrame);
            return false;
        }

        WGPUTexelCopyTextureInfo dstY = {};
        dstY.texture = m_yTexture;
        dstY.mipLevel = 0u;
        dstY.origin.x = 0u;
        dstY.origin.y = 0u;
        dstY.origin.z = 0u;
        dstY.aspect = WGPUTextureAspect_All;
 

        WGPUTexelCopyBufferLayout layoutY = {};
        layoutY.offset = 0;
        layoutY.bytesPerRow = static_cast<uint32_t>(swFrame->linesize[0]);
        layoutY.rowsPerImage = static_cast<uint32_t>(m_height);
    

        WGPUExtent3D sizeY = { };
        sizeY.width = static_cast<uint32_t>(m_width);
        sizeY.height = static_cast<uint32_t>(m_height);
        sizeY.depthOrArrayLayers = 1u;
        wgpuQueueWriteTexture(wgpContext.queue, &dstY, swFrame->data[0], layoutY.bytesPerRow * m_height, &layoutY, &sizeY);

        uint32_t uvWidth = m_width / 2;
        uint32_t uvHeight = m_height / 2;
        uint32_t stride = static_cast<uint32_t>(swFrame->linesize[0]);

        WGPUTexelCopyTextureInfo dstUV = {};
        dstUV.texture = m_uvTexture;
        dstUV.mipLevel = 0u;
        dstUV.origin.x = 0u;
        dstUV.origin.y = 0u;
        dstUV.origin.z = 0u;
        dstUV.aspect = WGPUTextureAspect_All;
           

        WGPUTexelCopyBufferLayout layoutUV = {};
        layoutUV.offset = 0;
        layoutY.bytesPerRow = stride;
        layoutY.rowsPerImage = static_cast<uint32_t>(uvHeight);
           

        WGPUExtent3D sizeUV = {};
        sizeUV.width = static_cast<uint32_t>(uvWidth);
        sizeUV.height = static_cast<uint32_t>(uvHeight);
        sizeUV.depthOrArrayLayers = 1u;

        uint8_t* uvDataStart = swFrame->data[0] + (stride * m_height);
        size_t uvDataSize = stride * uvHeight;

        //wgpuQueueWriteTexture(wgpContext.queue, &dstUV, uvDataStart, uvDataSize, &layoutUV, &sizeUV);


        av_frame_free(&swFrame);*/

        // 1. Holen der nativen ID3D12Resource aus dem FFmpeg-Frame
           // FFmpeg speichert bei D3D12va ein Array von AVD3D12VAFrame-Strukturen im Context
        //AVD3D12VAFramesContext* hw_frames_ctx = (AVD3D12VAFramesContext*)m_videoFrame->hw_frames_ctx->data;
        //ID3D12Resource* d3d12Texture = (ID3D12Resource*)m_videoFrame->data[0];


        AVD3D12VAFrame* ffmpegFrame = reinterpret_cast<AVD3D12VAFrame*>(m_videoFrame->data[0]);
        ID3D12Resource* d3d12Texture = ffmpegFrame->texture;
        UINT64 subresourceIndex = ffmpegFrame->subresource_index;

        ID3D12Device* ffmpegD3D12Device = nullptr;
        HRESULT hr = d3d12Texture->GetDevice(__uuidof(ID3D12Device), (void**)&ffmpegD3D12Device);

       
        SharedTextureMemoryD3D12ResourceDescriptor d3d12Desc = {};
        d3d12Desc.chain.next = NULL;
        d3d12Desc.chain.sType = WGPUSType_SharedTextureMemoryD3D12ResourceDescriptor;
        d3d12Desc.resource = d3d12Texture;

        WGPUSharedTextureMemoryDescriptor memoryDesc = {};
        memoryDesc.nextInChain = (WGPUChainedStruct*)&d3d12Desc;
        memoryDesc.label = WGPU_STR("FFmpeg_Direct_D3D12_ZeroCopy");
        m_sharedTextureMemory = wgpuDeviceImportSharedTextureMemory(wgpContext.device, &memoryDesc);


        WGPUTextureDescriptor textureDesc = {};
        textureDesc.nextInChain = NULL;
        textureDesc.label = WGPU_STR("FFmpeg_Hardware_Video_Texture");
        textureDesc.usage = WGPUTextureUsage_TextureBinding;
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.size.width = m_width;
        textureDesc.size.height = m_height;
        textureDesc.size.depthOrArrayLayers = 1;
        textureDesc.format = WGPUTextureFormat_R8BG8Biplanar420Unorm;
        textureDesc.mipLevelCount = 1;
        textureDesc.sampleCount = 1;

        m_videoTexture = wgpuSharedTextureMemoryCreateTexture(m_sharedTextureMemory, &textureDesc);

        WGPUTextureViewDescriptor yViewDesc = {};
        yViewDesc.format = WGPUTextureFormat_R8Unorm; // Standard-Enum aus webgpu.h
        yViewDesc.dimension = WGPUTextureViewDimension_2D;
        yViewDesc.baseMipLevel = 0;
        yViewDesc.mipLevelCount = 1;
        yViewDesc.baseArrayLayer = 0;
        yViewDesc.arrayLayerCount = 1;
        yViewDesc.aspect = WGPUTextureAspect_Plane0Only; // Sagt Dawn: Nur die Y-Ebene greifen
        m_textureViewY = wgpuTextureCreateView(m_videoTexture, &yViewDesc);

        //int err = av_hwframe_transfer_data(workingFrame, workingFrame, 0);

        /*AVFrame* swFrame = av_frame_alloc();
        swFrame->format = AV_PIX_FMT_NV12;
        swFrame->width = workingFrame->width;
        swFrame->height = workingFrame->height;

        int err = av_hwframe_map(swFrame, workingFrame, AV_HWFRAME_MAP_READ);

        std::cout << err << "  " << workingFrame->data[0] << "  " << workingFrame->linesize[0] << std::endl;

        WGPUTexelCopyTextureInfo dstY = {};
        dstY.texture = m_yTexture;
        dstY.mipLevel = 0u;
        dstY.origin.x = 0u;
        dstY.origin.y = 0u;
        dstY.origin.z = 0u;
        dstY.aspect = WGPUTextureAspect_All;


        WGPUTexelCopyBufferLayout layoutY = {};
        layoutY.offset = 0;
        layoutY.bytesPerRow = static_cast<uint32_t>(workingFrame->linesize[0]);
        layoutY.rowsPerImage = static_cast<uint32_t>(m_height);


        WGPUExtent3D sizeY = { };
        sizeY.width = static_cast<uint32_t>(m_width);
        sizeY.height = static_cast<uint32_t>(m_height);
        sizeY.depthOrArrayLayers = 1u;
        wgpuQueueWriteTexture(wgpContext.queue, &dstY, workingFrame->data[0], layoutY.bytesPerRow* m_height, &layoutY, &sizeY);


        av_frame_free(&workingFrame);*/

        // 4. Direktimport ohne den Umweg über Windows Shared Handles
        // Das verhindert das E_INVALIDARG Problem komplett, da die Ressource auf Geräte-Ebene geteilt wird.
       // return wgpuDeviceImportSharedTextureMemory(device, &memoryDesc);

        /*AVD3D12VAFrame* ffmpegFrame = reinterpret_cast<AVD3D12VAFrame*>(m_videoFrame->data[0]);
        ID3D12Resource* d3d12Texture = ffmpegFrame->texture;
        UINT64 subresourceIndex = ffmpegFrame->subresource_index;

        ID3D12Device* ffmpegD3D12Device = nullptr;
        HRESULT hr = d3d12Texture->GetDevice(__uuidof(ID3D12Device), (void**)&ffmpegD3D12Device);

        HANDLE sharedHandle = nullptr;

        // Wir fordern generische Lese-/Schreibrechte für das NT-Handle an
        hr = ffmpegD3D12Device->CreateSharedHandle(
            d3d12Texture,
            nullptr,
            GENERIC_ALL,
            nullptr,
            &sharedHandle
        );
       // ffmpegD3D12Device->Release();
        std::cout << hr << "  " << ffmpegD3D12Device << "  " << d3d12Texture << std::endl;
        WGPUSharedTextureMemoryDXGISharedHandleDescriptor dxgiDesc = {};
        dxgiDesc.chain.next = NULL;
        dxgiDesc.chain.sType = WGPUSType_SharedTextureMemoryDXGISharedHandleDescriptor;
        dxgiDesc.handle = sharedHandle;

        WGPUSharedTextureMemoryDescriptor memoryDesc = {};
        memoryDesc.nextInChain = (WGPUChainedStruct*)&dxgiDesc;
        memoryDesc.label = WGPU_STR("FFmpeg_DXGI_ZeroCopy_Memory");
        WGPUSharedTextureMemory sharedMemory = wgpuDeviceImportSharedTextureMemory(wgpContext.device, &memoryDesc);*/


        m_hasNewHwFrame = true;       
    }else if (workingFrame->format == AV_PIX_FMT_YUV420P) {
        uint8_t* dst = m_currentFramePixels.data();

        if (!m_isPackedYuv) {
            sws_scale(m_swsContext, (uint8_t const* const*)m_videoFrame->data, m_videoFrame->linesize,
                0, m_height, m_frameRgba->data, m_frameRgba->linesize);

            std::copy(m_rgbaBufferInternal, m_rgbaBufferInternal + m_currentFramePixels.size(),
                m_currentFramePixels.begin());
        }else {
            int w = m_width;
            int h = m_height;
            int uvW = w / 2;
            int uvH = h / 2;

            uint8_t* srcY = workingFrame->data[0];
            int linesizeY = workingFrame->linesize[0];
            for (int y = 0; y < h; y++) {
                std::memcpy(dst + (y * w), srcY + (y * linesizeY), w);
            }

            uint8_t* uDstStart = dst + (w * h);
            uint8_t* vDstStart = uDstStart + (uvW * uvH);

            uint8_t* srcU = workingFrame->data[1];
            int linesizeU = workingFrame->linesize[1];
            for (int y = 0; y < uvH; y++) {
                std::memcpy(uDstStart + (y * uvW), srcU + (y * linesizeU), uvW);
            }

            uint8_t* srcV = workingFrame->data[2];
            int linesizeV = workingFrame->linesize[2];
            for (int y = 0; y < uvH; y++) {
                std::memcpy(vDstStart + (y * uvW), srcV + (y * linesizeV), uvW);
            }
        }
    }
 
    return true;
}

bool MediaDecoder::decodeAudioFrame(std::vector<uint8_t>& outPcmData) {
    outPcmData.clear();
    if (avcodec_send_packet(m_audioCodecContext, m_packet) >= 0) {
        if (avcodec_receive_frame(m_audioCodecContext, m_audioFrame) >= 0) {
            
            int maxOutSamples = swr_get_out_samples(m_swrContext, m_audioFrame->nb_samples);
            std::vector<uint8_t> tempBuf(maxOutSamples * 2 * sizeof(int16_t)); // 2 Kanäle, 16-Bit

            uint8_t* outData[1] = { tempBuf.data() };
            int converted = swr_convert(m_swrContext, outData, maxOutSamples, (const uint8_t**)m_audioFrame->data, m_audioFrame->nb_samples);
            
            if (converted > 0) {
                size_t actualSize = converted * 2 * sizeof(int16_t);
                outPcmData.insert(outPcmData.end(), tempBuf.begin(), tempBuf.begin() + actualSize);
                return true;
            }
        }
    }
    return false;
}

void MediaDecoder::close() {
    if (m_swrContext) { swr_free(&m_swrContext); m_swrContext = nullptr; }
    if (m_swsContext) { sws_freeContext(m_swsContext); m_swsContext = nullptr; }
    if (m_rgbaBufferInternal) { av_free(m_rgbaBufferInternal); m_rgbaBufferInternal = nullptr; }
    if (m_videoCodecContext) { avcodec_free_context(&m_videoCodecContext); m_videoCodecContext = nullptr; }
    if (m_audioCodecContext) { avcodec_free_context(&m_audioCodecContext); m_audioCodecContext = nullptr; }
    if (m_formatContext) { avformat_close_input(&m_formatContext); m_formatContext = nullptr; }
}

bool MediaDecoder::updateOpenAL(double deltaTime, std::vector<uint8_t>& outRgbaBuffer, std::vector<uint8_t>& outPcmAudio) {
    m_accumulator += deltaTime;
    bool newFrameUploaded = false;
    outPcmAudio.clear(); // Alten Audio-Frame-Rest leeren

    if (m_isPaused) {
        outRgbaBuffer = m_currentFramePixels;
        return false;
    }

    // Wir lesen Pakete, solange die Zeit für das Video es verlangt
    while (m_accumulator >= m_timePerFrame && !newFrameUploaded) {

        if (av_read_frame(m_formatContext, m_packet) < 0) {
            av_seek_frame(m_formatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
            // Den internen Codec-Speicher leeren, um Bildartefakte beim Sprung zu verhindern
            if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
            if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);
            continue; // Schleife direkt mit dem ersten Paket von vorne fortsetzen
        }

        // VIDEO-PAKET
        if (m_packet->stream_index == m_videoStreamIndex) {
            avcodec_send_packet(m_videoCodecContext, m_packet);

            if (decodeVideoFrame()) {
                newFrameUploaded = true;
                m_accumulator -= m_timePerFrame;
            }
        }
        // AUDIO-PAKET: Wir sammeln den Ton dieses Frames direkt im Ausgabe-Vektor
        else if (m_packet->stream_index == m_audioStreamIndex && m_swrContext) {
            std::vector<uint8_t> pcmData;
            if (decodeAudioFrame(pcmData)) {
                // Audio-Daten für diesen Gameloop-Frame anhäufen
                outPcmAudio.insert(outPcmAudio.end(), pcmData.begin(), pcmData.end());
            }
        }

        av_packet_unref(m_packet);
    }

    if (m_accumulator > m_timePerFrame * 2.0) {
        m_accumulator = m_timePerFrame;
    }

    outRgbaBuffer = m_currentFramePixels;
    return newFrameUploaded;
}

void MediaDecoder::seekTo(double seconds) {
    if (!m_formatContext) return;

    m_currentTime = std::clamp(seconds, 0.0, m_duration);

    // 1. Berechne die Ziel-PTS für das Video
    int64_t targetVideoPts = static_cast<int64_t>(m_currentTime / m_videoTimebase);

    // 2. Springe in der Datei zum Keyframe davor
    av_seek_frame(m_formatContext, m_videoStreamIndex, targetVideoPts, AVSEEK_FLAG_BACKWARD);

    // Puffer leeren, damit keine alten Reste decodiert werden
    if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
    if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);

    while (av_read_frame(m_formatContext, m_packet) >= 0) {

        if (m_packet->stream_index == m_videoStreamIndex) {
            avcodec_send_packet(m_videoCodecContext, m_packet);
            if (avcodec_receive_frame(m_videoCodecContext, m_videoFrame) >= 0) {
                int64_t currentPts = m_videoFrame->pts;
                if (currentPts >= targetVideoPts) {
                    if (!m_isPackedYuv) {
                        sws_scale(m_swsContext, (uint8_t const* const*)m_videoFrame->data, m_videoFrame->linesize,
                            0, m_height, m_frameRgba->data, m_frameRgba->linesize);

                        std::copy(m_rgbaBufferInternal, m_rgbaBufferInternal + m_currentFramePixels.size(),
                            m_currentFramePixels.begin());
                    }else {
                        uint8_t* dst = m_currentFramePixels.data();
                        int w = m_width;
                        int h = m_height;
                        int uvW = w / 2;
                        int uvH = h / 2;

                        uint8_t* srcY = m_videoFrame->data[0];
                        int linesizeY = m_videoFrame->linesize[0];
                        for (int y = 0; y < h; y++) {
                            std::memcpy(dst + (y * w), srcY + (y * linesizeY), w);
                        }

                        uint8_t* uDstStart = dst + (w * h);
                        uint8_t* vDstStart = uDstStart + (uvW * uvH);

                        uint8_t* srcU = m_videoFrame->data[1];
                        int linesizeU = m_videoFrame->linesize[1];
                        for (int y = 0; y < uvH; y++) {
                            std::memcpy(uDstStart + (y * uvW), srcU + (y * linesizeU), uvW);
                        }

                        uint8_t* srcV = m_videoFrame->data[2];
                        int linesizeV = m_videoFrame->linesize[2];
                        for (int y = 0; y < uvH; y++) {
                            std::memcpy(vDstStart + (y * uvW), srcV + (y * linesizeV), uvW);
                        }
                    }
                    break;
                }
            }
        }
        av_packet_unref(m_packet);
    }
    m_accumulator = 0.0;
}