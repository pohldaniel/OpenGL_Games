#pragma once

#include "SceneNode.h"
#include "SceneNodeLC.h"
#include "../utils/StringHash.h"

class BoneNode : public SceneNodeLC {

	friend class AnimationState;

public:

	BoneNode();
	~BoneNode();

	void SetAnimationEnabled(bool enable);
	void CountChildBones();
	void setRootBone(bool rootBone);
	const bool isRootBone() const;


	void SetTransformSilent(const Vector3f& position_, const Quaternion& rotation_, const Vector3f& scale_) {
		m_position = position_;
		m_orientation = rotation_;
		m_scale = scale_;
	}

	void SetName(const std::string& newName) {
		name = newName;
		nameHash = StringHash(newName);
	}

	BoneNode* FindChildOfType(StringHash childNameHash, bool recursive) const;
	bool AnimationEnabled() const { return animationEnabled; }
	size_t NumChildBones() const { return numChildBones; }
	StringHash nameHash;
	std::string name;

	Matrix4f offsetMatrix;

protected:

	void OnTransformChanged() override;

private:

	bool animationEnabled;
	size_t numChildBones;
	bool m_rootBone;

};