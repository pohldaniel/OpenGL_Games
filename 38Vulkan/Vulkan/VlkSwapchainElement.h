#pragma once
#include <vector>
#include <functional>

struct VlkContext;
class VlkSwapchain;
class VlkTexture;
class VlkMesh;

class VlkSwapchainElement{

public:

    VlkSwapchainElement(VlkSwapchain* swapchain, VkImage image, VkImage depthImage);
    VlkSwapchainElement(const VlkSwapchainElement& rhs) = delete;
    VlkSwapchainElement(VlkSwapchainElement&& rhs) = delete;
    ~VlkSwapchainElement();

    VlkSwapchainElement& operator=(const VlkSwapchainElement& rhs) = delete;
    VlkSwapchainElement& operator=(VlkSwapchainElement&& rhs) = delete;

    void draw();
    void setOnDraw(std::function<void(const VkCommandBuffer& commandBuffer)> fun);

    VlkSwapchain* swapchain;

    VkCommandBuffer commandBuffer;

    VkImage image;
    VkImageView imageView;

    VkImage depthImage;
    VkImageView depthImageView;

    VkSemaphore startSemaphore;
    VkSemaphore endSemaphore;
    VkFence fence;
    VkFence lastFence = nullptr;

    VkRenderingAttachmentInfo colorAttachment;
    VkRenderingAttachmentInfo depthStencilAttachment;
    VkRenderingInfo renderingInfo;
    VkViewport viewport;
    VkRect2D scissor;

    std::function<void(const VkCommandBuffer& commandBuffer)> OnDraw;
};
