#include <iostream>
#include "WgpContext.h"
#include "WgpMesh.h"
#include "WgpData.h"

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath, const WGPUTextureView& textureView, const WgpBuffer& uniformBuffer, unsigned int stride) :
	m_textureView(NULL),
	m_bindGroupWF(NULL),
	m_bindGroup(NULL),
	m_drawCount(indexBuffer.size()),
	m_renderPipelineSlot("RP_PTN"),
	m_markForDelete(false),
	m_stride(stride),
	textureView(textureView),
	uniformBuffer(uniformBuffer),
	vertexBuffer(vertexBuffer),
	indexBuffer(indexBuffer) {

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_texture.loadFromFile(texturePath);
	m_textureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureAspect::WGPUTextureAspect_All, m_texture.m_texture);
}

WgpMesh::WgpMesh(WgpMesh const& rhs) : 
	m_vertexBuffer(rhs.m_vertexBuffer),
	m_indexBuffer(rhs.m_indexBuffer),
	m_colorBuffer(rhs.m_colorBuffer),
	m_texture(rhs.m_texture),
	m_textureView(rhs.m_textureView),
	m_bindGroupWF(rhs.m_bindGroupWF),
	m_bindGroup(rhs.m_bindGroup),
	m_drawCount(rhs.m_drawCount),
	m_renderPipelineSlot(rhs.m_renderPipelineSlot),
	m_markForDelete(false),
	m_stride(rhs.m_stride),
	textureView(rhs.textureView),
	uniformBuffer(rhs.uniformBuffer),
	vertexBuffer(rhs.vertexBuffer),
	indexBuffer(rhs.indexBuffer) {
}

WgpMesh::WgpMesh(WgpMesh&& rhs) noexcept :
	m_vertexBuffer(std::move(rhs.m_vertexBuffer)),
	m_indexBuffer(std::move(rhs.m_indexBuffer)),
	m_colorBuffer(std::move(rhs.m_colorBuffer)),
	m_texture(std::move(rhs.m_texture)),
	m_textureView(std::move(rhs.m_textureView)),
	m_bindGroupWF(std::move(rhs.m_bindGroupWF)),
	m_bindGroup(std::move(rhs.m_bindGroup)),
	m_drawCount(rhs.m_drawCount),
	m_renderPipelineSlot(rhs.m_renderPipelineSlot),
	m_markForDelete(false),
	m_stride(rhs.m_stride),	
	textureView(rhs.textureView),
	uniformBuffer(rhs.uniformBuffer),
	vertexBuffer(rhs.vertexBuffer),
	indexBuffer(rhs.indexBuffer) {
}

WgpMesh::~WgpMesh() {
	if (m_markForDelete) {
		cleanup();
	}
}

void WgpMesh::cleanup() {
	wgpuTextureViewRelease(m_textureView);
	m_textureView = NULL;

	if (m_bindGroupWF) {
		wgpuBindGroupRelease(m_bindGroupWF);
		m_bindGroupWF = NULL;
	}

	if (m_bindGroup) {
		wgpuBindGroupRelease(m_bindGroup);
		m_bindGroup = NULL;
	}
}

void WgpMesh::markForDelete() {
	m_vertexBuffer.markForDelete();
	m_indexBuffer.markForDelete();
	m_texture.markForDelete();
	m_markForDelete = true;
}

void WgpMesh::setRenderPipelineSlot(const std::string& renderPipelineSlot) {
	m_renderPipelineSlot = renderPipelineSlot;
}

void WgpMesh::setBindGroup(const std::function <WGPUBindGroup(const WGPUTextureView textureView)>& onBindGroup) {
	m_bindGroup = onBindGroup(m_textureView);
}

void WgpMesh::setBindGroupPTN(const std::function <WGPUBindGroup(const WGPUBuffer& buffer, const WGPUTextureView& textureView)>& onBindGroup) {
	m_bindGroup = onBindGroup(uniformBuffer.getBuffer(), m_textureView);
}

void WgpMesh::setBindGroupWF(const std::function <WGPUBindGroup(const WGPUBuffer& uniformBuffer, const WGPUBuffer& vertexBuffer, const WGPUBuffer& indexBuffer)>& onBindGroup) {
	m_bindGroupWF = onBindGroup(uniformBuffer.getBuffer(), m_vertexBuffer.getBuffer(), m_indexBuffer.getBuffer());
}

void WgpMesh::draw(const WGPURenderPassEncoder& renderPassEncoder) const {
	if (m_renderPipelineSlot == "RP_WF") {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WF"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupWF, 0u, NULL);

		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 1u, m_indexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 2u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));

		const uint32_t num_triangles = indexBuffer.size() / 3u;
		wgpuRenderPassEncoderDraw(renderPassEncoder, 6 * num_triangles, 1, 0, 0);
	}else {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at(m_renderPipelineSlot));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroup, 0u, NULL);

		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, m_drawCount, 1u, 0u, 0u, 0u);
	}
}

void WgpMesh::drawRaw(const WGPURenderPassEncoder& renderPassEncoder) const {
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroup, 0u, NULL);
	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
	wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
	wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, m_drawCount, 1u, 0u, 0u, 0u);
}