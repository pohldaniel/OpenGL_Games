#include <iostream>
#include "AnimationState.h"

AnimationStateTrack::AnimationStateTrack() :
	track(nullptr),
	//node(nullptr),
	weight(1.0f),
	keyFrame(0)
{
}

AnimationStateTrack::~AnimationStateTrack()
{
}

AnimationState::AnimationState(Animation* animation_, Bone* root) :
	//drawable(drawable_),
	animation(animation_),
	looped(false),
	weight(0.0f),
	time(0.0f),
	blendLayer(0),
	rootBone(root)
{
	//assert(drawable);
	//assert(animation);

	// Set default start bone (use all tracks.)
	SetStartBone(nullptr);
	SetWeight(1.0f);
}

AnimationState::~AnimationState(){

}

void AnimationState::SetStartBone(Bone* startBone_){
	//if (!drawable)
		//return;

	if (!startBone_)
		startBone_ = rootBone;

	// Do not reassign if the start bone did not actually change, and we already have valid bone nodes
	if (startBone_ == startBone && !stateTracks.empty())
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

		if (stateTrack.node)
			stateTracks.push_back(stateTrack);
	}

	//drawable->OnAnimationOrderChanged();
}

void AnimationState::SetLooped(bool looped_)
{
	looped = looped_;
}

void AnimationState::SetWeight(float weight_){

	//if (!drawable)
		//return;

	weight_ = Math::Clamp(weight_, 0.0f, 1.0f);
	if (weight_ != weight){
		weight = weight_;
		//drawable->OnAnimationChanged();
	}
}

void AnimationState::SetTime(float time_){
	time_ = Math::Clamp(time_, 0.0f, animation->Length());
	if (time_ != time){
		time = time_;
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
		const std::list<std::unique_ptr<BaseNode>>& children = stateTracks[index].node->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it){
			BaseNode* node = (*it).get();

			//if (node->Type() == Bone::TypeStatic())
			//{
				size_t childTrackIndex = FindTrackIndex(node);
				if (childTrackIndex < stateTracks.size())
					SetBoneWeight(childTrackIndex, weight, true);
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

void AnimationState::AddTime(float delta)
{
	float length = animation->Length();
	if (delta == 0.0f || length == 0.0f)
		return;

	float oldTime = time;
	float newTime = oldTime + delta;
	if (looped)
	{
		while (newTime >= length)
			newTime -= length;
		while (newTime < 0.0f)
			newTime += length;
	}

	SetTime(newTime);
}

void AnimationState::SetBlendLayer(unsigned char layer)
{
	if (layer != blendLayer)
	{
		blendLayer = layer;
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

size_t AnimationState::FindTrackIndex(BaseNode* node) const{
	for (unsigned i = 0; i < stateTracks.size(); ++i){
		if (stateTracks[i].node == node)
			return i;
	}

	return M_MAX_UNSIGNED;
}

size_t AnimationState::FindTrackIndex(const std::string& name) const{
	for (size_t i = 0; i < stateTracks.size(); ++i){
		Bone* node = stateTracks[i].node;
		if (node && node->name == name)
			return i;
	}

	return M_MAX_UNSIGNED;
}

size_t AnimationState::FindTrackIndex(StringHash nameHash) const
{
	for (unsigned i = 0; i < stateTracks.size(); ++i)
	{
		Bone* node = stateTracks[i].node;
		if (node && node->nameHash == nameHash)
			return i;
	}

	return M_MAX_UNSIGNED;
}

float AnimationState::Length() const{
	return animation->Length();
}

void AnimationState::Apply(){

	//if (drawable)
		//ApplyToModel();
	//else
		ApplyToNodes();
}

void AnimationState::ApplyToNodes(){
	
	for (auto it = stateTracks.begin(); it != stateTracks.end(); ++it){

		AnimationStateTrack& stateTrack = *it;
		const AnimationTrack* track = stateTrack.track;
		Bone* node = stateTrack.node;

		track->FindKeyFrameIndex(time, stateTrack.keyFrame);
		const AnimationKeyFrame& keyFrame = track->keyFrames[stateTrack.keyFrame];

		// Check if next frame to interpolate to is valid, or if wrapping is needed (looping animation only)
		size_t nextFrame = stateTrack.keyFrame + 1;
		bool interpolate = true;
		if (nextFrame >= track->keyFrames.size()){
			if (!looped){
				nextFrame = stateTrack.keyFrame;
				interpolate = false;
			}
			else
				nextFrame = 0;
		}
		//std::cout << "Name: " << node->name << std::endl;
		//std::cout << "Old Position: " << node->getPosition()[0] << "  " << node->getPosition()[1] << "  " << node->getPosition()[2] << std::endl;

		Vector3f newPosition = node->getPosition();
		Quaternion newRotation = node->getOrientation();
		Vector3f newScale = node->getScale();

		if (interpolate) {
			
			const AnimationKeyFrame& nextKeyFrame = track->keyFrames[nextFrame];
			float timeInterval = nextKeyFrame.time - keyFrame.time;
			if (timeInterval < 0.0f)
				timeInterval += animation->Length();
			float t = timeInterval > 0.0f ? (time - keyFrame.time) / timeInterval : 1.0f;

			if (track->channelMask & CHANNEL_POSITION) {
				newPosition = Math::Lerp(keyFrame.position, nextKeyFrame.position, t);
			}

			if (track->channelMask & CHANNEL_ROTATION)
				newRotation = Quaternion::SLerp2(keyFrame.rotation, nextKeyFrame.rotation, t);

			if (track->channelMask & CHANNEL_SCALE)
				newScale = Math::Lerp(keyFrame.scale, nextKeyFrame.scale, t);
		}else{
			if (track->channelMask & CHANNEL_POSITION)
				newPosition = keyFrame.position;
			if (track->channelMask & CHANNEL_ROTATION)
				newRotation = keyFrame.rotation;
			if (track->channelMask & CHANNEL_SCALE)
				newScale = keyFrame.scale;
		}
		node->setPosition(newPosition);
		node->setOrientation(newRotation);
		node->setScale(newScale);
		//node->SetTransform(newPosition, newRotation, newScale);

		//std::cout << "New Position: " << node->getPosition()[0] << "  " << node->getPosition()[1] << "  " << node->getPosition()[2] << std::endl;
		//std::cout << "---------------" << std::endl;
	}
}
