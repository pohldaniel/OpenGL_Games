#pragma once

#include "OctreeNode.h"
#include "BoneNode.h"
#include "../BoundingBox.h"
#include <Animation/AnimatedModel.h>

class AnimationNode : public OctreeNode {

	friend class AnimationController;

public:

	AnimationNode(const AnimatedModel& animatedModel);
	~AnimationNode();

	void drawRaw() const override;
	void update(float dt);
	
	void OnBoundingBoxChanged() override;
	void OnWorldBoundingBoxUpdate() const override;
	void OnOctreeUpdate() override;
	void OnOctreeSet(Octree* octree) override;
	void OnAnimationOrderChanged();

	AnimationState* addAnimationState(Animation* animation);
	AnimationState* addAnimationStateFront(Animation* animation);
	AnimationState* getAnimationState(size_t index) const;
	AnimationState* findAnimationState(Animation* animation) const;
	AnimationState* findAnimationState(const std::string& animationName) const;
	AnimationState* findAnimationState(const char* animationName) const;
	AnimationState* findAnimationState(StringHash animationNameHash) const;
	void removeAnimationState(Animation* animation);
	void removeAnimationState(const std::string& animationName);
	void removeAnimationState(const char* animationName);
	void removeAnimationState(StringHash animationNameHash);
	void removeAnimationState(AnimationState* state);
	void removeAnimationState(size_t index);
	void removeAllAnimationStates();
	void setUpdateSilent(bool updateSilent);

	using OctreeNode::addChild;
	void addChild(AnimationNode* node, bool drawDebug);
	const AnimatedModel& getAnimatedModel() const;
	unsigned short getNumBones();
	const Matrix4f* getSkinMatrices();
	BoneNode* getRootBone();

private:

	void updateAnimation(float dt);
	void updateSkinning();
	void createBones();
	
	mutable BoundingBox m_boneBoundingBox;
	mutable bool m_boneBoundingBoxDirty;

	mutable bool m_animationDirty;
	mutable bool m_skinningDirty;
	bool m_animationOrderDirty;
	bool m_hasAnimationController;
	bool m_updateSilent;
	unsigned short m_numBones;

	BoneNode* m_rootBone;
	BoneNode** m_bones;
	Matrix4f* m_skinMatrices;
	std::vector<std::shared_ptr<AnimationState>> m_animationStates;

	const AnimatedModel& animatedModel;
	const std::vector<MeshBone>& meshBones;
};