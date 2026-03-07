#pragma once
#include <vector>
#include <string>
#include "WgpBuffer.h"
#include "WgpTexture.h"

class WgpMesh {

	friend class WgpModel;

public:

	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const WgpBuffer& uniformBuffer);
	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath, const WgpBuffer& uniformBuffer);
	
	WgpMesh(WgpMesh const& rhs);
	WgpMesh(WgpMesh&& rhs) noexcept;
	~WgpMesh();

	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;
	void drawRaw(const WGPURenderPassEncoder& renderPassEncoder) const;

	void cleanup();
	void markForDelete();
	void setRenderPipelineSlot(const std::string& renderPipelineSlot);
	void setBindGroup(const std::function<WGPUBindGroup(const WGPUTextureView textureView)>& onBindGroup);
	void setBindGroupNormal(const std::function<WGPUBindGroup()>& onBindGroup);
	void setBindGroupPTN(const std::function<WGPUBindGroup(const WGPUBuffer& buffer, const WGPUTextureView& textureView)>& onBindGroup);
	void setBindGroupWF(const std::function< WGPUBindGroup(const WGPUBuffer& uniformBuffer, const WGPUBuffer& vertexBuffer, const WGPUBuffer& indexBuffer)> & onBindGroup);

	void setBindGroups(const std::function<std::vector<WGPUBindGroup>()>& onBindGroups);
	std::vector<WGPUBindGroup>& getBindGroups();

private:

	WgpBuffer m_vertexBuffer;
	WgpBuffer m_indexBuffer;
	WgpBuffer m_colorBuffer;
	WgpTexture m_texture;
	WGPUBindGroup m_bindGroupWF, m_bindGroup;
	std::vector<WGPUBindGroup> m_bindGroups;

	uint32_t m_drawCount;
	std::string m_renderPipelineSlot;
	bool m_markForDelete;

	const WgpBuffer& uniformBuffer;
	const std::vector<float>& vertexBuffer;
	const std::vector<unsigned int>& indexBuffer;	
};