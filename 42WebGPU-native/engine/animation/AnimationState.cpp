#include "AnimationState.h"

AnimationStateTrack::AnimationStateTrack() : m_track(nullptr), m_bone(nullptr), m_weight(1.0f), m_keyFrame(0) {

}

AnimationStateTrack::~AnimationStateTrack() {

}

AnimationState::AnimationState(const Animation& animation, Bone* startBone) :
	m_animation(animation),
	m_startBone(startBone),
	m_stateTime(0.0f),
	m_blendLayer(0),
	m_looped(false),
	m_blendWeight(1.0f),
	m_animationBlendMode(AnimationBlendMode::ABM_LERP) {

	setStartBone(m_startBone);
}

AnimationState::~AnimationState() {

}

void AnimationState::setStartBone(Bone* startBone) {

	if (!startBone)
		startBone = m_startBone;

	if (!m_stateTracks.empty())
		return;

	m_startBone = startBone;

	const std::map<std::string, AnimationTrack>& tracks = m_animation.getTracks();
	m_stateTracks.clear();

	for (auto it = tracks.begin(); it != tracks.end(); ++it) {
		if (it->second.m_keyFrames.empty())
			continue;

		AnimationStateTrack stateTrack;
		stateTrack.m_track = &it->second;

		const std::string& name = it->second.m_name;
		if (name == m_startBone->m_name) {
			stateTrack.m_bone = m_startBone;
		}else {
			stateTrack.m_bone = m_startBone->findChild(name, true);
		}

		stateTrack.m_initialPosition = it->second.m_keyFrames[0].m_position;
		stateTrack.m_initialScale = it->second.m_keyFrames[0].m_scale;
		stateTrack.m_initialOrientation = it->second.m_keyFrames[0].m_rotation;

		if (stateTrack.m_bone)
			m_stateTracks.push_back(stateTrack);
	}
}

void AnimationState::setLooped(bool looped) {
	m_looped = looped;
}

void AnimationState::setWeight(float weight) {
	weight = Math::Clamp(weight, 0.0f, 1.0f);
	if (weight != m_blendWeight) {
		m_blendWeight = weight;
	}
}

void AnimationState::setTime(float time) {
	time = Math::Clamp(time, 0.0f, m_animation.getLength());
	if (time != m_stateTime) {
		m_stateTime = time;
	}
}

void AnimationState::addTime(float dt) {
	float length = m_animation.getLength();

	if (dt == 0.0f || length == 0.0f)
		return;

	float oldTime = m_stateTime;
	float newTime = oldTime + dt;
	if (m_looped) {
		while (newTime >= length)
			newTime -= length;
		while (newTime < 0.0f)
			newTime += length;
	}

	setTime(newTime);
}

void AnimationState::setBlendLayer(unsigned char layer) {
	if (layer != m_blendLayer) {
		m_blendLayer = layer;
	}
}

float AnimationState::getBoneWeight(size_t index) const {
	return index < m_stateTracks.size() ? m_stateTracks[index].m_weight : 0.0f;
}

float AnimationState::getBoneWeight(const std::string& name) const {
	return getBoneWeight(findTrackIndex(name));
}

size_t AnimationState::findTrackIndex(Bone* bone) const {
	for (unsigned i = 0; i < m_stateTracks.size(); ++i) {
		if (m_stateTracks[i].m_bone == bone)
			return i;
	}
	return UINT_MAX;
}

size_t AnimationState::findTrackIndex(const std::string& name) const {
	for (size_t i = 0; i < m_stateTracks.size(); ++i) {
		Bone* bone = m_stateTracks[i].m_bone;
		if (bone && bone->m_name == name)
			return i;
	}
	return UINT_MAX;
}

float AnimationState::getLength() const {
	return m_animation.getLength();
}

void AnimationState::apply() {
	applyToModel();		
}

void AnimationState::reset() {
	for (auto it = m_stateTracks.begin(); it != m_stateTracks.end(); ++it) {
		AnimationStateTrack& stateTrack = *it;
		stateTrack.m_keyFrame = 0u;
		stateTrack.m_weight = 1.0f;
	}
	m_stateTime = 0.0f;
}

void AnimationState::applyToModel() {

	for (auto it = m_stateTracks.begin(); it != m_stateTracks.end(); ++it) {
		AnimationStateTrack& stateTrack = *it;

		const AnimationTrack* track = stateTrack.m_track;
		float finalWeight = m_blendWeight * stateTrack.m_weight;
		Bone* bone = stateTrack.m_bone;

		if (Math::Equals(finalWeight, 0.0f) || !bone->animationEnabled())
			continue;
		
		track->findKeyFrameIndex(m_stateTime, stateTrack.m_keyFrame);

		size_t nextFrame = stateTrack.m_keyFrame + 1;
		bool interpolate = true;

		if (nextFrame >= track->m_keyFrames.size()) {
			if (m_looped){		
				nextFrame = 0u;
			}else {			
				nextFrame = stateTrack.m_keyFrame;
				interpolate = false;
			}
		}

		Vector3f newPosition = bone->m_position;
		Quaternion newRotation = bone->m_orientation;
		Vector3f newScale = bone->m_scale;

		const AnimationKeyFrame& keyFrame = track->m_keyFrames[stateTrack.m_keyFrame];
		const AnimationKeyFrame& nextKeyFrame = track->m_keyFrames[nextFrame];

		float timeInterval = nextKeyFrame.m_time - keyFrame.m_time;
		
		if (timeInterval < 0.0f)
			timeInterval += m_animation.getLength();

		float t = timeInterval > 0.0f ? (m_stateTime - keyFrame.m_time) / timeInterval : 1.0f;
		
		if (track->m_channelMask & CHANNEL_POSITION)
			newPosition = Math::Lerp(keyFrame.m_position, nextKeyFrame.m_position, t);
		if (track->m_channelMask & CHANNEL_ROTATION)
			newRotation = Quaternion::SLerp2(keyFrame.m_rotation, nextKeyFrame.m_rotation, t);
		if (track->m_channelMask & CHANNEL_SCALE)
			newScale = Math::Lerp(keyFrame.m_scale, nextKeyFrame.m_scale, t);
	
		if (finalWeight < 1.0f) {
			if (track->m_channelMask & CHANNEL_POSITION)
				newPosition = Math::Lerp(bone->m_position, newPosition, finalWeight);
			if (track->m_channelMask & CHANNEL_ROTATION)
				newRotation = Quaternion::SLerp2(bone->m_orientation, newRotation, finalWeight);
			if (track->m_channelMask & CHANNEL_SCALE)
				newScale = Math::Lerp(bone->m_scale, newScale, finalWeight);
		}
	
		bone->setTransformSilent(newPosition, newRotation, newScale);
		bone->OnTransformChanged();
	}
}

void AnimationState::setBlendMode(AnimationBlendMode mode) {
	m_animationBlendMode = mode;
}

const AnimationBlendMode AnimationState::getAnimationBlendMode() const {
	return m_animationBlendMode;
}

const Animation& AnimationState::getAnimation() const {
	return m_animation;
}

Bone* AnimationState::getStartBone() const {
	return m_startBone;
}

bool AnimationState::isEnabled() const {
	return m_blendWeight > 0.0f;
}

bool AnimationState::isLooped() const {
	return m_looped;
}

float AnimationState::getWeight() const {
	return m_blendWeight;
}

float AnimationState::getTime() const {
	return m_stateTime;
}

unsigned char AnimationState::getBlendLayer() const {
	return m_blendLayer;
}