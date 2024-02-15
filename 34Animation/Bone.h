#pragma once

#include <engine/scene/SceneNode.h>
#include "StringHash.h"

class Bone : public SceneNode {

public:

	Bone();
	~Bone();

	//void SetDrawable(AnimatedModelDrawable* drawable);
	void SetAnimationEnabled(bool enable);
	void CountChildBones();

	void SetTransformSilent(const Vector3f& position_, const Quaternion& rotation_, const Vector3f& scale_){
		m_position = position_;
		m_orientation = rotation_;
		m_scale = scale_;
	}

	void SetName(const std::string& newName){
		name = newName;
		nameHash = StringHash(newName);
	}

	Bone* FindChildOfType(StringHash childNameHash, bool recursive) const;

	//AnimatedModelDrawable* GetDrawable() const { return drawable; }
	bool AnimationEnabled() const { return animationEnabled; }
	size_t NumChildBones() const { return numChildBones; }
	StringHash nameHash;
	std::string name;
protected:

	void OnTransformChanged() override;

private:
	
	//AnimatedModelDrawable* drawable;
	bool animationEnabled;
	size_t numChildBones;
	
	
};