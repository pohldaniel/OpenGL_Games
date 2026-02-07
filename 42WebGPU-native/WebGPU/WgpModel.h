#pragma once
#include <list>
#include <webgpu.h>

class WgpMesh;
class WgpBuffer;
class ObjModel;
enum RenderPipelineSlot;

class WgpModel {

public:

	WgpModel() = default;
	WgpModel(WgpModel const& rhs);
	WgpModel(WgpModel&& rhs) noexcept;

	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;
	void drawRaw(const WGPURenderPassEncoder& renderPassEncoder) const;

	void create(const ObjModel& model, const WGPUTextureView& textureView, const WgpBuffer& uniformBuffer);
	void setRenderPipelineSlot(const std::string& renderPipelineSlot);
	void setBindGroup(const std::function <WGPUBindGroup(const WGPUTextureView textureView)>& onBindGroup);
	void setBindGroupPTN(const std::function <WGPUBindGroup(const WGPUBuffer& buffer, const WGPUTextureView& textureView)>& onBindGroup);
	void setBindGroupWF(const std::function< WGPUBindGroup(const WGPUBuffer& uniformBuffer, const WGPUBuffer& vertexBuffer, const WGPUBuffer& indexBuffer)>& onBindGroup);

private:

	std::list<WgpMesh> m_meshes;
};