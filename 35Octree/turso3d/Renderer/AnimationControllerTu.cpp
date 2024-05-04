#include <iostream>

#include "AnimationControllerTu.h"
#include "AnimatedModelTu.h"
#include "AnimationTu.h"
#include "../Resource/ResourceCache.h"


AnimationControllerTu::AnimationControllerTu(AnimatedModelTu* model) : model(model)
{

}

AnimationControllerTu::~AnimationControllerTu()
{

}

void AnimationControllerTu::Update(float timeStep)
{
	// Loop through animations
	for (unsigned i = 0; i < animations.size();)
	{		
		AnimationControlTu& ctrl = animations[i];
		AnimationStateTu* state = GetAnimationState(ctrl.hash_);

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
	for (std::vector<SharedPtr<AnimationStateTu> >::iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i)
		(*i)->Apply();
}

bool AnimationControllerTu::Play(const std::string& name, unsigned char layer, bool looped, float fadeInTime)
{
	// Get the animation resource first to be able to get the canonical resource name
	// (avoids potential adding of duplicate animations)


	AnimationTu* newAnimation = Subsystem<ResourceCache>()->LoadResource<AnimationTu>(name);
	if (!newAnimation)
		return false;

	// Check if already exists
	unsigned index;
	AnimationStateTu* state;
	FindAnimation(newAnimation->Name(), index, state);

	if (!state)
	{
		state = AddAnimationState(newAnimation);
		if (!state)
			return false;
	}

	if (index == M_MAX_UNSIGNED)
	{
		AnimationControlTu newControl;
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

bool AnimationControllerTu::PlayExclusive(const std::string& name, unsigned char layer, bool looped, float fadeTime)
{
	bool success = Play(name, layer, looped, fadeTime);

	// Fade other animations only if successfully started the new one
	if (success)
		FadeOthers(name, 0.0f, fadeTime);

	return success;
}

bool AnimationControllerTu::Stop(const std::string& name, float fadeOutTime)
{
	unsigned index;
	AnimationStateTu* state;
	FindAnimation(name, index, state);
	if (index != M_MAX_UNSIGNED)
	{
		animations[index].targetWeight_ = 0.0f;
		animations[index].fadeTime_ = fadeOutTime;
		//MarkNetworkUpdate();
	}

	return index != M_MAX_UNSIGNED || state != 0;
}

void AnimationControllerTu::StopLayer(unsigned char layer, float fadeOutTime)
{
	bool needUpdate = false;
	for (std::vector<AnimationControlTu>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		AnimationStateTu* state = GetAnimationState(i->hash_);
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

void AnimationControllerTu::StopAll(float fadeOutTime)
{
	if (animations.size())
	{
		for (std::vector<AnimationControlTu>::iterator i = animations.begin(); i != animations.end(); ++i)
		{
			i->targetWeight_ = 0.0f;
			i->fadeTime_ = fadeOutTime;
		}

		//MarkNetworkUpdate();
	}
}

bool AnimationControllerTu::Fade(const std::string& name, float targetWeight, float fadeTime)
{
	unsigned index;
	AnimationStateTu* state;
	FindAnimation(name, index, state);
	if (index == M_MAX_UNSIGNED)
		return false;

	animations[index].targetWeight_ = Clamp(targetWeight, 0.0f, 1.0f);
	animations[index].fadeTime_ = fadeTime;
	//MarkNetworkUpdate();
	return true;
}


bool AnimationControllerTu::FadeOthers(const std::string& name, float targetWeight, float fadeTime)
{
	unsigned index;
	AnimationStateTu* state;
	FindAnimation(name, index, state);
	if (index == M_MAX_UNSIGNED || !state)
		return false;

	unsigned char layer = state->BlendLayer();

	bool needUpdate = false;
	for (unsigned i = 0; i < animations.size(); ++i)
	{
		if (i != index)
		{
			AnimationControlTu& control = animations[i];
			AnimationStateTu* otherState = GetAnimationState(control.hash_);
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

bool AnimationControllerTu::SetTime(const std::string& name, float time)
{
	unsigned index;
	AnimationStateTu* state;
	FindAnimation(name, index, state);
	if (index == M_MAX_UNSIGNED || !state)
		return false;

	time = Clamp(time, 0.0f, state->Length());
	state->SetTime(time);
	// Prepare "set time" command for network replication
	animations[index].setTime_ = (unsigned short)(time / state->Length() * 65535.0f);
	animations[index].setTimeTtl_ = COMMAND_STAY_TIME;
	++animations[index].setTimeRev_;

	return true;
}

bool AnimationControllerTu::IsAtEnd(const std::string& name) const
{
	unsigned index;
	AnimationStateTu* state;
	FindAnimation(name, index, state);
	if (index == M_MAX_UNSIGNED || !state)
		return false;
	else
		return state->Time() >= state->Length();
}

AnimationStateTu* AnimationControllerTu::GetAnimationState(StringHash nameHash) const
{
	// Model mode
	if (model)
		return model->FindAnimationState(nameHash);

	// Node hierarchy mode
	for (std::vector<SharedPtr<AnimationStateTu> >::const_iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i)
	{
		AnimationTu* animation = (*i)->GetAnimation();
		if (animation->NameHash() == nameHash || animation->AnimationNameHash() == nameHash)
			return *i;
	}

	return 0;
}

AnimationStateTu* AnimationControllerTu::AddAnimationState(AnimationTu* animation)
{
	if (!animation)
		return 0;

	// Model mode
	if (model)
		return model->AddAnimationState(animation);

	// Node hierarchy mode
	SharedPtr<AnimationStateTu> newState(new AnimationStateTu(node, animation));
	nodeAnimationStates.push_back(newState);
	return newState;
}

void AnimationControllerTu::RemoveAnimationState(AnimationStateTu* state)
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
	for (std::vector<SharedPtr<AnimationStateTu> >::iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i)
	{
		if ((*i) == state)
		{
			nodeAnimationStates.erase(i);
			return;
		}
	}
}

void AnimationControllerTu::FindAnimation(const std::string& name, unsigned& index, AnimationStateTu*& state) const
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