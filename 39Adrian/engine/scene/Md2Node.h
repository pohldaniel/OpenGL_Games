#pragma once

#include "../Md2Model.h"
#include "../BoundingBox.h"
#include "OctreeNode.h"

class Md2Node : public OctreeNode {

public:

	Md2Node(const Md2Model& md2Model);

	virtual void drawRaw() const override;
	void update(const float dt);

	using OctreeNode::addChild;
	void addChild(Md2Node* node, bool drawDebug);
	const Md2Model& getMd2Model() const;
	void setAnimationType(AnimationType animationType);

	short getMaterialIndex() const;
	short getTextureIndex() const;
	void setMaterialIndex(short index);
	void setTextureIndex(short index);

protected:

	const BoundingBox& getLocalBoundingBox() const;

	short m_materialIndex;
	short m_textureIndex;

private:

	void OnWorldBoundingBoxUpdate() const;	
	void OnAnimationChanged();
	void updateAnimation();
	
	std::vector<Utils::MD2IO::Vertex> m_interpolated;
	bool m_animationDirty;

	const Utils::MD2IO::Animation* currentAnimation;
	AnimationType m_animationType;
	float m_speed;
	short m_activeFrameIdx;
	float m_activeFrame;

	const Md2Model& md2Model;
};
