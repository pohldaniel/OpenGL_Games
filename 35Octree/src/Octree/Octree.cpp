// For conditions of distribution and use, see copyright notice in License.txt
#include <iostream>
#include <cassert>
#include <algorithm>

#include <engine/DebugRenderer.h>
#include "Octree.h"

static const float DEFAULT_OCTREE_SIZE = 1000.0f;
static const int DEFAULT_OCTREE_LEVELS = 8;
static const int MAX_OCTREE_LEVELS = 255;
static const size_t MIN_THREADED_UPDATE = 16;

static std::vector<unsigned> freeQueries;

static inline bool CompareDrawableDistances(const std::pair<ShapeNode*, float>& lhs, const std::pair<ShapeNode*, float>& rhs)
{
    return lhs.second < rhs.second;
}

static inline bool CompareDrawables(ShapeNode* lhs, ShapeNode* rhs)
{   
	return true;
}

/// %Task for octree drawables reinsertion.
struct ReinsertDrawablesTask : public MemberFunctionTask<Octree>
{
    /// Construct.
    ReinsertDrawablesTask(Octree* object_, MemberWorkFunctionPtr function_) :
        MemberFunctionTask<Octree>(object_, function_)
    {
    }

    /// Start pointer.
    ShapeNode** start;
    /// End pointer.
	ShapeNode** end;
};

Octant::Octant() :
    parent(nullptr),
    visibility(VIS_VISIBLE_UNKNOWN),
	occlusionQueryTimer(Random() * OCCLUSION_QUERY_INTERVAL),
    numChildren(0)
{
    for (size_t i = 0; i < NUM_OCTANTS; ++i)
        children[i] = nullptr;
}

Octant::~Octant()
{
   
}

void Octant::Initialize(Octant* parent_, const BoundingBox& boundingBox, unsigned char level_, unsigned char childIndex_)
{
	BoundingBox worldBoundingBox = boundingBox;
    center = worldBoundingBox.getCenter();
    halfSize = worldBoundingBox.getHalfSize();
    fittingBox = BoundingBox(worldBoundingBox.min - halfSize, worldBoundingBox.max + halfSize);

    parent = parent_;
    level = level_;
    childIndex = childIndex_;
    flags = OF_CULLING_BOX_DIRTY;
}

void Octant::OnRenderAABB(const Vector4f& color) {
	//if (!m_drawDebug)
		//return;

	DebugRenderer::Get().AddBoundingBox(CullingBox(), color);
}

void Octant::OnOcclusionQuery(unsigned queryId){
	// Mark pending
	occlusionQueryId = queryId;
}

void Octant::OnOcclusionQueryResult(bool visible){
	// Mark not pending
	occlusionQueryId = 0;

	// Do not change visibility if currently outside the frustum
	if (visibility == VIS_OUTSIDE_FRUSTUM)
		return;

	OctantVisibility lastVisibility = (OctantVisibility)visibility;
	OctantVisibility newVisibility = visible ? VIS_VISIBLE : VIS_OCCLUDED;

	visibility = newVisibility;

	if (lastVisibility <= VIS_OCCLUDED_UNKNOWN && newVisibility == VIS_VISIBLE){
		// If came into view after being occluded, mark children as still occluded but that should be tested in hierarchy
		if (numChildren)
			PushVisibilityToChildren(this, VIS_OCCLUDED_UNKNOWN);
	}else if (newVisibility == VIS_OCCLUDED && lastVisibility != VIS_OCCLUDED && parent && parent->visibility == VIS_VISIBLE){
		// If became occluded, mark parent unknown so it will be tested next
		parent->visibility = VIS_VISIBLE_UNKNOWN;
	}

	// Whenever is visible, push visibility to parents if they are not visible yet
	if (newVisibility == VIS_VISIBLE){
		Octant* octant = parent;

		while (octant && octant->visibility != newVisibility){
			octant->visibility = newVisibility;
			octant = octant->parent;
		}
	}
}

const BoundingBox& Octant::CullingBox() const
{
    if (TestFlag(OF_CULLING_BOX_DIRTY))
    {
        if (!numChildren && drawables.empty())
            cullingBox.define(center);
        else
        {
            // Use a temporary bounding box for calculations in case many threads call this simultaneously
			BoundingBox tempBox;

            for (auto it = drawables.begin(); it != drawables.end(); ++it)
                tempBox.merge((*it)->getWorldBoundingBox());

            if (numChildren)
            {
                for (size_t i = 0; i < NUM_OCTANTS; ++i)
                {
                    if (children[i])
                        tempBox.merge(children[i]->CullingBox());
                }
            }

            cullingBox = tempBox;
        }

        SetFlag(OF_CULLING_BOX_DIRTY, false);
    }

    return cullingBox;
}

Octree::Octree() : threadedUpdate(false), frameNumber(0), workQueue(WorkQueue::Get()){
    assert(workQueue);

    root.Initialize(nullptr, BoundingBox(-DEFAULT_OCTREE_SIZE, DEFAULT_OCTREE_SIZE), DEFAULT_OCTREE_LEVELS, 0);

    // Have at least 1 task for reinsert processing
    reinsertTasks.push_back(new ReinsertDrawablesTask(this, &Octree::CheckReinsertWork));
    reinsertQueues = new std::vector<ShapeNode*>[workQueue->NumThreads()];
}

Octree::~Octree()
{
    // Clear octree association from nodes that were never inserted
    // Note: the threaded queues cannot have nodes that were never inserted, only nodes that should be moved
    for (auto it = updateQueue.begin(); it != updateQueue.end(); ++it)
    {
		ShapeNode* drawable = *it;
        if (drawable)
        {
            drawable->m_octant = nullptr;
			drawable->m_reinsertQueued = false;
        }
    }

    DeleteChildOctants(&root, true);
}

void Octree::OnRenderAABB(const Vector4f& color) {
	root.OnRenderAABB(color);
}

void Octree::Update(unsigned short frameNumber_)
{
    frameNumber = frameNumber_;

    // Avoid overhead of threaded update if only a small number of objects to update / reinsert
    if (updateQueue.size())
    {
        SetThreadedUpdate(true);

        // Split into smaller tasks to encourage work stealing in case some thread is slower
        size_t nodesPerTask = std::max(MIN_THREADED_UPDATE, updateQueue.size() / workQueue->NumThreads() / 4);
        size_t taskIdx = 0;

        for (size_t start = 0; start < updateQueue.size(); start += nodesPerTask)
        {
            size_t end = start + nodesPerTask;
            if (end > updateQueue.size())
                end = updateQueue.size();

            if (reinsertTasks.size() <= taskIdx)
                reinsertTasks.push_back(new ReinsertDrawablesTask(this, &Octree::CheckReinsertWork));
            reinsertTasks[taskIdx]->start = &updateQueue[0] + start;
            reinsertTasks[taskIdx]->end = &updateQueue[0] + end;
            ++taskIdx;
        }

        numPendingReinsertionTasks.store((int)taskIdx);
        workQueue->QueueTasks(taskIdx, reinterpret_cast<Task**>(&reinsertTasks[0]));
    }
    else
        numPendingReinsertionTasks.store(0);
}

void Octree::FinishUpdate()
{
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
        Octant* octant = *it;
        std::sort(octant->drawables.begin(), octant->drawables.end(), CompareDrawables);
        octant->SetFlag(OF_DRAWABLES_SORT_DIRTY, false);
    }

    sortDirtyOctants.clear();
}

void Octree::Resize(const BoundingBox& boundingBox, int numLevels)
{
    // Collect nodes to the root and delete all child octants
    updateQueue.clear();  
    CollectDrawables(updateQueue, &root);
    DeleteChildOctants(&root, false);

    //allocator.Reset();
    root.Initialize(nullptr, boundingBox, (unsigned char)Math::Clamp(numLevels, 1, MAX_OCTREE_LEVELS), 0);
}

void Octree::QueueUpdate(ShapeNode* drawable)
{
    assert(drawable);

	if (drawable->m_octant) {
		drawable->m_octant->MarkCullingBoxDirty();
	}

    if (!threadedUpdate)
    {
        updateQueue.push_back(drawable);
        //drawable->SetFlag(DF_OCTREE_REINSERT_QUEUED, true);
		drawable->m_reinsertQueued = true;
    }
    else
    {
        //drawable->lastUpdateFrameNumber = frameNumber;

        // Do nothing if still fits the current octant
        const BoundingBox& box = drawable->getWorldBoundingBox();
        Octant* oldOctant = drawable->getOctant();
        if (!oldOctant || oldOctant->fittingBox.isInside(box) != BoundingBox::INSIDE)
        {
            reinsertQueues[WorkQueue::ThreadIndex()].push_back(drawable);
			drawable->m_reinsertQueued = true;
        }
    }
}



void Octree::RemoveDrawable(ShapeNode* drawable)
{
    if (!drawable)
        return;

    RemoveDrawable(drawable, drawable->getOctant());
    if (drawable->m_reinsertQueued)
    {
        RemoveDrawableFromQueue(drawable, updateQueue);

        // Remove also from threaded queues if was left over before next update
        for (size_t i = 0; i < workQueue->NumThreads(); ++i)
            RemoveDrawableFromQueue(drawable, reinsertQueues[i]);

		drawable->m_reinsertQueued = false;
    }

    drawable->m_octant = nullptr;
}

void Octree::SetBoundingBoxAttr(const BoundingBox& value)
{
    worldBoundingBox = value;
}

const BoundingBox& Octree::BoundingBoxAttr() const
{
    return worldBoundingBox;
}

void Octree::SetNumLevelsAttr(int numLevels)
{
    /// Setting the number of level (last attribute) triggers octree resize when deserializing
    Resize(worldBoundingBox, numLevels);
}

int Octree::NumLevelsAttr() const
{
    return root.level;
}

void Octree::ReinsertDrawables(std::vector<ShapeNode*>& drawables)
{
    for (auto it = drawables.begin(); it != drawables.end(); ++it)
    {
		ShapeNode* drawable = *it;

        const BoundingBox& box = drawable->getWorldBoundingBox();
        Octant* oldOctant = drawable->getOctant();
        Octant* newOctant = &root;
        Vector3f boxSize = box.getSize();

        for (;;)
        {
            // If drawable does not fit fully inside root octant, must remain in it
            bool insertHere = (newOctant == &root) ?
                (newOctant->fittingBox.isInside(box) != BoundingBox::INSIDE || newOctant->FitBoundingBox(box, boxSize)) :
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
                newOctant = CreateChildOctant(newOctant, newOctant->ChildIndex(box.getCenter()));
        }

		drawable->m_reinsertQueued = false;
    }

    drawables.clear();
}

void Octree::RemoveDrawableFromQueue(ShapeNode* drawable, std::vector<ShapeNode*>& drawables)
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

Octant* Octree::CreateChildOctant(Octant* octant, unsigned char index)
{
    if (octant->children[index])
        return octant->children[index];

    // Remove the culling extra from the bounding box before splitting
    Vector3f newMin = octant->fittingBox.min + octant->halfSize;
    Vector3f newMax = octant->fittingBox.max - octant->halfSize;
    const Vector3f& oldCenter = octant->center;

    if (index & 1)
        newMin[0] = oldCenter[0];
    else
        newMax[0] = oldCenter[0];

    if (index & 2)
        newMin[1] = oldCenter[1];
    else
        newMax[1] = oldCenter[1];

    if (index & 4)
        newMin[2] = oldCenter[2];
    else
        newMax[2] = oldCenter[2];

    Octant* child = new Octant();
    child->Initialize(octant, BoundingBox(newMin, newMax), octant->level - 1, index);
    octant->children[index] = child;
    ++octant->numChildren;

    return child;
}

void Octree::DeleteChildOctant(Octant* octant, unsigned char index)
{
	delete octant->children[index];
    octant->children[index] = nullptr;
    --octant->numChildren;
}

void Octree::DeleteChildOctants(Octant* octant, bool deletingOctree)
{
    for (auto it = octant->drawables.begin(); it != octant->drawables.end(); ++it)
    {
		ShapeNode* drawable = *it;
        drawable->m_octant = nullptr;
		drawable->m_reinsertQueued = false;
        //if (deletingOctree)
			//drawable->Owner()->octree = nullptr;
    }
    octant->drawables.clear();

    if (octant->numChildren)
    {
        for (size_t i = 0; i < NUM_OCTANTS; ++i)
        {
            if (octant->children[i])
            {
                DeleteChildOctants(octant->children[i], deletingOctree);
				delete octant->children[i];
                octant->children[i] = nullptr;
            }
        }
        octant->numChildren = 0;
    }
}

void Octree::CollectDrawables(std::vector<ShapeNode*>& result, Octant* octant) const
{
    result.insert(result.end(), octant->drawables.begin(), octant->drawables.end());

    if (octant->numChildren)
    {
        for (size_t i = 0; i < NUM_OCTANTS; ++i)
        {
            if (octant->children[i])
                CollectDrawables(result, octant->children[i]);
        }
    }
}

void Octree::CollectDrawables(std::vector<ShapeNode*>& result, Octant* octant, unsigned short drawableFlags, unsigned layerMask) const
{
    std::vector<ShapeNode*>& drawables = octant->drawables;

    for (auto it = drawables.begin(); it != drawables.end(); ++it)
    {
		ShapeNode* drawable = *it;
        //if ((drawable->Flags() & drawableFlags) == drawableFlags && (drawable->LayerMask() & layerMask))
            result.push_back(drawable);
    }

    if (octant->numChildren)
    {
        for (size_t i = 0; i < NUM_OCTANTS; ++i)
        {
            if (octant->children[i])
                CollectDrawables(result, octant->children[i], drawableFlags, layerMask);
        }
    }
}

void Octree::CheckReinsertWork(Task* task_, unsigned threadIndex_)
{
    ReinsertDrawablesTask* task = static_cast<ReinsertDrawablesTask*>(task_);
    ShapeNode** start = task->start;
	ShapeNode** end = task->end;
    std::vector<ShapeNode*>& reinsertQueue = reinsertQueues[threadIndex_];

    for (; start != end; ++start)
    {
        // If drawable was removed before reinsertion could happen, a null pointer will be in its place
		ShapeNode* drawable = *start;
        if (!drawable)
            continue;

        if (drawable->m_octreeUpdate)
            drawable->OnOctreeUpdate();

        //drawable->lastUpdateFrameNumber = frameNumber;

        // Do nothing if still fits the current octant
        const BoundingBox& box = drawable->getWorldBoundingBox();
        Octant* oldOctant = drawable->getOctant();
        if (!oldOctant || oldOctant->fittingBox.isInside(box) != BoundingBox::INSIDE)
            reinsertQueue.push_back(drawable);
        else
			drawable->m_reinsertQueued = false;
    }

    numPendingReinsertionTasks.fetch_add(-1);
}
