// For conditions of distribution and use, see copyright notice in License.txt

#include "../Graphics/Graphics.h"
#include "../IO/Log.h"
#include "../Math/RandomTu.h"
#include "../Math/Ray.h"
#include "DebugRendererTu.h"
#include "OctreeTu.h"
#include <Octree/Octree.h>

#include <cassert>
#include <algorithm>
#include <tracy/Tracy.hpp>

#include <engine/DebugRenderer.h>

static const float DEFAULT_OCTREE_SIZE = 1000.0f;
static const int DEFAULT_OCTREE_LEVELS = 8;
static const int MAX_OCTREE_LEVELS = 255;
static const size_t MIN_THREADED_UPDATE = 16;

static std::vector<unsigned> freeQueries;

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

static inline bool CompareDrawableDistances2(const std::pair<ShapeNode*, float>& lhs, const std::pair<ShapeNode*, float>& rhs){
	return lhs.second < rhs.second;
}

static inline bool CompareDrawables2(ShapeNode* lhs, ShapeNode* rhs){
	return true;
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


/// %Task for octree drawables reinsertion.
struct ReinsertDrawablesTaskTu2 : public MemberFunctionTaskTu<OctreeTu>
{
	/// Construct.
	ReinsertDrawablesTaskTu2(OctreeTu* object_, MemberWorkFunctionPtr function_) :
		MemberFunctionTaskTu<OctreeTu>(object_, function_)
	{
	}

	/// Start pointer.
	ShapeNode** start;
	/// End pointer.
	ShapeNode** end;
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

void OctantTu::Initialize(OctantTu* parent_, const BoundingBox& boundingBox, unsigned char level_, unsigned char childIndex_)
{
    BoundingBox worldBoundingBox = boundingBox;
    center = worldBoundingBox.getCenter();
    halfSize = worldBoundingBox.getHalfSize();
    fittingBox = BoundingBox(worldBoundingBox.min - halfSize, worldBoundingBox.max + halfSize);

    parent = parent_;
    level = level_;
    childIndex = childIndex_;
    flags = OF_CULLING_BOX_DIRTYTU;
}

void OctantTu::OnRenderAABB(const Vector4f& color) {
	//if (!m_drawDebug)
		//return;

	DebugRenderer::Get().AddBoundingBox(CullingBox(), color);
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

const BoundingBox& OctantTu::CullingBox() const
{
    if (TestFlag(OF_CULLING_BOX_DIRTYTU))
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
                for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
                {
                    if (children[i])
                        tempBox.merge(children[i]->CullingBox());
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
    workQueue(ObjectTu::Subsystem<WorkQueueTu>())
{
    assert(workQueue);

    root.Initialize(nullptr, BoundingBox(-DEFAULT_OCTREE_SIZE, DEFAULT_OCTREE_SIZE), DEFAULT_OCTREE_LEVELS, 0);

    // Have at least 1 task for reinsert processing
	//reinsertTasks.push_back(new ReinsertDrawablesTaskTu(this, &OctreeTu::CheckReinsertWork));
	_reinsertTasks.push_back(new ReinsertDrawablesTaskTu2(this, &OctreeTu::CheckReinsertWork2));
	//reinsertQueues = new std::vector<Drawable*>[workQueue->NumThreads()];
	_reinsertQueues = new std::vector<ShapeNode*>[workQueue->NumThreads()];
}

OctreeTu::~OctreeTu()
{
    // Clear octree association from nodes that were never inserted
    // Note: the threaded queues cannot have nodes that were never inserted, only nodes that should be moved
	for (auto it = _updateQueue.begin(); it != _updateQueue.end(); ++it){
		ShapeNode* drawable = *it;
		if (drawable){
			drawable->m_octant = nullptr;
			drawable->m_reinsertQueued = false;
		}
	}

    //DeleteChildOctants(&root, true);
}

void OctreeTu::Update()
{
    ZoneScoped;
    // Avoid overhead of threaded update if only a small number of objects to update / reinsert
    if (_updateQueue.size()) {
		SetThreadedUpdate(true);

		// Split into smaller tasks to encourage work stealing in case some thread is slower
		size_t nodesPerTask = std::max(MIN_THREADED_UPDATE, _updateQueue.size() / workQueue->NumThreads() / 4);
		size_t taskIdx = 0;

		for (size_t start = 0; start < _updateQueue.size(); start += nodesPerTask)
		{
			size_t end = start + nodesPerTask;
			if (end > _updateQueue.size())
				end = _updateQueue.size();

			if (_reinsertTasks.size() <= taskIdx)
				_reinsertTasks.push_back(new ReinsertDrawablesTaskTu2(this, &OctreeTu::CheckReinsertWork2));
			_reinsertTasks[taskIdx]->start = &_updateQueue[0] + start;
			_reinsertTasks[taskIdx]->end = &_updateQueue[0] + end;
			++taskIdx;
		}

		numPendingReinsertionTasks.store((int)taskIdx);
		workQueue->QueueTasks(taskIdx, reinterpret_cast<TaskTu**>(&_reinsertTasks[0]));
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
	// for (size_t i = 0; i < workQueue->NumThreads(); ++i)
	//     ReinsertDrawables(reinsertQueues[i]);

	for (size_t i = 0; i < workQueue->NumThreads(); ++i)
		ReinsertDrawables(_reinsertQueues[i]);

	//updateQueue.clear();
	_updateQueue.clear();

    // Sort octants' drawables by address and put lights first
    for (auto it = sortDirtyOctants.begin(); it != sortDirtyOctants.end(); ++it)
    {
        OctantTu* octant = *it;
		//std::sort(octant->drawables.begin(), octant->drawables.end(), CompareDrawables);
		std::sort(octant->drawables.begin(), octant->drawables.end(), CompareDrawables2);
        octant->SetFlag(OF_DRAWABLES_SORT_DIRTYTU, false);
    }

    sortDirtyOctants.clear();
}

void OctreeTu::Resize(const BoundingBox& boundingBox, int numLevels)
{
    ZoneScoped;

    // Collect nodes to the root and delete all child octants
	//updateQueue.clear();
	_updateQueue.clear();
    std::vector<Drawable*> occluders;
    
	//CollectDrawables(updateQueue, &root);
	CollectDrawables(_updateQueue, &root);
    //DeleteChildOctants(&root, false);

    allocator.Reset();
    root.Initialize(nullptr, boundingBox, (unsigned char)Clamp(numLevels, 1, MAX_OCTREE_LEVELS), 0);
}

void OctreeTu::OnRenderAABB(const Vector4f& color) {
	root.OnRenderAABB(color);
}

void OctreeTu::QueueUpdate(ShapeNode* drawable)
{
	assert(drawable);

	if (drawable->m_octantTu) {
		drawable->m_octantTu->MarkCullingBoxDirty();
	}

	if (!threadedUpdate)
	{
		_updateQueue.push_back(drawable);
		//drawable->SetFlag(DF_OCTREE_REINSERT_QUEUED, true);
		drawable->m_reinsertQueued = true;
	}
	else
	{
		//drawable->lastUpdateFrameNumber = frameNumber;

		// Do nothing if still fits the current octant
		const BoundingBox& box = drawable->getWorldBoundingBox();
		OctantTu* oldOctant = drawable->getOctantTu();
		if (!oldOctant || oldOctant->fittingBox.isInside(box) != BoundingBox::INSIDE)
		{
			_reinsertQueues[WorkQueueTu::ThreadIndex()].push_back(drawable);
			drawable->m_reinsertQueued = true;
		}
	}
}

void OctreeTu::RemoveDrawable(ShapeNode* drawable)
{
	if (!drawable)
		return;

	RemoveDrawable(drawable, drawable->getOctantTu());
	if (drawable->m_reinsertQueued)
	{
		RemoveDrawableFromQueue(drawable, _updateQueue);

		// Remove also from threaded queues if was left over before next update
		for (size_t i = 0; i < workQueue->NumThreads(); ++i)
			RemoveDrawableFromQueue(drawable, _reinsertQueues[i]);

		drawable->m_reinsertQueued = false;
	}

	drawable->m_octant = nullptr;
}

void OctreeTu::SetBoundingBoxAttr(const BoundingBox& value)
{
    worldBoundingBox = value;
}

const BoundingBox& OctreeTu::BoundingBoxAttr() const
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

/*void OctreeTu::ReinsertDrawables(std::vector<Drawable*>& drawables)
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
}*/

void OctreeTu::ReinsertDrawables(std::vector<ShapeNode*>& drawables)
{
	for (auto it = drawables.begin(); it != drawables.end(); ++it)
	{
		ShapeNode* drawable = *it;

		const BoundingBox& box = drawable->getWorldBoundingBox();
		OctantTu* oldOctant = drawable->getOctantTu();
		OctantTu* newOctant = &root;
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
			else {
				Vector3f center = box.getCenter();
				newOctant = CreateChildOctant(newOctant, newOctant->ChildIndex(Vector3(center[0], center[1], center[2])));
			}
		}

		drawable->m_reinsertQueued = false;
	}

	drawables.clear();
}

void OctreeTu::RemoveDrawableFromQueue(ShapeNode* drawable, std::vector<ShapeNode*>& drawables)
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

    OctantTu* child = allocator.Allocate();
    child->Initialize(octant, BoundingBox(newMin, newMax), octant->level - 1, index);
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

void OctreeTu::CollectDrawables(std::vector<ShapeNode*>& result, OctantTu* octant) const
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

void OctreeTu::CollectDrawables(std::vector<ShapeNode*>& result, OctantTu* octant, unsigned short drawableFlags, unsigned layerMask) const
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

void OctreeTu::CheckReinsertWork2(TaskTu* task_, unsigned threadIndex_)
{
	ReinsertDrawablesTaskTu2* task = static_cast<ReinsertDrawablesTaskTu2*>(task_);
	ShapeNode** start = task->start;
	ShapeNode** end = task->end;
	std::vector<ShapeNode*>& reinsertQueue = _reinsertQueues[threadIndex_];

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
		OctantTu* oldOctant = drawable->getOctantTu();
		if (!oldOctant || oldOctant->fittingBox.isInside(box) != BoundingBox::INSIDE)
			reinsertQueue.push_back(drawable);
		else
			drawable->m_reinsertQueued = false;
	}

	numPendingReinsertionTasks.fetch_add(-1);
}