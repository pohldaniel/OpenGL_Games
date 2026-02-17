#pragma once
#include <vector>
#include <string>
#include "WgpBuffer.h"
#include "WgpTexture.h"

class WgpMesh {

	friend class WgpModel;

public:

	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath, const WGPUTextureView& textureView, const WgpBuffer& uniformBuffer, unsigned int stride);
	WgpMesh(WgpMesh const& rhs);
	WgpMesh(WgpMesh&& rhs) noexcept;
	~WgpMesh();

	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;
	void drawRaw(const WGPURenderPassEncoder& renderPassEncoder) const;

	void cleanup();
	void markForDelete();
	void setRenderPipelineSlot(const std::string& renderPipelineSlot);
	void setBindGroup(const std::function<WGPUBindGroup(const WGPUTextureView textureView)>& onBindGroup);
	void setBindGroupPTN(const std::function<WGPUBindGroup(const WGPUBuffer& buffer, const WGPUTextureView& textureView)>& onBindGroup);
	void setBindGroupWF(const std::function< WGPUBindGroup(const WGPUBuffer& uniformBuffer, const WGPUBuffer& vertexBuffer, const WGPUBuffer& indexBuffer)> & onBindGroup);

private:

	WgpBuffer m_vertexBuffer;
	WgpBuffer m_indexBuffer;
	WgpBuffer m_colorBuffer;
	WgpTexture m_texture;
	WGPUTextureView m_textureView;
	WGPUBindGroup m_bindGroupWF, m_bindGroup;

	uint32_t m_drawCount;
	std::string m_renderPipelineSlot;
	bool m_markForDelete;
	unsigned int m_stride;

	const WGPUTextureView& textureView;
	const WgpBuffer& uniformBuffer;
	const std::vector<float>& vertexBuffer;
	const std::vector<unsigned int>& indexBuffer;	
};