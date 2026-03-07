#pragma once
#include <list>
#include <webgpu.h>

class WgpMesh;
class WgpBuffer;
class ObjModel;
class Shape;
enum RenderPipelineSlot;

class WgpModel {

public:

	WgpModel() = default;
	WgpModel(WgpModel const& rhs);
	WgpModel(WgpModel&& rhs) noexcept;

	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;
	void drawRaw(const WGPURenderPassEncoder& renderPassEncoder) const;

	void create(const ObjModel& model, const WgpBuffer& uniformBuffer);
	void create(const Shape& shape, const WgpBuffer& uniformBuffer);
	void setRenderPipelineSlot(const std::string& renderPipelineSlot);
	void setBindGroupPTN(const std::function <WGPUBindGroup(const WGPUBuffer& buffer, const WGPUTextureView& textureView)>& onBindGroup);
	void setBindGroupWF(const std::function< WGPUBindGroup(const WGPUBuffer& uniformBuffer, const WGPUBuffer& vertexBuffer, const WGPUBuffer& indexBuffer)>& onBindGroup);
	void setBindGroups(const std::function <std::vector<WGPUBindGroup>()>& onBindGroups);

	std::list<WgpMesh>& getMeshes() const;

private:

	mutable std::list<WgpMesh> m_meshes;
};