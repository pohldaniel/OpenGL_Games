#pragma once
#include <list>
#include <webgpu.h>

class WgpMesh;
class WgpBuffer;
class ObjModel;
class Shape;

class WgpModel {

public:

	WgpModel() = default;
	WgpModel(WgpModel const& rhs);
	WgpModel(WgpModel&& rhs) noexcept;

	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;

	void create(const ObjModel& model);
	void create(const Shape& shape);
	void setRenderPipelineSlot(const std::string& renderPipelineSlot);
	void setBindGroupsSlot(const std::string& bindGroupsSlot);
	void setBindGroups(std::string bindGroupsName, const std::function <std::vector<WGPUBindGroup>()>& onBindGroups);

	const std::list<WgpMesh>& getMeshes() const;

private:

	mutable std::list<WgpMesh> m_meshes;
};