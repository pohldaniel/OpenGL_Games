#include "ShapeNode.h"
#include "../DebugRenderer.h"

ShapeNode::ShapeNode(const Shape& shape) : OctreeNode(shape.getAABB()), shape(shape) {
	OnBoundingBoxChanged();
}

ShapeNode::~ShapeNode() {

}

void ShapeNode::drawRaw() const {
	Vector3f position = getWorldPosition();
	std::cout << "Position: " << position[0] << "  " << position[1] << "  " << position[2] << std::endl;
	shape.drawRaw();
}

void ShapeNode::addChild(ShapeNode* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const Shape& ShapeNode::getShape() const {
	return shape;
}