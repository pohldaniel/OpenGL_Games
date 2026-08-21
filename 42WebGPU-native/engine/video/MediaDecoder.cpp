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
    //av_log_set_level(AV_LOG_ERROR);
}

MediaDecoder::~MediaDecoder() {
    close();
    av_packet_free(&m_packet);
    av_frame_free(&m_videoFrame);
    av_frame_free(&m_frameRgba);
    av_frame_free(&m_audioFrame);
}

VkImage        m_exportImage = VK_NULL_HANDLE;
VkDeviceMemory m_exportMemory = VK_NULL_HANDLE;
HANDLE         m_win32Handle = nullptr;
VkCommandPool m_copyCommandPool = VK_NULL_HANDLE;
VkCommandBuffer m_copyCommandBuffer = VK_NULL_HANDLE;
VkQueue m_vulkanQueue = VK_NULL_HANDLE;
WGPUSharedTextureMemoryOpaqueFDDescriptor opaqueDesc = {};

void MediaDecoder::init_export_texture_vulkan(AVHWDeviceContext* vulkanDevCtx, int width, int height) {
    if (m_exportImage != VK_NULL_HANDLE) return; // Bereits initialisiert

    AVVulkanDeviceContext* vkctx = (AVVulkanDeviceContext*)vulkanDevCtx->hwctx;
    VkDevice vkDevice = vkctx->act_dev;

    VkExternalMemoryImageCreateInfoKHR extImageInfo = {};
    //extImageInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR;
    //extImageInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

    extImageInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR;
    extImageInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT_KHR;

    VkImageCreateInfo imgInfo = {};
    imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgInfo.pNext = &extImageInfo;
    imgInfo.imageType = VK_IMAGE_TYPE_2D;
    imgInfo.format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
    imgInfo.extent = { (uint32_t)width, (uint32_t)height, 1 };
    imgInfo.mipLevels = 1;
    imgInfo.arrayLayers = 1;
    imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    vkCreateImage(vkDevice, &imgInfo, nullptr, &m_exportImage);

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(vkDevice, m_exportImage, &memReq);

    VkMemoryDedicatedAllocateInfo dedicatedInfo = {};
    dedicatedInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
    dedicatedInfo.pNext = nullptr;
    dedicatedInfo.image = m_exportImage;
    dedicatedInfo.buffer = VK_NULL_HANDLE;

    VkExportMemoryWin32HandleInfoKHR win32HandleInfo = {};
    win32HandleInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
    win32HandleInfo.pNext = &dedicatedInfo;
    win32HandleInfo.pAttributes = nullptr;
    win32HandleInfo.dwAccess = DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE;

    VkExportMemoryAllocateInfoKHR extAllocInfo = {};
    extAllocInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_KHR;
    extAllocInfo.pNext = &win32HandleInfo;
    //extAllocInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
    extAllocInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT_KHR;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = &extAllocInfo;
    allocInfo.allocationSize = memReq.size;
    
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkctx->phys_dev, &memProperties);

    allocInfo.memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memReq.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) { // Zwingend DEVICE_LOCAL
            allocInfo.memoryTypeIndex = i;
            break;
        }
    }

    vkAllocateMemory(vkDevice, &allocInfo, nullptr, &m_exportMemory);
    vkBindImageMemory(vkDevice, m_exportImage, m_exportMemory, 0);

    // 3. Das Win32-Handle für WebGPU extrahieren
    PFN_vkGetMemoryWin32HandleKHR fpGetMemoryWin32HandleKHR = 
        (PFN_vkGetMemoryWin32HandleKHR)vkctx->get_proc_addr(vkctx->inst, "vkGetMemoryWin32HandleKHR");
    
    VkMemoryGetWin32HandleInfoKHR getHandleInfo = {};
    getHandleInfo.sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR;
    getHandleInfo.memory = m_exportMemory;
    //getHandleInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
    getHandleInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT_KHR;

    fpGetMemoryWin32HandleKHR(vkDevice, &getHandleInfo, &m_win32Handle);


    opaqueDesc.chain.next = nullptr;
    opaqueDesc.chain.sType = WGPUSType_SharedTextureMemoryOpaqueFDDescriptor;
    opaqueDesc.vkImageCreateInfo = &imgInfo;
    opaqueDesc.memoryFD = (int)(intptr_t)m_win32Handle;
    opaqueDesc.memoryTypeIndex = allocInfo.memoryTypeIndex;
    opaqueDesc.allocationSize = memReq.size;
    opaqueDesc.dedicatedAllocation = WGPU_TRUE;

    
    if (vkctx->nb_qf <= 0) {
        std::cerr << "[Vulkan-HW] Keine aktiven Queue-Familien im FFmpeg-Kontext gefunden!" << std::endl;
        return;
    }

    int chosenQueueFamilyIndex = vkctx->qf[0].idx;
    vkGetDeviceQueue(vkDevice, (uint32_t)chosenQueueFamilyIndex, 0, &m_vulkanQueue);

    if (m_vulkanQueue == VK_NULL_HANDLE) {
        std::cerr << "[Vulkan-HW] vkGetDeviceQueue fehlgeschlagen!" << std::endl;
        return;
    }

    // 2. Command Pool erstellen (mit dem korrekten, dynamisch ermittelten Index)
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = chosenQueueFamilyIndex; // Verwende die ermittelte Familie
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &m_copyCommandPool) != VK_SUCCESS) {
        std::cerr << "[Vulkan-HW] Erstellung des Command-Pools fehlgeschlagen." << std::endl;
        return;
    }

    // 3. Einen permanenten Command Buffer allokieren
    VkCommandBufferAllocateInfo allocInfoCmd = {};
    allocInfoCmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfoCmd.commandPool = m_copyCommandPool;
    allocInfoCmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfoCmd.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(vkDevice, &allocInfoCmd, &m_copyCommandBuffer) != VK_SUCCESS) {
        std::cerr << "[Vulkan-HW] Allokation des Command-Buffers fehlgeschlagen." << std::endl;
        return;
    }


    /*VkMemoryRequirements memReq2 = {};
    vkGetImageMemoryRequirements(vkDevice, m_exportImage, &memReq2);

    std::cout << "--- VULKAN FRAME INSPECION NEW ---" << std::endl;
    std::cout << "Speicherbedarf (Size): " << memReq2.size << " Bytes" << std::endl;
    std::cout << "Alignment: " << memReq2.alignment << std::endl;
    std::cout << "Memory Type Bits: " << memReq2.memoryTypeBits << std::endl;

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
        vkctx->phys_dev,
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
    }*/
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
    }else if (m_hardwareAcceleration == HW_D3D12) {
        int err = av_hwdevice_ctx_create(&m_hwDeviceContext, AV_HWDEVICE_TYPE_D3D12VA, NULL, NULL, 0);
        m_videoCodecContext->sw_pix_fmt = AV_PIX_FMT_NV12;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);
        m_videoCodecContext->get_format = get_hw_format_d3d12;

        AVHWDeviceContext* device_ctx = (AVHWDeviceContext*)m_videoCodecContext->hw_device_ctx->data;
        AVD3D12VADeviceContext* d3d12_ctx = (AVD3D12VADeviceContext*)device_ctx->hwctx;
        d3d12_ctx->resource_flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
        m_videoCodecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);

        m_d3d12_device = d3d12_ctx->device;

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
        AVFrame* cpuFrame = av_frame_alloc();
        cpuFrame->width = workingFrame->width;
        cpuFrame->height = workingFrame->height;

        int bufferRet = av_frame_get_buffer(cpuFrame, 0);
        int ret = av_hwframe_transfer_data(cpuFrame, workingFrame, 0);
        const char* formatName = av_get_pix_fmt_name((AVPixelFormat)cpuFrame->format);

        uint8_t* y_data = cpuFrame->data[0];
        int y_pitch = cpuFrame->linesize[0];

        uint8_t* uv_data = y_data + (y_pitch * cpuFrame->height);
        int uv_pitch = y_pitch;

        if (m_yTexture == nullptr) {
            WGPUTextureDescriptor texDesc = {};
            texDesc.dimension = WGPUTextureDimension_2D;
            texDesc.usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding;
            texDesc.mipLevelCount = 1u;
            texDesc.sampleCount = 1u;
            texDesc.label = WGPU_STR("FFmpeg Y Texture");
            texDesc.size = { static_cast<uint32_t>(cpuFrame->width), static_cast<uint32_t>(cpuFrame->height), 1u };
            texDesc.format = WGPUTextureFormat_R8Unorm;
            m_yTexture = wgpuDeviceCreateTexture(wgpContext.device, &texDesc);
        }

        if (m_uvTexture == nullptr) {
            WGPUTextureDescriptor texDesc = {};
            texDesc.dimension = WGPUTextureDimension_2D;
            texDesc.usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding;
            texDesc.mipLevelCount = 1u;
            texDesc.sampleCount = 1u;
            texDesc.label = WGPU_STR("FFmpeg UV Texture");
            texDesc.size = { static_cast<uint32_t>(cpuFrame->width / 2), static_cast<uint32_t>(cpuFrame->height / 2), 1u };
            texDesc.format = WGPUTextureFormat_RG8Unorm;
            m_uvTexture = wgpuDeviceCreateTexture(wgpContext.device, &texDesc);
        }
      
        WGPUTexelCopyTextureInfo destination = {};
        destination.texture = m_yTexture;
        destination.mipLevel = 0;
        destination.origin = { 0, 0, 0 };
        destination.aspect = WGPUTextureAspect_All;

        WGPUTexelCopyBufferLayout sourceLayout = {};
        sourceLayout.offset = 0;
        sourceLayout.bytesPerRow = y_pitch;
        sourceLayout.rowsPerImage = cpuFrame->height;

        WGPUExtent3D writeSizeY = { (uint32_t)cpuFrame->width, (uint32_t)cpuFrame->height, 1 };
        wgpuQueueWriteTexture(wgpContext.queue, &destination, y_data, y_pitch * cpuFrame->height, &sourceLayout, &writeSizeY);

        destination.texture = m_uvTexture;
        destination.aspect = WGPUTextureAspect_All;
        sourceLayout.bytesPerRow = uv_pitch;
        sourceLayout.rowsPerImage = cpuFrame->height / 2;

        WGPUExtent3D writeSizeUV = { (uint32_t)cpuFrame->width / 2, (uint32_t)cpuFrame->height / 2, 1 };
        wgpuQueueWriteTexture(wgpContext.queue, &destination, uv_data, uv_pitch * (cpuFrame->height / 2), &sourceLayout, &writeSizeUV);

        av_frame_free(&cpuFrame);

        if (m_textureViewY) {
            wgpuTextureViewRelease(m_textureViewY);
            m_textureViewY = nullptr;
        }

        if (m_textureViewUV) {
            wgpuTextureViewRelease(m_textureViewUV);
            m_textureViewUV = nullptr;
        }

        WGPUTextureViewDescriptor yViewDesc = {};
        yViewDesc.format = WGPUTextureFormat_R8Unorm;
        yViewDesc.dimension = WGPUTextureViewDimension_2D;
        yViewDesc.baseMipLevel = 0u;
        yViewDesc.mipLevelCount = 1u;
        yViewDesc.baseArrayLayer = 0u;
        yViewDesc.arrayLayerCount = 1u;
        yViewDesc.aspect = WGPUTextureAspect_All;
        m_textureViewY = wgpuTextureCreateView(m_yTexture, &yViewDesc);

        WGPUTextureViewDescriptor uvViewDesc = {};
        uvViewDesc.format = WGPUTextureFormat_RG8Unorm;
        uvViewDesc.dimension = WGPUTextureViewDimension_2D;
        uvViewDesc.baseMipLevel = 0u;
        uvViewDesc.mipLevelCount = 1u;
        uvViewDesc.baseArrayLayer = 0u;
        uvViewDesc.arrayLayerCount = 1u;
        uvViewDesc.aspect = WGPUTextureAspect_All;
        m_textureViewUV = wgpuTextureCreateView(m_uvTexture, &uvViewDesc);

        /*AVVkFrame* ffmpegFrame = reinterpret_cast<AVVkFrame*>(workingFrame->data[0]);
        VkImage vkImage = ffmpegFrame->img[0];
        VkDeviceMemory vkMemory = ffmpegFrame->mem[0];

        AVHWFramesContext* framesCtx = (AVHWFramesContext*)workingFrame->hw_frames_ctx->data;
        AVHWDeviceContext* deviceCtx = framesCtx->device_ctx;
        AVVulkanFramesContext* vk_ctx = (AVVulkanFramesContext*)framesCtx->hwctx;

        AVVulkanDeviceContext* vulkanDevCtx = (AVVulkanDeviceContext*)deviceCtx->hwctx;
        VkDevice vkDevice = vulkanDevCtx->act_dev;
        VkInstance vkInstance = vulkanDevCtx->inst;

        init_export_texture_vulkan(deviceCtx, m_width, m_height);
        vk_ctx->lock_frame(framesCtx, ffmpegFrame);

        vkResetCommandBuffer(m_copyCommandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(m_copyCommandBuffer, &beginInfo);


        VkImageMemoryBarrier srcBarriersStart[2] = {};
        srcBarriersStart[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        srcBarriersStart[0].oldLayout = ffmpegFrame->layout[0];
        srcBarriersStart[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        srcBarriersStart[0].srcAccessMask = ffmpegFrame->access[0];
        srcBarriersStart[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        srcBarriersStart[0].image = ffmpegFrame->img[0];
        srcBarriersStart[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT;
        srcBarriersStart[0].subresourceRange.levelCount = 1;
        srcBarriersStart[0].subresourceRange.layerCount = 1;

        srcBarriersStart[1] = srcBarriersStart[0];
        srcBarriersStart[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT;


        VkImageMemoryBarrier dstBarriersStart[2] = {};
        dstBarriersStart[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        dstBarriersStart[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Beim allerersten Mal okay
        dstBarriersStart[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstBarriersStart[0].srcAccessMask = 0;
        dstBarriersStart[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstBarriersStart[0].image = m_exportImage;
        dstBarriersStart[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT;
        dstBarriersStart[0].subresourceRange.levelCount = 1;
        dstBarriersStart[0].subresourceRange.layerCount = 1;

        dstBarriersStart[1] = dstBarriersStart[0];
        dstBarriersStart[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT;

        vkCmdPipelineBarrier(m_copyCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 2, srcBarriersStart);
        vkCmdPipelineBarrier(m_copyCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 2, dstBarriersStart);

        VkImageCopy copyY = {};
        copyY.srcSubresource.aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT;
        copyY.srcSubresource.layerCount = 1;
        copyY.dstSubresource.aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT;
        copyY.dstSubresource.layerCount = 1;
        copyY.extent.width = workingFrame->width;
        copyY.extent.height = workingFrame->height;
        copyY.extent.depth = 1;
        vkCmdCopyImage(m_copyCommandBuffer, ffmpegFrame->img[0], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_exportImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyY);

        VkImageCopy copyUV = {};
        copyUV.srcSubresource.aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT;
        copyUV.srcSubresource.layerCount = 1;
        copyUV.dstSubresource.aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT;
        copyUV.dstSubresource.layerCount = 1;
        copyUV.extent.width = workingFrame->width / 2;
        copyUV.extent.height = workingFrame->height / 2;
        copyUV.extent.depth = 1;
        vkCmdCopyImage(m_copyCommandBuffer, ffmpegFrame->img[0], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_exportImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyUV);

        VkImageMemoryBarrier srcBarriersEnd[2] = {};
        srcBarriersEnd[0] = srcBarriersStart[0];
        srcBarriersEnd[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        srcBarriersEnd[0].newLayout = ffmpegFrame->layout[0];
        srcBarriersEnd[0].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        srcBarriersEnd[0].dstAccessMask = ffmpegFrame->access[0];

        srcBarriersEnd[1] = srcBarriersEnd[0];
        srcBarriersEnd[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT;

        VkImageMemoryBarrier dstBarriersEnd[2] = {};
        dstBarriersEnd[0] = dstBarriersStart[0];
        dstBarriersEnd[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstBarriersEnd[0].newLayout = VK_IMAGE_LAYOUT_GENERAL;
        dstBarriersEnd[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstBarriersEnd[0].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        dstBarriersEnd[1] = dstBarriersEnd[0];
        dstBarriersEnd[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT;

        vkCmdPipelineBarrier(m_copyCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 2, srcBarriersEnd);
        vkCmdPipelineBarrier(m_copyCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 2, dstBarriersEnd);

        vkEndCommandBuffer(m_copyCommandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_copyCommandBuffer;

        vulkanDevCtx->lock_queue(deviceCtx, (uint32_t)vulkanDevCtx->qf[0].idx, 0);
        VkQueue dynamicQueue;
        vkGetDeviceQueue(vkDevice, (uint32_t)vulkanDevCtx->qf[0].idx, 0, &dynamicQueue);

        vkQueueSubmit(dynamicQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vulkanDevCtx->unlock_queue(deviceCtx, (uint32_t)vulkanDevCtx->qf[0].idx, 0);

        vkQueueWaitIdle(dynamicQueue);

        vk_ctx->unlock_frame(framesCtx, ffmpegFrame);
       
        WGPUSharedTextureMemoryOpaqueWin32HandleDescriptor dxgiDesc = {};
        dxgiDesc.chain.next = nullptr;
        dxgiDesc.chain.sType = WGPUSType_SharedTextureMemoryDXGISharedHandleDescriptor;
        dxgiDesc.handle = m_win32Handle;

        if (m_textureViewY) { wgpuTextureViewRelease(m_textureViewY); m_textureViewY = nullptr; }
        if (m_textureViewUV) { wgpuTextureViewRelease(m_textureViewUV); m_textureViewUV = nullptr; }
        if (m_videoTexture) { wgpuTextureRelease(m_videoTexture); m_videoTexture = nullptr; }
        if (m_sharedTextureMemory) { wgpuSharedTextureMemoryRelease(m_sharedTextureMemory); m_sharedTextureMemory = nullptr; }
       
        WGPUSharedTextureMemoryDescriptor memoryDesc = {};
        memoryDesc.nextInChain = (WGPUChainedStruct*)&dxgiDesc;
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
        m_textureViewUV = wgpuTextureCreateView(m_videoTexture, &uvViewDesc);*/



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
    av_frame_unref(workingFrame);
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