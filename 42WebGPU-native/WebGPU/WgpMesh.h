#pragma once
#include <vector>
#include <string>
#include "WgpBuffer.h"
#include "WgpTexture.h"

enum RenderPipelineSlot;
class WgpMesh {

	friend class WgpModel;

public:

	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath, const WGPUTextureView& textureView, const WGPUBuffer& uniformBuffer, unsigned int stride);
	WgpMesh(WgpMesh const& rhs);
	WgpMesh(WgpMesh&& rhs) noexcept;
	~WgpMesh();

	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;
	void drawRaw(const WGPURenderPassEncoder& renderPassEncoder) const;

	void cleanup();
	void markForDelete();
	void setRenderPipelineSlot(RenderPipelineSlot renderPipelineSlot);

private:

	WGPUBindGroup createBindGroupPTN(const WGPUTextureView& textureView);
	WGPUBindGroup createBindGroupWireframe();
	void createBindGroup(const std::string& pipelineName);

	WgpBuffer m_vertexBuffer;
	WgpBuffer m_indexBuffer;
	WgpBuffer m_colorBuffer;
	WgpTexture m_texture;
	WGPUTextureView m_textureView;
	WGPUBindGroup m_bindGroupPTN, m_bindGroupWireframe, m_bindGroup;

	uint32_t m_drawCount;
	RenderPipelineSlot m_renderPipelineSlot;
	bool m_markForDelete;
	unsigned int m_stride;

	const WGPUTextureView& textureView;
	const WGPUBuffer& uniformBuffer;
	const std::vector<float>& vertexBuffer;
	const std::vector<unsigned int>& indexBuffer;	
};