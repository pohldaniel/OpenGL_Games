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

	void draw(const WGPURenderPassEncoder& renderPass) const;
	void create(const ObjModel& model, const WGPUTextureView& textureView, const WgpBuffer& uniformBuffer);
	void setRenderPipelineSlot(RenderPipelineSlot renderPipelineSlot);

private:

	std::list<WgpMesh> m_meshes;
};