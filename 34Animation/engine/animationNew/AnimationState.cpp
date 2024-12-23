#include <iostream>
#include "AnimationState.h"

AnimationStateTrack::AnimationStateTrack() : track(nullptr), weight(1.0f),keyFrame(0){

}

AnimationStateTrack::~AnimationStateTrack(){

}

//const auto deleter = [](Animation* animation) {
	//delete animation;
//};


AnimationState::AnimationState(Animation* animation_, BoneNode* startBone) :
	//animation(animation_, [&](Animation* animation) {/*delete animation;*/}),
	animation(animation_),
	m_looped(false),
	m_blendWeight(1.0f),
	m_stateTime(0.0f),
	m_blendLayer(0),
	startBone(startBone),
	m_animationBlendMode(AnimationBlendMode::ABM_LERP),
	m_backward(false)
{

	SetStartBone(nullptr);
}

AnimationState::~AnimationState(){

}

void AnimationState::SetStartBone(BoneNode* startBone_){
	//if (!drawable)
		//return;

	if (!startBone_)
		startBone_ = startBone;

	// Do not reassign if the start bone did not actually change, and we already have valid bone nodes
	if (!stateTracks.empty())
		return;

	startBone = startBone_;

	const std::map<StringHash, AnimationTrack>& tracks = animation->Tracks();
	stateTracks.clear();

	for (auto it = tracks.begin(); it != tracks.end(); ++it){
		if (it->second.keyFrames.empty())
			continue;

		AnimationStateTrack stateTrack;
		stateTrack.track = &it->second;

		// Include those tracks that are either the start bone itself, or its children
		const StringHash& nameHash = it->second.nameHash;
		
		if (nameHash == startBone->nameHash) {
			stateTrack.node = startBone;
		}else {
			stateTrack.node = startBone->FindChildOfType(nameHash, true);
		}

		stateTrack.m_initialPosition = Math::Lerp(it->second.keyFrames[0].position, it->second.keyFrames[1].position, EPSILON * 3.0f);
		stateTrack.m_initialScale = Math::Lerp(it->second.keyFrames[0].scale, it->second.keyFrames[1].scale, EPSILON * 3.0f);
		stateTrack.m_initialOrientation = Quaternion::SLerp2(it->second.keyFrames[0].rotation, it->second.keyFrames[1].rotation, EPSILON * 3.0f);		
		stateTrack.m_initialOrientation.inverse();

		if (stateTrack.node)
			stateTracks.push_back(stateTrack);
	}

	//drawable->OnAnimationOrderChanged();

}

void AnimationState::SetLooped(bool looped_)
{
	m_looped = looped_;
}

void AnimationState::SetWeight(float weight_){

	//if (!drawable)
		//return;

	weight_ = Math::Clamp(weight_, 0.0f, 1.0f);
	if (weight_ != m_blendWeight){
		m_blendWeight = weight_;
		//drawable->OnAnimationChanged();
	}
}

void AnimationState::SetTime(float time_){
	time_ = Math::Clamp(time_, 0.0f, animation->Length());
	if (time_ != m_stateTime){
		m_stateTime = time_;
		//if (drawable && weight > 0.0f)
			//drawable->OnAnimationChanged();
	}
}

void AnimationState::SetBoneWeight(size_t index, float weight_, bool recursive){
	if (index >= stateTracks.size())
		return;

	weight_ = Math::Clamp(weight_, 0.0f, 1.0f);

	if (weight_ != stateTracks[index].weight){
		stateTracks[index].weight = weight_;
		//if (drawable)
			//drawable->OnAnimationChanged();
	}

	if (recursive && stateTracks[index].node){
		const std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& children = stateTracks[index].node->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it){
			Node* node = (*it).get();

			//if (node->Type() == Bone::TypeStatic())
			//{
				size_t childTrackIndex = FindTrackIndex(node);
				if (childTrackIndex < stateTracks.size())
					SetBoneWeight(childTrackIndex, m_blendWeight, true);
				//}
		}
	}
}

void AnimationState::SetBoneWeight(const std::string& name, float weight_, bool recursive)
{
	SetBoneWeight(FindTrackIndex(name), weight_, recursive);
}

void AnimationState::SetBoneWeight(StringHash nameHash, float weight_, bool recursive)
{
	SetBoneWeight(FindTrackIndex(nameHash), weight_, recursive);
}

void AnimationState::AddWeight(float delta)
{
	if (delta != 0.0f)
		SetWeight(Weight() + delta);
}

void AnimationState::AddTime(float dt)
{
	float length = animation->Length();
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

	SetTime(newTime);

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
		SetWeight(blendWeight);
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

void AnimationState::SetBlendLayer(unsigned char layer){
	if (layer != m_blendLayer){
		m_blendLayer = layer;
		//if (drawable)
			//drawable->OnAnimationOrderChanged();
	}
}

float AnimationState::BoneWeight(size_t index) const{
	return index < stateTracks.size() ? stateTracks[index].weight : 0.0f;
}

float AnimationState::BoneWeight(const std::string& name) const{
	return BoneWeight(FindTrackIndex(name));
}

float AnimationState::BoneWeight(StringHash nameHash) const{
	return BoneWeight(FindTrackIndex(nameHash));
}

size_t AnimationState::FindTrackIndex(Node* node) const{
	for (unsigned i = 0; i < stateTracks.size(); ++i){
		if (stateTracks[i].node == node)
			return i;
	}

	return UINT_MAX;
}

size_t AnimationState::FindTrackIndex(const std::string& name) const{
	for (size_t i = 0; i < stateTracks.size(); ++i){
		BoneNode* node = stateTracks[i].node;
		if (node && node->name == name)
			return i;
	}

	return UINT_MAX;
}

size_t AnimationState::FindTrackIndex(StringHash nameHash) const
{
	for (unsigned i = 0; i < stateTracks.size(); ++i)
	{
		BoneNode* node = stateTracks[i].node;
		if (node && node->nameHash == nameHash)
			return i;
	}

	return UINT_MAX;
}

float AnimationState::Length() const{
	return animation->Length();
}

void AnimationState::Apply(){

	if (m_animationBlendMode == ABM_NONE)
		ApplyToNodes();		
	else
		ApplyToModel();
}

void AnimationState::ApplyToModel(){
	 
	for (auto it = stateTracks.begin(); it != stateTracks.end(); ++it){
		AnimationStateTrack& stateTrack = *it;
		const AnimationTrack* track = stateTrack.track;
		float finalWeight = m_blendWeight * stateTrack.weight;
		BoneNode* bone = stateTrack.node;

		if (Math::Equals(finalWeight, 0.0f) || !bone->AnimationEnabled())
			continue;
				
		if (m_animationBlendMode != ABM_LERP)
			track->FindKeyFrameIndex(m_layeredTime, stateTrack.keyFrame);
		else
			track->FindKeyFrameIndex(m_stateTime, stateTrack.keyFrame);

		const AnimationKeyFrame& keyFrame = track->keyFrames[stateTrack.keyFrame];

		size_t nextFrame = stateTrack.keyFrame + 1;
		bool interpolate = true;

		if (nextFrame >= track->keyFrames.size()) {
			if (m_backward)
				nextFrame = track->keyFrames.size();
			else if (m_looped) {
				nextFrame = 0;
			}else {
				nextFrame = stateTrack.keyFrame;
				interpolate = false;
			}				
		}
		
		Vector3f newPosition = bone->getPosition();
		Quaternion newRotation = bone->getOrientation();
		Vector3f newScale = bone->getScale();

		const AnimationKeyFrame& nextKeyFrame = track->keyFrames[nextFrame];
		float timeInterval = nextKeyFrame.time - keyFrame.time;

		if (timeInterval < 0.0f)
			timeInterval += animation->Length();

		float t;
		if (m_animationBlendMode == ABM_ADDITIVE) {
			t = timeInterval + EPSILON > 0.0f ? (m_layeredTime - keyFrame.time) / (timeInterval + EPSILON) : 1.0f;
		}else if (m_animationBlendMode == ABM_FADE) {
			t = timeInterval > 0.0f ? (m_layeredTime - keyFrame.time) / timeInterval : 1.0f;
		}else {
			t = timeInterval > 0.0f ? (m_stateTime - keyFrame.time) / timeInterval : 1.0f;
		}

		if (track->channelMask & CHANNEL_POSITION)
			newPosition = Math::Lerp(keyFrame.position, nextKeyFrame.position, t);
		if (track->channelMask & CHANNEL_ROTATION)
			newRotation = Quaternion::SLerp2(keyFrame.rotation, nextKeyFrame.rotation, t);
		if (track->channelMask & CHANNEL_SCALE)
			newScale = Math::Lerp(keyFrame.scale, nextKeyFrame.scale, t);
		

		if (m_animationBlendMode == ABM_ADDITIVE) {
			if (track->channelMask & CHANNEL_POSITION){
				Vector3f delta = newPosition - stateTrack.m_initialPosition;
				newPosition = bone->getPosition() + delta * finalWeight;
			}

			if (track->channelMask & CHANNEL_ROTATION){
				newRotation = (newRotation * stateTrack.m_initialOrientation * bone->getOrientation());
				//newRotation.normalize();
				if (!Math::Equals(finalWeight, 1.0f))
					newRotation = Quaternion::SLerp2(bone->getOrientation(), newRotation, finalWeight);				
			}

			if (track->channelMask & CHANNEL_SCALE){
				Vector3f delta = newScale - stateTrack.m_initialScale;
				newScale = bone->getScale() + delta * finalWeight;
			}

		}else {
			if (finalWeight < 1.0f){
				if(track->channelMask & CHANNEL_POSITION)
					newPosition = Math::Lerp(bone->getPosition(), newPosition, finalWeight);
				if(track->channelMask & CHANNEL_ROTATION)
					newRotation = Quaternion::SLerp2(bone->getOrientation(), newRotation, finalWeight);
				if (track->channelMask & CHANNEL_SCALE)
					newScale = Math::Lerp(bone->getScale(), newScale, finalWeight);
			}		
		}

		bone->SetTransformSilent(newPosition, newRotation, newScale);
		bone->OnTransformChanged();		
	}
}

void AnimationState::ApplyToNodes(){
	
	for (auto it = stateTracks.begin(); it != stateTracks.end(); ++it){

		AnimationStateTrack& stateTrack = *it;
		const AnimationTrack* track = stateTrack.track;
		BoneNode* node = stateTrack.node;

		track->FindKeyFrameIndex(m_stateTime, stateTrack.keyFrame);
		const AnimationKeyFrame& keyFrame = track->keyFrames[stateTrack.keyFrame];

		size_t nextFrame = stateTrack.keyFrame + 1;
		bool interpolate = true;
		if (nextFrame >= track->keyFrames.size()){
			if (m_backward)
				nextFrame = track->keyFrames.size();
			else if (m_looped) {
				nextFrame = 0;
			}else {
				nextFrame = stateTrack.keyFrame;
				interpolate = false;
			}
		}
	
		Vector3f newPosition = node->getPosition();
		Quaternion newRotation = node->getOrientation();
		Vector3f newScale = node->getScale();

		if (interpolate) {

			const AnimationKeyFrame& nextKeyFrame = track->keyFrames[nextFrame];
			float timeInterval = nextKeyFrame.time - keyFrame.time;
			if (timeInterval < 0.0f)
				timeInterval += animation->Length();


			float t = timeInterval > 0.0f ? (m_stateTime - keyFrame.time) / timeInterval : 1.0f;

			if(track->channelMask & CHANNEL_POSITION)
				newPosition = Math::Lerp(keyFrame.position, nextKeyFrame.position, t);

			if(track->channelMask & CHANNEL_ROTATION) {
				newRotation = Quaternion::SLerp2(keyFrame.rotation, nextKeyFrame.rotation, t);
			}

			if(track->channelMask & CHANNEL_SCALE)
				newScale = Math::Lerp(keyFrame.scale, nextKeyFrame.scale, t);
		}else{

			if(track->channelMask & CHANNEL_POSITION)
				newPosition = keyFrame.position;
			if(track->channelMask & CHANNEL_ROTATION)
				newRotation = keyFrame.rotation;
			if(track->channelMask & CHANNEL_SCALE)
				newScale = keyFrame.scale;
		}
		
		node->setPosition(newPosition);
		node->setOrientation(newRotation);
		node->setScale(newScale);
	}
}

void AnimationState::SetBlendMode(AnimationBlendMode mode){
	m_animationBlendMode = mode;
}

void AnimationState::SetBackward(bool backward) {
	m_backward = backward;
}

void AnimationState::SetFadeLayerLength(float length) {
	m_fadeLayerLength = length;
}

const AnimationBlendMode AnimationState::getAnimationBlendMode() const {
	return m_animationBlendMode;
}

const float AnimationState::getRestTime() const {
	return std::max(Length() - m_stateTime, m_stateTime);
}