#include <engine/shape/Shape.h>
#include <engine/ObjModel.h>
#include <engine/AssimpModel.h>

#include "WgpModel.h"

WgpModel::WgpModel(WgpModel const& rhs) : m_meshes(rhs.m_meshes) {

}

WgpModel::WgpModel(WgpModel&& rhs) noexcept : m_meshes(rhs.m_meshes) {

}

void WgpModel::create(const ObjModel& model) {
	for (const ObjMesh* mesh : model.getMeshes()) {
		if (mesh->getMaterial().hasTexture(TextureSlot::TEXTURE_DIFFUSE))
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getMaterial().getTextures().at(TextureSlot::TEXTURE_DIFFUSE)));
		else
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer()));
	}

	for (WgpMesh& mesh : m_meshes) {
		mesh.markForDelete();
	}
}

void WgpModel::create(const AssimpModel& model) {
	for (const AssimpMesh* mesh : model.getMeshes()) {
		if (mesh->getMaterial().hasTexture(TextureSlot::TEXTURE_DIFFUSE) && mesh->getMaterial().getTextures().at(TextureSlot::TEXTURE_DIFFUSE)[0] != '*')
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getMaterial().getTextures().at(TextureSlot::TEXTURE_DIFFUSE)));
		else if(mesh->getEmbeddedTextures().count(TextureSlot::TEXTURE_DIFFUSE)) {
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getEmbeddedTextures().at(TextureSlot::TEXTURE_DIFFUSE)));
			mesh->removeEmbeddedTexture(TextureSlot::TEXTURE_DIFFUSE);
		}else
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer()));
	}

	for (WgpMesh& mesh : m_meshes) {
		mesh.markForDelete();
	}
}

void WgpModel::create(const Shape& shape) {
	m_meshes.push_back(WgpMesh(shape.getVertexBuffer(), shape.getIndexBuffer()));
	m_meshes.back().markForDelete();
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

void WgpModel::draw(const WGPURenderPassEncoder& renderPassEncoder, uint32_t instanceCount) const {
	for (std::list<WgpMesh>::const_iterator it = m_meshes.begin(); it != m_meshes.end(); ++it) {
		(*it).draw(renderPassEncoder, instanceCount);
	}
}