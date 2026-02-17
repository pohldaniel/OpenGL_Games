#include <engine/ObjModel.h>
#include "WgpContext.h"
#include "WgpModel.h"
#include "WgpMesh.h"
#include "WgpBuffer.h"

WgpModel::WgpModel(WgpModel const& rhs) : m_meshes(rhs.m_meshes) {

}

WgpModel::WgpModel(WgpModel&& rhs) noexcept : m_meshes(rhs.m_meshes) {

}

void WgpModel::create(const ObjModel& model, const WGPUTextureView& textureView, const WgpBuffer& uniformBuffer) {
	for (ObjMesh* mesh : model.getMeshes()) {
		m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getMaterial().textures.at(TextureSlot::TEXTURE_DIFFUSE), textureView, uniformBuffer, mesh->getStride()));
	}

	for (WgpMesh& mesh : m_meshes) {
		mesh.markForDelete();
	}
}

void WgpModel::setRenderPipelineSlot(const std::string& renderPipelineSlot) {
	for (WgpMesh& mesh : m_meshes) {
		mesh.setRenderPipelineSlot(renderPipelineSlot);
	}
}

void WgpModel::setBindGroup(const std::function<WGPUBindGroup(const WGPUTextureView textureView)>& onBindGroup) {
	for (WgpMesh& mesh : m_meshes) {
		mesh.setBindGroup(onBindGroup);
	}
}

void WgpModel::setBindGroupPTN(const std::function<WGPUBindGroup(const WGPUBuffer& buffer, const WGPUTextureView& textureView)>& onBindGroup) {
	for (WgpMesh& mesh : m_meshes) {
		mesh.setBindGroupPTN(onBindGroup);
	}
}

void WgpModel::setBindGroupWF(const std::function<WGPUBindGroup(const WGPUBuffer& uniformBuffer, const WGPUBuffer& vertexBuffer, const WGPUBuffer& indexBuffer)>& onBindGroup) {
	for (WgpMesh& mesh : m_meshes) {
		mesh.setBindGroupWF(onBindGroup);
	}
}

void WgpModel::draw(const WGPURenderPassEncoder& renderPassEncoder) const {
	for (std::list<WgpMesh>::const_iterator it = m_meshes.begin(); it != m_meshes.end(); ++it) {
		(*it).draw(renderPassEncoder);
	}
}

void WgpModel::drawRaw(const WGPURenderPassEncoder& renderPassEncoder) const {
	for (std::list<WgpMesh>::const_iterator it = m_meshes.begin(); it != m_meshes.end(); ++it) {
		(*it).drawRaw(renderPassEncoder);
	}
}