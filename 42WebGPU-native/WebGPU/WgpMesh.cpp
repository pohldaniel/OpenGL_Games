#include <iostream>
#include "WgpContext.h"
#include "WgpMesh.h"
#include "WgpData.h"

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const WgpBuffer& uniformBuffer) :
	m_drawCount(indexBuffer.size()),
	m_renderPipelineSlot("RP_PTN"),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false),
	uniformBuffer(uniformBuffer),
	vertexBuffer(vertexBuffer),
	indexBuffer(indexBuffer) {

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
}

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath, const WgpBuffer& uniformBuffer) :
	m_drawCount(indexBuffer.size()),
	m_renderPipelineSlot("RP_PTN"),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false),
	uniformBuffer(uniformBuffer),
	vertexBuffer(vertexBuffer),
	indexBuffer(indexBuffer) {

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_texture.loadFromFile(texturePath);
}

WgpMesh::WgpMesh(WgpMesh const& rhs) :
	m_vertexBuffer(rhs.m_vertexBuffer),
	m_indexBuffer(rhs.m_indexBuffer),
	m_colorBuffer(rhs.m_colorBuffer),
	m_texture(rhs.m_texture),
	m_bindGroupsNew(rhs.m_bindGroupsNew),
	m_drawCount(rhs.m_drawCount),
	m_renderPipelineSlot(rhs.m_renderPipelineSlot),
	m_bindGroupsSlot(rhs.m_bindGroupsSlot),
	m_markForDelete(false),
	uniformBuffer(rhs.uniformBuffer),
	vertexBuffer(rhs.vertexBuffer),
	indexBuffer(rhs.indexBuffer) {
}

WgpMesh::WgpMesh(WgpMesh&& rhs) noexcept :
	m_vertexBuffer(std::move(rhs.m_vertexBuffer)),
	m_indexBuffer(std::move(rhs.m_indexBuffer)),
	m_colorBuffer(std::move(rhs.m_colorBuffer)),
	m_texture(std::move(rhs.m_texture)),
	m_bindGroupsNew(std::move(rhs.m_bindGroupsNew)),
	m_drawCount(rhs.m_drawCount),
	m_renderPipelineSlot(rhs.m_renderPipelineSlot),
	m_bindGroupsSlot(rhs.m_bindGroupsSlot),
	m_markForDelete(false),	
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
	for (auto& it : m_bindGroupsNew) {
		for (auto& bindgroup : it.second) {
			wgpuBindGroupRelease(bindgroup);
		}
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

void WgpMesh::setBindGroupsSlot(const std::string& bindGroupsSlot) {
	m_bindGroupsSlot = bindGroupsSlot;
}

void WgpMesh::setBindGroups(std::string bindGroupsName, const std::function<std::vector<WGPUBindGroup>()>& onBindGroups) {
	m_bindGroupsNew[bindGroupsName] = onBindGroups();
	m_bindGroupsSlot = bindGroupsName;
}

void WgpMesh::addBindGroupTexture(std::string bindGroupsName, WGPUBindGroupLayout layout) {
	std::vector<WGPUBindGroupEntry> bindingGroupEntries(1);
	bindingGroupEntries[0].binding = 0u;
	bindingGroupEntries[0].textureView = m_texture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = layout;
	bindGroupDesc.entryCount = (uint32_t)bindingGroupEntries.size();
	bindGroupDesc.entries = bindingGroupEntries.data();

	m_bindGroupsNew[bindGroupsName].push_back(wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc));
}

void WgpMesh::addBindGroupWF(std::string bindGroupsName, WGPUBindGroupLayout layout) {
	std::vector<WGPUBindGroupEntry> bindingGroupEntries(2);
	bindingGroupEntries[0].binding = 0u;
	bindingGroupEntries[0].buffer = m_vertexBuffer.getBuffer();
	bindingGroupEntries[0].offset = 0u;
	bindingGroupEntries[0].size = wgpuBufferGetSize(m_vertexBuffer.getBuffer());

	bindingGroupEntries[1].binding = 1u;
	bindingGroupEntries[1].buffer = m_indexBuffer.getBuffer();
	bindingGroupEntries[1].offset = 0u;
	bindingGroupEntries[1].size = wgpuBufferGetSize(m_indexBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = layout;
	bindGroupDesc.entryCount = (uint32_t)bindingGroupEntries.size();
	bindGroupDesc.entries = bindingGroupEntries.data();

	m_bindGroupsNew[bindGroupsName].push_back(wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc));
}

std::vector<WGPUBindGroup>& WgpMesh::getBindGroup(std::string bindGroupsName) const {
	return m_bindGroupsNew.at(bindGroupsName);
}

const WgpTexture& WgpMesh::getTexture() const {
	return m_texture;
}

void WgpMesh::draw(const WGPURenderPassEncoder& renderPassEncoder) const {	
	const std::vector<WGPUBindGroup>& bindGroups = m_bindGroupsNew.at(m_bindGroupsSlot);

	if (m_renderPipelineSlot == "RP_WF") {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WF"));
		for (uint32_t i = 0u; i < bindGroups.size(); i++) {
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, i, bindGroups[i], 0u, NULL);
		}

		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u * indexBuffer.size() / 3u, 1u, 0u, 0u);
	}else {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at(m_renderPipelineSlot));

		for (uint32_t i = 0u; i < bindGroups.size(); i++) {
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, i, bindGroups[i], 0u, NULL);
		}

		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, m_drawCount, 1u, 0u, 0u, 0u);
	}
}

void WgpMesh::drawRaw(const WGPURenderPassEncoder& renderPassEncoder) const {
	const std::vector<WGPUBindGroup>& bindGroups = m_bindGroupsNew.at(m_bindGroupsSlot);

	for (uint32_t i = 0u; i < bindGroups.size(); i++) {
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, i, bindGroups[i], 0u, NULL);
	}
	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
	wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
	wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, m_drawCount, 1u, 0u, 0u, 0u);
}