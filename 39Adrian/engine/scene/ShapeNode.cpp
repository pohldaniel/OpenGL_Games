#include "ShapeNode.h"
#include "../DebugRenderer.h"
#include "../Material.h"
#include "../BuiltInShader.h"

ShapeNode::ShapeNode(const Shape& shape) : OctreeNode(), shape(shape), m_materialIndex(-1), m_textureIndex(-1), m_shader(nullptr){
	OnBoundingBoxChanged();
}

ShapeNode::~ShapeNode() {
	std::cout << "------" << std::endl;
}

void ShapeNode::drawRaw() const {

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].updateMaterialUbo(BuiltInShader::materialUbo);

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	if (m_shader) {
		m_shader->use();
		m_shader->loadMatrix("u_model", getWorldTransformation());
	}

	shape.drawRaw();

	if (m_shader)
		m_shader->unuse();
}

void ShapeNode::addChild(ShapeNode* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const Shape& ShapeNode::getShape() const {
	return shape;
}

const BoundingBox& ShapeNode::getLocalBoundingBox() const {
	return shape.getAABB();
}

short ShapeNode::getMaterialIndex() const {
	return m_materialIndex;
}

void ShapeNode::setMaterialIndex(short index) {
	m_materialIndex = index;
}

short ShapeNode::getTextureIndex() const {
	return m_textureIndex;
}

void ShapeNode::setTextureIndex(short index) {
	m_textureIndex = index;
}

void ShapeNode::setShader(Shader* shader) {
	m_shader = shader;
}