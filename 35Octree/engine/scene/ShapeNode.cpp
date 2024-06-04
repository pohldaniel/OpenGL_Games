#include "ShapeNode.h"
#include "../DebugRenderer.h"

ShapeNode::ShapeNode(const Shape& shape) : OctreeNode(shape.getAABB()), shape(shape) {
	OnBoundingBoxChanged();
}

ShapeNode::~ShapeNode() {

}

void ShapeNode::drawRaw() const {
	Vector3f position = getWorldPosition();
	shape.drawRaw();
}

void ShapeNode::addChild(ShapeNode* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const Shape& ShapeNode::getShape() const {
	return shape;
}