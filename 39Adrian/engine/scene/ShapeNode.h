#pragma once

#include "../MeshObject/Shape.h"
#include "../BoundingBox.h"
#include "../Shader.h"
#include "OctreeNode.h"

class Octant;
class ShapeNode : public OctreeNode {

public:

	ShapeNode(const Shape& shape);
	~ShapeNode();

	virtual void drawRaw() const override;
	virtual void drawShadow() const override;
	using OctreeNode::addChild;
	void addChild(ShapeNode* node, bool drawDebug);
	const Shape& getShape() const;

	short getMaterialIndex() const;
	short getTextureIndex() const;
	void setMaterialIndex(short index);
	void setTextureIndex(short index);
	void setShader(Shader* shader);
	void setColor(const Vector4f& color);

protected:

	const BoundingBox& getLocalBoundingBox() const;

	short m_materialIndex;
	short m_textureIndex;

private:

	const Shape& shape;
	Shader* m_shader;
	Vector4f m_color;
};