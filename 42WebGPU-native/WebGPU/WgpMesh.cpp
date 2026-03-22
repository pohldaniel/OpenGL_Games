#include "WgpContext.h"
#include "WgpMesh.h"

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer) :
	m_drawCount(indexBuffer.size()),
	m_renderPipelineSlot("RP_PTN"),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false),
	vertexBuffer(vertexBuffer),
	indexBuffer(indexBuffer) {

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Storage);
}

WgpMesh::WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath) :
	m_drawCount(indexBuffer.size()),
	m_renderPipelineSlot("RP_PTN"),
	m_bindGroupsSlot("BG"),
	m_markForDelete(false),
	vertexBuffer(vertexBuffer),
	indexBuffer(indexBuffer) {

	m_vertexBuffer.createBuffer(reinterpret_cast<const void*>(vertexBuffer.data()), sizeof(float) * vertexBuffer.size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_indexBuffer.createBuffer(reinterpret_cast<const void*>(indexBuffer.data()), sizeof(unsigned int) * indexBuffer.size(), WGPUBufferUsage_Index | WGPUBufferUsage_Storage);
	m_texture.loadFromFile(texturePath);
}

WgpMesh::WgpMesh(WgpMesh const& rhs) :
	m_vertexBuffer(rhs.m_vertexBuffer),
	m_indexBuffer(rhs.m_indexBuffer),
	m_texture(rhs.m_texture),
	m_bindGroups(rhs.m_bindGroups),
	m_drawCount(rhs.m_drawCount),
	m_renderPipelineSlot(rhs.m_renderPipelineSlot),
	m_bindGroupsSlot(rhs.m_bindGroupsSlot),
	m_markForDelete(false),
	vertexBuffer(rhs.vertexBuffer),
	indexBuffer(rhs.indexBuffer) {
}

WgpMesh::WgpMesh(WgpMesh&& rhs) noexcept :
	m_vertexBuffer(std::move(rhs.m_vertexBuffer)),
	m_indexBuffer(std::move(rhs.m_indexBuffer)),
	m_texture(std::move(rhs.m_texture)),
	m_bindGroups(std::move(rhs.m_bindGroups)),
	m_drawCount(rhs.m_drawCount),
	m_renderPipelineSlot(rhs.m_renderPipelineSlot),
	m_bindGroupsSlot(rhs.m_bindGroupsSlot),
	m_markForDelete(false),
	vertexBuffer(rhs.vertexBuffer),
	indexBuffer(rhs.indexBuffer) {
}

WgpMesh::~WgpMesh() {
	if (m_markForDelete) {
		cleanup();
	}
}

void WgpMesh::cleanup() {		
	for (auto& it : m_bindGroups) {
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
	m_bindGroups[bindGroupsName] = onBindGroups();
	m_bindGroupsSlot = bindGroupsName;
}

void WgpMesh::addBindGroup(std::string bindGroupsName, WGPUBindGroup bindGroup) const {
	m_bindGroups[bindGroupsName].push_back(bindGroup);
}

std::vector<WGPUBindGroup>& WgpMesh::getBindGroups(std::string bindGroupsName) const {
	return m_bindGroups.at(bindGroupsName);
}

std::vector<WGPUBindGroup>& WgpMesh::getBindGroups() const {
	return m_bindGroups.at(m_bindGroupsSlot);
}

const WgpTexture& WgpMesh::getTexture() const {
	return m_texture;
}

const WgpBuffer& WgpMesh::getVertexBuffer() const {
	return m_vertexBuffer;
}

const WgpBuffer& WgpMesh::getIndexBuffer() const {
	return m_indexBuffer;
}

const uint32_t WgpMesh::getDrawCount() const {
	return m_drawCount;
}

void WgpMesh::draw(const WGPURenderPassEncoder& renderPassEncoder) const {
	const std::vector<WGPUBindGroup>& bindGroups = m_bindGroups.at(m_bindGroupsSlot);

	for (uint32_t i = 0u; i < bindGroups.size(); i++) {
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, i, bindGroups[i], 0u, NULL);
	}
	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_vertexBuffer.m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.m_buffer));
	wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.m_buffer));
	wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, m_drawCount, 1u, 0u, 0u, 0u);
}