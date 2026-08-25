#include <dxgi1_2.h>
#include <WebGPU/WgpContext.h>
#include "VulkanTextureBridge.h"
#include <iostream>

VkImage        m_exportImage = VK_NULL_HANDLE;
VkDeviceMemory m_exportMemory = VK_NULL_HANDLE;
HANDLE         m_win32Handle = nullptr;
VkCommandPool m_copyCommandPool = VK_NULL_HANDLE;
VkCommandBuffer m_copyCommandBuffer = VK_NULL_HANDLE;
VkQueue m_vulkanQueue = VK_NULL_HANDLE;
WGPUSharedTextureMemoryOpaqueFDDescriptor opaqueDesc = {};
WGPUSharedTextureMemoryDXGISharedHandleDescriptor dxgiDesc = {};

void VulkanTextureBridge::init_export_texture_vulkan(AVHWDeviceContext* vulkanDevCtx, int width, int height) {
    if (m_exportImage != VK_NULL_HANDLE) return; // Bereits initialisiert

    AVVulkanDeviceContext* vkctx = (AVVulkanDeviceContext*)vulkanDevCtx->hwctx;
    VkDevice vkDevice = vkctx->act_dev;

    VkExternalMemoryImageCreateInfoKHR extImageInfo = {};
    extImageInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR;
    extImageInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR;

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
    extAllocInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR;

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
    getHandleInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR;

    fpGetMemoryWin32HandleKHR(vkDevice, &getHandleInfo, &m_win32Handle);


    opaqueDesc.chain.next = nullptr;
    opaqueDesc.chain.sType = WGPUSType_SharedTextureMemoryOpaqueFDDescriptor;
    opaqueDesc.vkImageCreateInfo = &imgInfo;
    opaqueDesc.memoryFD = (int)(intptr_t)m_win32Handle;
    opaqueDesc.memoryTypeIndex = allocInfo.memoryTypeIndex;
    opaqueDesc.allocationSize = memReq.size;
    opaqueDesc.dedicatedAllocation = WGPU_TRUE;

    dxgiDesc = {};
    dxgiDesc.chain.next = nullptr;
    dxgiDesc.chain.sType = WGPUSType_SharedTextureMemoryDXGISharedHandleDescriptor;
    dxgiDesc.handle = m_win32Handle;

    WGPUSharedTextureMemoryDescriptor memoryDesc = {};
    memoryDesc.nextInChain = (WGPUChainedStruct*)&dxgiDesc;
    memoryDesc.label = WGPU_STR("FFmpeg Shared Frame Memory");
    //m_sharedTextureMemory = wgpuDeviceImportSharedTextureMemory(wgpContext.device, &memoryDesc);
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