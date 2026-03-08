#pragma once
#include <vector>
#include <string>
#include "WgpBuffer.h"
#include "WgpTexture.h"

class WgpMesh {

	friend class WgpModel;

public:

	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer);
	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath);
	
	WgpMesh(WgpMesh const& rhs);
	WgpMesh(WgpMesh&& rhs) noexcept;
	~WgpMesh();

	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;

	void cleanup();
	void markForDelete();
	void setRenderPipelineSlot(const std::string& renderPipelineSlot);
	void setBindGroupsSlot(const std::string& bindGroupsSlot);

	void setBindGroups(std::string bindGroupsName, const std::function<std::vector<WGPUBindGroup>()>& onBindGroups);
	void addBindGroup(std::string bindGroupsName, WGPUBindGroup bindGroup) const;

	std::vector<WGPUBindGroup>& getBindGroups(std::string bindGroupsName) const;
	std::vector<WGPUBindGroup>& getBindGroups() const;

	const WgpTexture& getTexture() const;
	const WgpBuffer& getVertexBuffer() const;
	const WgpBuffer& getIndexBuffer() const;
	const uint32_t getDrawCount() const;

private:

	WgpBuffer m_vertexBuffer;
	WgpBuffer m_indexBuffer;
	WgpTexture m_texture;
	mutable std::unordered_map<std::string, std::vector<WGPUBindGroup>> m_bindGroups;

	uint32_t m_drawCount;
	std::string m_renderPipelineSlot, m_bindGroupsSlot;
	bool m_markForDelete;

	const std::vector<float>& vertexBuffer;
	const std::vector<unsigned int>& indexBuffer;	
};