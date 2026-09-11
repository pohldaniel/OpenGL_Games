#pragma once

#include <engine/shape/Shape.h>
#include "SceneNode.h"

class ShapeNode : public SceneNode {

public:

	ShapeNode(const Shape& shape);
	~ShapeNode();

	using SceneNode::addChild;
	void addChild(ShapeNode* node);
	const Shape& getShape() const;

private:

	const Shape& shape;
};