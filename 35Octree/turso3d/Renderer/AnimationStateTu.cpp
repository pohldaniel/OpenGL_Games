// For conditions of distribution and use, see copyright notice in License.txt

#include "../IO/Log.h"
#include "AnimatedModelTu.h"
#include "AnimationTu.h"
#include "AnimationStateTu.h"

AnimationStateTrackTu::AnimationStateTrackTu() :
    track(nullptr),
    node(nullptr),
    weight(1.0f),
    keyFrame(0)
{
}

AnimationStateTrackTu::~AnimationStateTrackTu()
{
}

AnimationStateTu::AnimationStateTu(AnimatedModelDrawable* drawable_, AnimationTu* animation_) :
    drawable(drawable_),
    animation(animation_),
    looped(false),
    weight(0.0f),
    time(0.0f),
    blendLayer(0)
{
    assert(drawable);
    assert(animation);

    // Set default start bone (use all tracks.)
    SetStartBone(nullptr);
}

AnimationStateTu::AnimationStateTu(SpatialNode* node, AnimationTu* animation_) :
    rootNode(node),
    animation(animation_),
    startBone(nullptr),
    looped(false),
    weight(1.0f),
    time(0.0f),
    blendLayer(0)
{
    assert(node);
    assert(animation);

    const std::map<StringHash, AnimationTrackTu>& tracks = animation->Tracks();
    stateTracks.clear();

    for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
        if (it->second.keyFrames.empty())
            continue;

        AnimationStateTrackTu stateTrack;
        stateTrack.track = &it->second;

        if (node->NameHash() == it->second.nameHash || tracks.size() == 1)
            stateTrack.node = node;
        else
        {
            SpatialNode* targetNode = node->FindChild<SpatialNode>(it->second.nameHash, true);
            if (targetNode)
                stateTrack.node = targetNode;
            else
                LOGWARNING("Node " + it->second.name + " not found for node animation " + animation->Name());
        }

        if (stateTrack.node)
            stateTracks.push_back(stateTrack);
    }
}

AnimationStateTu::~AnimationStateTu()
{
}

void AnimationStateTu::SetStartBone(BoneTu* startBone_)
{
    if (!drawable)
        return;

    if (!startBone_)
        startBone_ = drawable->RootBone();

    // Do not reassign if the start bone did not actually change, and we already have valid bone nodes
    if (startBone_ == startBone && !stateTracks.empty())
        return;

    startBone = startBone_;

    const std::map<StringHash, AnimationTrackTu>& tracks = animation->Tracks();
    stateTracks.clear();

    for (auto it  = tracks.begin(); it != tracks.end(); ++it)
    {
        if (it->second.keyFrames.empty())
            continue;

        AnimationStateTrackTu stateTrack;
        stateTrack.track = &it->second;

        // Include those tracks that are either the start bone itself, or its children
        const StringHash& nameHash = it->second.nameHash;

        if (nameHash == startBone->NameHash())
            stateTrack.node = startBone;
        else
            stateTrack.node = startBone->FindChild<BoneTu>(nameHash, true);

        if (stateTrack.node)
            stateTracks.push_back(stateTrack);
    }

    drawable->OnAnimationOrderChanged();
}

void AnimationStateTu::SetLooped(bool looped_)
{
    looped = looped_;
}

void AnimationStateTu::SetWeight(float weight_)
{
    // Weight can only be set in model mode. In node animation it is hardcoded to full
    if (!drawable)
        return;

    weight_ = Clamp(weight_, 0.0f, 1.0f);
    if (weight_ != weight)
    {
        weight = weight_;
        drawable->OnAnimationChanged();
    }
}

void AnimationStateTu::SetTime(float time_)
{
    time_ = Clamp(time_, 0.0f, animation->Length());
    if (time_ != time)
    {
        time = time_;
        if (drawable && weight > 0.0f)
            drawable->OnAnimationChanged();
    }
}

void AnimationStateTu::SetBoneWeight(size_t index, float weight_, bool recursive)
{
    if (index >= stateTracks.size())
        return;

    weight_ = Clamp(weight_, 0.0f, 1.0f);

    if (weight_ != stateTracks[index].weight)
    {
        stateTracks[index].weight = weight_;
        if (drawable)
            drawable->OnAnimationChanged();
    }

    if (recursive && stateTracks[index].node)
    {
        const std::vector<SharedPtr<NodeTu> >& children = stateTracks[index].node->Children();
        for (auto it = children.begin(); it != children.end(); ++it)
        {
            NodeTu* node = *it;
            if (node->Type() == BoneTu::TypeStatic())
            {
                size_t childTrackIndex = FindTrackIndex(static_cast<BoneTu*>(node));
                if (childTrackIndex < stateTracks.size())
                    SetBoneWeight(childTrackIndex, weight, true);
            }
        }
    }
}

void AnimationStateTu::SetBoneWeight(const std::string& name, float weight_, bool recursive)
{
    SetBoneWeight(FindTrackIndex(name), weight_, recursive);
}

void AnimationStateTu::SetBoneWeight(StringHash nameHash, float weight_, bool recursive)
{
    SetBoneWeight(FindTrackIndex(nameHash), weight_, recursive);
}

void AnimationStateTu::AddWeight(float delta)
{
    if (delta != 0.0f)
        SetWeight(Weight() + delta);
}

void AnimationStateTu::AddTime(float delta)
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

void AnimationStateTu::SetBlendLayer(unsigned char layer)
{
    if (layer != blendLayer)
    {
        blendLayer = layer;
        if (drawable)
            drawable->OnAnimationOrderChanged();
    }
}

float AnimationStateTu::BoneWeight(size_t index) const
{
    return index < stateTracks.size() ? stateTracks[index].weight : 0.0f;
}

float AnimationStateTu::BoneWeight(const std::string& name) const
{
    return BoneWeight(FindTrackIndex(name));
}

float AnimationStateTu::BoneWeight(StringHash nameHash) const
{
    return BoneWeight(FindTrackIndex(nameHash));
}

size_t AnimationStateTu::FindTrackIndex(SpatialNode* node) const
{
    for (unsigned i = 0; i < stateTracks.size(); ++i)
    {
        if (stateTracks[i].node == node)
            return i;
    }

    return M_MAX_UNSIGNED;
}

size_t AnimationStateTu::FindTrackIndex(const std::string& name) const
{
    for (size_t i = 0; i < stateTracks.size(); ++i)
    {
        SpatialNode* node = stateTracks[i].node;
        if (node && node->Name() == name)
            return i;
    }

    return M_MAX_UNSIGNED;
}

size_t AnimationStateTu::FindTrackIndex(StringHash nameHash) const
{
    for (unsigned i = 0; i < stateTracks.size(); ++i)
    {
        NodeTu* node = stateTracks[i].node;
        if (node && node->NameHash() == nameHash)
            return i;
    }

    return M_MAX_UNSIGNED;
}

float AnimationStateTu::Length() const
{
    return animation->Length();
}

void AnimationStateTu::Apply()
{
    if (drawable)
        ApplyToModel();
    else
        ApplyToNodes();
}

void AnimationStateTu::ApplyToModel()
{
    for (auto it = stateTracks.begin(); it != stateTracks.end(); ++it)
    {
        AnimationStateTrackTu& stateTrack = *it;
        const AnimationTrackTu* track = stateTrack.track;
        float finalWeight = weight * stateTrack.weight;
        BoneTu* bone = static_cast<BoneTu*>(stateTrack.node);

        // Do not apply if zero effective weight or the bone has animation disabled
        if (Equals(finalWeight, 0.0f) || !bone->AnimationEnabled())
            continue;

        track->FindKeyFrameIndex(time, stateTrack.keyFrame);
        const AnimationKeyFrameTu& keyFrame = track->keyFrames[stateTrack.keyFrame];

        // Check if next frame to interpolate to is valid, or if wrapping is needed (looping animation only)
        size_t nextFrame = stateTrack.keyFrame + 1;
        bool interpolate = true;
        if (nextFrame >= track->keyFrames.size())
        {
            if (!looped)
            {
                nextFrame = stateTrack.keyFrame;
                interpolate = false;
            }
            else
                nextFrame = 0;
        }

        Vector3 newPosition = bone->Position();
		QuaternionTu newRotation = bone->Rotation();
        Vector3 newScale = bone->Scale();

        if (interpolate)
        {
            const AnimationKeyFrameTu& nextKeyFrame = track->keyFrames[nextFrame];
            float timeInterval = nextKeyFrame.time - keyFrame.time;
            if (timeInterval < 0.0f)
                timeInterval += animation->Length();
            float t = timeInterval > 0.0f ? (time - keyFrame.time) / timeInterval : 1.0f;

            if (track->channelMask & CHANNEL_POSITIONTU)
                newPosition = keyFrame.position.Lerp(nextKeyFrame.position, t);
            if (track->channelMask & CHANNEL_ROTATIONTU)
                newRotation = keyFrame.rotation.Slerp(nextKeyFrame.rotation, t);
            if (track->channelMask & CHANNEL_SCALETU)
                newScale = keyFrame.scale.Lerp(nextKeyFrame.scale, t);
        }
        else
        {
            if (track->channelMask & CHANNEL_POSITIONTU)
                newPosition = keyFrame.position;
            if (track->channelMask & CHANNEL_ROTATIONTU)
                newRotation = keyFrame.rotation;
            if (track->channelMask & CHANNEL_SCALETU)
                newScale = keyFrame.scale;
        }

        // If not full weight, blend
        if (weight < 1.0f)
        {
            if (track->channelMask & CHANNEL_POSITIONTU)
                newPosition = bone->Position().Lerp(newPosition, weight);
            if (track->channelMask & CHANNEL_ROTATIONTU)
                newRotation = bone->Rotation().Slerp(newRotation, weight);
            if (track->channelMask & CHANNEL_SCALETU)
                newScale = bone->Scale().Lerp(newScale, weight);
        }

        bone->SetTransformSilent(newPosition, newRotation, newScale);
    }
}

void AnimationStateTu::ApplyToNodes()
{
    // When applying to a node hierarchy, can only use full weight (nothing to blend to)
    for (auto it = stateTracks.begin(); it != stateTracks.end(); ++it)
    {
        AnimationStateTrackTu& stateTrack = *it;
        const AnimationTrackTu* track = stateTrack.track;
        SpatialNode* node = stateTrack.node;

        track->FindKeyFrameIndex(time, stateTrack.keyFrame);
        const AnimationKeyFrameTu& keyFrame = track->keyFrames[stateTrack.keyFrame];

        // Check if next frame to interpolate to is valid, or if wrapping is needed (looping animation only)
        size_t nextFrame = stateTrack.keyFrame + 1;
        bool interpolate = true;
        if (nextFrame >= track->keyFrames.size())
        {
            if (!looped)
            {
                nextFrame = stateTrack.keyFrame;
                interpolate = false;
            }
            else
                nextFrame = 0;
        }

        Vector3 newPosition = node->Position();
		QuaternionTu newRotation = node->Rotation();
        Vector3 newScale = node->Scale();

        if (interpolate)
        {
            const AnimationKeyFrameTu& nextKeyFrame = track->keyFrames[nextFrame];
            float timeInterval = nextKeyFrame.time - keyFrame.time;
            if (timeInterval < 0.0f)
                timeInterval += animation->Length();
            float t = timeInterval > 0.0f ? (time - keyFrame.time) / timeInterval : 1.0f;

            if (track->channelMask & CHANNEL_POSITIONTU)
                newPosition = keyFrame.position.Lerp(nextKeyFrame.position, t);
            if (track->channelMask & CHANNEL_ROTATIONTU)
                newRotation = keyFrame.rotation.Slerp(nextKeyFrame.rotation, t);
            if (track->channelMask & CHANNEL_SCALETU)
                newScale = keyFrame.scale.Lerp(nextKeyFrame.scale, t);
        }
        else
        {
            if (track->channelMask & CHANNEL_POSITIONTU)
                newPosition = keyFrame.position;
            if (track->channelMask & CHANNEL_ROTATIONTU)
                newRotation = keyFrame.rotation;
            if (track->channelMask & CHANNEL_SCALETU)
                newScale = keyFrame.scale;
        }

        node->SetTransform(newPosition, newRotation, newScale);
    }
}
