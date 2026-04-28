#pragma once
#include <list>
#include <webgpu.h>

#include "WgpMesh.h"

class ObjModel;
class AssimpModel;
class Shape;

class WgpModel {

public:

	WgpModel() = default;
	WgpModel(WgpModel const& rhs);
	WgpModel(WgpModel&& rhs) noexcept;

	void draw(const WGPURenderPassEncoder& renderPassEncoder, uint32_t instanceCount = 1u) const;

	void create(const ObjModel& model);
	void create(const AssimpModel& model);
	void create(const Shape& shape);

	void markForDelete();

	void setBindGroupsSlot(const std::string& bindGroupsSlot);
	void setBindGroups(std::string bindGroupsName, const std::function <std::vector<WGPUBindGroup>()>& onBindGroups);

	const std::list<WgpMesh>& getMeshes() const;

private:

	mutable std::list<WgpMesh> m_meshes;
};