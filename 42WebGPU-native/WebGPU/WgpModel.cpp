#include <engine/shape/Shape.h>
#include <engine/ObjModel.h>
#include "WgpModel.h"
#include "WgpMesh.h"
#include "WgpBuffer.h"

WgpModel::WgpModel(WgpModel const& rhs) : m_meshes(rhs.m_meshes) {

}

WgpModel::WgpModel(WgpModel&& rhs) noexcept : m_meshes(rhs.m_meshes) {

}

void WgpModel::create(const ObjModel& model) {
	for (ObjMesh* mesh : model.getMeshes()) {
		m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getMaterial().textures.at(TextureSlot::TEXTURE_DIFFUSE)));
	}

	for (WgpMesh& mesh : m_meshes) {
		mesh.markForDelete();
	}
}

void WgpModel::create(const Shape& shape) {
	m_meshes.push_back(WgpMesh(shape.getVertexBuffer(), shape.getIndexBuffer()));
	m_meshes.back().markForDelete();
}

void WgpModel::setRenderPipelineSlot(const std::string& renderPipelineSlot) {
	for (WgpMesh& mesh : m_meshes) {
		mesh.setRenderPipelineSlot(renderPipelineSlot);
	}
}

void WgpModel::setBindGroupsSlot(const std::string& bindGroupsSlot) {
	for (WgpMesh& mesh : m_meshes) {
		mesh.setBindGroupsSlot(bindGroupsSlot);
	}
}

void WgpModel::setBindGroups(std::string bindGroupsName, const std::function <std::vector<WGPUBindGroup>()>& onBindGroups) {
	for (WgpMesh& mesh : m_meshes) {
		mesh.setBindGroups(bindGroupsName, onBindGroups);
	}
}

const std::list<WgpMesh>& WgpModel::getMeshes() const{
	return m_meshes;
}

void WgpModel::draw(const WGPURenderPassEncoder& renderPassEncoder) const {
	for (std::list<WgpMesh>::const_iterator it = m_meshes.begin(); it != m_meshes.end(); ++it) {
		(*it).draw(renderPassEncoder);
	}
}