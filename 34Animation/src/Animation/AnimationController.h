#pragma once

#include <memory>
#include "StringHash.h"
#include "AnimatedModel.h"

static const float COMMAND_STAY_TIME = 0.25f;

struct AnimationControl{
	/// Construct with defaults.
	AnimationControl() :
		speed_(1.0f),
		targetWeight_(0.0f),
		fadeTime_(0.0f),
		autoFadeTime_(0.0f),
		setTimeTtl_(0.0f),
		setWeightTtl_(0.0f),
		setTime_(0),
		setWeight_(0),
		setTimeRev_(0),
		setWeightRev_(0),
		removeOnCompletion_(true),
		ragdollRecovery_(false),
		ragdollRecoverTime_(0.0f),
		ragdollTimeElapsed_(0.0f)
	{
	}

	/// Animation resource name.
	std::string name_;
	/// Animation resource name hash.
	StringHash hash_;
	/// Animation speed.
	float speed_;
	/// Animation target weight.
	float targetWeight_;
	/// Animation weight fade time, 0 if no fade.
	float fadeTime_;
	/// Animation autofade on stop -time, 0 if disabled.
	float autoFadeTime_;
	/// Set time command time-to-live.
	float setTimeTtl_;
	/// Set weight command time-to-live.
	float setWeightTtl_;
	/// Set time command.
	unsigned short setTime_;
	/// Set weight command.
	unsigned char setWeight_;
	/// Set time command revision.
	unsigned char setTimeRev_;
	/// Set weight command revision.
	unsigned char setWeightRev_;
	/// Sets whether this should automatically be removed when it finishes playing.
	bool removeOnCompletion_;

	/// ragdoll
	bool  ragdollRecovery_;
	float ragdollRecoverTime_;
	float ragdollTimeElapsed_;
};

class AnimationController{


public:
	/// Construct. Register subsystem
	AnimationController(AnimatedModel* model = nullptr);
	/// Destruct.
	~AnimationController();

	/// Update the animations. Is called from HandleScenePostUpdate().
	virtual void Update(float timeStep);
	/// Play an animation and set full target weight. Name must be the full resource name. Return true on success.
	bool Play(const std::string& name, unsigned char layer, bool looped, float fadeInTime = 0.0f);
	/// Play an animation, set full target weight and fade out all other animations on the same layer. Name must be the full resource name. Return true on success.
	bool PlayExclusive(const std::string& name, unsigned char layer, bool looped, float fadeTime = 0.0f);
	/// Stop an animation. Zero fadetime is instant. Return true on success.
	bool Stop(const std::string& name, float fadeOutTime = 0.0f);
	/// Stop all animations on a specific layer. Zero fadetime is instant.
	void StopLayer(unsigned char layer, float fadeOutTime = 0.0f);
	/// Stop all animations. Zero fadetime is instant.
	void StopAll(float fadeTime = 0.0f);
	/// Fade animation to target weight. Return true on success.
	bool Fade(const std::string& name, float targetWeight, float fadeTime);
	/// Fade other animations on the same layer to target weight. Return true on success.
	bool FadeOthers(const std::string& name, float targetWeight, float fadeTime);
	/// Set animation time position. Return true on success.
	bool SetTime(const std::string& name, float time);
	/// Return whether an animation is at its end. Will return false if the animation is not active at all.
	bool IsAtEnd(const std::string& name) const;

	AnimationState* GetAnimationState(StringHash nameHash) const;
	AnimationState* AddAnimationState(Animation* animation);
	void RemoveAnimationState(AnimationState* state);
	void FindAnimation(const std::string& name, unsigned& index, AnimationState*& state) const;

	std::vector<AnimationControl> animations;
	std::vector<std::shared_ptr<AnimationState>> nodeAnimationStates;

	AnimatedModel* model;

};