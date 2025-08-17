#pragma once

#include <vulkan/vulkan.h>

class VlkBuffer {
public:

	VlkBuffer();
	VlkBuffer(VlkBuffer const& rhs);
	VlkBuffer(VlkBuffer&& rhs) noexcept;


	void createBuffer(const void* data, uint32_t size, VkBufferUsageFlagBits vkBufferUsageFlagBits);


	VkBuffer m_vkBuffer;
	VkDeviceMemory m_vkDeviceMemory;
};