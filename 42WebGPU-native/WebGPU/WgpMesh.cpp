#include <iostream>
#include "WgpContext.h"
#include "WgpMesh.h"
#include "WgpData.h"

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath, const WGPUTextureView& textureView, const WGPUBuffer& uniformBuffer, unsigned int stride) :
	vertexBuffer(vertexBuffer),
	indexBuffer(indexBuffer),
	textureView(textureView), 
	uniformBuffer(uniformBuffer), 
	m_stride(stride),
	m_textureView(NULL),
	m_bindGroup(NULL),
	m_bindGroupWireframe(NULL),
	m_drawCount(indexBuffer.size()),
	m_renderPipelineSlot(RP_PTN),
	m_markForDelete(false){

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_texture.loadFromFile(texturePath);

	m_textureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureAspect::WGPUTextureAspect_All, m_texture.m_texture);
	m_bindGroup = createBindGroup(m_textureView);
}

WgpMesh::WgpMesh(WgpMesh const& rhs) : 
	vertexBuffer(rhs.vertexBuffer),
	indexBuffer(rhs.indexBuffer),
	textureView(rhs.textureView),
	uniformBuffer(rhs.uniformBuffer),
	m_stride(rhs.m_stride),
	m_vertexBuffer(rhs.m_vertexBuffer),
	m_indexBuffer(rhs.m_indexBuffer),
	m_colorBuffer(rhs.m_colorBuffer),
	m_texture(rhs.m_texture),
	m_textureView(rhs.m_textureView),
	m_bindGroup(rhs.m_bindGroup),
	m_bindGroupWireframe(rhs.m_bindGroupWireframe),
	m_drawCount(rhs.m_drawCount),
	m_renderPipelineSlot(rhs.m_renderPipelineSlot),
	m_markForDelete(false) {
}

WgpMesh::WgpMesh(WgpMesh&& rhs) noexcept :
	vertexBuffer(rhs.vertexBuffer),
	indexBuffer(rhs.indexBuffer),
	textureView(rhs.textureView),
	uniformBuffer(rhs.uniformBuffer),
	m_stride(rhs.m_stride),
	m_vertexBuffer(rhs.m_vertexBuffer),
	m_indexBuffer(rhs.m_indexBuffer),
	m_colorBuffer(rhs.m_colorBuffer),
	m_texture(rhs.m_texture),
	m_textureView(rhs.m_textureView),
	m_bindGroup(rhs.m_bindGroup),
	m_bindGroupWireframe(rhs.m_bindGroupWireframe),
	m_drawCount(rhs.m_drawCount),
	m_renderPipelineSlot(rhs.m_renderPipelineSlot),
	m_markForDelete(false) {
}

WgpMesh::~WgpMesh() {
	if (m_markForDelete) {
		cleanup();
	}
}

void WgpMesh::cleanup() {
	wgpuTextureViewRelease(m_textureView);
	m_textureView = nullptr;

	wgpuBindGroupRelease(m_bindGroup);
	m_bindGroup = nullptr;

	if (m_bindGroupWireframe) {
		wgpuBindGroupRelease(m_bindGroupWireframe);
		m_bindGroup = nullptr;
	}
}

void WgpMesh::markForDelete() {
	m_vertexBuffer.markForDelete();
	m_indexBuffer.markForDelete();
	m_texture.markForDelete();
	m_markForDelete = true;
}

void WgpMesh::setRenderPipelineSlot(RenderPipelineSlot renderPipelineSlot) {
	m_renderPipelineSlot = renderPipelineSlot;

	if (m_renderPipelineSlot == RP_WIREFRAME && m_bindGroupWireframe == NULL)
		m_bindGroupWireframe = createBindGroupWireframe();

}

WGPUBindGroup WgpMesh::createBindGroup(const WGPUTextureView& textureView) {
	std::vector<WGPUBindGroupEntry> bindings(3);

	bindings[0].binding = 0;
	bindings[0].buffer = uniformBuffer;
	bindings[0].offset = 0;
	bindings[0].size = sizeof(Uniforms);

	bindings[1].binding = 1;
	bindings[1].textureView = textureView;

	bindings[2].binding = 2;
	bindings[2].sampler = wgpContext.getSampler(SS_LINEAR);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at(RP_PTN), 0);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup WgpMesh::createBindGroupWireframe() {
	uint32_t numVertices = vertexBuffer.size() / m_stride;
	std::vector<float> colors(4u * numVertices);
	float x = 0.0f, y = 0.0f, z = 0.0f;
	for (uint32_t i = 0; i < numVertices; i++) {
		colors[4 * i + 0] = vertexBuffer[m_stride * i + 0];
		colors[4 * i + 1] = vertexBuffer[m_stride * i + 1];
		colors[4 * i + 2] = vertexBuffer[m_stride * i + 2];
		colors[4 * i + 3] = 1.0f;
	}
	m_colorBuffer.createBuffer(reinterpret_cast<const void*>(colors.data()), sizeof(float) * colors.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_colorBuffer.markForDelete();

	std::vector<WGPUBindGroupEntry> bindings(4);

	bindings[0].binding = 0;
	bindings[0].buffer = uniformBuffer;
	bindings[0].offset = 0;
	bindings[0].size = sizeof(Uniforms);

	bindings[1].binding = 1;
	bindings[1].buffer = m_indexBuffer.m_buffer;
	bindings[1].offset = 0;
	bindings[1].size = sizeof(unsigned int) * indexBuffer.size();

	bindings[2].binding = 2;
	bindings[2].buffer = m_vertexBuffer.m_buffer;
	bindings[2].offset = 0;
	bindings[2].size = sizeof(float) * vertexBuffer.size();

	bindings[3].binding = 3;
	bindings[3].buffer = m_colorBuffer.m_buffer;
	bindings[3].offset = 0;
	bindings[3].size = sizeof(float) * colors.size();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at(RP_WIREFRAME), 0);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

void WgpMesh::draw(const WGPURenderPassEncoder& renderPass) const {
	if (m_renderPipelineSlot == RP_WIREFRAME) {
		wgpuRenderPassEncoderSetPipeline(renderPass, wgpContext.renderPipelines.at(m_renderPipelineSlot));
		wgpuRenderPassEncoderSetBindGroup(renderPass, 0u, m_bindGroupWireframe, 0u, NULL);

		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 1u, m_indexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 2u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 5u, m_colorBuffer.getWgpuBuffer(), 0u, wgpuBufferGetSize(m_colorBuffer.getWgpuBuffer()));

		const uint32_t num_triangles = indexBuffer.size() / 3u;
		wgpuRenderPassEncoderDraw(renderPass, 6 * num_triangles, 1, 0, 0);
	}else {
		wgpuRenderPassEncoderSetPipeline(renderPass, wgpContext.renderPipelines.at(m_renderPipelineSlot));
		wgpuRenderPassEncoderSetBindGroup(renderPass, 0u, m_bindGroup, 0u, NULL);

		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
		wgpuRenderPassEncoderSetIndexBuffer(renderPass, m_indexBuffer.m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
		wgpuRenderPassEncoderDrawIndexed(renderPass, m_drawCount, 1u, 0u, 0u, 0u);
	}
}