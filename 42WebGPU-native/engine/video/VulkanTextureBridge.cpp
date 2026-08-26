#include <iostream>
#include <dxgi1_2.h>
#include <WebGPU/WgpContext.h>
#include "VulkanTextureBridge.h"

static enum AVPixelFormat get_hw_format_vulkan_(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
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
                    av_buffer_unref(&ctx->hw_frames_ctx);
                    break;
                }
                return AV_PIX_FMT_VULKAN;
            }
        }
    }
    return ctx->sw_pix_fmt;
}

void VulkanTextureBridge::configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) {
    ctx->hw_device_ctx = av_buffer_ref(hwDeviceCtx);
    ctx->get_format = get_hw_format_vulkan_;
}

void VulkanTextureBridge::init(int width, int height) {
    m_width = width;
    m_height = height;
    initWebGPUEntities();
    m_cpuFrame = av_frame_alloc();
}

void VulkanTextureBridge::initWebGPUEntities() {
    WGPUTextureDescriptor texDescY = {};
    texDescY.dimension = WGPUTextureDimension_2D;
    texDescY.usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding;
    texDescY.mipLevelCount = 1u;
    texDescY.sampleCount = 1u;
    texDescY.label = WGPU_STR("FFmpeg Y Texture");
    texDescY.size = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1u };
    texDescY.format = WGPUTextureFormat_R8Unorm;
    m_yTexture = wgpuDeviceCreateTexture(wgpContext.device, &texDescY);

    WGPUTextureDescriptor texDescUV = {};
    texDescUV.dimension = WGPUTextureDimension_2D;
    texDescUV.usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding;
    texDescUV.mipLevelCount = 1u;
    texDescUV.sampleCount = 1u;
    texDescUV.label = WGPU_STR("FFmpeg UV Texture");
    texDescUV.size = { static_cast<uint32_t>(m_width / 2), static_cast<uint32_t>(m_height / 2), 1u };
    texDescUV.format = WGPUTextureFormat_RG8Unorm;
    m_uvTexture = wgpuDeviceCreateTexture(wgpContext.device, &texDescUV);

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
  
    AVPixelFormat pixFmt = AV_PIX_FMT_NV12; // Dein Ziel-CPU-Format
    int linesizes[4] = { 0 };
    av_image_fill_linesizes(linesizes, pixFmt, m_width);
    int y_pitch = linesizes[0];
    m_yBufferSize = y_pitch * m_height;
    m_uvBufferSize = y_pitch * (m_height / 2);
    uint32_t totalSize = m_yBufferSize + m_uvBufferSize;

    WGPUBufferDescriptor bufferDesc = {};
    bufferDesc.size = totalSize;
    bufferDesc.usage = WGPUBufferUsage_MapWrite | WGPUBufferUsage_CopySrc;
    bufferDesc.mappedAtCreation = false;

    m_stagingBuffers[0] = wgpuDeviceCreateBuffer(wgpContext.device, &bufferDesc);
    m_stagingBuffers[1] = wgpuDeviceCreateBuffer(wgpContext.device, &bufferDesc);

    bool firstMapDone = false;
    WGPUBufferMapCallbackInfo cbInfo = {};
    cbInfo.nextInChain = nullptr;
    cbInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    cbInfo.callback = [](WGPUMapAsyncStatus s, WGPUStringView m, void* u1, void* u2) {};
    cbInfo.userdata1 = &firstMapDone;
    cbInfo.userdata2 = nullptr;

    wgpuBufferMapAsync(m_stagingBuffers[0], WGPUMapMode_Write, 0, totalSize, cbInfo);
    wgpuInstanceProcessEvents(wgpContext.instance);
}

void VulkanTextureBridge::updateTexture(AVFrame* frame) {

    av_hwframe_transfer_data(m_cpuFrame, frame, 0);

    uint8_t* y_data = m_cpuFrame->data[0];
    int y_pitch = m_cpuFrame->linesize[0];
    uint8_t* uv_data = m_cpuFrame->data[1];
    int uv_pitch = m_cpuFrame->linesize[1];

    WGPUBuffer activeBuffer = m_stagingBuffers[m_currentFrameIndex];
    wgpuInstanceProcessEvents(wgpContext.instance);
    uint8_t* mappedData = (uint8_t*)wgpuBufferGetMappedRange(activeBuffer, 0, m_yBufferSize + m_uvBufferSize);
    std::memcpy(mappedData, y_data, m_yBufferSize);
    std::memcpy(mappedData + m_yBufferSize, uv_data, m_uvBufferSize);
    wgpuBufferUnmap(activeBuffer);

    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, nullptr);

    WGPUTexelCopyBufferLayout yLayout = {};
    yLayout.offset = 0;
    yLayout.bytesPerRow = y_pitch;
    yLayout.rowsPerImage = m_cpuFrame->height;

    WGPUTexelCopyBufferInfo yBufferInfo = {};
    yBufferInfo.buffer = activeBuffer;
    yBufferInfo.layout = yLayout;

    WGPUTexelCopyTextureInfo yDest = {};
    yDest.texture = m_yTexture;
    yDest.aspect = WGPUTextureAspect_All;

    WGPUExtent3D sizeY = { (uint32_t)m_cpuFrame->width, (uint32_t)m_cpuFrame->height, 1 };
    wgpuCommandEncoderCopyBufferToTexture(encoder, &yBufferInfo, &yDest, &sizeY);

    WGPUTexelCopyBufferLayout uvLayout = {};
    uvLayout.offset = m_yBufferSize;
    uvLayout.bytesPerRow = uv_pitch;
    uvLayout.rowsPerImage = m_cpuFrame->height / 2;

    WGPUTexelCopyBufferInfo uvBufferInfo = {};
    uvBufferInfo.buffer = activeBuffer;
    uvBufferInfo.layout = uvLayout;

    WGPUTexelCopyTextureInfo uvDest = {};
    uvDest.texture = m_uvTexture;
    uvDest.aspect = WGPUTextureAspect_All;

    WGPUExtent3D sizeUV = { (uint32_t)m_cpuFrame->width / 2, (uint32_t)m_cpuFrame->height / 2, 1 };
    wgpuCommandEncoderCopyBufferToTexture(encoder, &uvBufferInfo, &uvDest, &sizeUV);

    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(encoder, nullptr);
    wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);

    wgpuCommandBufferRelease(commandBuffer);
    wgpuCommandEncoderRelease(encoder);
    av_frame_unref(m_cpuFrame);

    uint32_t nextFrameIndex = (m_currentFrameIndex + 1) % 2;
    WGPUBuffer nextBuffer = m_stagingBuffers[nextFrameIndex];
    WGPUBufferMapCallbackInfo callbackInfo = {};
    callbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    callbackInfo.callback = [](WGPUMapAsyncStatus status, WGPUStringView message, void* u1, void* u2) {};
    wgpuBufferMapAsync(nextBuffer, WGPUMapMode_Write, 0, m_yBufferSize + m_uvBufferSize, callbackInfo);
    m_currentFrameIndex = nextFrameIndex;
}