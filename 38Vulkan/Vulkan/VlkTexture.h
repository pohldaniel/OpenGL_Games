#pragma once

#include <string>
#include <vulkan/vulkan.h>

class VlkTexture {

	friend class VlkSwapchainElement;
	friend class VlkMesh;

public:

	VlkTexture(const VkDescriptorSetLayout& vkDescriptorSetLayout);
	VlkTexture(VlkTexture const& rhs);
	VlkTexture(VlkTexture&& rhs) noexcept;

	void loadFromFile(std::string fileName, const bool flipVertical = false);	

private:

	VkImage m_vkImage;
	VkDeviceMemory m_vkDeviceMemory;
	VkImageView m_vkImageView;
	VkDescriptorSet m_vkDescriptorSet;
	const VkDescriptorSetLayout& vkDescriptorSetLayout;

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;

	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
};