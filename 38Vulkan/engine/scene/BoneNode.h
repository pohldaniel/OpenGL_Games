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

	void setTransformSilent(const Vector3f& position, const Quaternion& rotation, const Vector3f& scale);
	bool animationEnabled() const;
	unsigned int getNumChildBones() const;

protected:

	void OnTransformChanged() override;

private:

	bool m_animationEnabled;
	unsigned int m_numChildBones;
	bool m_isRootBone;
	Matrix4f m_offsetMatrix;
};