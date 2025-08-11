#include "ShapeNode.h"
#include "../DebugRenderer.h"
#include "../Material.h"
#include "../BuiltInShader.h"

ShapeNode::ShapeNode(const Shape& shape) : OctreeNode(), shape(shape), m_materialIndex(-1), m_textureIndex(-1), m_shader(nullptr), m_color(Vector4f::ONE){
	OnBoundingBoxChanged();
}

ShapeNode::~ShapeNode() {

}

void ShapeNode::drawRaw(bool force) const {

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].updateMaterialUbo(BuiltInShader::materialUbo);

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	bool change = !(m_color == Vector4f::ONE);
	if (m_shader && force) {
		m_shader->use();
		m_shader->loadMatrix("u_model", getWorldTransformation());

		if (change)
			m_shader->loadVector("u_color", m_color);		
	}

	shape.drawRaw();

	if (m_shader&& force) {
		if (change)
			m_shader->loadVector("u_color", Vector4f::ONE);

		m_shader->unuse();
	}
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

void ShapeNode::setColor(const Vector4f& color) {
	m_color = color;
}