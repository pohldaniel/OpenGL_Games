#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <Vulkan/VlkBuffer.h>
#include <Vulkan/VlkTexture.h>

class VlkMesh {

public:

	VlkMesh(const VlkBuffer& vlkBufferVertex, const VlkBuffer& vlkBufferIndex, const VlkTexture& vlkTexture, const uint32_t drawCount);
	VlkMesh(VlkMesh const& rhs);
	VlkMesh(VlkMesh&& rhs) noexcept;

	std::vector<VkShaderEXT> m_shader;

	void draw(const VkCommandBuffer& vkCommandBuffer) const;
	void setShader(std::vector<VkShaderEXT>& shader);


	const VlkBuffer& vlkBufferVertex;
	const VlkBuffer& vlkBufferIndex;
	const VlkTexture& vlkTexture;
	const uint32_t drawCount;
};