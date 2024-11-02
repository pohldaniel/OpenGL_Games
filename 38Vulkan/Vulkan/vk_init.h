#pragma once
#include <vulkan/vulkan.h>

extern "C" {
	VkCommandBufferBeginInfo cmd_begin_info();
	VkCommandBufferAllocateInfo cmd_alloc_info(VkCommandPool pool);
	VkFenceCreateInfo fence_info(VkFenceCreateFlags flags = 0);
	VkSubmitInfo submit_info(VkCommandBuffer* cmd, uint32_t cmdCount = 1);
}