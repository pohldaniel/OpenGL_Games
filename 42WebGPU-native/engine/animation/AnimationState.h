#pragma once

#include <functional>

#include "Animation.h"
#include "Bone.h"

enum AnimationBlendMode {
	ABM_NONE = 0,
	ABM_LERP,
	ABM_ADDITIVE,
	ABM_FADE
};

struct AnimationStateTrack {
	AnimationStateTrack();
	~AnimationStateTrack();

	const AnimationTrack* m_track;
	Bone* m_node;
	float m_weight;
	size_t m_keyFrame;

	Vector3f m_initialPosition;
	Vector3f m_initialScale;
	Quaternion m_initialOrientation;
};

class AnimationState {

public:

	AnimationState(const Animation& animation, Bone* startBone);
	~AnimationState();

	void setStartBone(Bone* startBone);
	void setLooped(bool looped);
	void setBackward(bool backward);
	void setWeight(float weight);
	void setBlendMode(AnimationBlendMode mode);
	void setTime(float time);
	void setBoneWeight(size_t index, float weight, bool recursive = false);
	void setBoneWeight(const std::string& name, float weight, bool recursive = false);
	void addWeight(float delta);
	void addTime(float dt);
	void setBlendLayer(unsigned char layer);
	void setFadeLayerLength(float length);

	const Animation& getAnimation() const;
	const AnimationBlendMode getAnimationBlendMode() const;

	Bone* getStartBone() const;
	float getBoneWeight(size_t index) const;
	float getBoneWeight(const std::string& name) const;

	size_t findTrackIndex(Bone* node) const;
	size_t findTrackIndex(const std::string& name) const;

	bool isEnabled() const;
	bool isLooped() const;
	float getWeight() const;
	float getTime() const;
	const float getRestTime() const;
	float getLength() const;
	unsigned char getBlendLayer() const;

	void apply();
	void reset();

private:

	void applyToModel();
	void applyToNodes();

	std::vector<AnimationStateTrack> m_stateTracks;
	//Just for fanciness the raw pointer is the way to go
	//std::unique_ptr<Animation, std::function<void(Animation* animation)>> m_animation;
	const Animation& m_animation;
	Bone* m_startBone;

	float m_stateTime;
	unsigned char m_blendLayer;

	bool m_looped;
	bool m_backward;
	float m_blendWeight;

	float m_layeredTime;
	float m_fadeLayerLength;
	float m_additiveDirection;
	bool m_invertBlend;
	AnimationBlendMode m_animationBlendMode;
};
