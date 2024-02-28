#pragma once

#include <functional>
#include <engine/scene/SceneNode.h>

#include "Bone.h"
#include "StringHash.h"
#include "Animation.h"

enum AnimationBlendMode{
	ABM_NONE = 0,
	ABM_LERP,
	ABM_ADDITIVE,
	ABM_FADE
};

struct AnimationStateTrack{
	AnimationStateTrack();
	~AnimationStateTrack();

	const AnimationTrack* track;
	Bone* node;
	float weight;
	size_t keyFrame;

	Vector3f m_initialPosition;
	Vector3f m_initialScale;
	Quaternion m_initialOrientation;
};

class AnimationState {

public:

	AnimationState(Animation* animation, Bone* startBone);
	~AnimationState();

	void SetStartBone(Bone* startBone);
	void SetLooped(bool looped);
	void SetBackward(bool backward);
	void SetWeight(float weight);
	void SetBlendMode(AnimationBlendMode mode);
	void SetTime(float time);
	void SetBoneWeight(size_t index, float weight, bool recursive = false);
	void SetBoneWeight(const std::string& name, float weight, bool recursive = false);
	void SetBoneWeight(StringHash nameHash, float weight, bool recursive = false);
	void AddWeight(float delta);
	void AddTime(float dt);
	void SetBlendLayer(unsigned char layer);
	void SetFadeLayerLength(float length);

	void SetEnabled(bool enable);
	void SetDrawable(bool drawable);

	Animation* GetAnimation() const { return animation.get(); }
	const AnimationBlendMode getAnimationBlendMode() const;

	Bone* StartBone() const { return startBone; }
	float BoneWeight(size_t index) const;
	float BoneWeight(const std::string& name) const;
	float BoneWeight(StringHash nameHash) const;



	size_t FindTrackIndex(BaseNode* node) const;
	size_t FindTrackIndex(const std::string& name) const;
	size_t FindTrackIndex(StringHash nameHash) const;
	bool Enabled() const { return weight > 0.0f && m_enabled; }
	bool Looped() const { return looped; }
	float Weight() const { return weight; }
	float Time() const { return time; }
	float Length() const;
	unsigned char BlendLayer() const { return blendLayer; }
	void Apply();

	
private:
	
	void ApplyToModel();
	void ApplyToNodes();
	std::function<void(Animation* animation)> m_fun;
	//Just for fanciness the raw pointer is the way to go
	std::unique_ptr<Animation, decltype(m_fun)> animation;
	//Animation* animation;

	Bone* startBone;
	std::vector<AnimationStateTrack> stateTracks;
	bool looped;
	bool m_backward;
	float weight;
	float time;
	unsigned char blendLayer;

	bool m_enabled;
	bool m_drawable;

	float m_blendWeight = 0.0f;
	float m_layeredTime = 0.0f;
	float m_fadeLayerLength = 1.0f;
	float m_additiveDirection = 1.0f;
	bool m_invertBlend = false;
	AnimationBlendMode m_animationBlendMode;
};
