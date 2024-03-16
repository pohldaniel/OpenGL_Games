// For conditions of distribution and use, see copyright notice in License.txt

#pragma once

#include <engine/scene/SceneNodeLC.h>
#include <engine/scene/ShapeNode.h>

class Octant;
class Octree;

static const unsigned short DF_STATIC_GEOMETRY = 0x0;
static const unsigned short DF_SKINNED_GEOMETRY = 0x1;
static const unsigned short DF_INSTANCED_GEOMETRY = 0x2;
static const unsigned short DF_CUSTOM_GEOMETRY = 0x3;
static const unsigned short DF_GEOMETRY_TYPE_BITS = 0x3;
static const unsigned short DF_LIGHT = 0x4;
static const unsigned short DF_GEOMETRY = 0x8;
static const unsigned short DF_STATIC = 0x10;
static const unsigned short DF_CAST_SHADOWS = 0x20;
static const unsigned short DF_UPDATE_INVISIBLE = 0x40;
static const unsigned short DF_HAS_LOD_LEVELS = 0x80;
static const unsigned short DF_OCTREE_UPDATE_CALL = 0x100;
static const unsigned short DF_WORLD_TRANSFORM_DIRTY = 0x200;
static const unsigned short DF_BOUNDING_BOX_DIRTY = 0x400;
static const unsigned short DF_OCTREE_REINSERT_QUEUED = 0x800;

/// Common base class for renderable scene objects and occluders.
class OctreeNodeBase : public SceneNodeLC{
    friend class Octree;

public:

    OctreeNodeBase();

protected:

    Octree* octree;
	ShapeNode* drawable;
};

/// Base class for scene nodes that insert drawables to the octree for rendering.
class OctreeNode : public OctreeNodeBase {

public:

    /// Set whether is static. Used for optimizations. A static node should not move after scene load. Default false.
    void SetStatic(bool enable);
    /// Set whether to cast shadows. Default false on both lights and geometries.
    void SetCastShadows(bool enable);
    /// Set whether to update animation when invisible. Default false for better performance.
    void SetUpdateInvisible(bool enable);
    /// Set max distance for rendering. 0 is unlimited.
    void SetMaxDistance(float distance);
    
    /// Return drawable's world space bounding box. Update if necessary. 
    const BoundingBox& WorldBoundingBox() const { return drawable->getWorldBoundingBox(); }
    /// Return whether is static.
    bool IsStatic() const { return true; }
    /// Return whether casts shadows.
    bool CastShadows() const { return false; }
    /// Return whether updates animation when invisible. Not relevant for non-animating geometry.
    bool UpdateInvisible() const { return false; }
    /// Return current octree this node resides in.
    Octree* GetOctree() const { return octree; }
    /// Return the drawable for internal use.
	ShapeNode* GetDrawable() const { return drawable; }
    /// Return distance from camera in the current view.
    //float Distance() const { return drawable->Distance(); }
    /// Return max distance for rendering, or 0 for unlimited.
    //float MaxDistance() const { return drawable->MaxDistance(); }
    /// Return last frame number when was visible. The frames are counted by Renderer internally and have no significance outside it.
    //unsigned short LastFrameNumber() const { return drawable->LastFrameNumber(); }
    /// Return last frame number when was reinserted to octree (moved or animated.) The frames are counted by Renderer internally and have no significance outside it.
    //unsigned short LastUpdateFrameNumber() const { return drawable->LastUpdateFrameNumber(); }
    /// Check whether is marked in view this frame.
    //bool InView(unsigned short frameNumber) { return drawable->InView(frameNumber); }
    /// Check whether was in view last frame, compared to the current.
    //bool WasInView(unsigned short frameNumber) const { return drawable->WasInView(frameNumber); }

protected:
    /// Search for an octree from the scene root and add self to it.
	//void OnSceneSet(Scene* newScene, Scene* oldScene) override;
    /// Handle the transform matrix changing. Queue octree reinsertion for the drawable.
    void OnTransformChanged() override;
    /// Handle the bounding box changing. Only queue octree reinsertion, does not dirty the node hierarchy.
    void OnBoundingBoxChanged();
    /// Handle the enabled status changing.
	//void OnEnabledChanged(bool newEnabled) override;
    /// Remove from the current octree.
    void RemoveFromOctree();
};
