#pragma once

#include "OctreeNode.h"
#include "../MeshObject/Shape.h"
#include "../BoundingBox.h"

class Octant;
class ShapeNode : public OctreeNode {

public:

	ShapeNode(const Shape& shape);
	~ShapeNode();
	using OctreeNode::addChild;
	void addChild(ShapeNode* node, bool drawDebug);
	const Shape& getShape() const;
	
protected:

	const Shape& shape;
};