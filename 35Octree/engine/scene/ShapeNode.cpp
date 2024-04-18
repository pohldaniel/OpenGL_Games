#include "ShapeNode.h"
#include "../DebugRenderer.h"
#include "Octree/Octree.h"

ShapeNode::ShapeNode(const Shape& shape) : SceneNodeLC(), shape(shape), m_drawDebug(true), localBoundingBox(shape.getAABB()), m_octreeUpdate(true) , m_reinsertQueued(true), m_octant(nullptr){
	OnBoundingBoxChanged();
}

ShapeNode::~ShapeNode() {

}

void ShapeNode::OnWorldBoundingBoxUpdate() const {
	m_worldBoundingBox = getLocalBoundingBox().transformed(getWorldTransformation());
}

void ShapeNode::OnRenderOBB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getWorldTransformation(), color);
}

void ShapeNode::OnRenderAABB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), color);
}

void ShapeNode::OnTransformChanged() {
	SceneNodeLC::OnTransformChanged();
	OnBoundingBoxChanged();
}

void ShapeNode::OnBoundingBoxChanged() const {
	m_worldBoundingBoxDirty = true;
}

void ShapeNode::OnOctreeUpdate() const {
	m_octreeUpdate = false;
}

void ShapeNode::addChild(ShapeNode* node, bool drawDebug) {
	SceneNodeLC::addChild(node);
	node->setDrawDebug(drawDebug);
}

const BoundingBox& ShapeNode::getWorldBoundingBox() const {
	if (m_worldBoundingBoxDirty) {
		OnWorldBoundingBoxUpdate();
		m_worldBoundingBoxDirty = false;
	}
	return m_worldBoundingBox;
}

const BoundingBox& ShapeNode::getLocalBoundingBox() const {
	return localBoundingBox;
}

void ShapeNode::setDrawDebug(bool drawDebug) {
	m_drawDebug = drawDebug;
}

const Shape& ShapeNode::getShape() const {
	return shape;
}

Octant* ShapeNode::getOctant() const { 
	return m_octant; 
}