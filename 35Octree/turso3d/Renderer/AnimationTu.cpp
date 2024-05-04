// For conditions of distribution and use, see copyright notice in License.txt

#include "../IO/Log.h"
#include "../IO/Stream.h"
#include "AnimationTu.h"

#include <tracy/Tracy.hpp>

void AnimationTrackTu::FindKeyFrameIndex(float time, size_t& index) const
{
    if (time < 0.0f)
        time = 0.0f;

    if (index >= keyFrames.size())
        index = keyFrames.size() - 1;

    // Check for being too far ahead
    while (index && time < keyFrames[index].time)
        --index;

    // Check for being too far behind
    while (index < keyFrames.size() - 1 && time >= keyFrames[index + 1].time)
        ++index;
}

AnimationTu::AnimationTu() :
    length(0.0f)
{
}

AnimationTu::~AnimationTu()
{
}

void AnimationTu::RegisterObject()
{
    RegisterFactory<AnimationTu>();
}

bool AnimationTu::BeginLoad(Stream& source)
{
    ZoneScoped;

    /// \todo Develop own format for Turso3D
    if (source.ReadFileID() != "UANI")
    {
        LOGERROR(source.Name() + " is not a valid animation file");
        return false;
    }

    // Read name and length
    animationName = source.Read<std::string>();
    animationNameHash = animationName;
    length = source.Read<float>();
    tracks.clear();

    size_t numTracks = source.Read<unsigned>();

    // Read tracks
    for (size_t i = 0; i < numTracks; ++i)
    {
        AnimationTrackTu* newTrack = CreateTrack(source.Read<std::string>());
        newTrack->channelMask = source.Read<unsigned char>();

        size_t numKeyFrames = source.Read<unsigned>();
        newTrack->keyFrames.resize(numKeyFrames);

        // Read keyframes of the track
        for (size_t j = 0; j < numKeyFrames; ++j)
        {
            AnimationKeyFrameTu& newKeyFrame = newTrack->keyFrames[j];
            newKeyFrame.time = source.Read<float>();
            if (newTrack->channelMask & CHANNEL_POSITION)
                newKeyFrame.position = source.Read<Vector3>();
            if (newTrack->channelMask & CHANNEL_ROTATION)
                newKeyFrame.rotation = source.Read<QuaternionTu>();
            if (newTrack->channelMask & CHANNEL_SCALE)
                newKeyFrame.scale = source.Read<Vector3>();
        }
    }

    return true;
}

void AnimationTu::SetAnimationName(const std::string& name_)
{
    animationName = name_;
    animationNameHash = StringHash(name_);
}

void AnimationTu::SetLength(float length_)
{
    length = Max(length_, 0.0f);
}

AnimationTrackTu* AnimationTu::CreateTrack(const std::string& name_)
{
    StringHash nameHash_(name_);
    AnimationTrackTu* oldTrack = FindTrack(nameHash_);
    if (oldTrack)
        return oldTrack;

    AnimationTrackTu& newTrack = tracks[nameHash_];
    newTrack.name = name_;
    newTrack.nameHash = nameHash_;
    return &newTrack;
}

void AnimationTu::RemoveTrack(const std::string& name_)
{
    auto it = tracks.find(StringHash(name_));
    if (it != tracks.end())
        tracks.erase(it);
}

void AnimationTu::RemoveAllTracks()
{
    tracks.clear();
}

AnimationTrackTu* AnimationTu::Track(size_t index) const
{
    if (index >= tracks.size())
        return nullptr;

    size_t j = 0;
    for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
        if (j == index)
            return const_cast<AnimationTrackTu*>(&(it->second));
        ++j;
    }

    return nullptr;
}

AnimationTrackTu* AnimationTu::FindTrack(const std::string& name_) const
{
    auto it = tracks.find(StringHash(name_));
    return it != tracks.end() ? const_cast<AnimationTrackTu*>(&(it->second)) : nullptr;
}

AnimationTrackTu* AnimationTu::FindTrack(StringHash nameHash_) const
{
    auto it = tracks.find(nameHash_);
    return it != tracks.end() ? const_cast<AnimationTrackTu*>(&(it->second)) : nullptr;
}
