// For conditions of distribution and use, see copyright notice in License.txt

#pragma once

#include "../Math/FrustumTu.h"
#include "../Thread/WorkQueueTu.h"
#include "OctreeNodeTu.h"

#include <atomic>
#include <engine/scene/ShapeNode.h>

static const size_t NUM_OCTANTSTU = 8;
static const unsigned char OF_DRAWABLES_SORT_DIRTYTU = 0x1;
static const unsigned char OF_CULLING_BOX_DIRTYTU = 0x2;
static const float OCCLUSION_QUERY_INTERVALTU = 0.133333f; // About 8 frame stagger at 60fps

class Ray;
class WorkQueueTu;
struct ReinsertDrawablesTaskTu;
struct ReinsertDrawablesTaskTu2;

/// %Octant occlusion query visibility states.
enum OctantVisibilityTu
{
    VIS_OUTSIDE_FRUSTUMTU = 0,
    VIS_OCCLUDEDTU,
    VIS_OCCLUDED_UNKNOWNTU,
    VIS_VISIBLE_UNKNOWNTU,
    VIS_VISIBLETU
};
/// %Octree cell, contains up to 8 child octants.
class OctantTu
{
    friend class OctreeTu;

public:
    /// Construct with defaults.
    OctantTu();
    /// Destruct. If has a pending occlusion query, free it.
    ~OctantTu();

    /// Initialize parent and bounds.
    void Initialize(OctantTu* parent, const BoundingBox& boundingBox, unsigned char level, unsigned char childIndex);
    /// Add debug geometry to be rendered.
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });
    /// React to occlusion query being rendered for the octant. Store the query ID to know not to re-test until have the result.
    void OnOcclusionQuery(unsigned queryId);
    /// React to occlusion query result. Push changed visibility to parents or children as necessary. If outside frustum, no operation.
    void OnOcclusionQueryResult(bool visible);

    /// Return the culling box. Update as necessary.
    const BoundingBox& CullingBox() const;
    /// Return drawables in this octant.
	const std::vector<ShapeNode*>& Drawables() const { return drawables; }
    /// Return whether has child octants.
    bool HasChildren() const { return numChildren > 0; }
    /// Return child octant by index.
    OctantTu* Child(size_t index) const { return children[index]; }
    /// Return parent octant.
    OctantTu* Parent() const { return parent; }
    /// Return child octant index based on position.
    unsigned char ChildIndex(const Vector3& position) const { unsigned char ret = position.x < center[0] ? 0 : 1; ret += position.y < center[1] ? 0 : 2; ret += position.z < center[2] ? 0 : 4; return ret; }
    /// Return last occlusion visibility status.
    OctantVisibilityTu Visibility() const { return (OctantVisibilityTu)visibility; }
    /// Return whether is pending an occlusion query result.
    bool OcclusionQueryPending() const { return occlusionQueryId != 0; }
    /// Set bit flag. Called internally.
    void SetFlag(unsigned char bit, bool set) const { if (set) flags |= bit; else flags &= ~bit; }
    /// Test bit flag. Called internally.
    bool TestFlag(unsigned char bit) const { return (flags & bit) != 0; }

    /// Test if a drawable should be inserted in this octant or if a smaller child octant should be created.
    bool FitBoundingBox(const BoundingBoxTu& box, const Vector3& boxSize) const
    {
        // If max split level, size always OK, otherwise check that box is at least half size of octant
        if (level <= 1 || boxSize.x >= halfSize[0] || boxSize.y >= halfSize[1] || boxSize.z >= halfSize[2])
            return true;
        // Also check if the box can not fit inside a child octant's culling box, in that case size OK (must insert here)
        else
        {
            Vector3f quarterSize = 0.5f * halfSize;
            if (box.min.x <= fittingBox.min[0] + quarterSize[0] || box.max.x >= fittingBox.max[0] - quarterSize[0] ||
                box.min.y <= fittingBox.min[1] + quarterSize[1] || box.max.y >= fittingBox.max[1] - quarterSize[1] ||
                box.max.z <= fittingBox.min[2] + quarterSize[2] || box.max.z >= fittingBox.max[2] - quarterSize[2])
                return true;
        }

        // Bounding box too small, should create a child octant
        return false;
    }

	bool FitBoundingBox(const BoundingBox& box, const Vector3f& boxSize) const
	{
		// If max split level, size always OK, otherwise check that box is at least half size of octant
		if (level <= 1 || boxSize[0] >= halfSize[0] || boxSize[1] >= halfSize[1] || boxSize[2] >= halfSize[2])
			return true;
		// Also check if the box can not fit inside a child octant's culling box, in that case size OK (must insert here)
		else
		{
			Vector3f quarterSize = 0.5f * halfSize;
			if (box.min[0] <= fittingBox.min[0] + quarterSize[0] || box.max[0] >= fittingBox.max[0] - quarterSize[0] ||
				box.min[1] <= fittingBox.min[1] + quarterSize[1] || box.max[1] >= fittingBox.max[1] - quarterSize[1] ||
				box.max[2] <= fittingBox.min[2] + quarterSize[2] || box.max[2] >= fittingBox.max[2] - quarterSize[2])
				return true;
		}

		// Bounding box too small, should create a child octant
		return false;
	}

    /// Mark culling boxes dirty in the parent hierarchy.
    void MarkCullingBoxDirty() const
    {
        const OctantTu* octant = this;

        while (octant && !octant->TestFlag(OF_CULLING_BOX_DIRTYTU))
        {
            octant->SetFlag(OF_CULLING_BOX_DIRTYTU, true);
            octant = octant->parent;
        }
    }

    /// Push visibility status to child octants.
    void PushVisibilityToChildren(OctantTu* octant, OctantVisibilityTu newVisibility)
    {
        for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
        {
            if (octant->children[i])
            {
                octant->children[i]->visibility = newVisibility;
                if (octant->children[i]->numChildren)
                    PushVisibilityToChildren(octant->children[i], newVisibility);
            }
        }
    }

    /// Set visibility status manually.
    void SetVisibility(OctantVisibilityTu newVisibility, bool pushToChildren = false)
    {
        visibility = newVisibility;

        if (pushToChildren)
            PushVisibilityToChildren(this, newVisibility);
    }

    /// Return true if a new occlusion query should be executed. Use a time interval for already visible octants. Return false if previous query still pending.
    bool CheckNewOcclusionQuery(float frameTime)
    {
        if (visibility != VIS_VISIBLETU)
            return occlusionQueryId == 0;

        occlusionQueryTimer += frameTime;

        if (occlusionQueryId != 0)
            return false;

        if (occlusionQueryTimer >= OCCLUSION_QUERY_INTERVALTU)
        {
            occlusionQueryTimer = fmodf(occlusionQueryTimer, OCCLUSION_QUERY_INTERVALTU);
            return true;
        }
        else
            return false;
    }

private:
    /// Combined drawable and child octant bounding box. Used for culling tests.
    mutable BoundingBox cullingBox;
    /// Drawables contained in the octant.
	std::vector<ShapeNode*> drawables;
    /// Expanded (loose) bounding box used for fitting drawables within the octant.
    BoundingBox fittingBox;
    /// Bounding box center.
    Vector3f center;
    /// Bounding box half size.
    Vector3f halfSize;
    /// Child octants.
    OctantTu* children[NUM_OCTANTSTU];
    /// Parent octant.
    OctantTu* parent;
    /// Last occlusion query visibility.
    OctantVisibilityTu visibility;
    /// Occlusion query id, or 0 if no query pending.
    unsigned occlusionQueryId;
    /// Occlusion query interval timer.
    float occlusionQueryTimer;
    /// Number of child octants.
    unsigned char numChildren;
    /// Subdivision level, decreasing for child octants.
    unsigned char level;
    /// The child index of this octant.
    unsigned char childIndex;
    /// Dirty flags.
    mutable unsigned char flags;
};

/// Acceleration structure for rendering. Should be created as a child of the scene root.
class OctreeTu{

public:
    /// Construct. The WorkQueue subsystem must have been initialized, as it will be used during update.
    OctreeTu();
    /// Destruct. Delete all child octants and detach the drawables.
    ~OctreeTu();
   
    /// Process the queue of nodes to be reinserted. This will utilize worker threads.
    void Update();
    /// Finish the octree update.
    void FinishUpdate();
    /// Resize the octree.
    void Resize(const BoundingBox& boundingBox, int numLevels);
    /// Enable or disable threaded update mode. In threaded mode reinsertions go to per-thread queues, which are processed in FinishUpdate().
    void SetThreadedUpdate(bool enable) { threadedUpdate = enable; }
    /// Queue octree reinsertion for a drawable.
	//void QueueUpdate(Drawable* drawable);
	void QueueUpdate(ShapeNode* drawable);
    /// Remove a drawable from the octree.
	void RemoveDrawable(ShapeNode* drawable);
    /// Add debug geometry to be rendered. Visualizes the whole octree.
	//void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

    /// Query for drawables using a volume such as frustum or sphere.
    template <class T> void FindDrawables(std::vector<ShapeNode*>& result, const T& volume, unsigned short drawableFlags, unsigned layerMask = LAYERMASK_ALL) const { CollectDrawables(result, const_cast<OctantTu*>(&root), volume, drawableFlags, layerMask); }
    /// Query for drawables using a frustum and masked testing.
	//void FindDrawablesMasked(std::vector<Drawable*>& result, const FrustumTu& frustum, unsigned short drawableFlags, unsigned layerMask = LAYERMASK_ALL) const;
    /// Return whether threaded update is enabled.
    bool ThreadedUpdate() const { return threadedUpdate; }
    /// Return the root octant.
    OctantTu* Root() const { return const_cast<OctantTu*>(&root); }

private:
    /// Set bounding box. Used in serialization.
    void SetBoundingBoxAttr(const BoundingBox& value);
    /// Return bounding box. Used in serialization.
    const BoundingBox& BoundingBoxAttr() const;
    /// Set number of levels. Used in serialization.
    void SetNumLevelsAttr(int numLevels);
    /// Return number of levels. Used in serialization.
    int NumLevelsAttr() const;
    /// Process a list of drawables to be reinserted. Clear the list afterward.
	void ReinsertDrawables(std::vector<ShapeNode*>& drawables);
    /// Remove a drawable from a reinsert queue.
    void RemoveDrawableFromQueue(ShapeNode* drawable, std::vector<ShapeNode*>& drawables);
    
    /// Add drawable to a specific octant.
	void AddDrawable(ShapeNode* drawable, OctantTu* octant)
	{
		octant->drawables.push_back(drawable);
		octant->MarkCullingBoxDirty();
		drawable->m_octantTu = octant;

		if (!octant->TestFlag(OF_DRAWABLES_SORT_DIRTYTU))
		{
			octant->SetFlag(OF_DRAWABLES_SORT_DIRTYTU, true);
			sortDirtyOctants.push_back(octant);
		}
	}

	/// Remove drawable from an octant.
	void RemoveDrawable(ShapeNode* drawable, OctantTu* octant)
	{
		if (!octant)
			return;

		octant->MarkCullingBoxDirty();

		// Do not set the drawable's octant pointer to zero, as the drawable may already be added into another octant. Just remove from octant
		for (auto it = octant->drawables.begin(); it != octant->drawables.end(); ++it)
		{
			if ((*it) == drawable)
			{
				octant->drawables.erase(it);

				// Erase empty octants as necessary, but never the root
				while (!octant->drawables.size() && !octant->numChildren && octant->parent)
				{
					OctantTu* parentOctant = octant->parent;
					DeleteChildOctant(parentOctant, octant->childIndex);
					octant = parentOctant;
				}
				return;
			}
		}
	}

	/// Create a new child octant.
	OctantTu* CreateChildOctant(OctantTu* octant, unsigned char index);
	/// Delete one child octant.
	void DeleteChildOctant(OctantTu* octant, unsigned char index);
	/// Delete a child octant hierarchy. If not deleting the octree for good, moves any nodes back to the root octant.
	void DeleteChildOctants(OctantTu* octant, bool deletingOctree);
	/// Return all drawables from an octant recursively.
	void CollectDrawables(std::vector<ShapeNode*>& result, OctantTu* octant) const;
	/// Return all drawables matching flags from an octant recursively.
	void CollectDrawables(std::vector<ShapeNode*>& result, OctantTu* octant, unsigned short drawableFlags, unsigned layerMask) const;
	/// Work function to check reinsertion of nodes.
	void CheckReinsertWork2(TaskTu* task, unsigned threadIndex);

	/// Collect nodes matching flags using a volume such as frustum or sphere.
	template <class T> void CollectDrawables(std::vector<ShapeNode*>& result, Octant* octant, const T& volume, unsigned short drawableFlags, unsigned layerMask) const
	{
		Intersection res = volume.IsInside(octant->CullingBox());
		if (res == OUTSIDE)
			return;

		// If this octant is completely inside the volume, can include all contained octants and their nodes without further tests
		if (res == INSIDE)
			CollectDrawables(result, octant, drawableFlags, layerMask);
		else
		{
			std::vector<ShapeNode*>& drawables = octant->drawables;

			for (auto it = drawables.begin(); it != drawables.end(); ++it)
			{
				ShapeNode* drawable = *it;
				if ((drawable->Flags() & drawableFlags) == drawableFlags && (drawable->LayerMask() & layerMask) && volume.IsInsideFast(drawable->WorldBoundingBox()) != OUTSIDE)
					result.push_back(drawable);
			}

			if (octant->numChildren)
			{
				for (size_t i = 0; i < NUM_OCTANTS; ++i)
				{
					if (octant->children[i])
						CollectDrawables(result, octant->children[i], volume, drawableFlags, layerMask);
				}
			}
		}
	}

    /// Threaded update flag. During threaded update moved drawables should go directly to thread-specific reinsert queues.
    volatile bool threadedUpdate;
    /// Queue of nodes to be reinserted.
	std::vector<ShapeNode*> _updateQueue;
    /// Octants which need to have their drawables sorted.
    std::vector<OctantTu*> sortDirtyOctants;
    /// Extents of the octree root level box.
    BoundingBox worldBoundingBox;
    /// Root octant.
    OctantTu root;
    /// Allocator for child octants.
    Allocator<OctantTu> allocator;
    /// Cached %WorkQueue subsystem.
    WorkQueueTu* workQueue;
    /// Tasks for threaded reinsert execution.
	//std::vector<AutoPtr<ReinsertDrawablesTaskTu>> reinsertTasks;
	std::vector<AutoPtr<ReinsertDrawablesTaskTu2>> _reinsertTasks;
    /// Intermediate reinsert queues for threaded execution.
	//AutoArrayPtr<std::vector<Drawable*> > reinsertQueues;
	AutoArrayPtr<std::vector<ShapeNode*> > _reinsertQueues;
    /// RaycastSingle initial coarse result.
    mutable std::vector<std::pair<Drawable*, float> > initialRayResult;
    /// Remaining drawable reinsertion tasks.
    std::atomic<int> numPendingReinsertionTasks;
};
