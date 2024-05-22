#include <iostream>
#include "AnimationState.h"

AnimationStateTrack::AnimationStateTrack() : m_track(nullptr), m_weight(1.0f), m_keyFrame(0){

}

AnimationStateTrack::~AnimationStateTrack(){

}

//const auto deleter = [](Animation* animation) {
	//delete animation;
//};


AnimationState::AnimationState(Animation* animation, BoneNode* startBone) :
	//m_animation(animation, [&](Animation* animation) {/*delete animation;*/}),
	m_animation(animation),
	m_looped(false),
	m_blendWeight(1.0f),
	m_stateTime(0.0f),
	m_blendLayer(0),
	m_startBone(startBone),
	m_animationBlendMode(AnimationBlendMode::ABM_LERP),
	m_backward(false){

	setStartBone(nullptr);
}

AnimationState::~AnimationState(){

}

void AnimationState::setStartBone(BoneNode* startBone_){
	
	if (!startBone_)
		startBone_ = m_startBone;

	// Do not reassign if the start bone did not actually change, and we already have valid bone nodes
	if (!m_stateTracks.empty())
		return;

	m_startBone = startBone_;

	const std::map<StringHash, AnimationTrack>& tracks = m_animation->getTracks();
	m_stateTracks.clear();

	for (auto it = tracks.begin(); it != tracks.end(); ++it){
		if (it->second.m_keyFrames.empty())
			continue;

		AnimationStateTrack stateTrack;
		stateTrack.m_track = &it->second;

		// Include those tracks that are either the start bone itself, or its children
		const StringHash& nameHash = it->second.m_nameHash;
		
		if (nameHash == m_startBone->m_nameHash) {
			stateTrack.m_node = m_startBone;
		}else {
			stateTrack.m_node = m_startBone->findChild<BoneNode>(nameHash, true);
		}

		stateTrack.m_initialPosition = Math::Lerp(it->second.m_keyFrames[0].m_position, it->second.m_keyFrames[1].m_position, EPSILON * 3.0f);
		stateTrack.m_initialScale = Math::Lerp(it->second.m_keyFrames[0].m_scale, it->second.m_keyFrames[1].m_scale, EPSILON * 3.0f);
		stateTrack.m_initialOrientation = Quaternion::SLerp2(it->second.m_keyFrames[0].m_rotation, it->second.m_keyFrames[1].m_rotation, EPSILON * 3.0f);
		stateTrack.m_initialOrientation.inverse();

		if (stateTrack.m_node)
			m_stateTracks.push_back(stateTrack);
	}
}

void AnimationState::setLooped(bool looped){
	m_looped = looped;
}

void AnimationState::setWeight(float weight){
	weight = Math::Clamp(weight, 0.0f, 1.0f);
	if (weight != m_blendWeight){
		m_blendWeight = weight;
	}
}

void AnimationState::setTime(float time_){
	time_ = Math::Clamp(time_, 0.0f, m_animation->getLength());
	if (time_ != m_stateTime){
		m_stateTime = time_;
	}
}

void AnimationState::setBoneWeight(size_t index, float weight_, bool recursive){
	if (index >= m_stateTracks.size())
		return;

	weight_ = Math::Clamp(weight_, 0.0f, 1.0f);

	if (weight_ != m_stateTracks[index].m_weight){
		m_stateTracks[index].m_weight = weight_;
	}

	if (recursive && m_stateTracks[index].m_node){
		const std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& children = m_stateTracks[index].m_node->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it){
			Node* node = (*it).get();
	
			size_t childTrackIndex = findTrackIndex(node);
			if (childTrackIndex < m_stateTracks.size())
				setBoneWeight(childTrackIndex, m_blendWeight, true);
				
		}
	}
}

void AnimationState::setBoneWeight(const std::string& name, float weight_, bool recursive){
	setBoneWeight(findTrackIndex(name), weight_, recursive);
}

void AnimationState::setBoneWeight(StringHash nameHash, float weight_, bool recursive){
	setBoneWeight(findTrackIndex(nameHash), weight_, recursive);
}

void AnimationState::addWeight(float delta){
	if (delta != 0.0f)
		setWeight(getWeight() + delta);
}

void AnimationState::addTime(float dt){
	float length = m_animation->getLength();
	if (dt == 0.0f || length == 0.0f)
		return;

	float oldTime = m_stateTime;
	float newTime = oldTime + dt;
	if (m_looped){
		while (newTime >= length)
			newTime -= length;
		while (newTime < 0.0f)
			newTime += length;
	}

	setTime(newTime);

	float ticksPerSecond = 1.0f;

	if (m_animationBlendMode == ABM_FADE) {
		m_blendWeight2 += dt;
		if (m_blendWeight2 >= 2.0f) {
			m_blendWeight2 = 0.0f;
			m_invertBlend = !m_invertBlend;
			m_additiveDirection *= -1.0f;
		}

		float blendWeight = m_blendWeight2;
		if (blendWeight < 0.0f) { blendWeight = 0.0f; }
		if (blendWeight > 1.0f) { blendWeight = 1.0f; }
		if (m_invertBlend) { blendWeight = 1.0f - blendWeight; }
		
		float a = length / m_fadeLayerLength;
		const float animSpeedMultiplierDown = (1.0f - blendWeight) * a + blendWeight;

		m_layeredTime += ticksPerSecond * dt * animSpeedMultiplierDown;

		if (m_layeredTime > length) {
			m_layeredTime = fmod(m_layeredTime, length);
		}
		setWeight(blendWeight);
	}
	
	if (m_animationBlendMode != ABM_ADDITIVE)
		return;

	float speed = 1.0f;
	float startTime = 0.0f;


	m_layeredTime += dt * m_additiveDirection * ticksPerSecond * speed;
	if (m_layeredTime < 0.0f) {
		m_layeredTime = 0.0f;
		m_additiveDirection *= -1.0f;
	}

	if (m_layeredTime > 1.0f) {
		m_layeredTime = 1.0f;
		m_additiveDirection *= -1.0f;
	}

	m_layeredTime = startTime + (length * m_layeredTime);	
}

void AnimationState::setBlendLayer(unsigned char layer){
	if (layer != m_blendLayer){
		m_blendLayer = layer;
	}
}

float AnimationState::getBoneWeight(size_t index) const {
	return index < m_stateTracks.size() ? m_stateTracks[index].m_weight : 0.0f;
}

float AnimationState::getBoneWeight(const std::string& name) const{
	return getBoneWeight(findTrackIndex(name));
}

float AnimationState::getBoneWeight(StringHash nameHash) const{
	return getBoneWeight(findTrackIndex(nameHash));
}

size_t AnimationState::findTrackIndex(Node* node) const{
	for (unsigned i = 0; i < m_stateTracks.size(); ++i){
		if (m_stateTracks[i].m_node == node)
			return i;
	}

	return UINT_MAX;
}

size_t AnimationState::findTrackIndex(const std::string& name) const{
	StringHash nameHash = StringHash(name);
	for (size_t i = 0; i < m_stateTracks.size(); ++i){
		BoneNode* node = m_stateTracks[i].m_node;
		if (node && node->m_nameHash == nameHash)
			return i;
	}

	return UINT_MAX;
}

size_t AnimationState::findTrackIndex(StringHash nameHash) const{
	for (unsigned i = 0; i < m_stateTracks.size(); ++i){
		BoneNode* node = m_stateTracks[i].m_node;
		if (node && node->m_nameHash == nameHash)
			return i;
	}

	return UINT_MAX;
}

float AnimationState::getLength() const{
	return m_animation->getLength();
}

void AnimationState::apply(){

	if (m_animationBlendMode == ABM_NONE)
		applyToNodes();		
	else
		applyToModel();
}

void AnimationState::applyToModel(){
	 
	for (auto it = m_stateTracks.begin(); it != m_stateTracks.end(); ++it){
		AnimationStateTrack& stateTrack = *it;
		const AnimationTrack* track = stateTrack.m_track;
		float finalWeight = m_blendWeight * stateTrack.m_weight;
		BoneNode* bone = stateTrack.m_node;

		if (Math::Equals(finalWeight, 0.0f) || !bone->animationEnabled())
			continue;
				
		if (m_animationBlendMode != ABM_LERP)
			track->findKeyFrameIndex(m_layeredTime, stateTrack.m_keyFrame);
		else
			track->findKeyFrameIndex(m_stateTime, stateTrack.m_keyFrame);

		const AnimationKeyFrame& keyFrame = track->m_keyFrames[stateTrack.m_keyFrame];

		size_t nextFrame = stateTrack.m_keyFrame + 1;
		bool interpolate = true;

		if (nextFrame >= track->m_keyFrames.size()) {
			if (m_backward)
				nextFrame = track->m_keyFrames.size();
			else if (m_looped) {
				nextFrame = 0;
			}else {
				nextFrame = stateTrack.m_keyFrame;
				interpolate = false;
			}				
		}
		
		Vector3f newPosition = bone->getPosition();
		Quaternion newRotation = bone->getOrientation();
		Vector3f newScale = bone->getScale();

		const AnimationKeyFrame& nextKeyFrame = track->m_keyFrames[nextFrame];
		float timeInterval = nextKeyFrame.m_time - keyFrame.m_time;

		if (timeInterval < 0.0f)
			timeInterval += m_animation->getLength();

		float t;
		if (m_animationBlendMode == ABM_ADDITIVE) {
			t = timeInterval + EPSILON > 0.0f ? (m_layeredTime - keyFrame.m_time) / (timeInterval + EPSILON) : 1.0f;
		}else if (m_animationBlendMode == ABM_FADE) {
			t = timeInterval > 0.0f ? (m_layeredTime - keyFrame.m_time) / timeInterval : 1.0f;
		}else {
			t = timeInterval > 0.0f ? (m_stateTime - keyFrame.m_time) / timeInterval : 1.0f;
		}

		if (track->m_channelMask & CHANNEL_POSITION)
			newPosition = Math::Lerp(keyFrame.m_position, nextKeyFrame.m_position, t);
		if (track->m_channelMask & CHANNEL_ROTATION)
			newRotation = Quaternion::SLerp2(keyFrame.m_rotation, nextKeyFrame.m_rotation, t);
		if (track->m_channelMask & CHANNEL_SCALE)
			newScale = Math::Lerp(keyFrame.m_scale, nextKeyFrame.m_scale, t);
		

		if (m_animationBlendMode == ABM_ADDITIVE) {
			if (track->m_channelMask & CHANNEL_POSITION){
				Vector3f delta = newPosition - stateTrack.m_initialPosition;
				newPosition = bone->getPosition() + delta * finalWeight;
			}

			if (track->m_channelMask & CHANNEL_ROTATION){
				newRotation = (newRotation * stateTrack.m_initialOrientation * bone->getOrientation());
				//newRotation.normalize();
				if (!Math::Equals(finalWeight, 1.0f))
					newRotation = Quaternion::SLerp2(bone->getOrientation(), newRotation, finalWeight);				
			}

			if (track->m_channelMask & CHANNEL_SCALE){
				Vector3f delta = newScale - stateTrack.m_initialScale;
				newScale = bone->getScale() + delta * finalWeight;
			}

		}else {
			if (finalWeight < 1.0f){
				if(track->m_channelMask & CHANNEL_POSITION)
					newPosition = Math::Lerp(bone->getPosition(), newPosition, finalWeight);
				if(track->m_channelMask & CHANNEL_ROTATION)
					newRotation = Quaternion::SLerp2(bone->getOrientation(), newRotation, finalWeight);
				if (track->m_channelMask & CHANNEL_SCALE)
					newScale = Math::Lerp(bone->getScale(), newScale, finalWeight);
			}		
		}

		bone->setTransformSilent(newPosition, newRotation, newScale);
		bone->OnTransformChanged();		
	}
}

void AnimationState::applyToNodes(){
	
	for (auto it = m_stateTracks.begin(); it != m_stateTracks.end(); ++it){

		AnimationStateTrack& stateTrack = *it;
		const AnimationTrack* track = stateTrack.m_track;
		BoneNode* node = stateTrack.m_node;

		track->findKeyFrameIndex(m_stateTime, stateTrack.m_keyFrame);
		const AnimationKeyFrame& keyFrame = track->m_keyFrames[stateTrack.m_keyFrame];

		size_t nextFrame = stateTrack.m_keyFrame + 1;
		bool interpolate = true;
		if (nextFrame >= track->m_keyFrames.size()){
			if (m_backward)
				nextFrame = track->m_keyFrames.size();
			else if (m_looped) {
				nextFrame = 0;
			}else {
				nextFrame = stateTrack.m_keyFrame;
				interpolate = false;
			}
		}
	
		Vector3f newPosition = node->getPosition();
		Quaternion newRotation = node->getOrientation();
		Vector3f newScale = node->getScale();

		if (interpolate) {

			const AnimationKeyFrame& nextKeyFrame = track->m_keyFrames[nextFrame];
			float timeInterval = nextKeyFrame.m_time - keyFrame.m_time;
			if (timeInterval < 0.0f)
				timeInterval += m_animation->getLength();


			float t = timeInterval > 0.0f ? (m_stateTime - keyFrame.m_time) / timeInterval : 1.0f;

			if(track->m_channelMask & CHANNEL_POSITION)
				newPosition = Math::Lerp(keyFrame.m_position, nextKeyFrame.m_position, t);

			if(track->m_channelMask & CHANNEL_ROTATION) {
				newRotation = Quaternion::SLerp2(keyFrame.m_rotation, nextKeyFrame.m_rotation, t);
			}

			if(track->m_channelMask & CHANNEL_SCALE)
				newScale = Math::Lerp(keyFrame.m_scale, nextKeyFrame.m_scale, t);
		}else{

			if(track->m_channelMask & CHANNEL_POSITION)
				newPosition = keyFrame.m_position;
			if(track->m_channelMask & CHANNEL_ROTATION)
				newRotation = keyFrame.m_rotation;
			if(track->m_channelMask & CHANNEL_SCALE)
				newScale = keyFrame.m_scale;
		}
		
		node->setPosition(newPosition);
		node->setOrientation(newRotation);
		node->setScale(newScale);
	}
}

void AnimationState::setBlendMode(AnimationBlendMode mode){
	m_animationBlendMode = mode;
}

void AnimationState::setBackward(bool backward) {
	m_backward = backward;
}

void AnimationState::setFadeLayerLength(float length) {
	m_fadeLayerLength = length;
}

const AnimationBlendMode AnimationState::getAnimationBlendMode() const {
	return m_animationBlendMode;
}

const float AnimationState::getRestTime() const {
	return std::max(getLength() - m_stateTime, m_stateTime);
}

Animation* AnimationState::getAnimation() const { 
	return m_animation; 
}

BoneNode* AnimationState::getStartBone() const { 
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