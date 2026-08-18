#include <iostream>
#include <algorithm>
#include <thread>

#include <WebGPU/WgpContext.h>

#include "MediaDecoder.h"
#include "../sound/AudioRingBuffer.h"
#include <dxgi.h>
#include <dxgi1_2.h>

MediaDecoder::MediaDecoder() {
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

                static SECURITY_ATTRIBUTES win32_security_attributes = {};
                win32_security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
                win32_security_attributes.lpSecurityDescriptor = nullptr;
                win32_security_attributes.bInheritHandle = TRUE;

                static VkExportMemoryWin32HandleInfoKHR win32_handle_info = {};
                win32_handle_info.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
                win32_handle_info.pNext = nullptr;
                win32_handle_info.pAttributes = &win32_security_attributes;
                win32_handle_info.dwAccess = DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE;

                static VkExportMemoryAllocateInfoKHR win32_export_alloc_info = {};
                win32_export_alloc_info.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_KHR;
                win32_export_alloc_info.pNext = &win32_handle_info;
                win32_export_alloc_info.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

                static VkExternalMemoryImageCreateInfoKHR win32_ext_image_info = {};
                win32_ext_image_info.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR;
                win32_ext_image_info.pNext = nullptr;
                win32_ext_image_info.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

                for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) {
                    if (vk_frames_ctx->alloc_pnext[i] == nullptr) {
                        vk_frames_ctx->alloc_pnext[i] = &win32_export_alloc_info;
                    }
                    else {
                        VkBaseOutStructure* current = reinterpret_cast<VkBaseOutStructure*>(vk_frames_ctx->alloc_pnext[i]);
                        while (current->pNext != nullptr) {
                            current = current->pNext;
                        }
                        current->pNext = reinterpret_cast<VkBaseOutStructure*>(&win32_export_alloc_info);
                    }
                }

                if (vk_frames_ctx->create_pnext == nullptr) {
                    vk_frames_ctx->create_pnext = &win32_ext_image_info;
                }else {
                    VkBaseOutStructure* current = reinterpret_cast<VkBaseOutStructure*>(vk_frames_ctx->create_pnext);
                    while (current->pNext != nullptr) {
                        current = current->pNext;
                    }
                    current->pNext = reinterpret_cast<VkBaseOutStructure*>(&win32_ext_image_info);
                }

                err = av_hwframe_ctx_init(ctx->hw_frames_ctx);
                if (err < 0) {
                    std::cerr << "av_hwframe_ctx_init fehlgeschlagen!" << std::endl;
                    av_buffer_unref(&ctx->hw_frames_ctx);
                    return AV_PIX_FMT_NONE;
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
    if (m_hardwareAcceleration == HW_VULKAN){
        int err =  av_hwdevice_ctx_create(&m_hwDeviceContext, AV_HWDEVICE_TYPE_VULKAN, nullptr, options, 0);
        m_videoCodecContext->get_format = get_hw_format_vulkan;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);

    }else if (m_hardwareAcceleration == HW_D3D12) {
        int err = av_hwdevice_ctx_create(&m_hwDeviceContext, AV_HWDEVICE_TYPE_D3D12VA, NULL, NULL, 0);
        m_videoCodecContext->sw_pix_fmt = AV_PIX_FMT_NV12;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);
        m_videoCodecContext->get_format = get_hw_format_d3d12;

        AVHWDeviceContext* device_ctx = (AVHWDeviceContext*)m_videoCodecContext->hw_device_ctx->data;
        AVD3D12VADeviceContext* d3d12_ctx = (AVD3D12VADeviceContext*)device_ctx->hwctx;
        d3d12_ctx->resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);

    }else if (m_hardwareAcceleration == HW_D3D11) {
        int err = av_hwdevice_ctx_create(&m_hwDeviceContext, AV_HWDEVICE_TYPE_D3D11VA, NULL, NULL, 0);

        AVHWDeviceContext* device_ctx = (AVHWDeviceContext*)(m_hwDeviceContext->data);
        AVD3D11VADeviceContext* d3d11_device_ctx = (AVD3D11VADeviceContext*)device_ctx->hwctx;
        d3d11_device_ctx->MiscFlags |= D3D11_RESOURCE_MISC_SHARED;

        m_videoCodecContext->sw_pix_fmt = AV_PIX_FMT_NV12;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);
        m_videoCodecContext->get_format = get_hw_format_d3d11;

        m_d3d11_device = d3d11_device_ctx->device;
        m_d3d11_context = d3d11_device_ctx->device_context;
    }

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

    return true;
}

bool MediaDecoder::update(double deltaTime, std::vector<uint8_t>& outRgbaBuffer, AudioRingBuffer& targetBuffer) {
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

    if (workingFrame->format == AV_PIX_FMT_VULKAN) {
        AVVkFrame* ffmpegFrame = reinterpret_cast<AVVkFrame*>(m_videoFrame->data[0]);
        VkImage vkImage = ffmpegFrame->img[0];
        VkDeviceMemory vkMemory = ffmpegFrame->mem[0];

        AVHWFramesContext* framesCtx = (AVHWFramesContext*)m_videoFrame->hw_frames_ctx->data;
        AVHWDeviceContext* deviceCtx = framesCtx->device_ctx;
        AVVulkanDeviceContext* vulkanDevCtx = (AVVulkanDeviceContext*)deviceCtx->hwctx;

        VkDevice vkDevice = vulkanDevCtx->act_dev;
        VkInstance vkInstance = vulkanDevCtx->inst;

        VkMemoryRequirements memReq = {};
        vkGetImageMemoryRequirements(vkDevice, vkImage, &memReq);

        std::cout << "--- VULKAN FRAME INSPECION ---" << std::endl;
        std::cout << "Speicherbedarf (Size): " << memReq.size << " Bytes" << std::endl;
        std::cout << "Alignment: " << memReq.alignment << std::endl;
        std::cout << "Memory Type Bits: " << memReq.memoryTypeBits << std::endl;

        VkPhysicalDeviceExternalImageFormatInfo externalImageFormatInfo = {};
        externalImageFormatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO;
        externalImageFormatInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

        VkImageFormatProperties2 imageFormatProperties = {};
        imageFormatProperties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;

        VkExternalImageFormatProperties externalImageFormatProperties = {};
        externalImageFormatProperties.sType = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES;
        imageFormatProperties.pNext = &externalImageFormatProperties;

        VkPhysicalDeviceImageFormatInfo2 formatInfo = {};
        formatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
        formatInfo.format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM; // Das typische NV12-Format in Vulkan
        formatInfo.type = VK_IMAGE_TYPE_2D;
        formatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        formatInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;


        VkResult checkResult = vkGetPhysicalDeviceImageFormatProperties2(
            vulkanDevCtx->phys_dev,
            &formatInfo,
            &imageFormatProperties
        );

        if (checkResult == VK_SUCCESS) {
            VkExternalMemoryFeatureFlags flags = externalImageFormatProperties.externalMemoryProperties.externalMemoryFeatures;
            std::cout << "External Memory Flags: " << flags << std::endl;
            if (flags & VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT) {
                std::cout << "-> SPEICHER IST EXPORTIERBAR! (GetWin32Handle wird funktionieren)" << std::endl;
            }
            else {
                std::cout << "-> FEHLER: FFmpeg hat diesen Speicher OHNE Export-Flags allokiert!" << std::endl;
            }
        }

        VkMemoryGetWin32HandleInfoKHR getFdInfo = {};
        getFdInfo.sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR;
        getFdInfo.pNext = NULL;
        getFdInfo.memory = vkMemory;
        getFdInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

        PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr =
            (PFN_vkGetDeviceProcAddr)vulkanDevCtx->get_proc_addr(vkInstance, "vkGetDeviceProcAddr");
        
        PFN_vkGetMemoryWin32HandleKHR fpGetMemoryWin32HandleKHR =
            (PFN_vkGetMemoryWin32HandleKHR)fpGetDeviceProcAddr(vkDevice, "vkGetMemoryWin32HandleKHR");
        

        HANDLE win32Handle = nullptr;
        VkResult result = fpGetMemoryWin32HandleKHR(vkDevice, &getFdInfo, &win32Handle);
        std::cout << win32Handle << "  " << result << std::endl;
    }else if (workingFrame->format == AV_PIX_FMT_D3D12) {
      
        AVD3D12VAFrame* ffmpegFrame = reinterpret_cast<AVD3D12VAFrame*>(m_videoFrame->data[0]);
        ID3D12Resource* d3d12Texture = ffmpegFrame->texture;
 
        SharedTextureMemoryD3D12ResourceDescriptor d3d12Desc = {};
        d3d12Desc.chain.next = NULL;
        d3d12Desc.chain.sType = WGPUSType_SharedTextureMemoryD3D12ResourceDescriptor;
        d3d12Desc.resource = d3d12Texture;
  
        if (m_textureViewY) {
            wgpuTextureViewRelease(m_textureViewY);
            m_textureViewY = nullptr;
        }

        if (m_textureViewUV) {
            wgpuTextureViewRelease(m_textureViewUV);
            m_textureViewUV = nullptr;
        }

        if (m_videoTexture) {
            wgpuTextureRelease(m_videoTexture);
            m_videoTexture = nullptr;
        }

        if (m_sharedTextureMemory) {
            wgpuSharedTextureMemoryRelease(m_sharedTextureMemory);
            m_sharedTextureMemory = nullptr;
        }

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
        yViewDesc.format = WGPUTextureFormat_R8Unorm;
        yViewDesc.dimension = WGPUTextureViewDimension_2D;
        yViewDesc.baseMipLevel = 0u;
        yViewDesc.mipLevelCount = 1u;
        yViewDesc.baseArrayLayer = 0u;
        yViewDesc.arrayLayerCount = 1u;
        yViewDesc.aspect = WGPUTextureAspect_Plane0Only;
        m_textureViewY = wgpuTextureCreateView(m_videoTexture, &yViewDesc);

        WGPUTextureViewDescriptor uvViewDesc = {};
        uvViewDesc.format = WGPUTextureFormat_RG8Unorm;
        uvViewDesc.dimension = WGPUTextureViewDimension_2D;
        uvViewDesc.baseMipLevel = 0u;
        uvViewDesc.mipLevelCount = 1u;
        uvViewDesc.baseArrayLayer = 0u;
        uvViewDesc.arrayLayerCount = 1u;
        uvViewDesc.aspect = WGPUTextureAspect_Plane1Only;
        m_textureViewUV = wgpuTextureCreateView(m_videoTexture, &uvViewDesc);

    }else if (workingFrame->format == AV_PIX_FMT_D3D11) {
        //ID3D11Texture2D* hw_texture = (ID3D11Texture2D*)m_videoFrame->data[0];
        //D3D11_TEXTURE2D_DESC texDesc;
        //hw_texture->GetDesc(&texDesc);

        //printf("Allokierte Textur Flags - Bind: %u, Misc: %u, ArraySize: %u\n",
        //    texDesc.BindFlags, texDesc.MiscFlags, texDesc.ArraySize);
        
        
        ID3D11Texture2D* d3d11Texture = reinterpret_cast<ID3D11Texture2D*>(m_videoFrame->data[0]);      
        IDXGIResource1* dxgiResource = nullptr;
        HRESULT hr = d3d11Texture->QueryInterface(__uuidof(IDXGIResource1), (void**)&dxgiResource);

        HANDLE sharedHandle = nullptr;       
        hr = dxgiResource->GetSharedHandle(&sharedHandle);
        dxgiResource->Release();
      
        ID3D11Texture2D* ffmpeg_texture_array = nullptr;
        hr = m_d3d11_device->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), (void**)&ffmpeg_texture_array);

        if (!m_single_texture) {
            D3D11_TEXTURE2D_DESC single_desc = {};
            single_desc.Width = m_width;
            single_desc.Height = m_height;
            single_desc.MipLevels = 1;
            single_desc.ArraySize = 1;
            single_desc.Format = DXGI_FORMAT_NV12;
            single_desc.SampleDesc.Count = 1;
            single_desc.SampleDesc.Quality = 0;
            single_desc.Usage = D3D11_USAGE_DEFAULT;
            single_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            single_desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED | D3D11_RESOURCE_MISC_SHARED_NTHANDLE;
            hr = m_d3d11_device->CreateTexture2D(&single_desc, nullptr, &m_single_texture);

            IDXGIResource1* dxgi_res1 = nullptr;
            hr = m_single_texture->QueryInterface(__uuidof(IDXGIResource1), (void**)&dxgi_res1);

            HANDLE webgpu_compatible_handle = nullptr;
            hr = dxgi_res1->CreateSharedHandle(nullptr, DXGI_SHARED_RESOURCE_READ, nullptr, &webgpu_compatible_handle);
            dxgi_res1->Release();


            WGPUSharedTextureMemoryDXGISharedHandleDescriptor desc = {};
            desc.chain.next = nullptr;
            desc.chain.sType = WGPUSType_SharedTextureMemoryDXGISharedHandleDescriptor;
            desc.handle = webgpu_compatible_handle;

            if (m_textureViewY) {
                wgpuTextureViewRelease(m_textureViewY);
                m_textureViewY = nullptr;
            }

            if (m_textureViewUV) {
                wgpuTextureViewRelease(m_textureViewUV);
                m_textureViewUV = nullptr;
            }

            if (m_videoTexture) {
                wgpuTextureRelease(m_videoTexture);
                m_videoTexture = nullptr;
            }

            if (m_sharedTextureMemory) {
                wgpuSharedTextureMemoryRelease(m_sharedTextureMemory);
                m_sharedTextureMemory = nullptr;
            }

            WGPUSharedTextureMemoryDescriptor memoryDesc = {};
            memoryDesc.nextInChain = (WGPUChainedStruct*)&desc;
            memoryDesc.label = WGPU_STR("FFmpeg Shared Frame Memory");
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
            yViewDesc.format = WGPUTextureFormat_R8Unorm;
            yViewDesc.dimension = WGPUTextureViewDimension_2D;
            yViewDesc.baseMipLevel = 0u;
            yViewDesc.mipLevelCount = 1u;
            yViewDesc.baseArrayLayer = 0u;
            yViewDesc.arrayLayerCount = 1u;
            yViewDesc.aspect = WGPUTextureAspect_Plane0Only;
            m_textureViewY = wgpuTextureCreateView(m_videoTexture, &yViewDesc);

            WGPUTextureViewDescriptor uvViewDesc = {};
            uvViewDesc.format = WGPUTextureFormat_RG8Unorm;
            uvViewDesc.dimension = WGPUTextureViewDimension_2D;
            uvViewDesc.baseMipLevel = 0u;
            uvViewDesc.mipLevelCount = 1u;
            uvViewDesc.baseArrayLayer = 0u;
            uvViewDesc.arrayLayerCount = 1u;
            uvViewDesc.aspect = WGPUTextureAspect_Plane1Only;
            m_textureViewUV = wgpuTextureCreateView(m_videoTexture, &uvViewDesc);
        }

        int current_slice_index = (int)(intptr_t)m_videoFrame->data[1];
        UINT src_subresource = D3D11CalcSubresource(0, current_slice_index, 1);

        m_d3d11_context->CopySubresourceRegion(m_single_texture, 0, 0, 0, 0, ffmpeg_texture_array, src_subresource, nullptr);
        m_d3d11_context->Flush();

        ffmpeg_texture_array->Release();

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

    int64_t targetVideoPts = static_cast<int64_t>(m_currentTime / m_videoTimebase);


    av_seek_frame(m_formatContext, m_videoStreamIndex, targetVideoPts, AVSEEK_FLAG_BACKWARD);

    if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
    if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);
    bool seekDone = false;
   
    while (!seekDone && av_read_frame(m_formatContext, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {
            int send_resp = avcodec_send_packet(m_videoCodecContext, m_packet);
            if (send_resp >= 0) {
                while (avcodec_receive_frame(m_videoCodecContext, m_videoFrame) >= 0) {
                    int64_t currentPts = m_videoFrame->pts;
                    if (currentPts >= targetVideoPts) {

                        if (m_videoFrame->format == AV_PIX_FMT_D3D12) {
                            seekDone = true;
                            break;

                        }else {
                            if (!m_isPackedYuv) {
                                sws_scale(m_swsContext, (uint8_t const* const*)m_videoFrame->data, m_videoFrame->linesize,
                                    0, m_height, m_frameRgba->data, m_frameRgba->linesize);

                                std::copy(m_rgbaBufferInternal, m_rgbaBufferInternal + m_currentFramePixels.size(),
                                    m_currentFramePixels.begin());
                            }
                            else {
                                uint8_t* dst = m_currentFramePixels.data();
                                int w = m_width; int h = m_height; int uvW = w / 2; int uvH = h / 2;

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
                            seekDone = true;
                            break;
                        }
                    }
                }
            }
        }
        av_packet_unref(m_packet);
    }
    m_accumulator = 0.0;
}