#pragma once

#include "../BoundingBox.h"
#include "../MeshSequence.h"
#include "OctreeNode.h"

class SequenceNode : public OctreeNode {

public:

	SequenceNode(const MeshSequence& meshSequence, short meshIndex = 0);

	virtual void drawRaw(bool force = true) const override;
	virtual void update(const float dt);

	using OctreeNode::addChild;
	void addChild(SequenceNode* node, bool drawDebug);
	
	const MeshSequence& getSequence() const;
	short getMeshIndex() const;
	void setMeshIndex(short index);
	short getMaterialIndex() const;
	short getTextureIndex() const;
	void setMaterialIndex(short index);
	void setTextureIndex(short index);
	
protected:

	const BoundingBox& getLocalBoundingBox() const;
	short m_materialIndex;
	short m_textureIndex;

	const MeshSequence& meshSequence;

private:

	short m_meshIndex;
	float m_speed;
	float m_activeFrame;
};