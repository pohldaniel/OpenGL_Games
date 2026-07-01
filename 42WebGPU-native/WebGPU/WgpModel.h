#pragma once
#include <list>
#include <webgpu.h>

#include "WgpMesh.h"

class ObjModel;
class AssimpModel;
class AnimatedModel;
class Shape;

class WgpModel {

public:

	WgpModel() = default;
	WgpModel(WgpModel const& rhs);
	WgpModel(WgpModel&& rhs) noexcept;

	void draw(const WGPURenderPassEncoder& renderPassEncoder, uint32_t instanceCount = 1u) const;
	void draw(const WGPURenderBundleEncoder& renderBundleEncoder, uint32_t instanceCount = 1u) const;

	void create(const ObjModel& model);
	void create(const AssimpModel& model);
	void create(const AnimatedModel& model);
	void create(const Shape& shape);
	void create(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer);
	void create(const WgpBuffer& vertexBuffer, const WgpBuffer& indexBuffer);

	void markForDelete();

	void setBindGroupsSlot(const std::string& bindGroupsSlot);
	void setBindGroups(const std::string& bindGroupsName, const std::function <std::vector<WGPUBindGroup>()>& onBindGroups);
	void addBindGroups(const std::string& bindGroupsName, const std::function <std::vector<WGPUBindGroup>()>& onBindGroups);
	void addBindGroup(const std::string& bindGroupsName, WGPUBindGroup bindGroup) const;
	void addColor(std::array<float, 4> color);

	const std::list<WgpMesh>& getMeshes() const;

private:

	mutable std::list<WgpMesh> m_meshes;
};