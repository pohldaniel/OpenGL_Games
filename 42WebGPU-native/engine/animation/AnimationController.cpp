#include <iostream>
#include "AnimatedModel.h"
#include "AnimationController.h"

AnimationController::AnimationController(AnimatedModel* animatedModel) : m_animatedModel(animatedModel) {
	m_animatedModel->m_hasAnimationController = true;
}

AnimationController::~AnimationController() {

}

void AnimationController::update(float dt) {
	// Loop through animations
	for (unsigned i = 0; i < m_animationControls.size();) {

		AnimationControl& ctrl = m_animationControls[i];
		AnimationState* state = getAnimationState(ctrl.m_name);

		bool remove = false;

		if (!state) {
			remove = true;
		}
		else {
			if (ctrl.m_speed != 0.0f) {
				state->addTime(ctrl.m_speed * dt);
			}

			float targetWeight = ctrl.m_targetWeight;
			float fadeTime = ctrl.m_fadeTime;
			float fadeTimeOffset = ctrl.m_fadeTimeOffset;

			// If non-looped animation at the end, activate autofade as applicable
			if (!state->isLooped() && state->getTime() >= state->getLength() && ctrl.m_autoFadeTime > 0.0f) {
				targetWeight = 0.0f;
				fadeTime = ctrl.m_autoFadeTime;
				//fadeTimeOffset = ctrl.autoFadeTime_;
			}

			// Process weight fade
			float currentWeight = state->getWeight();

			if (currentWeight != targetWeight) {
				if (fadeTime > 0.0f) {

					if (ctrl.m_weightOffset > 0.0f) {
						float weightDelta = 1.0f / fadeTimeOffset * dt;
						ctrl.m_weightOffset -= weightDelta;
					}
					else {
						float weightDelta = 1.0f / fadeTime * dt;
						if (ctrl.m_invertWeight) {

							if (currentWeight > targetWeight)
								currentWeight = std::max(currentWeight - weightDelta, targetWeight);
							else if (currentWeight < targetWeight)
								currentWeight = std::min(currentWeight + weightDelta, targetWeight);
							state->setWeight(currentWeight);
						}
						else {
							if (currentWeight < targetWeight)
								currentWeight = std::min(currentWeight + weightDelta, targetWeight);
							else if (currentWeight > targetWeight)
								currentWeight = std::max(currentWeight - weightDelta, targetWeight);
							state->setWeight(ctrl.m_invertWeight ? 1.0f - currentWeight : currentWeight);
						}
					}
				}
				else
					state->setWeight(targetWeight);
			}

			// Remove if weight zero and target weight zero
			if (state->getWeight() == 0.0f && (targetWeight == 0.0f || fadeTime == 0.0f) && ctrl.m_removeOnCompletion) {
				remove = true;
			}
		}

		if (remove) {
			if (state)
				removeAnimationState(state);
			m_animationControls.erase(m_animationControls.begin() + i);
		}
		else
			++i;
	}
}

bool AnimationController::play(const std::string& name, unsigned char layer, bool looped, float fadeInTime) {
	// Get the animation resource first to be able to get the canonical resource name
	// (avoids potential adding of duplicate animations)
	const Animation& newAnimation = AnimationManager::Get().getAnimation(name);
	
	if (!&newAnimation)
		return false;

	// Check if already exists
	unsigned index;
	AnimationState* state;
	findAnimation(newAnimation.m_animationName, index, state);

	if (!state) {
		state = addAnimationState(newAnimation);
		if (!state)
			return false;
		state->setWeight(0.0f);
	}

	if (index == UINT_MAX) {
		AnimationControl newControl;
		newControl.m_name = newAnimation.m_animationName;
		m_animationControls.push_back(newControl);
		index = m_animationControls.size() - 1;
	}

	state->setBlendLayer(layer);
	state->setLooped(looped);
	m_animationControls[index].m_targetWeight = 1.0f;
	m_animationControls[index].m_fadeTime = fadeInTime;

	return true;
}

bool AnimationController::playExclusive(const std::string& name, unsigned char layer, bool looped, float fadeTime) {
	bool success = play(name, layer, looped, fadeTime);

	// Fade other animations only if successfully started the new one
	if (success)
		fadeOthers(name, 0.0f, fadeTime);

	std::cout << "SUCCESS: " << success << std::endl;

	return success;
}

bool AnimationController::stop(const std::string& name, float fadeOutTime) {
	unsigned index;
	AnimationState* state;
	findAnimation(name, index, state);
	if (index != UINT_MAX) {
		m_animationControls[index].m_targetWeight = 0.0f;
		m_animationControls[index].m_fadeTime = fadeOutTime;
	}

	return index != UINT_MAX || state != 0;
}

void AnimationController::stopLayer(unsigned char layer, float fadeOutTime) {

	for (std::vector<AnimationControl>::iterator i = m_animationControls.begin(); i != m_animationControls.end(); ++i) {
		AnimationState* state = getAnimationState(i->m_name);

		if (state && state->getBlendLayer() == layer) {
			i->m_targetWeight = 0.0f;
			i->m_fadeTime = fadeOutTime;
		}
	}
}

void AnimationController::stopAll(float fadeOutTime) {
	if (m_animationControls.size()) {
		for (std::vector<AnimationControl>::iterator i = m_animationControls.begin(); i != m_animationControls.end(); ++i) {
			i->m_targetWeight = 0.0f;
			i->m_fadeTime = fadeOutTime;
		}
	}
}

bool AnimationController::fade(const std::string& name, float targetWeight, float fadeTime) {
	unsigned index;
	AnimationState* state;
	findAnimation(name, index, state);
	if (index == UINT_MAX)
		return false;

	m_animationControls[index].m_targetWeight = Math::Clamp(targetWeight, 0.0f, 1.0f);
	m_animationControls[index].m_fadeTime = fadeTime;
	return true;
}

bool AnimationController::fadeOthers(const std::string& name, float targetWeight, float fadeTime) {
	unsigned index;
	AnimationState* state;
	findAnimation(name, index, state);

	

	if (index == UINT_MAX || !state)
		return false;

	unsigned char layer = state->getBlendLayer();

	//std::cout << "Name1: " << name << "  " << index << "  " << m_animationControls.size() << std::endl;

	for (unsigned i = 0; i < m_animationControls.size(); ++i) {
		if (i != index) {
			AnimationControl& control = m_animationControls[i];
			AnimationState* otherState = getAnimationState(control.m_name);

			//std::cout << "NAME2: " << control.m_name << "  " << i << std::endl;

			if (otherState && otherState->getBlendLayer() == layer) {
				control.m_targetWeight = Math::Clamp(targetWeight, 0.0f, 1.0f);
				control.m_fadeTime = fadeTime;
			}
		}
	}

	return true;
}

float AnimationController::getRestTime(const std::string& name) {
	AnimationState* state = getAnimationState(name);
	return state ? state->getRestTime() : 0.0f;
}

bool AnimationController::fadeOtherExclusive(const std::string& targetName, float targetWeight, const float restTime, float weightOffset) {
	unsigned index;
	AnimationState* targetState;
	findAnimation(targetName, index, targetState);

	if (index == UINT_MAX || !targetState)
		return false;

	unsigned char layer = targetState->getBlendLayer();

	bool needUpdate = false;

	for (unsigned i = 0; i < m_animationControls.size(); ++i) {
		if (i != index) {
			AnimationControl& control = m_animationControls[i];
			AnimationState* otherState = getAnimationState(control.m_name);

			if (otherState && otherState->getBlendLayer() == layer) {
				control.m_targetWeight = Math::Clamp(targetWeight, 0.0f, 1.0f);
				control.m_fadeTimeOffset = restTime;
				control.m_weightOffset = weightOffset;
				control.m_fadeTime = 1.0f - control.m_weightOffset;
				needUpdate = true;
			}
		}
	}

	return true;
}

bool AnimationController::setTime(const std::string& name, float time) {
	unsigned index;
	AnimationState* state;
	findAnimation(name, index, state);
	if (index == UINT_MAX || !state)
		return false;

	time = Math::Clamp(time, 0.0f, state->getLength());
	state->setTime(time);

	return true;
}

bool AnimationController::isAtEnd(const std::string& name) const {
	unsigned index;
	AnimationState* state;
	findAnimation(name, index, state);

	if (index == UINT_MAX || !state)
		return false;
	else
		return state->getTime() >= state->getLength();
}

AnimationState* AnimationController::getAnimationState(const std::string& name) const {
	if (m_animatedModel)
		return m_animatedModel->findAnimationState(name);

	return nullptr;
}

const bool AnimationController::hasAnimationControl(const std::string& name) const {
	for (auto it = m_animationControls.begin(); it != m_animationControls.end(); ++it) {
		if ((*it).m_name == name)
			return true;
	}
	return false;
}

AnimationState* AnimationController::addAnimationStateFront(const Animation& animation, bool invertWeight, float weightOffset, float fadeTimeOffset) {
	if (!&animation)
		return nullptr;

	AnimationControl newControl;
	newControl.m_name = animation.m_animationName;
	newControl.m_invertWeight = invertWeight;
	newControl.m_weightOffset = weightOffset;
	newControl.m_fadeTimeOffset = fadeTimeOffset;

	if (!getAnimationState(newControl.m_name))
		m_animationControls.push_back(newControl);

	if (m_animatedModel) {
		AnimationState* state = m_animatedModel->addAnimationStateFront(animation);
		if (invertWeight) {
			state->setWeight(0.0f);
		}
		return state;
	}

	return nullptr;
}

AnimationState* AnimationController::addAnimationStateFront2(const Animation& animation, bool invertWeight, float weightOffset, float fadeTimeOffset) {
	if (!&animation)
		return nullptr;

	AnimationControl newControl;
	newControl.m_name = animation.m_animationName;
	newControl.m_invertWeight = invertWeight;
	newControl.m_weightOffset = weightOffset;
	newControl.m_fadeTimeOffset = fadeTimeOffset;

	//if (!getAnimationState(newControl.m_name)) {
	//	m_animationControls.push_back(newControl);
	//}

	if (!hasAnimationControl(newControl.m_name)) {
		m_animationControls.push_back(newControl);
	}

	std::cout << "SIZE: " << m_animationControls.size() << "  " << newControl.m_name << std::endl;
	if (m_animatedModel) {
		AnimationState* state = m_animatedModel->addAnimationStateFront(animation);
		if (invertWeight) {
			state->setWeight(0.0f);
		}
		return state;
	} 
	
	return nullptr;
}

AnimationState* AnimationController::addAnimationState(const Animation& animation) {
	if (!&animation)
		return nullptr;

	if (m_animatedModel)
		return m_animatedModel->addAnimationState(animation);

	return nullptr;
}

void AnimationController::removeAnimationState(AnimationState* state) {
	if (!state)
		return;

	if (m_animatedModel) {
		m_animatedModel->removeAnimationState(state);
		return;
	}
}

void AnimationController::findAnimation(const std::string& name, unsigned& index, AnimationState*& state) const {
	std::string animationName;
	
	state = getAnimationState(name);
	
	index = UINT_MAX;
	for (unsigned i = 0; i < m_animationControls.size(); ++i) {
		if (m_animationControls[i].m_name == name) {
			index = i;
			break;
		}
	}
}

bool AnimationController::setSpeed(const std::string& name, float speed) {
	unsigned index;
	AnimationState* state;
	findAnimation(name, index, state);
	if (index == UINT_MAX)
		return false;

	m_animationControls[index].m_speed = speed;
	return true;
}

float AnimationController::getTime(const std::string& name) const {
	AnimationState* state = getAnimationState(name);
	return state ? state->getTime() : 0.0f;
}

AnimatedModel* AnimationController::getAnimationNode() {
	return m_animatedModel;
}

AnimationManager AnimationManager::Instance;

Animation& AnimationManager::getAnimation(const std::string& name) {
	return m_animations[name];
}

bool AnimationManager::containsAnimation(const std::string& name) {
	return m_animations.count(name) == 1;
}

void AnimationManager::removeAnimation(const std::string& name) {
	if (containsAnimation(name)) {
		m_animations.erase(name);
	}
}

AnimationManager& AnimationManager::Get() {
	return Instance;
}