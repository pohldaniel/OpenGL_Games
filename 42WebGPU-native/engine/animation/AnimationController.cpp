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
					}else {
						float weightDelta = 1.0f / fadeTime * dt;
						if (ctrl.m_invertWeight) {

							if (currentWeight > targetWeight)
								currentWeight = std::max(currentWeight - weightDelta, targetWeight);
							else if (currentWeight < targetWeight)
								currentWeight = std::min(currentWeight + weightDelta, targetWeight);
							state->setWeight(currentWeight);
						}else {
							if (currentWeight < targetWeight)
								currentWeight = std::min(currentWeight + weightDelta, targetWeight);
							else if (currentWeight > targetWeight)
								currentWeight = std::max(currentWeight - weightDelta, targetWeight);
							state->setWeight(ctrl.m_invertWeight ? 1.0f - currentWeight : currentWeight);
						}
					}
				}else
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

//fades from the current Animmation to the target with fadetime uses fadeOut for fading back
void AnimationController::fadeAndPlay(const std::string& target, float fadeInTime, float fadeBackTime) {

	if (!m_animationControls.empty()) {
		if (m_animationControls.size() > 1u && fadeBackTime) {
			m_animationControls.front().m_targetWeight = 0.0f;
			m_animationControls.front().m_fadeTime = fadeBackTime;

			m_animationControls.back().m_targetWeight = 1.0f;
			m_animationControls.back().m_fadeTime = fadeBackTime;
		}

		if (m_animationControls.back().m_name == target) {
			return;
		}

		m_animationControls.back().m_targetWeight = 0.0f;
		m_animationControls.back().m_fadeTime = fadeInTime;
	}
	addAnimationStateFront(AnimationManager::Get().getAnimation(target), 0.0f, 1.0f, false, 0.0f, 0.0f);
}

bool AnimationController::play(const std::string& name, bool looped, float fadeInTime) {
	const Animation& animation = AnimationManager::Get().getAnimation(name);

	unsigned index;
	AnimationState* state;
	findAnimation(animation.m_animationName, index, state);

	if (!state) {
		state = m_animatedModel->addAnimationState(animation);

		if (!state)
			return false;
		state->setWeight(0.0f);
		state->setLooped(looped);
	}

	if (index == UINT_MAX) {

		AnimationControl newControl;
		newControl.m_name = animation.m_animationName;
		m_animationControls.push_back(newControl);
		index = m_animationControls.size() - 1;
	}
	m_animationControls[index].m_targetWeight = 1.0f;
	m_animationControls[index].m_fadeTime = fadeInTime;

	return true;
}

AnimationState* AnimationController::addAnimationStateFront(const Animation& animation, float fadeTime, float targetWeight, bool invertWeight, float weightOffset, float fadeTimeOffset) {
	if (!&animation)
		return nullptr;

	AnimationControl newControl;
	newControl.m_name = animation.m_animationName;
	newControl.m_invertWeight = invertWeight;
	newControl.m_weightOffset = weightOffset;
	newControl.m_fadeTimeOffset = fadeTimeOffset;
	newControl.m_targetWeight = targetWeight;
	newControl.m_fadeTime = fadeTime;

	if (!getAnimationState(newControl.m_name))
		m_animationControls.insert(m_animationControls.begin(), newControl);

	if (m_animatedModel) {
		AnimationState* state = m_animatedModel->addAnimationStateFront(animation);
		state->setLooped(true);
		if (invertWeight) {
			state->setWeight(0.0f);			
		}
		return state;
	}

	return nullptr;
}

AnimationState* AnimationController::getAnimationState(const std::string& name) const {
	if (m_animatedModel)
		return m_animatedModel->findAnimationState(name);

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

AnimatedModel* AnimationController::getAnimatedModel() {
	return m_animatedModel;
}

///////////////////////////////////////////////////////////////////////////////////////////////
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