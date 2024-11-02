#pragma once
#include "vk_types.h"

struct VkContext{
	VkExtent2D screenSize;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkSurfaceFormatKHR surfaceFormat;
	VkPhysicalDevice gpu;
	VkDevice device;
	VkQueue graphicsQueue;
	VkSwapchainKHR swapchain;
	VkRenderPass renderPass;
	VkCommandPool commandPool;
	VkCommandBuffer cmd;

	// TODO: Will be inside an array
	Image image;

	Buffer stagingBuffer;

	VkDescriptorPool descPool;

	// TODO: We will abstract this later
	VkSampler sampler;
	VkDescriptorSet descSet;
	VkDescriptorSetLayout setLayout;
	VkPipelineLayout pipeLayout;
	//VkPipeline pipeline;
	VkPipeline solid{ VK_NULL_HANDLE };
	VkPipeline wireframe{ VK_NULL_HANDLE };

	VkSemaphore aquireSemaphore;
	VkSemaphore submitSemaphore;
	VkFence imgAvailableFence;

	uint32_t scImgCount;
	// TODO: Suballocation from Main Memory
	VkImage scImages[5];
	VkImageView scImgViews[5];
	VkFramebuffer framebuffers[5];

	int graphicsIdx;
	bool enableWireFrame;
};

extern "C" {
	bool vk_init(VkContext *vkcontext, void *window);
	bool vk_render(VkContext *vkcontext);
	void setWireFrame(VkContext *vkcontext, bool flag);
}
