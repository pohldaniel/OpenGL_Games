#pragma once

#include "../BoundingBox.h"
#include "../AssimpModel.h"
#include "OctreeNode.h"

class MeshNode : public OctreeNode {

public:

	MeshNode(const AssimpModel& model);
	~MeshNode() = default;

	virtual void drawRaw(bool force = true) const override;

	using OctreeNode::addChild;
	void addChild(MeshNode* node, bool drawDebug);
	const AssimpModel& getModel() const;

	short getMaterialIndex() const;
	short getTextureIndex() const;
	void setMaterialIndex(short index);
	void setTextureIndex(short index);

protected:

	const BoundingBox& getLocalBoundingBox() const;
	short m_materialIndex;
	short m_textureIndex;

private:
	const AssimpModel& model;
};