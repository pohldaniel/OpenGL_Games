#include "AnimationController.h"
#include "Globals.h"

AnimationController::AnimationController(AnimatedModel* model) : model(model) {
	model->m_hasAnimationController = true;
}

AnimationController::~AnimationController(){

}

void AnimationController::Update(float timeStep) {
	
	// Loop through animations
	for (unsigned i = 0; i < animations.size();){

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
			float fadeTimeOffset = ctrl.fadeTimeOffset_;

			// If non-looped animation at the end, activate autofade as applicable
			if (!state->Looped() && state->Time() >= state->Length() && ctrl.autoFadeTime_ > 0.0f)
			{
				targetWeight = 0.0f;
				fadeTime = ctrl.autoFadeTime_;
				//fadeTimeOffset = ctrl.autoFadeTime_;
			}
			if (ctrl.ragdollRecovery_ && ctrl.ragdollRecoverTime_ > 0.0f)
			{
				fadeTime = ctrl.ragdollRecoverTime_ * 10.0f;
				//fadeTimeOffset = ctrl.ragdollRecoverTime_ * 10.0f;
			}

			// Process weight fade
			float currentWeight =  state->Weight();

			//std::cout << "Name: " << state->GetAnimation()->animationName << " Weight: " << currentWeight << std::endl;
			if (currentWeight != targetWeight)
			{
				if (fadeTime > 0.0f)
				{					
					if (ctrl.weightOffset_ > 0.0f) {
						float weightDelta = 1.0f / fadeTimeOffset * timeStep;

						ctrl.weightOffset_ -= weightDelta;
					}else {
						float weightDelta = 1.0f / fadeTime * timeStep;
						if (ctrl.invertWeight_) {

							if (currentWeight > targetWeight)
								currentWeight = std::max(currentWeight - weightDelta, targetWeight);
							else if (currentWeight < targetWeight)
								currentWeight = std::min(currentWeight + weightDelta, targetWeight);
							state->SetWeight(currentWeight);
						}
						else {
							if (currentWeight < targetWeight)
								currentWeight = std::min(currentWeight + weightDelta, targetWeight);
							else if (currentWeight > targetWeight)
								currentWeight = std::max(currentWeight - weightDelta, targetWeight);
							state->SetWeight(ctrl.invertWeight_ ? 1.0f - currentWeight : currentWeight);
						}
					}
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
			ctrl.setTimeTtl_ = std::max(ctrl.setTimeTtl_ - timeStep, 0.0f);
		if (ctrl.setWeightTtl_ > 0.0f)
			ctrl.setWeightTtl_ = std::max(ctrl.setWeightTtl_ - timeStep, 0.0f);

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
	for (std::vector<std::shared_ptr<AnimationState> >::iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i) {
		(*i)->Apply();
	}
}

bool AnimationController::Play(const std::string& name, unsigned char layer, bool looped, float fadeInTime){
	// Get the animation resource first to be able to get the canonical resource name
	// (avoids potential adding of duplicate animations)
	
	Animation* newAnimation = Globals::animationManagerNew.getAssetPointer(name);

	if (!newAnimation)
		return false;

	// Check if already exists
	unsigned index;
	AnimationState* state;
	FindAnimation(newAnimation->animationName, index, state);

	if (!state){
		state = AddAnimationState(newAnimation);
		if (!state)
			return false;
		state->SetWeight(0.0f);
	}

	if (index == UINT_MAX){
		AnimationControl newControl;
		newControl.name_ = newAnimation->animationName;
		newControl.hash_ = newAnimation->animationNameHash;
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

bool AnimationController::PlayExclusive(const std::string& name, unsigned char layer, bool looped, float fadeTime){
	bool success = Play(name, layer, looped, fadeTime);

	// Fade other animations only if successfully started the new one
	if (success)
		FadeOthers(name, 0.0f, fadeTime);

	return success;
}

bool AnimationController::Stop(const std::string& name, float fadeOutTime){
	unsigned index;
	AnimationState* state;
	FindAnimation(name, index, state);
	if (index != UINT_MAX){
		animations[index].targetWeight_ = 0.0f;
		animations[index].fadeTime_ = fadeOutTime;
		//MarkNetworkUpdate();
	}

	return index != UINT_MAX || state != 0;
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
	if (index == UINT_MAX)
		return false;

	animations[index].targetWeight_ = Math::Clamp(targetWeight, 0.0f, 1.0f);
	animations[index].fadeTime_ = fadeTime;
	//MarkNetworkUpdate();
	return true;
}


bool AnimationController::FadeOthers(const std::string& name, float targetWeight, float fadeTime)
{
	unsigned index;
	AnimationState* state;
	FindAnimation(name, index, state);

	if (index == UINT_MAX || !state)
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
				control.targetWeight_ = Math::Clamp(targetWeight, 0.0f, 1.0f);
				control.fadeTime_ = fadeTime;
				needUpdate = true;
			}
		}
	}

	//if (needUpdate)
		//MarkNetworkUpdate();
	return true;
}

float AnimationController::getRestTime(const std::string& name) {
	AnimationState* state = GetAnimationState(StringHash(name));
	return state ? state->getRestTime() : 0.0f;
}

bool AnimationController::FadeOtherExclusive(const std::string& targetName, float targetWeight, const float restTime, float weightOffset) {
	unsigned index;
	AnimationState* targetState;
	FindAnimation(targetName, index, targetState);

	if (index == UINT_MAX || !targetState)
		return false;

	unsigned char layer = targetState->BlendLayer();

	bool needUpdate = false;

	for (unsigned i = 0; i < animations.size(); ++i){
		if (i != index){
			AnimationControl& control = animations[i];
			AnimationState* otherState = GetAnimationState(control.hash_);
			
			if (otherState && otherState->BlendLayer() == layer)
			{
				control.targetWeight_ = Math::Clamp(targetWeight, 0.0f, 1.0f);
				control.fadeTimeOffset_ = restTime;
				control.weightOffset_ = weightOffset;
				control.fadeTime_ = 1.0f - control.weightOffset_;
				needUpdate = true;
			}
		}
	}

	//if (needUpdate)
		//MarkNetworkUpdate();
	return true;
}

bool AnimationController::SetTime(const std::string& name, float time)
{
	unsigned index;
	AnimationState* state;
	FindAnimation(name, index, state);
	if (index == UINT_MAX || !state)
		return false;

	time = Math::Clamp(time, 0.0f, state->Length());
	state->SetTime(time);
	// Prepare "set time" command for network replication
	animations[index].setTime_ = (unsigned short)(time / state->Length() * 65535.0f);
	animations[index].setTimeTtl_ = COMMAND_STAY_TIME;
	++animations[index].setTimeRev_;

	return true;
}

bool AnimationController::IsAtEnd(const std::string& name) const
{
	unsigned index;
	AnimationState* state;
	FindAnimation(name, index, state);
	if (index == UINT_MAX || !state)
		return false;
	else
		return state->Time() >= state->Length();
}

AnimationState* AnimationController::GetAnimationState(StringHash nameHash) const{
	// Model mode
	if (model)
		return model->findAnimationState(nameHash);

	// Node hierarchy mode
	for (std::vector<std::shared_ptr<AnimationState> >::const_iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i){
		Animation* animation = (*i)->GetAnimation();
		if (animation->animationNameHash == nameHash)
			return (*i).get();
	}

	return 0;
}

AnimationState* AnimationController::AddAnimationStateFront(Animation* animation, bool invertWeight, float weightOffset, float fadeTimeOffset) {
	if (!animation)
		return nullptr;

	AnimationControl newControl;
	newControl.name_ = animation->animationName;
	newControl.hash_ = animation->animationNameHash;
	newControl.invertWeight_ = invertWeight;
	newControl.weightOffset_ = weightOffset;
	newControl.fadeTimeOffset_ = fadeTimeOffset;

	if(!GetAnimationState(newControl.hash_))
		animations.push_back(newControl);
	
	// Model mode
	if (model) {
		AnimationState* state = model->addAnimationStateFront(animation);
		if (invertWeight) {
			state->SetWeight(0.0f);		
		}
		return state;
	}

	// Node hierarchy mode
	std::shared_ptr<AnimationState> newState(new AnimationState(animation, model->m_meshes[0]->m_rootBone));
	nodeAnimationStates.insert(nodeAnimationStates.begin(),newState);
	return newState.get();

}

AnimationState* AnimationController::AddAnimationState2(Animation* animation) {
	if (!animation)
		return nullptr;

	AnimationControl newControl;
	newControl.name_ = animation->animationName;
	newControl.hash_ = animation->animationNameHash;

	if (!GetAnimationState(newControl.hash_))
		animations.push_back(newControl);

	// Model mode
	if (model) {
		AnimationState* state = model->addAnimationState(animation);
		return state;
	}

	// Node hierarchy mode
	std::shared_ptr<AnimationState> newState(new AnimationState(animation, model->m_meshes[0]->m_rootBone));
	nodeAnimationStates.insert(nodeAnimationStates.begin(), newState);
	return newState.get();
}


AnimationState* AnimationController::AddAnimationState(Animation* animation){
	if (!animation)
		return 0;

	// Model mode
	if (model)
		return model->addAnimationState(animation);

	// Node hierarchy mode
	std::shared_ptr<AnimationState> newState(new AnimationState(animation, model->m_meshes[0]->m_rootBone));
	nodeAnimationStates.push_back(newState);
	return newState.get();
}

void AnimationController::RemoveAnimationState(AnimationState* state){
	if (!state)
		return;

	// Model mode
	if (model){
		model->removeAnimationState(state);
		return;
	}

	// Node hierarchy mode
	for (std::vector<std::shared_ptr<AnimationState> >::iterator i = nodeAnimationStates.begin(); i != nodeAnimationStates.end(); ++i){
		if ((*i).get() == state){
			nodeAnimationStates.erase(i);
			return;
		}
	}
}

void AnimationController::FindAnimation(const std::string& name, unsigned& index, AnimationState*& state) const{
	StringHash nameHash(name);

	// Find the AnimationState
	state = GetAnimationState(nameHash);
	if (state){
		// Either a resource name or animation name may be specified. We store resource names, so correct the hash if necessary
		nameHash = state->GetAnimation()->animationNameHash;
	}

	// Find the internal control structure
	index = UINT_MAX;

	for (unsigned i = 0; i < animations.size(); ++i){
		if (animations[i].hash_ == nameHash){
			index = i;
			break;
		}
	}
}