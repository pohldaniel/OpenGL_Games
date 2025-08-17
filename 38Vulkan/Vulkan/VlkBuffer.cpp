#include <Vulkan/VlkContext.h>
#include "VlkBuffer.h"

VlkBuffer::VlkBuffer(): m_vkBuffer(VK_NULL_HANDLE), m_vkDeviceMemory(VK_NULL_HANDLE) {

}

VlkBuffer::VlkBuffer(VlkBuffer const& rhs) : m_vkBuffer (rhs.m_vkBuffer), m_vkDeviceMemory(rhs.m_vkDeviceMemory) {

}

VlkBuffer::VlkBuffer(VlkBuffer&& rhs) noexcept : m_vkBuffer(std::move(rhs.m_vkBuffer)), m_vkDeviceMemory(std::move(rhs.m_vkDeviceMemory)) {

}

void VlkBuffer::createBuffer(const void* data, uint32_t size, VkBufferUsageFlagBits vkBufferUsageFlagBits) {
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	vlkCreateBuffer(stagingBuffer, stagingBufferMemory, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vlkMapBuffer(stagingBufferMemory, data, size);
	vlkCreateBuffer(m_vkBuffer, m_vkDeviceMemory, size, vkBufferUsageFlagBits | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vlkCopyBuffer(stagingBuffer, m_vkBuffer, size);

	vkDestroyBuffer(vlkContext.vkDevice, stagingBuffer, VK_NULL_HANDLE);
	vkFreeMemory(vlkContext.vkDevice, stagingBufferMemory, VK_NULL_HANDLE);
}