#pragma once

#include "SceneNode.h"
#include "SceneNodeLC.h"
#include "../utils/StringHash.h"

class BoneNode : public SceneNodeLC {

	friend class AnimationState;
	friend class AnimationNode;
	friend class AnimatedMesh;

public:

	BoneNode();
	~BoneNode();

	void setAnimationEnabled(bool enable);
	void countChildBones();
	void setIsRootBone(bool rootBone);
	const bool isRootBone() const;

	void setTransformSilent(const Vector3f& position_, const Quaternion& rotation_, const Vector3f& scale_);
	bool animationEnabled() const;
	size_t getNumChildBones() const;

protected:

	void OnTransformChanged() override;

private:

	bool m_animationEnabled;
	size_t m_numChildBones;
	bool m_isRootBone;
	Matrix4f m_offsetMatrix;
};