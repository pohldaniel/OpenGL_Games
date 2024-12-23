#pragma once

#include <functional>
#include "../scene/SceneNode.h"
#include "../scene/BoneNode.h"
#include "../utils/StringHash.h"

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
	BoneNode* node;
	float weight;
	size_t keyFrame;

	Vector3f m_initialPosition;
	Vector3f m_initialScale;
	Quaternion m_initialOrientation;
};

class AnimationState {

public:

	AnimationState(Animation* animation, BoneNode* startBone);
	~AnimationState();

	void SetStartBone(BoneNode* startBone);
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

	Animation* GetAnimation() const { return animation; }
	const AnimationBlendMode getAnimationBlendMode() const;

	BoneNode* StartBone() const { return startBone; }
	float BoneWeight(size_t index) const;
	float BoneWeight(const std::string& name) const;
	float BoneWeight(StringHash nameHash) const;

	size_t FindTrackIndex(Node* node) const;
	size_t FindTrackIndex(const std::string& name) const;
	size_t FindTrackIndex(StringHash nameHash) const;

	bool Enabled() const { return m_blendWeight > 0.0f; }
	bool Looped() const { return m_looped; }
	float Weight() const { return m_blendWeight; }
	float Time() const { return m_stateTime; }
	float Length() const;
	unsigned char BlendLayer() const { return m_blendLayer; }
	void Apply();

	const float getRestTime() const;

private:
	
	void ApplyToModel();
	void ApplyToNodes();

	//Just for fanciness the raw pointer is the way to go
	//std::unique_ptr<Animation, std::function<void(Animation* animation)>> animation;
	Animation* animation;

	BoneNode* startBone;
	std::vector<AnimationStateTrack> stateTracks;
	
	float m_stateTime;
	unsigned char m_blendLayer;


	bool m_looped;
	bool m_backward;
	float m_blendWeight;

	float m_blendWeight2 = 0.0f;
	float m_layeredTime = 0.0f;
	float m_fadeLayerLength = 1.0f;
	float m_additiveDirection = 1.0f;
	bool m_invertBlend = false;
	AnimationBlendMode m_animationBlendMode;
};
