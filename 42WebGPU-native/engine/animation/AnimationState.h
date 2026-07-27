#pragma once

#include <functional>

#include "Animation.h"
#include "Bone.h"

enum AnimationBlendMode {
	ABM_LERP = 0
};

struct AnimationStateTrack {
	AnimationStateTrack();
	~AnimationStateTrack();

	const AnimationTrack* m_track;
	Bone* m_bone;
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
	void setWeight(float weight);
	void setBlendMode(AnimationBlendMode mode);
	void setTime(float time);
	void addTime(float dt);
	void setBlendLayer(unsigned char layer);

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
	float getLength() const;
	unsigned char getBlendLayer() const;

	void apply();
	void reset();

private:

	void applyToModel();

	std::vector<AnimationStateTrack> m_stateTracks;
	const Animation& m_animation;
	Bone* m_startBone;

	float m_stateTime;
	unsigned char m_blendLayer;

	bool m_looped;
	float m_blendWeight;

	AnimationBlendMode m_animationBlendMode;
};
