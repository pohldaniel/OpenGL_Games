#pragma once
#include <vector>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpTexture.h>

enum RenderPipelineSlot;

class WgpMesh {

public:

	WgpMesh(const WgpBuffer& wgpBufferVertex, const WgpBuffer& wgpBufferIndex, const WgpTexture& wgpTexture, const uint32_t drawCount);
	WgpMesh(WgpMesh const& rhs);
	WgpMesh(WgpMesh&& rhs) noexcept;


	void draw(const WGPURenderPassEncoder& renderPass) const;

	const WgpBuffer& wgpBufferVertex;
	const WgpBuffer& wgpBufferIndex;
	const WgpTexture& wgpTexture;
	const uint32_t m_drawCount;

	RenderPipelineSlot m_renderPipelineSlot;
};