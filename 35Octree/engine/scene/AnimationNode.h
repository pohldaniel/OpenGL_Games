#pragma once

#include "OctreeNode.h"
#include "../BoundingBox.h"
#include <Animation/AnimatedModel.h>

class AnimationNode : public OctreeNode {

public:

	AnimationNode(const AnimatedModel& animatedModel);
	~AnimationNode() = default;

	void OnBoundingBoxChanged() override;
	void OnWorldBoundingBoxUpdate() const override;
	void OnOctreeUpdate() override;

	using OctreeNode::addChild;
	void addChild(AnimationNode* node, bool drawDebug);
	const AnimatedModel& getAnimatedModel() const;

protected:

	const AnimatedModel& animatedModel;
	mutable BoundingBox m_boneBoundingBox;
	mutable bool m_boneBoundingBoxDirty;

	mutable bool m_animationDirty;
	mutable bool m_skinningDirty;
};