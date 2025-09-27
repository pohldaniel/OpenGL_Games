#pragma once
#include <vector>
#include <list>
#include <algorithm>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.h>

class VlkSwapchain;
class VlkMesh;
class VlkTexture;

struct VlkContext {

    const int maxDescriptorSets = 10;
    const int maxDescriptorCount = 65536;

    void createShaders(const VkDevice& vkDevice);
    void createDescriptorSetLayout(const VkDevice& vkDevice, std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts);

    VkInstance vkInstance;
    VkDevice vkDevice;
    VkPhysicalDevice vkPhysicalDevice;
    VkDebugUtilsMessengerEXT vkDebugMessenger;

    VkSurfaceKHR vkSurfaceKHR;
    VkQueue vkQueue;
    VkCommandPool vkCommandPool;
    VkCommandBuffer vkCommandBuffer;
    VkFormat vkDepthFormat;
    VkDescriptorPool vkDescriptorPool;
    uint32_t queueFamilyIndex;

    VlkSwapchain* swapchain;
    VlkSwapchain* newSwapchain;

    VkPolygonMode vkPolygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
    VkPresentModeKHR vkPresentModeKHR = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
    bool drawUi;

    //Sample scope
    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts;
    VkDescriptorSet vkDescriptorSetUbo;
    VkPipelineLayout vkPipelineLayout;
    VkSampler sampler;
    std::vector<VkShaderEXT> shader;

    static void CheckVKResult(VkResult err);
};

extern VlkContext vlkContext;
extern int maxDescriptorSets;
extern int maxDescriptorCount;

extern "C" {

    void vlkInit(void* window);
    bool vlkCreateDevice(VlkContext& vlkcontext, void* window);
    void vlkResize();
    void vlkToggleVerticalSync();
    void vlkWaitIdle();
    void vlkToggleWireframe();
    void vlkShutDown();
    void vlkToggleUI();
    void vlkSetDrawUI(bool flag);
    void vlkDraw();

    void vlkGetDeviceQueue(uint32_t queueFamilyIndex, VkQueue& vkQueue);

    void vlkMapBuffer(const VkDeviceMemory& vkDeviceMemory, const void* data, uint32_t size);
    void vlkCreateBuffer(VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory, uint32_t size, VkBufferUsageFlags vkBufferUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags);
    void vlkCopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, uint32_t size);
   
    void vlkCreateImage(VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, uint32_t width, uint32_t height, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags);
    void vlkCreateImageView(VkImageView& vkImageView, const VkImage& vkImage, VkFormat vkFormat, VkImageAspectFlags vkImageAspectFlags, VkComponentMapping vkComponentMapping = {});  
    void vlkDestroyImage(const VkImage& vkImage, const VkDeviceMemory& vkDeviceMemory = NULL);
    void vlkCopyBufferToImage(const VkBuffer& srcBuffer, const VkImage& dstImage, uint32_t width, uint32_t height);

    void vlkCreateCommandPool(VkCommandPool& vkCommandPool);
    void vlkCreateCommandBuffer(VkCommandBuffer& vkCommandBuffer);

    void vlkTransitionImageLayout(const VkCommandBuffer& commandBuffer, const VkImage& vkImage, VkImageAspectFlags vkImageAspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);  
    void vlkCreateSemaphore(VkSemaphore& vkSemaphore);
    void vlkCreateFence(VkFence& vkFence);
    void vlkBeginCommandBuffer(const VkCommandBuffer& vkCommandBuffer, VkCommandBufferUsageFlags vkCommandBufferUsageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    void vlkEndCommandBuffer(const VkCommandBuffer& vkCommandBuffer, bool submit = false);
    void vlkQueueSubmit(const VkCommandBuffer& vkCommandBuffer);

    void vlkCreateSwapChain(VkSwapchainKHR& vkSwapchainKHR, VkFormat& vkFormat, uint32_t width, uint32_t height, const VkPresentModeKHR vkPresentModeKHR = VK_PRESENT_MODE_FIFO_KHR, VkSwapchainKHR vkOldSwapchainKHR = NULL);

    void vlkReadFile(const char* filename, std::vector<char>& data);
    void vlkCompileSahder(const char* path, shaderc_shader_kind kind, std::vector<uint32_t>& shaderCode);
    std::vector<VkShaderEXT>& vlkCreateShader(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<uint32_t>& vertexCode, const std::vector<uint32_t>& fragmentCode, std::vector<VkShaderEXT>& shader);

    void vlkCreateSampler(VkSampler& vkSampler);

    uint32_t vlkFindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void vlkBindImageViewToDescriptorSet(const VkImageView& vkImageView, const VkDescriptorSet& vkDescriptorSet, uint32_t dstBinding);
  
    void vlkCreateDescriptorPool(VkDescriptorPool& vkDescriptorPool);
    void vlkAllocateDescriptorSet(VkDescriptorSet& vkDescriptorSet, const VkDescriptorSetLayout& vkDescriptorSetLayout);

    void vlkCreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<VkPushConstantRange> vkPushConstantRanges, VkPipelineLayout& vkPipelineLayout);
    void vlkCreateUniformBuffer(VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory, void*& mapping, uint32_t size);
    void vlkBindBufferToDescriptorSet(const VkBuffer& vkBuffer, const VkDescriptorSet& vkDescriptorSet, VkDescriptorType vkDescriptorType, uint32_t dstBinding);
};