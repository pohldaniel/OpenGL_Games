#include <fstream>
#include <sstream>
#include <iostream>

#include "Application.h"
#include "VlkExtension.h"
#include "VlkContext.h"
#include "VlkSwapchain.h"
#include "VlkMesh.h"
#include "VlkTexture.h"

VlkContext vlkContext = {};
int maxDescriptorSets = 10;
int maxDescriptorCount = 65536;

#define ArraySize(arr) sizeof((arr)) / sizeof((arr[0]))

#define VK_CHECK(result)                                  \
if (result != VK_SUCCESS) {                               \
    std::cout << "Vulkan Error: " << result << std::endl; \
    __debugbreak();                                       \
    return false;                                         \
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vlkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
    VkDebugUtilsMessageTypeFlagsEXT msgFlags,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cout << pCallbackData->pMessage << std::endl;
    return false;
}

void VlkContext::CheckVKResult(VkResult err) {
    if (err == 0)
        return;

    std::cout << "[vulkan] Error: VkResult = " << err << std::endl;

    if (err < 0) {
        abort();
    }
}

bool isTypeOf(VkPhysicalDevice& device, VkPhysicalDeviceType physicalDeviceType) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    return properties.deviceType == physicalDeviceType;
}

void vlkInit(void* window) {
    vlkCreateDevice(vlkContext, window);
    vlkGetDeviceQueue(vlkContext.queueFamilyIndex, vlkContext.vkQueue);
    vlkCreateCommandPool(vlkContext.vkCommandPool);
    vlkCreateCommandBuffer(vlkContext.vkCommandBuffer);
    vlkCreateDescriptorPool(vlkContext.vkDescriptorPool);
    vlkContext.swapchain = new VlkSwapchain(Application::Width, Application::Height, vlkContext.vkPresentModeKHR);
    vlkContext.drawUi = true;

    //Sample Scope
    vlkContext.createDescriptorSetLayout(vlkContext.vkDevice, vlkContext.vkDescriptorSetLayouts);
    vlkContext.createShaders(vlkContext.vkDevice);

    vlkCreatePipelineLayout(vlkContext.vkDescriptorSetLayouts, { }, vlkContext.vkPipelineLayout);
    vlkCreateSampler(vlkContext.sampler);
}

void vlkResize() {
    vlkWaitIdle();

    vlkContext.newSwapchain = new VlkSwapchain(Application::Width, Application::Height, vlkContext.vkPresentModeKHR, vlkContext.swapchain);
    delete vlkContext.swapchain;
    vlkContext.swapchain = vlkContext.newSwapchain;
}

void vlkToggleVerticalSync() {
    if (vlkContext.vkPresentModeKHR == VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR)
        vlkContext.vkPresentModeKHR = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
    else
        vlkContext.vkPresentModeKHR = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;

    vlkWaitIdle();

    vlkContext.newSwapchain = new VlkSwapchain(Application::Width, Application::Height, vlkContext.vkPresentModeKHR, vlkContext.swapchain);
    delete vlkContext.swapchain;
    vlkContext.swapchain = vlkContext.newSwapchain;
}

void vlkWaitIdle() {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    vkDeviceWaitIdle(vkDevice);
}

void vlkToggleWireframe() {
    if (vlkContext.vkPolygonMode == VkPolygonMode::VK_POLYGON_MODE_FILL)
        vlkContext.vkPolygonMode = VkPolygonMode::VK_POLYGON_MODE_LINE;
    else
        vlkContext.vkPolygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
}

void vlkShutDown() {
    const VkInstance& vkInstance = vlkContext.vkInstance;
    const VkSurfaceKHR& vkSurfaceKHR = vlkContext.vkSurfaceKHR;
    delete vlkContext.swapchain;
    vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, VK_NULL_HANDLE);
}

void vlkToggleUI() {
    vlkContext.drawUi = !vlkContext.drawUi;
}

void vlkSetDrawUI(bool flag) {
    vlkContext.drawUi = flag;
}

void vlkDraw() {
    vlkContext.swapchain->draw();
}

void vlkGetDeviceQueue(uint32_t queueFamilyIndex, VkQueue& vkQueue) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    vkGetDeviceQueue(vkDevice, queueFamilyIndex, 0, &vkQueue);
}

void vlkMapBuffer(const VkDeviceMemory& vkDeviceMemory, const void* data, uint32_t size) {
    const VkDevice& vkDevice = vlkContext.vkDevice;

    void* pMem = nullptr;
    vkMapMemory(vkDevice, vkDeviceMemory, 0, size, 0, &pMem);
    memcpy(pMem, data, size);
    vkUnmapMemory(vkDevice, vkDeviceMemory);
}

void vlkCreateBuffer(VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory, uint32_t size, VkBufferUsageFlags vkBufferUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    
    VkBufferCreateInfo vkBufferCreateInfo = {};
    vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkBufferCreateInfo.size = size;
    vkBufferCreateInfo.usage = vkBufferUsageFlags;
    vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vkBuffer);

    VkMemoryRequirements vkMemoryRequirements;
    vkGetBufferMemoryRequirements(vkDevice, vkBuffer, &vkMemoryRequirements);

    VkMemoryAllocateInfo vkMemoryAllocateInfo = {};
    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    vkMemoryAllocateInfo.memoryTypeIndex = vlkFindMemoryType(vkMemoryRequirements.memoryTypeBits, vkMemoryPropertyFlags);

    vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vkDeviceMemory);
    vkBindBufferMemory(vkDevice, vkBuffer, vkDeviceMemory, 0);
}

void vlkCopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, uint32_t size) {
    const VkCommandBuffer& vkCommandBuffer = vlkContext.vkCommandBuffer; 
    VkBufferCopy vkBufferCopy = {};
    vkBufferCopy.srcOffset = 0;
    vkBufferCopy.dstOffset = 0;
    vkBufferCopy.size = size;

    vlkBeginCommandBuffer(vkCommandBuffer);
    vkCmdCopyBuffer(vkCommandBuffer, srcBuffer, dstBuffer, 1, &vkBufferCopy);
    vlkEndCommandBuffer(vkCommandBuffer, true);
}

void vlkCreateImage(VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, uint32_t width, uint32_t height, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    
    VkImageCreateInfo vkImageCreateInfo = {};
    vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    vkImageCreateInfo.extent.width = width;
    vkImageCreateInfo.extent.height = height;
    vkImageCreateInfo.extent.depth = 1;
    vkImageCreateInfo.mipLevels = 1;
    vkImageCreateInfo.arrayLayers = 1;
    vkImageCreateInfo.format = vkFormat;
    vkImageCreateInfo.tiling = vkImageTiling;
    vkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkImageCreateInfo.usage = vkImageUsageFlags;
    vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateImage(vkDevice, &vkImageCreateInfo, nullptr, &vkImage);

    VkMemoryRequirements vkMemoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, vkImage, &vkMemoryRequirements);

    VkMemoryAllocateInfo vkMemoryAllocateInfo = {};
    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    vkMemoryAllocateInfo.memoryTypeIndex = vlkFindMemoryType(vkMemoryRequirements.memoryTypeBits, vkMemoryPropertyFlags);

    vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, nullptr, &vkDeviceMemory);
    vkBindImageMemory(vkDevice, vkImage, vkDeviceMemory, 0);
}

void vlkCreateImageView(VkImageView& vkImageView, const VkImage& vkImage, VkFormat vkFormat, VkImageAspectFlags vkImageAspectFlags, VkComponentMapping vkComponentMapping) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    
    VkImageViewCreateInfo vkImageViewCreateInfo = {};
    vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    vkImageViewCreateInfo.image = vkImage;
    vkImageViewCreateInfo.format = vkFormat;
    vkImageViewCreateInfo.subresourceRange.aspectMask = vkImageAspectFlags;
    vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    vkImageViewCreateInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    vkImageViewCreateInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
    vkImageViewCreateInfo.components = vkComponentMapping;
    vkCreateImageView(vkDevice, &vkImageViewCreateInfo, NULL, &vkImageView);
}

void vlkDestroyImage(const VkImage& vkImage, const VkDeviceMemory& vkDeviceMemory) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    if(vkDeviceMemory)
      vkFreeMemory(vkDevice, vkDeviceMemory, NULL);
    vkDestroyImage(vkDevice, vkImage, NULL);
}

void vlkCopyBufferToImage(const VkBuffer& srcBuffer, const VkImage& dstImage, uint32_t width, uint32_t height) {
    const VkCommandBuffer& vkCommandBuffer = vlkContext.vkCommandBuffer;
    

    VkBufferImageCopy vkBufferImageCopy = {};
    vkBufferImageCopy.bufferOffset = 0;
    vkBufferImageCopy.bufferRowLength = 0;
    vkBufferImageCopy.bufferImageHeight = 0;

    vkBufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkBufferImageCopy.imageSubresource.mipLevel = 0;
    vkBufferImageCopy.imageSubresource.baseArrayLayer = 0;
    vkBufferImageCopy.imageSubresource.layerCount = 1;

    vkBufferImageCopy.imageOffset = { 0, 0, 0 };
    vkBufferImageCopy.imageExtent = {width, height, 1};

    vlkBeginCommandBuffer(vkCommandBuffer);
    vkCmdCopyBufferToImage(vkCommandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferImageCopy);
    vlkEndCommandBuffer(vkCommandBuffer, true);
}

void vlkDestroyImage(const VkImage& vkImage) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    vkDestroyImage(vkDevice, vkImage, NULL);
}

void vlkCreateCommandPool(VkCommandPool& vkCommandPool) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {};
    vkCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    vkCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCommandPoolCreateInfo.queueFamilyIndex = vlkContext.queueFamilyIndex;
    vkCreateCommandPool(vkDevice, &vkCommandPoolCreateInfo, 0, &vkCommandPool);
}

void vlkCreateCommandBuffer(VkCommandBuffer& vkCommandBuffer) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo{};
    vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    vkCommandBufferAllocateInfo.commandPool = vlkContext.vkCommandPool;
    vkCommandBufferAllocateInfo.commandBufferCount = 1;
    vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(vkDevice, &vkCommandBufferAllocateInfo, &vkCommandBuffer);
}

void vlkTransitionImageLayout(const VkCommandBuffer& commandBuffer, const VkImage& vkImage, VkImageAspectFlags vkImageAspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage) {
    VkImageSubresourceRange access;
    access.aspectMask = vkImageAspectFlags;
    access.baseMipLevel = 0;
    access.levelCount = 1;
    access.baseArrayLayer = 0;
    access.layerCount = 1;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vkImage;
    barrier.subresourceRange = access;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;
    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, 0, 0, 0, 1, &barrier);
}

void vlkCreateSemaphore(VkSemaphore& vkSemaphore) {
    const VkDevice& vkDevice = vlkContext.vkDevice;

    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(vkDevice, &createInfo, nullptr, &vkSemaphore);
}

void vlkCreateFence(VkFence& vkFence) {
    const VkDevice& vkDevice = vlkContext.vkDevice;

    VkFenceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(vkDevice, &createInfo, nullptr, &vkFence);
}

void vlkBeginCommandBuffer(const VkCommandBuffer& vkCommandBuffer, VkCommandBufferUsageFlags vkCommandBufferUsageFlags) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = vkCommandBufferUsageFlags;
    vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);
}

void vlkEndCommandBuffer(const VkCommandBuffer& vkCommandBuffer, bool submit) {
    vkEndCommandBuffer(vkCommandBuffer);
    if(submit)
      vlkQueueSubmit(vkCommandBuffer);
}

void vlkQueueSubmit(const VkCommandBuffer& vkCommandBuffer) {
    const VkQueue& vkQueue = vlkContext.vkQueue;

    VkSubmitInfo vkSubmitInfo = {};
    vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    vkSubmitInfo.commandBufferCount = 1;
    vkSubmitInfo.pCommandBuffers = &vkCommandBuffer;
    vkSubmitInfo.waitSemaphoreCount = 0;
    vkSubmitInfo.pWaitSemaphores = VK_NULL_HANDLE;
    vkSubmitInfo.pWaitDstStageMask = VK_NULL_HANDLE;
    vkSubmitInfo.signalSemaphoreCount = 0;
    vkSubmitInfo.pSignalSemaphores = VK_NULL_HANDLE;
    vkSubmitInfo.pNext = NULL;

    vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkQueue);
}

void vlkCreateSwapChain(VkSwapchainKHR& vkSwapchainKHR, VkFormat& vkFormat, uint32_t width, uint32_t height, const VkPresentModeKHR vkPresentModeKHR, VkSwapchainKHR vkOldSwapchainKHR) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    const VkPhysicalDevice& vkPhysicalDevice = vlkContext.vkPhysicalDevice;
    const VkSurfaceKHR& vkSurfaceKHR = vlkContext.vkSurfaceKHR;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurfaceKHR, &capabilities);

    width = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    height = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurfaceKHR, &formatCount, VK_NULL_HANDLE);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurfaceKHR, &formatCount, formats.data());

    VkSurfaceFormatKHR chosenFormat = formats[0];

    for (const VkSurfaceFormatKHR& format : formats){
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM){
            chosenFormat = format;
            break;
        }
    }

    vkFormat = chosenFormat.format;
    
    VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKHR = {};
    vkSwapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkSwapchainCreateInfoKHR.surface = vkSurfaceKHR;
    vkSwapchainCreateInfoKHR.minImageCount = capabilities.minImageCount;
    vkSwapchainCreateInfoKHR.imageFormat = chosenFormat.format;
    vkSwapchainCreateInfoKHR.imageColorSpace = chosenFormat.colorSpace;
    vkSwapchainCreateInfoKHR.imageExtent = { width, height };
    vkSwapchainCreateInfoKHR.imageArrayLayers = 1;
    vkSwapchainCreateInfoKHR.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    vkSwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkSwapchainCreateInfoKHR.preTransform = capabilities.currentTransform;
    vkSwapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    vkSwapchainCreateInfoKHR.clipped = true;
    vkSwapchainCreateInfoKHR.presentMode = vkPresentModeKHR;
    vkSwapchainCreateInfoKHR.oldSwapchain = vkOldSwapchainKHR;
    vkCreateSwapchainKHR(vkDevice, &vkSwapchainCreateInfoKHR, VK_NULL_HANDLE, &vkSwapchainKHR);
}

void vlkReadFile(const char* path, std::vector<char>& buffer) {

    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::stringstream line_builder;
        line_builder << "Failed to load \""
            << path << "\"" << std::endl;
        std::string line = line_builder.str();
    }

    size_t filesize{ static_cast<size_t>(file.tellg()) };

    buffer.resize(filesize);
    file.seekg(0);
    file.read(buffer.data(), filesize);

    file.close();
}

void vlkCompileSahder(const char* path, shaderc_shader_kind kind, std::vector<uint32_t>& shaderCode) {
    shaderc_compile_options_t options = shaderc_compile_options_initialize();
    shaderc_compile_options_set_target_env(options, shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    shaderc_compile_options_set_source_language(options, shaderc_source_language_glsl);
    shaderc_compile_options_set_target_spirv(options, shaderc_spirv_version_1_3);
    shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_zero);
    //shaderc_compile_options_set_suppress_warnings(options);
    //shaderc_compile_options_set_warnings_as_errors(options);
    //shaderc_compile_options_set_auto_bind_uniforms(options, true);
    //shaderc_compile_options_set_preserve_bindings(options, true);
    //shaderc_compile_options_set_vulkan_rules_relaxed(options, true);
    //shaderc_compile_options_set_auto_combined_image_sampler(options, true);

    std::vector<char> source;
    vlkReadFile(path, source);
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    shaderc_compilation_result_t result = shaderc_compile_into_preprocessed_text(compiler, source.data(), source.size(), kind, path, "main", options);

    if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
        std::cout << "Error: preprocess" << std::endl;
    }

    //copy result into info for next compilation operation
    const char* src = reinterpret_cast<const char*>(shaderc_result_get_bytes(result));
    size_t newSize = reinterpret_cast<const char*>(shaderc_result_get_bytes(result)) + shaderc_result_get_length(result) / sizeof(char) - src;
    source.resize(newSize);
    memcpy(source.data(), src, newSize);

    compiler = shaderc_compiler_initialize();
    result = shaderc_compile_into_spv_assembly(compiler, source.data(), source.size(), kind, path, "main", options);

    if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
        std::cout << "Error: toSpvAssembly" << std::endl;
    }

    src = reinterpret_cast<const char*>(shaderc_result_get_bytes(result));
    newSize = reinterpret_cast<const char*>(shaderc_result_get_bytes(result)) + shaderc_result_get_length(result) / sizeof(char) - src;
    source.resize(newSize);
    memcpy(source.data(), src, newSize);

    compiler = shaderc_compiler_initialize();
    result = shaderc_assemble_into_spv(compiler, source.data(), source.size(), options);
    if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
        std::cout << "Error: compile" << std::endl;
    }

    const uint32_t* res = reinterpret_cast<const uint32_t*>(shaderc_result_get_bytes(result));
    size_t wordCount = shaderc_result_get_length(result) / sizeof(uint32_t);
    shaderCode.resize(wordCount);
    memcpy(shaderCode.data(), res, shaderc_result_get_length(result));
}

std::vector<VkShaderEXT>& vlkCreateShader(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts,  const std::vector<uint32_t>& vertexCode, const std::vector<uint32_t>& fragmentCode, std::vector<VkShaderEXT>& shader) {
    const VkDevice& vkDevice = vlkContext.vkDevice;

    VkShaderCreateInfoEXT vkShaderCreateInfoEXT[2] = {{}};
    vkShaderCreateInfoEXT[0].sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    vkShaderCreateInfoEXT[0].flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT;
    vkShaderCreateInfoEXT[0].codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    vkShaderCreateInfoEXT[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    vkShaderCreateInfoEXT[0].pName = "main";
    vkShaderCreateInfoEXT[0].codeSize = sizeof(uint32_t) * vertexCode.size();
    vkShaderCreateInfoEXT[0].pCode = vertexCode.data();
    vkShaderCreateInfoEXT[0].setLayoutCount = vkDescriptorSetLayouts.size();
    vkShaderCreateInfoEXT[0].pSetLayouts = vkDescriptorSetLayouts.data();
    vkShaderCreateInfoEXT[0].nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;

    vkShaderCreateInfoEXT[1].sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    vkShaderCreateInfoEXT[1].flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT;
    vkShaderCreateInfoEXT[1].codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    vkShaderCreateInfoEXT[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vkShaderCreateInfoEXT[1].pName = "main";
    vkShaderCreateInfoEXT[1].codeSize = sizeof(uint32_t) * fragmentCode.size();
    vkShaderCreateInfoEXT[1].pCode = fragmentCode.data();
    vkShaderCreateInfoEXT[1].setLayoutCount = vkDescriptorSetLayouts.size();
    vkShaderCreateInfoEXT[1].pSetLayouts = vkDescriptorSetLayouts.data();

    shader.resize(2);
    vkCreateShadersEXT(vkDevice, 2, vkShaderCreateInfoEXT, VK_NULL_HANDLE, shader.data());
    return shader;
}

void vlkCreateSampler(VkSampler& vkSampler) {
    const VkDevice& vkDevice = vlkContext.vkDevice;

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.maxAnisotropy = 16;
    createInfo.maxLod = VK_LOD_CLAMP_NONE;
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    vkCreateSampler(vkDevice, &createInfo, nullptr, &vkSampler);
}

uint32_t vlkFindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vlkContext.vkPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}

void vlkCreateDescriptorPool(VkDescriptorPool& vkDescriptorPool) {
    const VkDevice& vkDevice = vlkContext.vkDevice;

    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxDescriptorSets * maxDescriptorCount },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxDescriptorSets * maxDescriptorCount },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxDescriptorSets * maxDescriptorCount }
    };
    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = maxDescriptorSets;
    createInfo.poolSizeCount = sizeof(poolSizes) / sizeof(VkDescriptorPoolSize);
    createInfo.pPoolSizes = poolSizes;

    vkCreateDescriptorPool(vkDevice, &createInfo, nullptr, &vkDescriptorPool);
}

void vlkAllocateDescriptorSet(VkDescriptorSet& vkDescriptorSet, const VkDescriptorSetLayout& vkDescriptorSetLayout) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    const VkDescriptorPool& vkDescriptorPool = vlkContext.vkDescriptorPool;
    
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vkDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &vkDescriptorSetLayout;

    vkAllocateDescriptorSets(vkDevice, &allocInfo, &vkDescriptorSet);
}

void vlkCreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<VkPushConstantRange> vkPushConstantRanges, VkPipelineLayout& vkPipelineLayout) {
    const VkDevice& vkDevice = vlkContext.vkDevice;

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = vkDescriptorSetLayouts.size();
    createInfo.pSetLayouts = vkDescriptorSetLayouts.data();
    //createInfo.pushConstantRangeCount = vkPushConstantRanges.size();
    //createInfo.pPushConstantRanges = vkPushConstantRanges.data();
    vkCreatePipelineLayout(vkDevice, &createInfo, nullptr, &vkPipelineLayout);
}

void vlkCreateUniformBuffer(VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory, void*& mapping, uint32_t size) {
    const VkDevice& vkDevice = vlkContext.vkDevice;
    vlkCreateBuffer(vkBuffer, vkDeviceMemory, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkMapMemory(vkDevice, vkDeviceMemory, 0, size, 0, &mapping);
}

void vlkBindBufferToDescriptorSet(const VkBuffer& vkBuffer, const VkDescriptorSet& vkDescriptorSet, VkDescriptorType vkDescriptorType, uint32_t dstBinding) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = vkBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.descriptorType = vkDescriptorType;
    descriptorWrite.dstSet = vkDescriptorSet;
    descriptorWrite.dstBinding = dstBinding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(vlkContext.vkDevice, 1, &descriptorWrite, 0, VK_NULL_HANDLE);
}


void vlkBindImageViewToDescriptorSet(const VkImageView& vkImageView, const VkDescriptorSet& vkDescriptorSet, uint32_t dstBinding) {  
    VkDescriptorImageInfo vkDescriptorImageInfo = {};
    vkDescriptorImageInfo.sampler = vlkContext.sampler;
    vkDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkDescriptorImageInfo.imageView = vkImageView;

    VkWriteDescriptorSet vkWriteDescriptorSet = {};
    vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    vkWriteDescriptorSet.dstSet = vkDescriptorSet;
    vkWriteDescriptorSet.dstBinding = dstBinding;
    vkWriteDescriptorSet.descriptorCount = 1;
    vkWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vkWriteDescriptorSet.pImageInfo = &vkDescriptorImageInfo;

    vkUpdateDescriptorSets(vlkContext.vkDevice, 1, &vkWriteDescriptorSet, 0, VK_NULL_HANDLE);
}

bool vlkCreateDevice(VlkContext& vlkContext, void* window){
	/*uint32_t amountOfInstanceLayers = 0;
	vkEnumerateInstanceLayerProperties(&amountOfInstanceLayers, NULL);
	VkLayerProperties* instanceLayers = new VkLayerProperties[amountOfInstanceLayers];
	vkEnumerateInstanceLayerProperties(&amountOfInstanceLayers, instanceLayers);

	std::cout << "Amount of Instance Layers: " << amountOfInstanceLayers << std::endl;

	for (size_t i = 0; i < amountOfInstanceLayers; i++){
		std::cout << std::endl;
		std::cout << "Name:         " << instanceLayers[i].layerName << std::endl;
		std::cout << "Spec Version: " << instanceLayers[i].specVersion << std::endl;
		std::cout << "Impl Version: " << instanceLayers[i].implementationVersion << std::endl;
		std::cout << "Description:  " << instanceLayers[i].description << std::endl;
	}

	uint32_t amountOfInstanceExtensions = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &amountOfInstanceExtensions, NULL);
	VkExtensionProperties* instanceExtensions = new VkExtensionProperties[amountOfInstanceExtensions];
	vkEnumerateInstanceExtensionProperties(NULL, &amountOfInstanceExtensions, instanceExtensions);
	std::cout << std::endl;
	std::cout << "Amount of Instance Extension: " << amountOfInstanceExtensions << std::endl;

	for (size_t i = 0; i < amountOfInstanceExtensions; i++) {
		std::cout << std::endl;
		std::cout << "Name:         " << instanceExtensions[i].extensionName << std::endl;
		std::cout << "Spec Version: " << instanceExtensions[i].specVersion << std::endl;
	}*/

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "vkApplication";
	appInfo.pEngineName = "vkEngine";
	appInfo.apiVersion = VK_API_VERSION_1_4;

	const char* instanceExtensionsList[] = {
		"VK_KHR_win32_surface",
		"VK_EXT_debug_utils",
		"VK_KHR_surface",
	};

	const char* instanceLayersList[]{
		"VK_LAYER_KHRONOS_validation",
	};

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.ppEnabledExtensionNames = instanceExtensionsList;
	instanceInfo.enabledExtensionCount = ArraySize(instanceExtensionsList);
	instanceInfo.ppEnabledLayerNames = instanceLayersList;
	instanceInfo.enabledLayerCount = ArraySize(instanceLayersList);
	VK_CHECK(vkCreateInstance(&instanceInfo, 0, &vlkContext.vkInstance));

    vkExtensionInit(vlkContext.vkInstance);

	VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
	debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	debugInfo.pfnUserCallback = vlkDebugCallback;

	vkCreateDebugUtilsMessengerEXT(vlkContext.vkInstance, &debugInfo, 0, &vlkContext.vkDebugMessenger);

	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.hwnd = (HWND)window;
	surfaceInfo.hinstance = GetModuleHandleA(0);
	VK_CHECK(vkCreateWin32SurfaceKHR(vlkContext.vkInstance, &surfaceInfo, 0, &vlkContext.vkSurfaceKHR));

	vlkContext.queueFamilyIndex = 0;

	uint32_t amountOfPhysicalDevices = 0;
	//TODO: Suballocation from Main Allocation
	//VkPhysicalDevice gpus[10];
	VK_CHECK(vkEnumeratePhysicalDevices(vlkContext.vkInstance, &amountOfPhysicalDevices, 0));
	VkPhysicalDevice* vkPhysicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];
	VK_CHECK(vkEnumeratePhysicalDevices(vlkContext.vkInstance, &amountOfPhysicalDevices, vkPhysicalDevices));

	for (uint32_t i = 0; i < amountOfPhysicalDevices; i++) {
		VkPhysicalDevice vkPhysicalDevice = vkPhysicalDevices[i];
		//printStats(vkPhysicalDevice);
		if (isTypeOf(vkPhysicalDevice, VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU))
			continue;

		uint32_t amountOfqueueFamilies = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &amountOfqueueFamilies, 0);
		VkQueueFamilyProperties* vkQueueFamilyProperties = new VkQueueFamilyProperties[amountOfqueueFamilies];
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &amountOfqueueFamilies, vkQueueFamilyProperties);

		for (uint32_t j = 0; j < amountOfqueueFamilies; j++) {
			if (vkQueueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				VkBool32 surfaceSupport = VK_FALSE;
				VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, j, vlkContext.vkSurfaceKHR, &surfaceSupport));

				if (surfaceSupport) {
					vlkContext.queueFamilyIndex = j;
					vlkContext.vkPhysicalDevice = vkPhysicalDevice;
					break;
				}
			}
		}
		delete[] vkQueueFamilyProperties;
	}
	delete[] vkPhysicalDevices;

	/*uint32_t amountOfDeviceLayers = 0;
	vkEnumerateDeviceLayerProperties(vlkcontext->gpu, &amountOfDeviceLayers, NULL);
	VkLayerProperties* deviceLayers = new VkLayerProperties[amountOfDeviceLayers];
	vkEnumerateDeviceLayerProperties(vlkcontext->gpu, &amountOfDeviceLayers, deviceLayers);
	std::cout << std::endl;
	std::cout << "Amount of Device Layers: " << amountOfDeviceLayers << std::endl;

	for (size_t i = 0; i < amountOfDeviceLayers; i++) {
		std::cout << std::endl;
		std::cout << "Name:         " << deviceLayers[i].layerName << std::endl;
		std::cout << "Spec Version: " << deviceLayers[i].specVersion << std::endl;
		std::cout << "Impl Version: " << deviceLayers[i].implementationVersion << std::endl;
		std::cout << "Description:  " << deviceLayers[i].description << std::endl;
	}

	uint32_t amountOfDeviceExtensions = 0;
	vkEnumerateDeviceExtensionProperties(vlkcontext->gpu, NULL, &amountOfDeviceExtensions, NULL);
	VkExtensionProperties* deviceExtensions = new VkExtensionProperties[amountOfDeviceExtensions];
	vkEnumerateDeviceExtensionProperties(vlkcontext->gpu, NULL, &amountOfDeviceExtensions, deviceExtensions);
	std::cout << std::endl;
	std::cout << "Amount of Device Extension: " << amountOfDeviceExtensions << std::endl;
	std::cout << std::endl;
	for (size_t i = 0; i < amountOfDeviceExtensions; i++) {

		std::cout << "Name:         " << deviceExtensions[i].extensionName << std::endl;
		//std::cout << "Spec Version: " << deviceExtensions[i].specVersion << std::endl;
	}*/

	const char* deviceExtensionsList[] = {
		"VK_KHR_shader_terminate_invocation",
		"VK_KHR_dynamic_rendering",
		"VK_EXT_shader_object",
		"VK_KHR_swapchain"
	};

	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = vlkContext.queueFamilyIndex;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
	deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
    deviceFeatures.wideLines = VK_TRUE;

	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering = {};
	dynamicRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	dynamicRendering.dynamicRendering = true;

	VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures2.features = deviceFeatures;
	deviceFeatures2.pNext = &dynamicRendering;

	VkPhysicalDeviceShaderObjectFeaturesEXT deviceShaderObjectFeatures = {};
	deviceShaderObjectFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;
	deviceShaderObjectFeatures.shaderObject = VK_TRUE;
	deviceShaderObjectFeatures.pNext = &deviceFeatures2;

	VkPhysicalDeviceSynchronization2Features deviceSynchronization2Features = {};
	deviceSynchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	deviceSynchronization2Features.synchronization2 = VK_TRUE;
	deviceSynchronization2Features.pNext = &deviceShaderObjectFeatures;

	VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures = {};
	indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	indexingFeatures.pNext = &deviceSynchronization2Features;
	indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
	indexingFeatures.runtimeDescriptorArray = VK_TRUE;

    VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT unusedAttachmentsFeatures = {};
    unusedAttachmentsFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT;
    unusedAttachmentsFeatures.dynamicRenderingUnusedAttachments = VK_TRUE;
    unusedAttachmentsFeatures.pNext = &indexingFeatures;

	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.ppEnabledExtensionNames = deviceExtensionsList;
	deviceInfo.enabledExtensionCount = ArraySize(deviceExtensionsList);
	//deviceInfo.pEnabledFeatures = &deviceFeatures;
	deviceInfo.pEnabledFeatures = NULL;
	deviceInfo.pNext = &unusedAttachmentsFeatures;

	vkCreateDevice(vlkContext.vkPhysicalDevice, &deviceInfo, 0, &vlkContext.vkDevice);

    const VkFormat depthFormats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };


    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlagBits features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (uint32_t i = 0; i < 3; i++) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(vlkContext.vkPhysicalDevice, depthFormats[i], &properties);
        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
            vlkContext.vkDepthFormat = depthFormats[i];
        }else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
            vlkContext.vkDepthFormat = depthFormats[i];
        }
    }
    
	return true;
}

void VlkContext::createShaders(const VkDevice& vkDevice){  
    std::vector<uint32_t> vertexCode, fragmentCode;
    vlkCompileSahder("res/shader/mesh.vert", shaderc_vertex_shader, vertexCode);
    vlkCompileSahder("res/shader/mesh.frag", shaderc_fragment_shader, fragmentCode);
    vlkCreateShader(vkDescriptorSetLayouts,  vertexCode, fragmentCode, shader);
}

void VlkContext::createDescriptorSetLayout(const VkDevice& vkDevice, std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts) {
    vkDescriptorSetLayouts.resize(2);

    VkDescriptorSetLayoutBinding bindings[2]{};
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = maxDescriptorCount;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    bindings[1].binding = 0;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = maxDescriptorCount;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorBindingFlags bindingFlags[] = { VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT };
    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo = {};
    bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingFlagsCreateInfo.pBindingFlags = bindingFlags;
    bindingFlagsCreateInfo.bindingCount = 1;

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = &bindingFlagsCreateInfo;
    createInfo.bindingCount = 1;
    createInfo.pBindings = &bindings[0];

    vkCreateDescriptorSetLayout(vkDevice, &createInfo, nullptr, &vkDescriptorSetLayouts[0]);

    bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingFlagsCreateInfo.pBindingFlags = bindingFlags;
    bindingFlagsCreateInfo.bindingCount = 1;

    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = &bindingFlagsCreateInfo;
    createInfo.bindingCount = 1;
    createInfo.pBindings = &bindings[1];

    vkCreateDescriptorSetLayout(vkDevice, &createInfo, nullptr, &vkDescriptorSetLayouts[1]);
}