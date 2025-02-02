#pragma once

#include "../animationNew/AnimatedModel.h"
#include "../BoundingBox.h"
#include "../Shader.h"

#include "OctreeNode.h"
#include "BoneNode.h"

class AnimationNode : public OctreeNode {

	friend class AnimationController;

public:

	AnimationNode(const AnimatedModel& animatedModel);
	~AnimationNode();

	void OnOctreeSet(Octree* octree) override;

	virtual void drawRaw() const override;
	void update(const float dt);

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
	short getMaterialIndex() const;
	short getTextureIndex() const;
	void setMaterialIndex(short index);
	void setTextureIndex(short index);
	void setShader(Shader* shader);

protected:

	const BoundingBox& getLocalBoundingBox() const;

	short m_materialIndex;
	short m_textureIndex;

private:

	void OnTransformChanged() override;
	void OnBoundingBoxChanged() override;
	void OnWorldBoundingBoxUpdate() const override;
	void OnAnimationOrderChanged();
	void OnAnimationChanged();

	void updateAnimation(float dt);
	void updateSkinning();
	void createBones();
	static bool CompareAnimationStates(const std::shared_ptr<AnimationState>& lhs, const std::shared_ptr<AnimationState>& rhs);

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
	Shader* m_shader;
};