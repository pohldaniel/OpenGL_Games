#include <engine/DebugRenderer.h>
#include "OctreeNew.h"

static const float DEFAULT_OCTREE_SIZE = 1000.0f;
static const int DEFAULT_OCTREE_LEVELS = 8;
static const int MAX_OCTREE_LEVELS = 255;
static const size_t MIN_THREADED_UPDATE = 16;

static inline bool CompareDrawables(ShapeNode* lhs, ShapeNode* rhs){
	/*unsigned short lhsFlags = lhs->Flags() & (DF_LIGHT | DF_GEOMETRY);
	unsigned short rhsFlags = rhs->Flags() & (DF_LIGHT | DF_GEOMETRY);
	if (lhsFlags != rhsFlags)
		return lhsFlags < rhsFlags;
	else
		return lhs < rhs;*/
	return true;
}

Octant::Octant() : parent(nullptr), numChildren(0), m_drawDebug(true), m_cullingBoxDirty(true), m_sortDirty(true){
	for (size_t i = 0; i < NUM_OCTANTS; ++i)
		children[i] = nullptr;
}

Octant::~Octant(){
	
}

void Octant::Initialize(Octant* parent_, const BoundingBox& boundingBox, unsigned char level_, unsigned char childIndex_){
	BoundingBox worldBoundingBox = boundingBox;
	center = worldBoundingBox.getCenter();
	halfSize = worldBoundingBox.getCenter();
	fittingBox = BoundingBox(worldBoundingBox.min - halfSize, worldBoundingBox.max + halfSize);

	parent = parent_;
	level = level_;
	childIndex = childIndex_;
	m_cullingBoxDirty = true;
}

/*void Octant::OnRenderOBB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getCullingBox(), getWorldTransformation(), color);
}*/

void Octant::OnRenderAABB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getCullingBox(), color);
}

const BoundingBox& Octant::getCullingBox() const {
	if (m_cullingBoxDirty){
		if (!numChildren && drawables.empty())
			cullingBox.define(center);
		else{
			// Use a temporary bounding box for calculations in case many threads call this simultaneously
			BoundingBox tempBox;

			for (auto it = drawables.begin(); it != drawables.end(); ++it)
				tempBox.merge((*it)->getWorldBoundingBox());

			if (numChildren){
				for (size_t i = 0; i < NUM_OCTANTS; ++i){
					if (children[i])
						tempBox.merge(children[i]->getCullingBox());
				}
			}

			cullingBox = tempBox;
		}
		m_cullingBoxDirty = false;
	}

	return cullingBox;
}

void Octant::markCullingBoxDirty() {
	
	const Octant* octant = this;
	while (octant && !octant->m_cullingBoxDirty){
		octant->m_cullingBoxDirty = true;
		octant = octant->parent;
	}
}

bool Octant::FitBoundingBox(const BoundingBox& box, const Vector3f& boxSize) const {
	// If max split level, size always OK, otherwise check that box is at least half size of octant
	if (level <= 1 || boxSize[0] >= halfSize[0] || boxSize[1] >= halfSize[1] || boxSize[2] >= halfSize[2])
		return true;
	// Also check if the box can not fit inside a child octant's culling box, in that case size OK (must insert here)
	else{
		Vector3f quarterSize = 0.5f * halfSize;
		if (box.min[0] <= fittingBox.min[0] + quarterSize[0] || box.max[0] >= fittingBox.max[0] - quarterSize[0] ||
			box.min[1] <= fittingBox.min[1] + quarterSize[1] || box.max[1] >= fittingBox.max[1] - quarterSize[1] ||
			box.max[2] <= fittingBox.min[2] + quarterSize[2] || box.max[2] >= fittingBox.max[2] - quarterSize[2])
			return true;
	}

	// Bounding box too small, should create a child octant
	return false;
}

OctantVisibility Octant::Visibility() const { 
	return (OctantVisibility)visibility; 
}

void Octant::PushVisibilityToChildren(Octant* octant, OctantVisibility newVisibility){
	for (size_t i = 0; i < NUM_OCTANTS; ++i){
		if (octant->children[i]){
			octant->children[i]->visibility = newVisibility;
			if (octant->children[i]->numChildren)
				PushVisibilityToChildren(octant->children[i], newVisibility);
		}
	}
}

void Octant::SetVisibility(OctantVisibility newVisibility, bool pushToChildren){
	visibility = newVisibility;

	if (pushToChildren)
		PushVisibilityToChildren(this, newVisibility);
}

unsigned char Octant::ChildIndex(const Vector3f& position) const {
	unsigned char ret = position[0] < center[0] ? 0 : 1; ret += position[1] < center[1] ? 0 : 2; ret += position[2] < center[2] ? 0 : 4;
	return ret;
}

const std::vector<ShapeNode*>& Octant::getDrawables() const { 
	return drawables; 
}

bool Octant::hasChildren() const { 
	return numChildren > 0; 
}

Octant*  Octant::getChild(size_t index) const {
	return children[index]; 
}

///////////////////////////////////////////////////////////////////
Octree::Octree() : m_threadedUpdate(false), m_frameNumber(0),workQueue(WorkQueue::Get()){

	root.Initialize(nullptr, BoundingBox(-DEFAULT_OCTREE_SIZE, DEFAULT_OCTREE_SIZE), DEFAULT_OCTREE_LEVELS, 0);
	reinsertTasks.push_back(new ReinsertDrawablesTask(this, &Octree::CheckReinsertWork));
	reinsertQueues = new std::vector<ShapeNode*>[workQueue->NumThreads()];
}

Octree::~Octree(){	
	for (auto it = updateQueue.begin(); it != updateQueue.end(); ++it){
		ShapeNode* drawable = *it;
		if (drawable){
			drawable->m_octant = nullptr;
			drawable->m_reinsertQueued = false;
		}
	}

	DeleteChildOctants(&root, true);
}

void Octree::Update(unsigned short frameNumber_){

	m_frameNumber = frameNumber_;

	// Avoid overhead of threaded update if only a small number of objects to update / reinsert
	if (updateQueue.size()){
		SetThreadedUpdate(true);

		// Split into smaller tasks to encourage work stealing in case some thread is slower
		size_t nodesPerTask = std::max(MIN_THREADED_UPDATE, updateQueue.size() / workQueue->NumThreads() / 4);
		size_t taskIdx = 0;

		for (size_t start = 0; start < updateQueue.size(); start += nodesPerTask){
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
		workQueue->QueueTasks(taskIdx, reinterpret_cast<TaskOct**>(&reinsertTasks[0]));
	}
	else
		numPendingReinsertionTasks.store(0);
}

void Octree::FinishUpdate(){

	// Complete tasks until reinsertions done. There may other tasks going on at the same time
	while (numPendingReinsertionTasks.load() > 0)
		workQueue->TryComplete();

	SetThreadedUpdate(false);

	// Now reinsert drawables that actually need reinsertion into a different octant
	for (size_t i = 0; i < workQueue->NumThreads(); ++i)
		ReinsertDrawables(reinsertQueues[i]);

	updateQueue.clear();

	// Sort octants' drawables by address and put lights first
	for (auto it = sortDirtyOctants.begin(); it != sortDirtyOctants.end(); ++it){
		Octant* octant = *it;
		std::sort(octant->drawables.begin(), octant->drawables.end(), CompareDrawables);
		octant->m_sortDirty = false;
	}

	sortDirtyOctants.clear();
}

void Octree::Resize(const BoundingBox& boundingBox, int numLevels){

	updateQueue.clear();
	CollectDrawables(updateQueue, &root);
	DeleteChildOctants(&root, false);

	root.Initialize(nullptr, boundingBox, (unsigned char)Math::Clamp(numLevels, 1, MAX_OCTREE_LEVELS), 0);
}

void Octree::OnRenderAABB(const Vector4f& color) {
	root.OnRenderAABB(color);
}

void Octree::RemoveDrawable(ShapeNode* drawable){
	if (!drawable)
		return;

	RemoveDrawable(drawable, drawable->getOctant());
	if (drawable->m_reinsertQueued){
		RemoveDrawableFromQueue(drawable, updateQueue);

		// Remove also from threaded queues if was left over before next update
		for (size_t i = 0; i < workQueue->NumThreads(); ++i)
			RemoveDrawableFromQueue(drawable, reinsertQueues[i]);

		drawable->m_reinsertQueued = false;
	}

	drawable->m_octant = nullptr;
}

void Octree::CollectDrawables(std::vector<ShapeNode*>& result, Octant* octant) const {
	result.insert(result.end(), octant->drawables.begin(), octant->drawables.end());

	if (octant->numChildren){
		for (size_t i = 0; i < NUM_OCTANTS; ++i){
			if (octant->children[i])
				CollectDrawables(result, octant->children[i]);
		}
	}
}

void Octree::AddDrawable(ShapeNode* drawable, Octant* octant){
	octant->drawables.push_back(drawable);
	octant->markCullingBoxDirty();
	drawable->m_octant = octant;

	if (!octant->m_sortDirty){
		octant->m_sortDirty = true;
		sortDirtyOctants.push_back(octant);
	}
}

void Octree::QueueUpdate(ShapeNode* drawable){

	if (drawable->m_octant)
		drawable->m_octant->markCullingBoxDirty();

	if (!m_threadedUpdate){
		updateQueue.push_back(drawable);
		drawable->m_reinsertQueued = true;

	}else{
		//drawable->lastUpdateFrameNumber = frameNumber;

		// Do nothing if still fits the current octant
		const BoundingBox& box = drawable->getWorldBoundingBox();
		Octant* oldOctant = drawable->getOctant();
		if (!oldOctant || oldOctant->fittingBox.isInside(box) != INSIDE){
			reinsertQueues[WorkQueue::ThreadIndex()].push_back(drawable);
			drawable->m_reinsertQueued = true;
		}
	}
}

void Octree::RemoveDrawable(ShapeNode* drawable, Octant* octant){
	if (!octant)
		return;

	octant->markCullingBoxDirty();
	for (auto it = octant->drawables.begin(); it != octant->drawables.end(); ++it){
		if ((*it) == drawable){

			octant->drawables.erase(it);

			while (!octant->drawables.size() && !octant->numChildren && octant->parent){
				Octant* parentOctant = octant->parent;
				DeleteChildOctant(parentOctant, octant->childIndex);
				octant = parentOctant;
			}
			return;
		}
	}
}

void Octree::ReinsertDrawables(std::vector<ShapeNode*>& drawables) {
	for (auto it = drawables.begin(); it != drawables.end(); ++it){
		ShapeNode* drawable = *it;

		const BoundingBox& box = drawable->getWorldBoundingBox();
		Octant* oldOctant = drawable->getOctant();
		Octant* newOctant = &root;
		Vector3f boxSize = box.getSize();

		for (;;){
			// If drawable does not fit fully inside root octant, must remain in it
			bool insertHere = (newOctant == &root) ?
				(newOctant->fittingBox.isInside(box) != INSIDE || newOctant->FitBoundingBox(box, boxSize)) :
				newOctant->FitBoundingBox(box, boxSize);

			if (insertHere){
				if (newOctant != oldOctant){
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


void Octree::RemoveDrawableFromQueue(ShapeNode* drawable, std::vector<ShapeNode*>& drawables){
	for (auto it = drawables.begin(); it != drawables.end(); ++it){
		if ((*it) == drawable){
			*it = nullptr;
			break;
		}
	}
}

Octant* Octree::CreateChildOctant(Octant* octant, unsigned char index){
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

void Octree::DeleteChildOctant(Octant* octant, unsigned char index){
	delete octant->children[index];
	octant->children[index] = nullptr;
	--octant->numChildren;
}

void Octree::DeleteChildOctants(Octant* octant, bool deletingOctree){
	for (auto it = octant->drawables.begin(); it != octant->drawables.end(); ++it){
		ShapeNode* drawable = *it;
		drawable->m_octant = nullptr;
		drawable->m_reinsertQueued = false;
		//if (deletingOctree)
			//drawable->Owner()->octree = nullptr;
	}
	octant->drawables.clear();

	if (octant->numChildren){
		for (size_t i = 0; i < NUM_OCTANTS; ++i){
			if (octant->children[i]){
				DeleteChildOctants(octant->children[i], deletingOctree);
				delete octant->children[i];
				octant->children[i] = nullptr;
			}
		}
		octant->numChildren = 0;
	}
}

void Octree::CheckReinsertWork(TaskOct* task_, unsigned threadIndex_) {
	ReinsertDrawablesTask* task = static_cast<ReinsertDrawablesTask*>(task_);
	ShapeNode** start = task->start;
	ShapeNode** end = task->end;
	std::vector<ShapeNode*>& reinsertQueue = reinsertQueues[threadIndex_];

	for (; start != end; ++start) {
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
		if (!oldOctant || oldOctant->fittingBox.isInside(box) != Intersection::INSIDE)
			reinsertQueue.push_back(drawable);
		else
			drawable->m_reinsertQueued = false;
	}

	numPendingReinsertionTasks.fetch_add(-1);
}

void Octree::SetThreadedUpdate(bool enable) {
	m_threadedUpdate = enable; 
}

bool  Octree::GetThreadedUpdate() const { 
	return m_threadedUpdate; 
}

Octant* Octree::getRoot() const { 
	return const_cast<Octant*>(&root); 
}