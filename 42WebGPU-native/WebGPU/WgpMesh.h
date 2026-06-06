#pragma once
#include <vector>
#include <string>

#include "WgpBuffer.h"
#include "WgpTexture.h"

struct VertexAnimated {
	std::array<float, 3> position;
	std::array<float, 2> uv;
	std::array<float, 3> normal;
	std::array<float, 4> weight;
	std::array<unsigned int, 4> joint;
};

class WgpMesh {

	friend class WgpModel;

public:

	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer);
	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::vector<std::array<float, 4>>& weights, const std::vector<std::array<unsigned int, 4>>& joints, uint32_t stride);
	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::string& texturePath);
	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::vector<std::array<float, 4>>& weights, const std::vector<std::array<unsigned int, 4>>& joints, uint32_t stride, const std::string& texturePath);
	WgpMesh(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, const std::pair<unsigned char*, unsigned int>& texture);
	WgpMesh(const WgpBuffer& vertexBuffer, const WgpBuffer& indexBuffer);

	WgpMesh(WgpMesh const& rhs);
	WgpMesh(WgpMesh&& rhs) noexcept;
	~WgpMesh();

	void draw(const WGPURenderPassEncoder& renderPassEncoder, uint32_t instanceCount = 1u) const;

	void cleanup();
	void markForDelete();

	void setBindGroupsSlot(const std::string& bindGroupsSlot);
	void setBindGroups(std::string bindGroupsName, const std::function<std::vector<WGPUBindGroup>()>& onBindGroups);
	void addBindGroups(std::string bindGroupsName, const std::function<std::vector<WGPUBindGroup>()>& onBindGroups);
	void addBindGroup(std::string bindGroupsName, WGPUBindGroup bindGroup) const;
	void addColor(std::array<float, 4> color);

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
	std::string m_bindGroupsSlot;
	bool m_markForDelete;

	static void OnMapColorToBuffer(WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2);
	static void OnMapIndexBuffer(WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2);
};