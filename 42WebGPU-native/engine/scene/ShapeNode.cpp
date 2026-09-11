#include "ShapeNode.h"

ShapeNode::ShapeNode(const Shape& shape) : SceneNode(), shape(shape) {
	
}

ShapeNode::~ShapeNode() {

}

void ShapeNode::addChild(ShapeNode* node) {
	SceneNode::addChild(node);
}

const Shape& ShapeNode::getShape() const {
	return shape;
}