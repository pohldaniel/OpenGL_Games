#include <engine/animation/AnimatedModel.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>

#include "WgpModel.h"

WgpModel::WgpModel(WgpModel const& rhs) : m_meshes(rhs.m_meshes) {

}

WgpModel::WgpModel(WgpModel&& rhs) noexcept : m_meshes(rhs.m_meshes) {

}

void WgpModel::create(const ObjModel& model) {
	for (const Mesh* _mesh : model.getMeshes()) {
		const ObjMesh* mesh = static_cast<const ObjMesh*>(_mesh);
		if (mesh->hasMaterial() &&  mesh->getMaterial().hasTexture(TextureSlot::TEXTURE_DIFFUSE))
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getMaterial().getTextures().at(TextureSlot::TEXTURE_DIFFUSE)));
		else
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer()));
	}
	markForDelete();
}

void WgpModel::create(const AssimpModel& model) {
	for (const Mesh* _mesh : model.getMeshes()) {
		const AssimpMesh* mesh = static_cast<const AssimpMesh*>(_mesh);
		if (mesh->getEmbeddedTextures().count(TextureSlot::TEXTURE_DIFFUSE)) {
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getEmbeddedTextures().at(TextureSlot::TEXTURE_DIFFUSE)));
			mesh->removeEmbeddedTexture(TextureSlot::TEXTURE_DIFFUSE);
		}else if (mesh->hasMaterial() && mesh->getMaterial().hasTexture(TextureSlot::TEXTURE_DIFFUSE))
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getMaterial().getTextures().at(TextureSlot::TEXTURE_DIFFUSE)));
		else 
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer()));
	}
	markForDelete();
}

void WgpModel::create(const AnimatedModel& model) {
	for (const Mesh* _mesh : model.getMeshes()) {
		const AnimatedMesh* mesh = static_cast<const AnimatedMesh*>(_mesh);
		if (mesh->hasMaterial() && mesh->getMaterial().hasTexture(TextureSlot::TEXTURE_DIFFUSE))
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getWeights(), mesh->getJoints(), mesh->getStride(), mesh->getMaterial().getTextures().at(TextureSlot::TEXTURE_DIFFUSE)));
		else
			m_meshes.push_back(WgpMesh(mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getWeights(), mesh->getJoints(), mesh->getStride()));
	}
	markForDelete();
}

void WgpModel::create(const Shape& shape) {
	m_meshes.push_back(WgpMesh(shape.getVertexBuffer(), shape.getIndexBuffer()));
	markForDelete();
}

void WgpModel::create(const WgpBuffer& vertexBuffer, const WgpBuffer& indexBuffer) {
	m_meshes.push_back(WgpMesh(vertexBuffer, indexBuffer));
}

void WgpModel::markForDelete() {
	for (WgpMesh& mesh : m_meshes) {
		mesh.markForDelete();
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

void WgpModel::addBindGroups(std::string bindGroupsName, const std::function <std::vector<WGPUBindGroup>()>& onBindGroups) {
	for (WgpMesh& mesh : m_meshes) {
		mesh.addBindGroups(bindGroupsName, onBindGroups);
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