// For conditions of distribution and use, see copyright notice in License.txt

#pragma once

#include <engine/utils/StringHash.h>
#include "../Object/Ptr.h"

#include <vector>

class AnimationTu;
class AnimatedModelDrawable;
class BoneTu;
class SpatialNode;
struct AnimationTrackTu;

/// %Animation instance per-track data.
struct AnimationStateTrackTu
{
    /// Construct.
    AnimationStateTrackTu();
    /// Destruct
    ~AnimationStateTrackTu();

    /// Animation track.
    const AnimationTrackTu* track;
    /// %Scene node. May be a model's bone or a plain scene node.
    SpatialNode* node;
    /// Blending weight.
    float weight;
    /// Last key frame.
    size_t keyFrame;
};

/// %Animation instance.
class AnimationStateTu : public RefCounted
{
public:
    /// Construct with animated model drawable and animation pointers.
    AnimationStateTu(AnimatedModelDrawable* drawable, AnimationTu* animation);
    /// Construct with root scene node and animation pointers.
    AnimationStateTu(SpatialNode* node, AnimationTu* animation);
    /// Destruct.
    ~AnimationStateTu() override;

    /// Set start bone. Not supported in node animation mode. Resets any assigned per-bone weights.
    void SetStartBone(BoneTu* startBone);
    /// Set looping enabled/disabled.
    void SetLooped(bool looped);
    /// Set blending weight.
    void SetWeight(float weight);
    /// Set time position.
    void SetTime(float time);
    /// Set per-bone blending weight by track index. Default is 1.0 (full), is multiplied  with the state's blending weight when applying the animation. Optionally recurses to child bones.
    void SetBoneWeight(size_t index, float weight, bool recursive = false);
    /// Set per-bone blending weight by name.
    void SetBoneWeight(const std::string& name, float weight, bool recursive = false);
    /// Set per-bone blending weight by name hash.
    void SetBoneWeight(StringHash nameHash, float weight, bool recursive = false);
    /// Modify blending weight.
    void AddWeight(float delta);
    /// Modify time position.
    void AddTime(float delta);
    /// Set blending layer.
    void SetBlendLayer(unsigned char layer);

    /// Return animation.
    AnimationTu* GetAnimation() const { return animation; }
    /// Return start bone.
    BoneTu* StartBone() const { return startBone; }
    /// Return per-bone blending weight by track index.
    float BoneWeight(size_t index) const;
    /// Return per-bone blending weight by name.
    float BoneWeight(const std::string& name) const;
    /// Return per-bone blending weight by name.
    float BoneWeight(StringHash nameHash) const;
    /// Return track index with matching bone node, or M_MAX_UNSIGNED if not found.
    size_t FindTrackIndex(SpatialNode* node) const;
    /// Return track index by bone name, or M_MAX_UNSIGNED if not found.
    size_t FindTrackIndex(const std::string& name) const;
    /// Return track index by bone name hash, or M_MAX_UNSIGNED if not found.
    size_t FindTrackIndex(StringHash nameHash) const;
    /// Return whether weight is nonzero.
    bool Enabled() const { return weight > 0.0f; }
    /// Return whether is looped.
    bool Looped() const { return looped; }
    /// Return blending weight.
    float Weight() const { return weight; }
    /// Return time position.
    float Time() const { return time; }
    /// Return animation length.
    float Length() const;
    /// Return blending layer.
    unsigned char BlendLayer() const { return blendLayer; }

    /// Apply the animation at the current time position. Called by AnimatedModel. Needs to be called manually for node hierarchies.
    void Apply();

private:
    /// Apply animation to a skeleton. Transform changes are applied silently, so the model needs to dirty its root model afterward.
    void ApplyToModel();
    /// Apply animation to a scene node hierarchy.
    void ApplyToNodes();

    /// Animated model drawable (model mode.)
    AnimatedModelDrawable* drawable;
    /// Root scene node (node hierarchy mode.)
    WeakPtr<SpatialNode> rootNode;
    /// %Animation resource.
    SharedPtr<AnimationTu> animation;
    /// Start bone.
    BoneTu* startBone;
    /// Per-track data.
    std::vector<AnimationStateTrackTu> stateTracks;
    /// Looped flag.
    bool looped;
    /// Blending weight.
    float weight;
    /// Time position.
    float time;
    /// Blending layer.
    unsigned char blendLayer;
};
