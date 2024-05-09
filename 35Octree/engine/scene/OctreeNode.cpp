#include "OctreeNode.h"
#include "../DebugRenderer.h"
#include "../octree/Octree.h"

OctreeNode::OctreeNode(const BoundingBox& localBoundingBox) : localBoundingBox(localBoundingBox), m_drawDebug(true), m_reinsertQueued(true), m_octant(nullptr), m_octree(nullptr), m_octreeUpdate(true), m_lastFrameNumber(0), frameNumber(nullptr) {
	OnBoundingBoxChanged();
}

OctreeNode::~OctreeNode() {

}

void OctreeNode::drawRaw() const {

}

void OctreeNode::update(float dt) {

}

const BoundingBox& OctreeNode::getWorldBoundingBox() const {
	if (m_worldBoundingBoxDirty) {
		OnWorldBoundingBoxUpdate();
		m_worldBoundingBoxDirty = false;
	}
	return m_worldBoundingBox;
}

const BoundingBox& OctreeNode::getLocalBoundingBox() const {
	return localBoundingBox;
}

void OctreeNode::OnWorldBoundingBoxUpdate() const {
	m_worldBoundingBox = getLocalBoundingBox().transformed(getWorldTransformation());
}

void OctreeNode::OnRenderOBB(const Vector4f& color) {
	if (!m_drawDebug)
		return;
	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getWorldTransformation(), color);
}

void OctreeNode::OnRenderAABB(const Vector4f& color) {
	if (!m_drawDebug)
		return;
	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), color);
}

void OctreeNode::OnTransformChanged() {
	SceneNodeLC::OnTransformChanged();
	OnBoundingBoxChanged();
}

void OctreeNode::OnBoundingBoxChanged() {
	m_worldBoundingBoxDirty = true;

	if (m_octant && !m_reinsertQueued)
		m_octree->QueueUpdate(this);
}

void OctreeNode::OnOctreeUpdate() {
	m_octreeUpdate = false;

}

Octant* OctreeNode::getOctant() const {
	return m_octant;
}

void OctreeNode::setDrawDebug(bool drawDebug) {
	m_drawDebug = drawDebug;
}

Octree* OctreeNode::getOctree() const {
	return m_octree;
}

void OctreeNode::removeFromOctree() {
	if (m_octree) {
		m_octree->RemoveDrawable(this);
		m_octree = nullptr;
	}
}

void OctreeNode::OnOctreeSet(Octree* octree) {
	removeFromOctree();
	m_octree = octree;
	if (m_octree) {
		frameNumber = &m_octree->m_frameNumber;
		m_octree->QueueUpdate(this);
	}
}

void OctreeNode::OnPrepareRender(unsigned short frameNumber) {
	m_lastFrameNumber = frameNumber;
}

bool OctreeNode::wasInView(unsigned short frameNumber) const {
	if (frameNumber == 1 && m_lastFrameNumber == 0) return true;

	unsigned short previousFrameNumber = frameNumber - 1;
	if (!previousFrameNumber)
		--previousFrameNumber;
	return m_lastFrameNumber == previousFrameNumber;
}