// For conditions of distribution and use, see copyright notice in License.txt

#include "../Graphics/Graphics.h"
#include "../IO/Log.h"
#include "../Math/RandomTu.h"
#include "../Math/Ray.h"
#include "DebugRendererTu.h"
#include "OctreeTu.h"

#include <cassert>
#include <algorithm>
#include <tracy/Tracy.hpp>

static const float DEFAULT_OCTREE_SIZE = 1000.0f;
static const int DEFAULT_OCTREE_LEVELS = 8;
static const int MAX_OCTREE_LEVELS = 255;
static const size_t MIN_THREADED_UPDATE = 16;

static std::vector<unsigned> freeQueries;

static inline bool CompareRaycastResults(const RaycastResult& lhs, const RaycastResult& rhs)
{
    return lhs.distance < rhs.distance;
}

static inline bool CompareDrawableDistances(const std::pair<Drawable*, float>& lhs, const std::pair<Drawable*, float>& rhs)
{
    return lhs.second < rhs.second;
}

static inline bool CompareDrawables(Drawable* lhs, Drawable* rhs)
{
    unsigned short lhsFlags = lhs->Flags() & (DF_LIGHTTU | DF_GEOMETRYTU);
    unsigned short rhsFlags = rhs->Flags() & (DF_LIGHTTU | DF_GEOMETRYTU);
    if (lhsFlags != rhsFlags)
        return lhsFlags < rhsFlags;
    else
        return lhs < rhs;
}

/// %Task for octree drawables reinsertion.
struct ReinsertDrawablesTaskTu : public MemberFunctionTaskTu<OctreeTu>
{
    /// Construct.
    ReinsertDrawablesTaskTu(OctreeTu* object_, MemberWorkFunctionPtr function_) :
        MemberFunctionTaskTu<OctreeTu>(object_, function_)
    {
    }

    /// Start pointer.
    Drawable** start;
    /// End pointer.
    Drawable** end;
};

OctantTu::OctantTu() :
    parent(nullptr),
    visibility(VIS_VISIBLE_UNKNOWNTU),
    occlusionQueryId(0),
    occlusionQueryTimer(Random() * OCCLUSION_QUERY_INTERVALTU),
    numChildren(0)
{
    for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
        children[i] = nullptr;
}

OctantTu::~OctantTu()
{
    if (occlusionQueryId)
    {
        Graphics* graphics = ObjectTu::Subsystem<Graphics>();
        if (graphics)
            graphics->FreeOcclusionQuery(occlusionQueryId);
    }
}

void OctantTu::Initialize(OctantTu* parent_, const BoundingBoxTu& boundingBox, unsigned char level_, unsigned char childIndex_)
{
    BoundingBoxTu worldBoundingBox = boundingBox;
    center = worldBoundingBox.Center();
    halfSize = worldBoundingBox.HalfSize();
    fittingBox = BoundingBoxTu(worldBoundingBox.min - halfSize, worldBoundingBox.max + halfSize);

    parent = parent_;
    level = level_;
    childIndex = childIndex_;
    flags = OF_CULLING_BOX_DIRTYTU;
}

void OctantTu::OnRenderDebug(DebugRendererTu* debug)
{
    debug->AddBoundingBox(CullingBox(), Color::GRAY, true);
}

void OctantTu::OnOcclusionQuery(unsigned queryId)
{
    // Should not have an existing query in flight
    assert(!occlusionQueryId);

    // Mark pending
    occlusionQueryId = queryId;
}

void OctantTu::OnOcclusionQueryResult(bool visible)
{
    // Mark not pending
    occlusionQueryId = 0;

    // Do not change visibility if currently outside the frustum
    if (visibility == VIS_OUTSIDE_FRUSTUMTU)
        return;

    OctantVisibilityTu lastVisibility = (OctantVisibilityTu)visibility;
    OctantVisibilityTu newVisibility = visible ? VIS_VISIBLETU : VIS_OCCLUDEDTU;

    visibility = newVisibility;

    if (lastVisibility <= VIS_OCCLUDED_UNKNOWNTU && newVisibility == VIS_VISIBLETU)
    {
        // If came into view after being occluded, mark children as still occluded but that should be tested in hierarchy
        if (numChildren)
            PushVisibilityToChildren(this, VIS_OCCLUDED_UNKNOWNTU);
    }
    else if (newVisibility == VIS_OCCLUDEDTU && lastVisibility != VIS_OCCLUDEDTU && parent && parent->visibility == VIS_VISIBLETU)
    {
        // If became occluded, mark parent unknown so it will be tested next
        parent->visibility = VIS_VISIBLE_UNKNOWNTU;
    }

    // Whenever is visible, push visibility to parents if they are not visible yet
    if (newVisibility == VIS_VISIBLETU)
    {
        OctantTu* octant = parent;

        while (octant && octant->visibility != newVisibility)
        {
            octant->visibility = newVisibility;
            octant = octant->parent;
        }
    }
}

const BoundingBoxTu& OctantTu::CullingBox() const
{
    if (TestFlag(OF_CULLING_BOX_DIRTYTU))
    {
        if (!numChildren && drawables.empty())
            cullingBox.Define(center);
        else
        {
            // Use a temporary bounding box for calculations in case many threads call this simultaneously
            BoundingBoxTu tempBox;

            for (auto it = drawables.begin(); it != drawables.end(); ++it)
                tempBox.Merge((*it)->WorldBoundingBox());

            if (numChildren)
            {
                for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
                {
                    if (children[i])
                        tempBox.Merge(children[i]->CullingBox());
                }
            }

            cullingBox = tempBox;
        }

        SetFlag(OF_CULLING_BOX_DIRTYTU, false);
    }

    return cullingBox;
}

OctreeTu::OctreeTu() :
    threadedUpdate(false),
    frameNumber(0),
    workQueue(Subsystem<WorkQueueTu>())
{
    assert(workQueue);

    root.Initialize(nullptr, BoundingBoxTu(-DEFAULT_OCTREE_SIZE, DEFAULT_OCTREE_SIZE), DEFAULT_OCTREE_LEVELS, 0);

    // Have at least 1 task for reinsert processing
    reinsertTasks.push_back(new ReinsertDrawablesTaskTu(this, &OctreeTu::CheckReinsertWork));
    reinsertQueues = new std::vector<Drawable*>[workQueue->NumThreads()];
}

OctreeTu::~OctreeTu()
{
    // Clear octree association from nodes that were never inserted
    // Note: the threaded queues cannot have nodes that were never inserted, only nodes that should be moved
    for (auto it = updateQueue.begin(); it != updateQueue.end(); ++it)
    {
        Drawable* drawable = *it;
        if (drawable)
        {
            drawable->octant = nullptr;
            drawable->SetFlag(DF_OCTREE_REINSERT_QUEUEDTU, false);
        }
    }

    DeleteChildOctants(&root, true);
}

void OctreeTu::RegisterObject()
{
    // Register octree allocator with small initial capacity with the assumption that we don't create many of them (unlike other scene nodes)
    RegisterFactory<OctreeTu>(1);
    CopyBaseAttributes<OctreeTu, NodeTu>();
    RegisterDerivedType<OctreeTu, NodeTu>();
    RegisterRefAttribute("boundingBox", &OctreeTu::BoundingBoxAttr, &OctreeTu::SetBoundingBoxAttr);
    RegisterAttribute("numLevels", &OctreeTu::NumLevelsAttr, &OctreeTu::SetNumLevelsAttr);
}

void OctreeTu::Update(unsigned short frameNumber_)
{
    ZoneScoped;

    frameNumber = frameNumber_;

    // Avoid overhead of threaded update if only a small number of objects to update / reinsert
    if (updateQueue.size())
    {
        SetThreadedUpdate(true);

        // Split into smaller tasks to encourage work stealing in case some thread is slower
        size_t nodesPerTask = Max(MIN_THREADED_UPDATE, updateQueue.size() / workQueue->NumThreads() / 4);
        size_t taskIdx = 0;

        for (size_t start = 0; start < updateQueue.size(); start += nodesPerTask)
        {
            size_t end = start + nodesPerTask;
            if (end > updateQueue.size())
                end = updateQueue.size();

            if (reinsertTasks.size() <= taskIdx)
                reinsertTasks.push_back(new ReinsertDrawablesTaskTu(this, &OctreeTu::CheckReinsertWork));
            reinsertTasks[taskIdx]->start = &updateQueue[0] + start;
            reinsertTasks[taskIdx]->end = &updateQueue[0] + end;
            ++taskIdx;
        }

        numPendingReinsertionTasks.store((int)taskIdx);
        workQueue->QueueTasks(taskIdx, reinterpret_cast<TaskTu**>(&reinsertTasks[0]));
    }
    else
        numPendingReinsertionTasks.store(0);
}

void OctreeTu::FinishUpdate()
{
    ZoneScoped;

    // Complete tasks until reinsertions done. There may other tasks going on at the same time
    while (numPendingReinsertionTasks.load() > 0)
        workQueue->TryComplete();

    SetThreadedUpdate(false);

    // Now reinsert drawables that actually need reinsertion into a different octant
    for (size_t i = 0; i < workQueue->NumThreads(); ++i)
        ReinsertDrawables(reinsertQueues[i]);

    updateQueue.clear();

    // Sort octants' drawables by address and put lights first
    for (auto it = sortDirtyOctants.begin(); it != sortDirtyOctants.end(); ++it)
    {
        OctantTu* octant = *it;
        std::sort(octant->drawables.begin(), octant->drawables.end(), CompareDrawables);
        octant->SetFlag(OF_DRAWABLES_SORT_DIRTYTU, false);
    }

    sortDirtyOctants.clear();
}

void OctreeTu::Resize(const BoundingBoxTu& boundingBox, int numLevels)
{
    ZoneScoped;

    // Collect nodes to the root and delete all child octants
    updateQueue.clear();
    std::vector<Drawable*> occluders;
    
    CollectDrawables(updateQueue, &root);
    DeleteChildOctants(&root, false);

    allocator.Reset();
    root.Initialize(nullptr, boundingBox, (unsigned char)Clamp(numLevels, 1, MAX_OCTREE_LEVELS), 0);
}

void OctreeTu::OnRenderDebug(DebugRendererTu* debug)
{
    root.OnRenderDebug(debug);
}

void OctreeTu::Raycast(std::vector<RaycastResult>& result, const Ray& ray, unsigned short nodeFlags, float maxDistance, unsigned layerMask) const
{
    ZoneScoped;

    result.clear();
    CollectDrawables(result, const_cast<OctantTu*>(&root), ray, nodeFlags, maxDistance, layerMask);
    std::sort(result.begin(), result.end(), CompareRaycastResults);
}

RaycastResult OctreeTu::RaycastSingle(const Ray& ray, unsigned short nodeFlags, float maxDistance, unsigned layerMask) const
{
    ZoneScoped;

    // Get the potential hits first
    initialRayResult.clear();
    CollectDrawables(initialRayResult, const_cast<OctantTu*>(&root), ray, nodeFlags, maxDistance, layerMask);
    std::sort(initialRayResult.begin(), initialRayResult.end(), CompareDrawableDistances);

    // Then perform actual per-node ray tests and early-out when possible
    finalRayResult.clear();
    float closestHit = M_INFINITY;
    for (auto it = initialRayResult.begin(); it != initialRayResult.end(); ++it)
    {
        if (it->second < Min(closestHit, maxDistance))
        {
            size_t oldSize = finalRayResult.size();
            it->first->OnRaycast(finalRayResult, ray, maxDistance);
            if (finalRayResult.size() > oldSize)
                closestHit = Min(closestHit, finalRayResult.back().distance);
        }
        else
            break;
    }

    if (finalRayResult.size())
    {
        std::sort(finalRayResult.begin(), finalRayResult.end(), CompareRaycastResults);
        return finalRayResult.front();
    }
    else
    {
        RaycastResult emptyRes;
        emptyRes.position = emptyRes.normal = Vector3::ZERO;
        emptyRes.distance = M_INFINITY;
        emptyRes.drawable = nullptr;
        emptyRes.subObject = 0;
        return emptyRes;
    }
}

void OctreeTu::FindDrawablesMasked(std::vector<Drawable*>& result, const FrustumTu& frustum, unsigned short drawableFlags, unsigned layerMask) const
{
    ZoneScoped;

    CollectDrawablesMasked(result, const_cast<OctantTu*>(&root), frustum, drawableFlags, layerMask);
}

void OctreeTu::QueueUpdate(Drawable* drawable)
{
    assert(drawable);

    if (drawable->octant)
        drawable->octant->MarkCullingBoxDirty();

    if (!threadedUpdate)
    {
        updateQueue.push_back(drawable);
        drawable->SetFlag(DF_OCTREE_REINSERT_QUEUEDTU, true);
    }
    else
    {
        drawable->lastUpdateFrameNumber = frameNumber;

        // Do nothing if still fits the current octant
        const BoundingBoxTu& box = drawable->WorldBoundingBox();
        OctantTu* oldOctant = drawable->GetOctant();
        if (!oldOctant || oldOctant->fittingBox.IsInside(box) != INSIDE)
        {
            reinsertQueues[WorkQueueTu::ThreadIndex()].push_back(drawable);
            drawable->SetFlag(DF_OCTREE_REINSERT_QUEUEDTU, true);
        }
    }
}

void OctreeTu::RemoveDrawable(Drawable* drawable)
{
    if (!drawable)
        return;

    RemoveDrawable(drawable, drawable->GetOctant());
    if (drawable->TestFlag(DF_OCTREE_REINSERT_QUEUEDTU))
    {
        RemoveDrawableFromQueue(drawable, updateQueue);

        // Remove also from threaded queues if was left over before next update
        for (size_t i = 0; i < workQueue->NumThreads(); ++i)
            RemoveDrawableFromQueue(drawable, reinsertQueues[i]);

        drawable->SetFlag(DF_OCTREE_REINSERT_QUEUEDTU, false);
    }

    drawable->octant = nullptr;
}

void OctreeTu::SetBoundingBoxAttr(const BoundingBoxTu& value)
{
    worldBoundingBox = value;
}

const BoundingBoxTu& OctreeTu::BoundingBoxAttr() const
{
    return worldBoundingBox;
}

void OctreeTu::SetNumLevelsAttr(int numLevels)
{
    /// Setting the number of level (last attribute) triggers octree resize when deserializing
    Resize(worldBoundingBox, numLevels);
}

int OctreeTu::NumLevelsAttr() const
{
    return root.level;
}

void OctreeTu::ReinsertDrawables(std::vector<Drawable*>& drawables)
{
    for (auto it = drawables.begin(); it != drawables.end(); ++it)
    {
        Drawable* drawable = *it;

        const BoundingBoxTu& box = drawable->WorldBoundingBox();
        OctantTu* oldOctant = drawable->GetOctant();
        OctantTu* newOctant = &root;
        Vector3 boxSize = box.Size();

        for (;;)
        {
            // If drawable does not fit fully inside root octant, must remain in it
            bool insertHere = (newOctant == &root) ?
                (newOctant->fittingBox.IsInside(box) != INSIDE || newOctant->FitBoundingBox(box, boxSize)) :
                newOctant->FitBoundingBox(box, boxSize);

            if (insertHere)
            {
                if (newOctant != oldOctant)
                {
                    // Add first, then remove, because drawable count going to zero deletes the octree branch in question
                    AddDrawable(drawable, newOctant);
                    if (oldOctant)
                        RemoveDrawable(drawable, oldOctant);
                }
                break;
            }
            else
                newOctant = CreateChildOctant(newOctant, newOctant->ChildIndex(box.Center()));
        }

        drawable->SetFlag(DF_OCTREE_REINSERT_QUEUEDTU, false);
    }

    drawables.clear();
}

void OctreeTu::RemoveDrawableFromQueue(Drawable* drawable, std::vector<Drawable*>& drawables)
{
    for (auto it = drawables.begin(); it != drawables.end(); ++it)
    {
        if ((*it) == drawable)
        {
            *it = nullptr;
            break;
        }
    }
}

OctantTu* OctreeTu::CreateChildOctant(OctantTu* octant, unsigned char index)
{
    if (octant->children[index])
        return octant->children[index];

    // Remove the culling extra from the bounding box before splitting
    Vector3 newMin = octant->fittingBox.min + octant->halfSize;
    Vector3 newMax = octant->fittingBox.max - octant->halfSize;
    const Vector3& oldCenter = octant->center;

    if (index & 1)
        newMin.x = oldCenter.x;
    else
        newMax.x = oldCenter.x;

    if (index & 2)
        newMin.y = oldCenter.y;
    else
        newMax.y = oldCenter.y;

    if (index & 4)
        newMin.z = oldCenter.z;
    else
        newMax.z = oldCenter.z;

    OctantTu* child = allocator.Allocate();
    child->Initialize(octant, BoundingBoxTu(newMin, newMax), octant->level - 1, index);
    octant->children[index] = child;
    ++octant->numChildren;

    return child;
}

void OctreeTu::DeleteChildOctant(OctantTu* octant, unsigned char index)
{
    allocator.Free(octant->children[index]);
    octant->children[index] = nullptr;
    --octant->numChildren;
}

void OctreeTu::DeleteChildOctants(OctantTu* octant, bool deletingOctree)
{
    for (auto it = octant->drawables.begin(); it != octant->drawables.end(); ++it)
    {
        Drawable* drawable = *it;
        drawable->octant = nullptr;
        drawable->SetFlag(DF_OCTREE_REINSERT_QUEUEDTU, false);
        if (deletingOctree)
            drawable->Owner()->octree = nullptr;
    }
    octant->drawables.clear();

    if (octant->numChildren)
    {
        for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
        {
            if (octant->children[i])
            {
                DeleteChildOctants(octant->children[i], deletingOctree);
                allocator.Free(octant->children[i]);
                octant->children[i] = nullptr;
            }
        }
        octant->numChildren = 0;
    }
}

void OctreeTu::CollectDrawables(std::vector<Drawable*>& result, OctantTu* octant) const
{
    result.insert(result.end(), octant->drawables.begin(), octant->drawables.end());

    if (octant->numChildren)
    {
        for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
        {
            if (octant->children[i])
                CollectDrawables(result, octant->children[i]);
        }
    }
}

void OctreeTu::CollectDrawables(std::vector<Drawable*>& result, OctantTu* octant, unsigned short drawableFlags, unsigned layerMask) const
{
    std::vector<Drawable*>& drawables = octant->drawables;

    for (auto it = drawables.begin(); it != drawables.end(); ++it)
    {
        Drawable* drawable = *it;
        if ((drawable->Flags() & drawableFlags) == drawableFlags && (drawable->LayerMask() & layerMask))
            result.push_back(drawable);
    }

    if (octant->numChildren)
    {
        for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
        {
            if (octant->children[i])
                CollectDrawables(result, octant->children[i], drawableFlags, layerMask);
        }
    }
}

void OctreeTu::CollectDrawables(std::vector<RaycastResult>& result, OctantTu* octant, const Ray& ray, unsigned short drawableFlags, float maxDistance, unsigned layerMask) const
{
    float octantDist = ray.HitDistance(octant->CullingBox());
    if (octantDist >= maxDistance)
        return;

    std::vector<Drawable*>& drawables = octant->drawables;

    for (auto it = drawables.begin(); it != drawables.end(); ++it)
    {
        Drawable* drawable = *it;
        if ((drawable->Flags() & drawableFlags) == drawableFlags && (drawable->LayerMask() & layerMask))
            drawable->OnRaycast(result, ray, maxDistance);
    }

    if (octant->numChildren)
    {
        for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
        {
            if (octant->children[i])
                CollectDrawables(result, octant->children[i], ray, drawableFlags, maxDistance, layerMask);
        }
    }
}

void OctreeTu::CollectDrawables(std::vector<std::pair<Drawable*, float> >& result, OctantTu* octant, const Ray& ray, unsigned short drawableFlags, float maxDistance, unsigned layerMask) const
{
    float octantDist = ray.HitDistance(octant->CullingBox());
    if (octantDist >= maxDistance)
        return;

    std::vector<Drawable*>& drawables = octant->drawables;

    for (auto it = drawables.begin(); it != drawables.end(); ++it)
    {
        Drawable* drawable = *it;
        if ((drawable->Flags() & drawableFlags) == drawableFlags && (drawable->LayerMask() & layerMask))
        {
            float distance = ray.HitDistance(drawable->WorldBoundingBox());
            if (distance < maxDistance)
                result.push_back(std::make_pair(drawable, distance));
        }
    }

    if (octant->numChildren)
    {
        for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
        {
            if (octant->children[i])
                CollectDrawables(result, octant->children[i], ray, drawableFlags, maxDistance, layerMask);
        }
    }
}

void OctreeTu::CheckReinsertWork(TaskTu* task_, unsigned threadIndex_)
{
    ZoneScoped;

    ReinsertDrawablesTaskTu* task = static_cast<ReinsertDrawablesTaskTu*>(task_);
    Drawable** start = task->start;
    Drawable** end = task->end;
    std::vector<Drawable*>& reinsertQueue = reinsertQueues[threadIndex_];

    for (; start != end; ++start)
    {
        // If drawable was removed before reinsertion could happen, a null pointer will be in its place
        Drawable* drawable = *start;
        if (!drawable)
            continue;

        if (drawable->TestFlag(DF_OCTREE_UPDATE_CALLTU))
            drawable->OnOctreeUpdate(frameNumber);

        drawable->lastUpdateFrameNumber = frameNumber;

        // Do nothing if still fits the current octant
        const BoundingBoxTu& box = drawable->WorldBoundingBox();
        OctantTu* oldOctant = drawable->GetOctant();
        if (!oldOctant || oldOctant->fittingBox.IsInside(box) != INSIDE)
            reinsertQueue.push_back(drawable);
        else
            drawable->SetFlag(DF_OCTREE_REINSERT_QUEUEDTU, false);
    }

    numPendingReinsertionTasks.fetch_add(-1);
}
