#include <iostream>

#include "AnimationController.h"
#include "AnimatedModel.h"
#include "Animation.h"
#include "../Resource/ResourceCache.h"


AnimationController::AnimationController(AnimatedModel* model) : model(model)
{
	RegisterSubsystem(this);
}

AnimationController::~AnimationController()
{
	RemoveSubsystem(this);
}

void AnimationController::Update(float timeStep)
{
	// Loop through animations
	for (unsigned i = 0; i < animations.size();)
	{
		
		AnimationControl& ctrl = animations[i];

		
		AnimationState* state = GetAnimationState(ctrl.hash_);
		bool remove = false;

		if (!state) {
			remove = true;
		}
		else
		{
			if (ctrl.ragdollRecovery_)
			{
				ctrl.ragdollTimeElapsed_ += timeStep;
				if (ctrl.ragdollTimeElapsed_ > ctrl.ragdollRecoverTime_)
				{
					ctrl.ragdollRecovery_ = false;
				}
			}
			else
			{
				// Advance the animation
				if (ctrl.speed_ != 0.0f) {
					state->AddTime(ctrl.speed_ * timeStep);
				}
			}

			float targetWeight = ctrl.targetWeight_;
			float fadeTime = ctrl.fadeTime_;

			// If non-looped animation at the end, activate autofade as applicable
			if (!state->Looped() && state->Time() >= state->Length() && ctrl.autoFadeTime_ > 0.0f)
			{
				targetWeight = 0.0f;
				fadeTime = ctrl.autoFadeTime_;
			}
			if (ctrl.ragdollRecovery_ && ctrl.ragdollRecoverTime_ > 0.0f)
			{
				fadeTime = ctrl.ragdollRecoverTime_ * 10.0f;
			}

			// Process weight fade
			float currentWeight = state->Weight();
			if (currentWeight != targetWeight)
			{
				if (fadeTime > 0.0f)
				{
					float weightDelta = 1.0f / fadeTime * timeStep;
					if (currentWeight < targetWeight)
						currentWeight = Min(currentWeight + weightDelta, targetWeight);
					else if (currentWeight > targetWeight)
						currentWeight = Max(currentWeight - weightDelta, targetWeight);
					state->SetWeight(currentWeight);
				}
				else
					state->SetWeight(targetWeight);
			}
			if (ctrl.ragdollRecovery_ && currentWeight >= 1.0f)
			{
				ctrl.ragdollRecovery_ = false;
			}

			// Remove if weight zero and target weight zero
			if (state->Weight() == 0.0f && (targetWeight == 0.0f || fadeTime == 0.0f) && ctrl.removeOnCompletion_)
				remove = true;
		}

		// Decrement the command time-to-live values
		if (ctrl.setTimeTtl_ > 0.0f)
			ctrl.setTimeTtl_ = Max(ctrl.setTimeTtl_ - timeStep, 0.0f);
		if (ctrl.setWeightTtl_ > 0.0f)
			ctrl.setWeightTtl_ = Max(ctrl.setWeightTtl_ - timeStep, 0.0f);

		if (remove)
		{
			if (state)
				RemoveAnimationState(state);
			animations.erase(animations.begin() + i);
		}
		else
			++i;
	}

	// Node hierarchy animations need to be applied manually
	for (std::vector<SharedPtr<AnimationState> >::iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i)
		(*i)->Apply();
}

bool AnimationController::Play(const std::string& name, unsigned char layer, bool looped, float fadeInTime)
{
	// Get the animation resource first to be able to get the canonical resource name
	// (avoids potential adding of duplicate animations)


	Animation* newAnimation = Subsystem<ResourceCache>()->LoadResource<Animation>(name);
	if (!newAnimation)
		return false;

	// Check if already exists
	unsigned index;
	AnimationState* state;
	FindAnimation(newAnimation->Name(), index, state);

	if (!state)
	{
		state = AddAnimationState(newAnimation);
		if (!state)
			return false;
	}

	if (index == M_MAX_UNSIGNED)
	{
		AnimationControl newControl;
		newControl.name_ = newAnimation->Name();
		newControl.hash_ = newAnimation->NameHash();
		animations.push_back(newControl);
		index = animations.size() - 1;
	}

	state->SetBlendLayer(layer);
	state->SetLooped(looped);
	animations[index].targetWeight_ = 1.0f;
	animations[index].fadeTime_ = fadeInTime;
	animations[index].ragdollRecovery_ = false;

	return true;
}

bool AnimationController::PlayExclusive(const std::string& name, unsigned char layer, bool looped, float fadeTime)
{
	bool success = Play(name, layer, looped, fadeTime);

	// Fade other animations only if successfully started the new one
	if (success)
		FadeOthers(name, 0.0f, fadeTime);

	return success;
}

bool AnimationController::Stop(const std::string& name, float fadeOutTime)
{
	unsigned index;
	AnimationState* state;
	FindAnimation(name, index, state);
	if (index != M_MAX_UNSIGNED)
	{
		animations[index].targetWeight_ = 0.0f;
		animations[index].fadeTime_ = fadeOutTime;
		//MarkNetworkUpdate();
	}

	return index != M_MAX_UNSIGNED || state != 0;
}

void AnimationController::StopLayer(unsigned char layer, float fadeOutTime)
{
	bool needUpdate = false;
	for (std::vector<AnimationControl>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		AnimationState* state = GetAnimationState(i->hash_);
		if (state && state->BlendLayer() == layer)
		{
			i->targetWeight_ = 0.0f;
			i->fadeTime_ = fadeOutTime;
			needUpdate = true;
		}
	}

	//if (needUpdate)
		//MarkNetworkUpdate();
}

void AnimationController::StopAll(float fadeOutTime)
{
	if (animations.size())
	{
		for (std::vector<AnimationControl>::iterator i = animations.begin(); i != animations.end(); ++i)
		{
			i->targetWeight_ = 0.0f;
			i->fadeTime_ = fadeOutTime;
		}

		//MarkNetworkUpdate();
	}
}

bool AnimationController::Fade(const std::string& name, float targetWeight, float fadeTime)
{
	unsigned index;
	AnimationState* state;
	FindAnimation(name, index, state);
	if (index == M_MAX_UNSIGNED)
		return false;

	animations[index].targetWeight_ = Clamp(targetWeight, 0.0f, 1.0f);
	animations[index].fadeTime_ = fadeTime;
	//MarkNetworkUpdate();
	return true;
}


bool AnimationController::FadeOthers(const std::string& name, float targetWeight, float fadeTime)
{
	unsigned index;
	AnimationState* state;
	FindAnimation(name, index, state);
	if (index == M_MAX_UNSIGNED || !state)
		return false;

	unsigned char layer = state->BlendLayer();

	bool needUpdate = false;
	for (unsigned i = 0; i < animations.size(); ++i)
	{
		if (i != index)
		{
			AnimationControl& control = animations[i];
			AnimationState* otherState = GetAnimationState(control.hash_);
			if (otherState && otherState->BlendLayer() == layer)
			{
				control.targetWeight_ = Clamp(targetWeight, 0.0f, 1.0f);
				control.fadeTime_ = fadeTime;
				needUpdate = true;
			}
		}
	}

	//if (needUpdate)
		//MarkNetworkUpdate();
	return true;
}

AnimationState* AnimationController::GetAnimationState(StringHash nameHash) const
{
	// Model mode
	if (model)
		return model->FindAnimationState(nameHash);

	// Node hierarchy mode
	for (std::vector<SharedPtr<AnimationState> >::const_iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i)
	{
		Animation* animation = (*i)->GetAnimation();
		if (animation->NameHash() == nameHash || animation->AnimationNameHash() == nameHash)
			return *i;
	}

	return 0;
}

AnimationState* AnimationController::AddAnimationState(Animation* animation)
{
	if (!animation)
		return 0;

	// Model mode
	if (model)
		return model->AddAnimationState(animation);

	// Node hierarchy mode
	SharedPtr<AnimationState> newState(new AnimationState(node, animation));
	nodeAnimationStates.push_back(newState);
	return newState;
}

void AnimationController::RemoveAnimationState(AnimationState* state)
{
	if (!state)
		return;

	// Model mode
	if (model)
	{
		model->RemoveAnimationState(state);
		return;
	}

	// Node hierarchy mode
	for (std::vector<SharedPtr<AnimationState> >::iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i)
	{
		if ((*i) == state)
		{
			nodeAnimationStates.erase(i);
			return;
		}
	}
}

void AnimationController::FindAnimation(const std::string& name, unsigned& index, AnimationState*& state) const
{
	StringHash nameHash(name);

	// Find the AnimationState
	state = GetAnimationState(nameHash);
	if (state)
	{
		// Either a resource name or animation name may be specified. We store resource names, so correct the hash if necessary
		nameHash = state->GetAnimation()->NameHash();
	}

	// Find the internal control structure
	index = M_MAX_UNSIGNED;
	for (unsigned i = 0; i < animations.size(); ++i)
	{
		if (animations[i].hash_ == nameHash)
		{
			index = i;
			break;
		}
	}
}