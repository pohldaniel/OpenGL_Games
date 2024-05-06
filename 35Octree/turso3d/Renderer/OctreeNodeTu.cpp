// For conditions of distribution and use, see copyright notice in License.txt

#include "../Math/Ray.h"
#include "../Scene/Scene.h"
#include "CameraTu.h"
#include "DebugRendererTu.h"
#include "OctreeTu.h"

OctreeNodeBaseTu::OctreeNodeBaseTu() :
    octree(nullptr),
    drawable(nullptr)
{
}

void OctreeNodeBaseTu::OnLayerChanged(unsigned char newLayer)
{
    if (drawable)
        drawable->SetLayer(newLayer);
}

Drawable::Drawable() :
    owner(nullptr),
    octant(nullptr),
    flags(0),
    layer(LAYER_DEFAULT),
    lastFrameNumber(0),
    lastUpdateFrameNumber(0),
    distance(0.0f),
    maxDistance(0.0f)
{
    SetFlag(DF_BOUNDING_BOX_DIRTYTU, true);
}

Drawable::~Drawable()
{
}

void Drawable::OnWorldBoundingBoxUpdate() const
{
    // The Drawable base class does not have a defined size, so represent as a point
    worldBoundingBox.Define(WorldPosition());
}

void Drawable::OnOctreeUpdate(unsigned short)
{
}

bool Drawable::OnPrepareRender(unsigned short frameNumber, CameraTu* camera)
{
    distance = camera->Distance(WorldBoundingBox().Center());

    if (maxDistance > 0.0f && distance > maxDistance)
        return false;

    lastFrameNumber = frameNumber;
    return true;
}

void Drawable::OnRaycast(std::vector<RaycastResult>& dest, const Ray& ray, float maxDistance_)
{
    float hitDistance = ray.HitDistance(WorldBoundingBox());
    if (hitDistance < maxDistance_)
    {
        RaycastResult res;
        res.position = ray.origin + hitDistance * ray.direction;
        res.normal = -ray.direction;
        res.distance = hitDistance;
        res.drawable = this;
        res.subObject = 0;
        dest.push_back(res);
    }
}

void Drawable::OnRenderDebug(DebugRendererTu* debug)
{
    debug->AddBoundingBox(WorldBoundingBox(), Color::GREEN, false);
}

void Drawable::SetOwner(OctreeNodeBaseTu* owner_)
{
    owner = owner_;
    worldTransform = const_cast<Matrix3x4*>(&owner_->WorldTransform());
}

void Drawable::SetLayer(unsigned char newLayer)
{
    layer = newLayer;
}

void OctreeNodeTu::RegisterObject()
{
    CopyBaseAttributes<OctreeNodeTu, SpatialNode>();
    RegisterDerivedType<OctreeNodeTu, SpatialNode>();
    RegisterAttribute("static", &OctreeNodeTu::IsStatic, &OctreeNodeTu::SetStatic, false);
    RegisterAttribute("castShadows", &OctreeNodeTu::CastShadows, &OctreeNodeTu::SetCastShadows, false);
    RegisterAttribute("updateInvisible", &OctreeNodeTu::UpdateInvisible, &OctreeNodeTu::SetUpdateInvisible, false);
    RegisterAttribute("maxDistance", &OctreeNodeTu::MaxDistance, &OctreeNodeTu::SetMaxDistance, 0.0f);
}

void OctreeNodeTu::SetStatic(bool enable)
{
    if (enable != IsStatic())
    {
        drawable->SetFlag(DF_STATICTU, enable);
        // Reinsert into octree so that cached shadow map invalidation is handled
        OnBoundingBoxChanged();
    }
}

void OctreeNodeTu::SetCastShadows(bool enable)
{
    if (drawable->TestFlag(DF_CAST_SHADOWSTU) != enable)
    {
        drawable->SetFlag(DF_CAST_SHADOWSTU, enable);
        // Reinsert into octree so that cached shadow map invalidation is handled
        OnBoundingBoxChanged();
    }
}

void OctreeNodeTu::SetUpdateInvisible(bool enable)
{
    drawable->SetFlag(DF_UPDATE_INVISIBLETU, enable);
}

void OctreeNodeTu::SetMaxDistance(float distance_)
{
    drawable->maxDistance = Max(distance_, 0.0f);
}

void OctreeNodeTu::OnSceneSet(Scene* newScene, Scene*)
{
    /// Remove from current octree if any
    RemoveFromOctree();

    if (newScene)
    {
        // Octree must be attached to the scene root as a child
        octree = newScene->FindChild<OctreeTu>();
        // Transform may not be final yet. Schedule insertion for next octree update
		//if (octree && IsEnabled())
            //octree->QueueUpdate(drawable);
    }
}

void OctreeNodeTu::OnTransformChanged()
{
    SpatialNode::OnTransformChanged();

    drawable->SetFlag(DF_WORLD_TRANSFORM_DIRTYTU | DF_BOUNDING_BOX_DIRTYTU, true);
	//if (drawable->GetOctant() && !drawable->TestFlag(DF_OCTREE_REINSERT_QUEUEDTU))
		//octree->QueueUpdate(drawable);
}

void OctreeNodeTu::OnBoundingBoxChanged()
{
    drawable->SetFlag(DF_BOUNDING_BOX_DIRTYTU, true);
	//if (drawable->GetOctant() && !drawable->TestFlag(DF_OCTREE_REINSERT_QUEUEDTU))
		//octree->QueueUpdate(drawable);
}

void OctreeNodeTu::RemoveFromOctree()
{
    if (octree)
    {
		//octree->RemoveDrawable(drawable);
        octree = nullptr;
    }
}

void OctreeNodeTu::OnEnabledChanged(bool newEnabled)
{
    if (octree)
    {
		//if (newEnabled)
			//octree->QueueUpdate(drawable);
		//else
			//octree->RemoveDrawable(drawable);
    }
}
