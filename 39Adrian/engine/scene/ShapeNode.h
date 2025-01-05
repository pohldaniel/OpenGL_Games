#pragma once

#include "../MeshObject/Shape.h"
#include "../BoundingBox.h"
#include "OctreeNode.h"

class Octant;
class ShapeNode : public OctreeNode {

public:

	ShapeNode(const Shape& shape);
	~ShapeNode();

	virtual void drawRaw() const override;
	using OctreeNode::addChild;
	void addChild(ShapeNode* node, bool drawDebug);
	const Shape& getShape() const;

	short getMaterialIndex() const;
	short getTextureIndex() const;
	void setMaterialIndex(short index);
	void setTextureIndex(short index);
	
protected:

	short m_materialIndex;
	short m_textureIndex;

private:

	const Shape& shape;
};