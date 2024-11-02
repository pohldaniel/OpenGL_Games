#pragma once

#include <functional>
#include <engine/scene/BoneNode.h>
#include <engine/utils/StringHash.h>

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

	const AnimationTrack* m_track;
	BoneNode* m_node;
	float m_weight;
	size_t m_keyFrame;

	Vector3f m_initialPosition;
	Vector3f m_initialScale;
	Quaternion m_initialOrientation;
};

class AnimationState {

public:

	AnimationState(Animation* animation, BoneNode* startBone);
	~AnimationState();

	void setStartBone(BoneNode* startBone);
	void setLooped(bool looped);
	void setBackward(bool backward);
	void setWeight(float weight);
	void setBlendMode(AnimationBlendMode mode);
	void setTime(float time);
	void setBoneWeight(size_t index, float weight, bool recursive = false);
	void setBoneWeight(const std::string& name, float weight, bool recursive = false);
	void setBoneWeight(StringHash nameHash, float weight, bool recursive = false);
	void addWeight(float delta);
	void addTime(float dt);
	void setBlendLayer(unsigned char layer);
	void setFadeLayerLength(float length);

	Animation* getAnimation() const;
	const AnimationBlendMode getAnimationBlendMode() const;

	BoneNode* getStartBone() const;
	float getBoneWeight(size_t index) const;
	float getBoneWeight(const std::string& name) const;
	float getBoneWeight(StringHash nameHash) const;

	size_t findTrackIndex(Node* node) const;
	size_t findTrackIndex(const std::string& name) const;
	size_t findTrackIndex(StringHash nameHash) const;

	bool isEnabled() const;
	bool isLooped() const;
	float getWeight() const;
	float getTime() const;
	const float getRestTime() const;
	float getLength() const;
	unsigned char getBlendLayer() const;

	void apply();

private:
	
	void applyToModel();
	void applyToNodes();

	//Just for fanciness the raw pointer is the way to go
	//std::unique_ptr<Animation, std::function<void(Animation* animation)>> m_animation;
	Animation* m_animation;

	BoneNode* m_startBone;
	std::vector<AnimationStateTrack> m_stateTracks;
	
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
