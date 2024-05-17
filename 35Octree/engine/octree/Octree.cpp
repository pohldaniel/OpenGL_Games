#include <algorithm>
#include <engine/DebugRenderer.h>
#include "Octree.h"

const float Octree::OCCLUSION_MARGIN = 0.1f;
std::unique_ptr<Shader> Octree::ShaderOcclusion= nullptr;
std::vector<std::pair<unsigned, void*>> Octree::PendingQueries;

static const float DEFAULT_OCTREE_SIZE = 1000.0f;
static const int DEFAULT_OCTREE_LEVELS = 8;
static const int MAX_OCTREE_LEVELS = 255;
static const size_t MIN_THREADED_UPDATE = 16;
static unsigned randomSeed = 1;

static inline bool CompareDrawableDistances(const std::pair<OctreeNode*, float>& lhs, const std::pair<OctreeNode*, float>& rhs){
	return lhs.second < rhs.second;
}

static inline bool CompareDrawables(OctreeNode* lhs, OctreeNode* rhs){
	return lhs < rhs;
}

static int Rand() {
	randomSeed = randomSeed * 214013 + 2531011;
	return (randomSeed >> 16) & 32767;
}

static float Random() { return Rand() / 32768.0f; }

Octant::Octant() :
	m_parent(nullptr),
	m_visibility(VIS_VISIBLE_UNKNOWN),
	m_occlusionQueryId(0),
	m_occlusionQueryTimer(Random() * OCCLUSION_QUERY_INTERVAL),
	m_numChildren(0){
    for (size_t i = 0; i < NUM_OCTANTS; ++i)
		m_children[i] = nullptr;
}

Octant::~Octant(){
    if (m_occlusionQueryId){
		Octree::FreeOcclusionQuery(m_occlusionQueryId);
    }
}

void Octant::Initialize(Octant* parent_, const BoundingBox& boundingBox, unsigned char level_, unsigned char childIndex_){
    BoundingBox worldBoundingBox = boundingBox;
	m_center = worldBoundingBox.getCenter();
	m_halfSize = worldBoundingBox.getHalfSize();
	m_fittingBox = BoundingBox(worldBoundingBox.min - m_halfSize, worldBoundingBox.max + m_halfSize);

	m_parent = parent_;
	m_level = level_;
	m_childIndex = childIndex_;

	m_cullingBoxDirty = true;
	m_sortDrawables = true;
	m_drawDebug = true;
}

void Octant::OnRenderAABB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getCullingBox(), color);
}

void Octant::OnOcclusionQuery(unsigned queryId){
	m_occlusionQueryId = queryId;
}

void Octant::OnOcclusionQueryResult(bool visible){
    // Mark not pending
	m_occlusionQueryId = 0;

    // Do not change visibility if currently outside the frustum
    if (m_visibility == VIS_OUTSIDE_FRUSTUM)
        return;

    OctantVisibility lastVisibility = static_cast<OctantVisibility>(m_visibility);
    OctantVisibility newVisibility = visible ? VIS_VISIBLE : VIS_OCCLUDED;

	m_visibility = newVisibility;

    if (lastVisibility <= VIS_OCCLUDED_UNKNOWN && newVisibility == VIS_VISIBLE){
        // If came into view after being occluded, mark children as still occluded but that should be tested in hierarchy
        if (m_numChildren)
            pushVisibilityToChildren(this, VIS_OCCLUDED_UNKNOWN);
    }else if (newVisibility == VIS_OCCLUDED && lastVisibility != VIS_OCCLUDED && m_parent && m_parent->m_visibility == VIS_VISIBLE){
        // If became occluded, mark parent unknown so it will be tested next
		m_parent->m_visibility = VIS_VISIBLE_UNKNOWN;
    }

    // Whenever is visible, push visibility to parents if they are not visible yet
    if (newVisibility == VIS_VISIBLE){
        Octant* octant = m_parent;

        while (octant && octant->m_visibility != newVisibility){
            octant->m_visibility = newVisibility;
            octant = octant->m_parent;
        }
    }
}

const BoundingBox& Octant::getCullingBox() const{
	updateCullingBox();
    return m_cullingBox;
}

void Octant::updateCullingBox() const{
	if (m_cullingBoxDirty){
		if (!m_numChildren && m_octreeNodes.empty())
			m_cullingBox.define(m_center);
		else{
			// Use a temporary bounding box for calculations in case many threads call this simultaneously
			BoundingBox tempBox;

			for (auto it = m_octreeNodes.begin(); it != m_octreeNodes.end(); ++it)
				tempBox.merge((*it)->getWorldBoundingBox());

			if (m_numChildren){
				for (size_t i = 0; i < NUM_OCTANTS; ++i){
					if (m_children[i])
						tempBox.merge(m_children[i]->getCullingBox());
				}
			}

			m_cullingBox = tempBox;
		}
		m_cullingBoxDirty = false;
	}
}

const std::vector<OctreeNode*>& Octant::getOctreeNodes() const {
	return m_octreeNodes;
}

bool Octant::hasChildren() const {
	return m_numChildren > 0;
}

Octant* Octant::getChild(size_t index) const {
	return m_children[index];
}

Octant* Octant::getParent() const {
	return m_parent;
}

unsigned char Octant::getChildIndex(const Vector3f& position) const {
	unsigned char ret = position[0] < m_center[0] ? 0 : 1;
	ret += position[1] < m_center[1] ? 0 : 2;
	ret += position[2] < m_center[2] ? 0 : 4;
	return ret; 
}

OctantVisibility Octant::getVisibility() const {
	return static_cast<OctantVisibility>(m_visibility);
}

bool Octant::OcclusionQueryPending() const {
	return m_occlusionQueryId != 0;
}

bool Octant::fitBoundingBox(const BoundingBox& box, const Vector3f& boxSize) const{
	// If max split level, size always OK, otherwise check that box is at least half size of octant
	if (m_level <= 1 || boxSize[0] >= m_halfSize[0] || boxSize[1] >= m_halfSize[1] || boxSize[2] >= m_halfSize[2]) {
		return true;
		// Also check if the box can not fit inside a child octant's culling box, in that case size OK (must insert here)
	}else {
		Vector3f quarterSize = 0.5f * m_halfSize;
		if (box.min[0] <= m_fittingBox.min[0] + quarterSize[0] || box.max[0] >= m_fittingBox.max[0] - quarterSize[0] ||
			box.min[1] <= m_fittingBox.min[1] + quarterSize[1] || box.max[1] >= m_fittingBox.max[1] - quarterSize[1] ||
			box.max[2] <= m_fittingBox.min[2] + quarterSize[2] || box.max[2] >= m_fittingBox.max[2] - quarterSize[2])
			return true;
	}
	// Bounding box too small, should create a child octant
	return false;
}

void Octant::markCullingBoxDirty() const {
	const Octant* octant = this;

	while (octant && !octant->m_cullingBoxDirty) {
		octant->m_cullingBoxDirty = true;
		octant = octant->m_parent;
	}
}

void Octant::pushVisibilityToChildren(Octant* octant, OctantVisibility newVisibility){
	for (size_t i = 0; i < NUM_OCTANTS; ++i){
		if (octant->m_children[i]){
			octant->m_children[i]->m_visibility = newVisibility;
			if (octant->m_children[i]->m_numChildren)
				pushVisibilityToChildren(octant->m_children[i], newVisibility);
		}
	}
}

void  Octant::setVisibility(OctantVisibility newVisibility, bool pushToChildren){
	m_visibility = newVisibility;

	if (pushToChildren)
		pushVisibilityToChildren(this, newVisibility);
}

bool  Octant::checkNewOcclusionQuery(float dt){
	if (m_visibility != VIS_VISIBLE)
		return m_occlusionQueryId == 0;

	m_occlusionQueryTimer += dt;

	if (m_occlusionQueryId != 0)
		return false;

	if (m_occlusionQueryTimer >= OCCLUSION_QUERY_INTERVAL){
		m_occlusionQueryTimer = fmodf(m_occlusionQueryTimer, OCCLUSION_QUERY_INTERVAL);
		return true;
	}else
		return false;
}


Octree::Octree(const Camera& camera, const Frustum& frustum, const float& dt) :
	m_threadedUpdate(false),
	workQueue(WorkQueue::Get()), 
	frustum(frustum), 
	camera(camera), 
	m_dt(dt),
	m_frameNumber(0), 
	m_useCulling(true), 
	m_useOcclusionCulling(true){

	m_root.Initialize(nullptr, BoundingBox(-DEFAULT_OCTREE_SIZE, DEFAULT_OCTREE_SIZE), DEFAULT_OCTREE_LEVELS, 0);

    // Have at least 1 task for reinsert processing
	m_reinsertTasks.push_back(new ReinsertDrawablesTask(this, &Octree::checkReinsertWork));
	m_reinsertQueues = new std::vector<OctreeNode*>[workQueue->NumThreads()];

	m_octantResults = new ThreadOctantResult[NUM_OCTANT_TASKS];

	for (size_t i = 0; i < NUM_OCTANTS + 1; ++i) {
		m_collectOctantsTasks[i] = new CollectOctantsTask(this, &Octree::collectOctantsWork);
		m_collectOctantsTasks[i]->resultIdx = i;
	}

	if (!ShaderOcclusion) {
		ShaderOcclusion = std::unique_ptr<Shader>(new Shader(OCCLUSION_VERTEX, OCCLUSION_FRGAMENT, false));
	}

	createCube();
}

Octree::~Octree(){
    // Clear octree association from nodes that were never inserted
    // Note: the threaded queues cannot have nodes that were never inserted, only nodes that should be moved
	for (auto it = m_updateQueue.begin(); it != m_updateQueue.end(); ++it){
		OctreeNode* drawable = *it;
		if (drawable){
			drawable->m_octant = nullptr;
			drawable->m_reinsertQueued = false;
		}
	}

    deleteChildOctants(&m_root, true);

	delete[] m_octantResults;
	delete[] m_reinsertQueues;

	for (size_t i = 0; i < NUM_OCTANTS + 1; ++i) {
		delete m_collectOctantsTasks[i];
		m_collectOctantsTasks[i] = nullptr;
	}

	for (size_t i = 0; i < m_reinsertTasks.size(); ++i) {
		delete m_reinsertTasks[i];
		m_reinsertTasks[i] = nullptr;
	}
}

void Octree::updateFrameNumber() {
	++m_frameNumber;
	if (!m_frameNumber)
		++m_frameNumber;
}

void Octree::update(){

    // Avoid overhead of threaded update if only a small number of objects to update / reinsert
    if (m_updateQueue.size()) {
		setThreadedUpdate(true);

		// Split into smaller tasks to encourage work stealing in case some thread is slower
		size_t nodesPerTask = std::max(MIN_THREADED_UPDATE, m_updateQueue.size() / workQueue->NumThreads() / 4);
		size_t taskIdx = 0;

		for (size_t start = 0; start < m_updateQueue.size(); start += nodesPerTask){
			size_t end = start + nodesPerTask;
			if (end > m_updateQueue.size())
				end = m_updateQueue.size();

			if (m_reinsertTasks.size() <= taskIdx)
				m_reinsertTasks.push_back(new ReinsertDrawablesTask(this, &Octree::checkReinsertWork));
			m_reinsertTasks[taskIdx]->start = &m_updateQueue[0] + start;
			m_reinsertTasks[taskIdx]->end = &m_updateQueue[0] + end;
			++taskIdx;
		}

		m_numPendingReinsertionTasks.store((int)taskIdx);
		workQueue->QueueTasks(taskIdx, reinterpret_cast<Task**>(&m_reinsertTasks[0]));
	}else
		m_numPendingReinsertionTasks.store(0);
}

void Octree::finishUpdate(){
    // Complete tasks until reinsertions done. There may other tasks going on at the same time
    while (m_numPendingReinsertionTasks.load() > 0)
        workQueue->TryComplete();

    setThreadedUpdate(false);

    // Now reinsert drawables that actually need reinsertion into a different octant
	for (size_t i = 0; i < workQueue->NumThreads(); ++i)
		reinsertDrawables(m_reinsertQueues[i]);

	m_updateQueue.clear();

    // Sort octants' drawables by address and put lights first
    for (auto it = m_sortDirtyOctants.begin(); it != m_sortDirtyOctants.end(); ++it){
        Octant* octant = *it;
		std::sort(octant->m_octreeNodes.begin(), octant->m_octreeNodes.end(), CompareDrawables);
		octant->m_sortDrawables = false;
    }

	m_sortDirtyOctants.clear();
}

void Octree::resize(const BoundingBox& boundingBox, int numLevels){
    // Collect nodes to the root and delete all child octants
	m_updateQueue.clear();
	collectDrawables(m_updateQueue, &m_root);
    deleteChildOctants(&m_root, false);
	m_root.Initialize(nullptr, boundingBox, (unsigned char)Math::Clamp(numLevels, 1, MAX_OCTREE_LEVELS), 0);
}

void Octree::OnRenderAABB(const Vector4f& color) {
	m_root.OnRenderAABB(color);
}

void Octree::setThreadedUpdate(bool threadedUpdate) {
	m_threadedUpdate = threadedUpdate;
}

bool Octree::getThreadedUpdate() const { 
	return m_threadedUpdate;
}

Octant* Octree::getRoot() const { 
	return const_cast<Octant*>(&m_root);
}

size_t Octree::pendingOcclusionQueries() const {
	return PendingQueries.size(); 
}

const std::vector<Octant*>& Octree::getRootLevelOctants() const{
	return m_rootLevelOctants;
}

const Octree::ThreadOctantResult* Octree::getOctantResults() const {
	return  m_octantResults;
}

void Octree::queueUpdate(OctreeNode* drawable){
	
	if (drawable->m_octant) {
		drawable->m_octant->markCullingBoxDirty();
	}

	if (!m_threadedUpdate){
		m_updateQueue.push_back(drawable);
		drawable->m_reinsertQueued = true;
	}else{
		//drawable->lastUpdateFrameNumber = frameNumber;

		// Do nothing if still fits the current octant
		const BoundingBox& box = drawable->getWorldBoundingBox();
		Octant* oldOctant = drawable->getOctant();
		if (!oldOctant || oldOctant->m_fittingBox.isInside(box) != BoundingBox::INSIDE){
			m_reinsertQueues[WorkQueue::ThreadIndex()].push_back(drawable);
			drawable->m_reinsertQueued = true;
		}
	}
}

void Octree::removeDrawable(OctreeNode* drawable){
	if (!drawable)
		return;

	removeDrawable(drawable, drawable->getOctant());
	if (drawable->m_reinsertQueued){
		removeDrawableFromQueue(drawable, m_updateQueue);

		// Remove also from threaded queues if was left over before next update
		for (size_t i = 0; i < workQueue->NumThreads(); ++i)
			removeDrawableFromQueue(drawable, m_reinsertQueues[i]);

		drawable->m_reinsertQueued = false;
	}

	drawable->m_octant = nullptr;
}

void Octree::reinsertDrawables(std::vector<OctreeNode*>& drawables){
	for (auto it = drawables.begin(); it != drawables.end(); ++it){
		OctreeNode* drawable = *it;

		const BoundingBox& box = drawable->getWorldBoundingBox();
		Octant* oldOctant = drawable->getOctant();
		Octant* newOctant = &m_root;
		Vector3f boxSize = box.getSize();

		for (;;){
			// If drawable does not fit fully inside root octant, must remain in it
			bool insertHere = (newOctant == &m_root) ?
				(newOctant->m_fittingBox.isInside(box) != BoundingBox::INSIDE || newOctant->fitBoundingBox(box, boxSize)) :
				newOctant->fitBoundingBox(box, boxSize);

			if (insertHere){
				if (newOctant != oldOctant){
					
					// Add first, then remove, because drawable count going to zero deletes the octree branch in question
					addDrawable(drawable, newOctant);
					callRebuild(newOctant);
					if (oldOctant)
						removeDrawable(drawable, oldOctant);
				}
				break;
			}else {
				Vector3f center = box.getCenter();
				newOctant = createChildOctant(newOctant, newOctant->getChildIndex(center));
			}
		}
		drawable->m_reinsertQueued = false;
	}

	drawables.clear();
}

void Octree::callRebuild(Octant* octant) {
	octant->markCullingBoxDirty();
	for (size_t i = 0; i < NUM_OCTANTS; ++i) {
		if (octant->getChild(i)) {
			callRebuild(octant->getChild(i));
		}
	}
}

void Octree::removeDrawableFromQueue(OctreeNode* drawable, std::vector<OctreeNode*>& drawables){
    for (auto it = drawables.begin(); it != drawables.end(); ++it){
        if ((*it) == drawable){
            *it = nullptr;
            break;
        }
    }
}

void Octree::addDrawable(OctreeNode* drawable, Octant* octant) {
	octant->m_octreeNodes.push_back(drawable);
	octant->markCullingBoxDirty();
	octant->updateCullingBox();
	drawable->m_octant = octant;

	if (!octant->m_sortDrawables) {
		octant->m_sortDrawables = true;
		m_sortDirtyOctants.push_back(octant);
	}
}

/// Remove drawable from an octant.
void Octree::removeDrawable(OctreeNode* drawable, Octant* octant)
{
	if (!octant)
		return;

	octant->markCullingBoxDirty();

	// Do not set the drawable's octant pointer to zero, as the drawable may already be added into another octant. Just remove from octant
	for (auto it = octant->m_octreeNodes.begin(); it != octant->m_octreeNodes.end(); ++it)
	{
		if ((*it) == drawable)
		{
			octant->m_octreeNodes.erase(it);

			// Erase empty octants as necessary, but never the root
			while (!octant->m_octreeNodes.size() && !octant->m_numChildren && octant->m_parent)
			{
				Octant* parentOctant = octant->m_parent;
				deleteChildOctant(parentOctant, octant->m_childIndex);
				octant = parentOctant;
			}
			return;
		}
	}
}


Octant* Octree::createChildOctant(Octant* octant, unsigned char index){
    if (octant->m_children[index])
        return octant->m_children[index];

    // Remove the culling extra from the bounding box before splitting
    Vector3f newMin = octant->m_fittingBox.min + octant->m_halfSize;
    Vector3f newMax = octant->m_fittingBox.max - octant->m_halfSize;
    const Vector3f& oldCenter = octant->m_center;

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
	child->Initialize(octant, BoundingBox(newMin, newMax), octant->m_level - 1, index);
	octant->m_children[index] = child;
	++octant->m_numChildren;

	return child;
}

void Octree::deleteChildOctant(Octant* octant, unsigned char index){
	delete octant->m_children[index];
	octant->m_children[index] = nullptr;
	--octant->m_numChildren;
}

void Octree::deleteChildOctants(Octant* octant, bool deletingOctree){
	for (auto it = octant->m_octreeNodes.begin(); it != octant->m_octreeNodes.end(); ++it){
		OctreeNode* drawable = *it;
		drawable->m_octant = nullptr;
		drawable->m_reinsertQueued = false;
		if (deletingOctree)
			drawable->m_octree = nullptr;
	}
	octant->m_octreeNodes.clear();

	if (octant->m_numChildren){
		for (size_t i = 0; i < NUM_OCTANTS; ++i){
			if (octant->m_children[i]){
				deleteChildOctants(octant->m_children[i], deletingOctree);
				delete octant->m_children[i];
				octant->m_children[i] = nullptr;
			}
		}
		octant->m_numChildren = 0;
	}
}

void Octree::collectDrawables(std::vector<OctreeNode*>& result, Octant* octant) const{
	result.insert(result.end(), octant->m_octreeNodes.begin(), octant->m_octreeNodes.end());

	if (octant->m_numChildren){
		for (size_t i = 0; i < NUM_OCTANTS; ++i){
			if (octant->m_children[i])
				collectDrawables(result, octant->m_children[i]);
		}
	}
}

void Octree::checkReinsertWork(Task* task_, unsigned threadIndex_){
	ReinsertDrawablesTask* task = static_cast<ReinsertDrawablesTask*>(task_);
	OctreeNode** start = task->start;
	OctreeNode** end = task->end;
	std::vector<OctreeNode*>& reinsertQueue = m_reinsertQueues[threadIndex_];

	for (; start != end; ++start){
		// If drawable was removed before reinsertion could happen, a null pointer will be in its place
		OctreeNode* drawable = *start;
		if (!drawable)
			continue;

		if (drawable->m_octreeUpdate)
			drawable->OnOctreeUpdate();

		//drawable->setLastFrameNumber(m_frameNumber);

		// Do nothing if still fits the current octant
		const BoundingBox& box = drawable->getWorldBoundingBox();
		Octant* oldOctant = drawable->getOctant();
		if (!oldOctant || oldOctant->m_fittingBox.isInside(box) != BoundingBox::INSIDE)
			reinsertQueue.push_back(drawable);
		else
			drawable->m_reinsertQueued = false;
	}
	m_numPendingReinsertionTasks.fetch_add(-1);
}

void Octree::updateOctree(){
	// Clear results from last frame
	m_rootLevelOctants.clear();

	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i)
		m_octantResults[i].Clear();

	// Process moved / animated objects' octree reinsertions
	update();

	// Check arrived occlusion query results while octree update goes on, then finish octree update
	checkOcclusionQueries();
	finishUpdate();

	// Enable threaded update during geometry / light gathering in case nodes' OnPrepareRender() causes further reinsertion queuing
	setThreadedUpdate(workQueue->NumThreads() > 1);

	// Find the starting points for octree traversal. Include the root if it contains drawables that didn't fit elsewhere
	Octant& rootOctant = m_root;
	if (rootOctant.getOctreeNodes().size()) {
		m_rootLevelOctants.push_back(&rootOctant);
	}

	for (size_t i = 0; i < NUM_OCTANTS; ++i) {
		if (rootOctant.getChild(i))
			m_rootLevelOctants.push_back(rootOctant.getChild(i));
	}

	// Keep track of both batch + octant task progress before main batches can be sorted (batch tasks will add to the counter when queued)
	m_numPendingBatchTasks.store((int)m_rootLevelOctants.size());

	// Find octants in view and their plane masks for node frustum culling. At the same time, find lights and process them
	// When octant collection tasks complete, they queue tasks for collecting batches from those octants.
	for (size_t i = 0; i < m_rootLevelOctants.size(); ++i) {
		m_collectOctantsTasks[i]->startOctant = m_rootLevelOctants[i];
	}

	workQueue->QueueTasks(m_rootLevelOctants.size(), reinterpret_cast<Task**>(&m_collectOctantsTasks[0]));

	// Execute tasks until can sort the main batches. Perform that in the main thread to potentially run faster
	while (m_numPendingBatchTasks.load() > 0)
		workQueue->TryComplete();

	// Finish remaining view preparation tasks (shadowcaster batches, light culling to frustum grid)
	workQueue->Complete();

	// No more threaded reinsertion will take place
	setThreadedUpdate(false);
}

void Octree::collectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask){
	const BoundingBox& octantBox = octant->getCullingBox();

	if (planeMask) {
		// If not already inside all frustum planes, do frustum test and terminate if completely outside
		//planeMask = m_frustum.isInsideMasked(octantBox, planeMask);
		planeMask = m_useCulling ? frustum.isInsideMasked(octantBox, planeMask) : 0x00;

		if (planeMask == 0xff) {
			// If octant becomes frustum culled, reset its visibility for when it comes back to view, including its children
			if (m_useOcclusionCulling && octant->getVisibility() != VIS_OUTSIDE_FRUSTUM)
				octant->setVisibility(VIS_OUTSIDE_FRUSTUM, true);
			return;
		}
	}

	// Process occlusion now before going further
	if (m_useOcclusionCulling) {
		// If was previously outside frustum, reset to visible-unknown
		if (octant->getVisibility() == VIS_OUTSIDE_FRUSTUM)
			octant->setVisibility(VIS_VISIBLE_UNKNOWN, false);

		switch (octant->getVisibility()) {
			// If octant is occluded, issue query if not pending, and do not process further this frame
		case VIS_OCCLUDED:
			addOcclusionQuery(octant, result, planeMask);
			return;

			// If octant was occluded previously, but its parent came into view, issue tests along the hierarchy but do not render on this frame
		case VIS_OCCLUDED_UNKNOWN:
			addOcclusionQuery(octant, result, planeMask);
			if (octant != &m_root && octant->hasChildren()) {
				for (size_t i = 0; i < NUM_OCTANTS; ++i) {
					if (octant->getChild(i))
						collectOctants(octant->getChild(i), result, planeMask);
				}
			}
			return;

			// If octant has unknown visibility, issue query if not pending, but collect child octants and drawables
		case VIS_VISIBLE_UNKNOWN:
			addOcclusionQuery(octant, result, planeMask);
			break;

			// If the octant's parent is already visible too, only test the octant if it is a "leaf octant" with drawables
			// Note: visible octants will also add a time-based staggering to reduce queries
		case VIS_VISIBLE:
			Octant* parent = octant->getParent();
			if (octant->getOctreeNodes().size() > 0 || (parent && parent->getVisibility() != VIS_VISIBLE))
				addOcclusionQuery(octant, result, planeMask);
			break;
		}
	}else {
		// When occlusion not in use, reset all traversed octants to visible-unknown
		octant->setVisibility(VIS_VISIBLE_UNKNOWN, false);
	}

	const std::vector<OctreeNode*>& drawables = octant->getOctreeNodes();
	std::for_each(drawables.begin(), drawables.end(), std::bind(std::mem_fn<void(unsigned short)>(&OctreeNode::setLastFrameNumber), std::placeholders::_1, m_frameNumber));

	for (auto it = drawables.begin(); it != drawables.end(); ++it) {
		OctreeNode* drawable = *it;
		result.octants.push_back(std::make_pair(octant, planeMask));
		result.drawableAcc += drawables.end() - it;
		break;
	}

	// Root octant is handled separately. Otherwise recurse into child octants
	if (octant != &m_root && octant->hasChildren()) {
		for (size_t i = 0; i < NUM_OCTANTS; ++i) {
			if (octant->getChild(i))
				collectOctants(octant->getChild(i), result, planeMask);
		}
	}
}

void Octree::addOcclusionQuery(Octant* octant, ThreadOctantResult& result, unsigned char planeMask) {
	// No-op if previous query still ongoing. Also If the octant intersects the frustum, verify with SAT test that it actually covers some screen area
	// Otherwise the occlusion test will produce a false negative
	if (octant->checkNewOcclusionQuery(m_dt) && (!planeMask || frustum.isInsideSAT(octant->getCullingBox(), frustum.m_frustumSATData))) {
		result.occlusionQueries.push_back(octant);
	}
}

void Octree::checkOcclusionQueries() {
	static std::vector<OcclusionQueryResult> results;
	results.clear();
	checkOcclusionQueryResults(results);

	for (auto it = results.begin(); it != results.end(); ++it) {
		Octant* octant = static_cast<Octant*>(it->object);
		octant->OnOcclusionQueryResult(it->visible);
	}
}

void Octree::renderOcclusionQueries() {
	Matrix4f boxMatrix(Matrix4f::IDENTITY);
	float nearClip = camera.getNear();

	// Use camera's motion since last frame to enlarge the bounding boxes. Use multiplied movement speed to account for latency in query results
	Vector3f cameraPosition = camera.getPosition();
	Vector3f cameraMove = cameraPosition - m_previousCameraPosition;
	Vector3f enlargement = (OCCLUSION_MARGIN + 4.0f * cameraMove.length()) * Vector3f::ONE;

	glDisable(GL_BLEND);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	ShaderOcclusion->use();
	ShaderOcclusion->loadMatrix("u_vp", camera.getPerspectiveMatrix() * camera.getViewMatrix());
	glBindVertexArray(m_vao);
	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i) {
		for (auto it = m_octantResults[i].occlusionQueries.begin(); it != m_octantResults[i].occlusionQueries.end(); ++it) {
			Octant* octant = *it;

			const BoundingBox& octantBox = octant->getCullingBox();
			BoundingBox box(octantBox.min - enlargement, octantBox.max + enlargement);

			// If bounding box could be clipped by near plane, assume visible without performing query
			if (box.distance(cameraPosition) < 2.0f * nearClip) {
				octant->OnOcclusionQueryResult(true);
				continue;
			}

			Vector3f size = box.getHalfSize();
			Vector3f center = box.getCenter();

			boxMatrix[0][0] = size[0];
			boxMatrix[1][1] = size[1];
			boxMatrix[2][2] = size[2];
			boxMatrix[3][0] = center[0];
			boxMatrix[3][1] = center[1];
			boxMatrix[3][2] = center[2];

			ShaderOcclusion->loadMatrix("u_model", boxMatrix);
			unsigned queryId = beginOcclusionQuery(octant);
			
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
			
			endOcclusionQuery();

			// Store query to octant to make sure we don't re-test it until result arrives
			octant->OnOcclusionQuery(queryId);
		}
	}
	glBindVertexArray(0);
	ShaderOcclusion->unuse();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	m_previousCameraPosition = cameraPosition;
}

void Octree::collectOctantsWork(Task* task_, unsigned) {
	CollectOctantsTask* task = static_cast<CollectOctantsTask*>(task_);

	// Go through octants in this task's octree branch
	Octant* octant = task->startOctant;
	ThreadOctantResult& result = m_octantResults[task->resultIdx];

	collectOctants(octant, result);
	m_numPendingBatchTasks.fetch_add(-1);
}

unsigned Octree::beginOcclusionQuery(void* object){
	GLuint queryId;

	if (m_freeQueries.size()){
		queryId = m_freeQueries.back();
		m_freeQueries.pop_back();
	}else
		glGenQueries(1, &queryId);

	glBeginQuery(GL_ANY_SAMPLES_PASSED, queryId);
	PendingQueries.push_back(std::make_pair(queryId, object));

	return queryId;
}

void Octree::endOcclusionQuery(){
	glEndQuery(GL_ANY_SAMPLES_PASSED);
}


void Octree::FreeOcclusionQuery(unsigned queryId){
	if (!queryId)
		return;

	for (auto it = PendingQueries.begin(); it != PendingQueries.end(); ++it){
		if (it->first == queryId){
			PendingQueries.erase(it);
			break;
		}
	}

	glDeleteQueries(1, &queryId);
}

void Octree::checkOcclusionQueryResults(std::vector<OcclusionQueryResult>& result) {
	GLuint available = 0;

	// To save API calls, go through queries in reverse order and assume that if a later query has its result available, then all earlier queries will have too
	for (size_t i = PendingQueries.size() - 1; i < PendingQueries.size(); --i){
		GLuint queryId = PendingQueries[i].first;

		if (!available)
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT_AVAILABLE, &available);

		if (available){
			GLuint passed = 0;
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &passed);

			OcclusionQueryResult newResult;
			newResult.id = queryId;
			newResult.object = PendingQueries[i].second;
			newResult.visible = passed > 0;
			result.push_back(newResult);

			m_freeQueries.push_back(queryId);
			PendingQueries.erase(PendingQueries.begin() + i);
		}
	}
}

void Octree::ThreadOctantResult::Clear() {
	drawableAcc = 0;
	taskOctantIdx = 0;
	batchTaskIdx = 0;
	octants.clear();
	occlusionQueries.clear();
}

void Octree::createCube() {
	float vertices[] = {
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f
	};

	unsigned short indices[] = {
		0, 2, 1,
		2, 3, 1,
		2, 4, 3,
		4, 5, 3,
		4, 6, 5,
		6, 7, 5,
		6, 0, 7,
		0, 1, 7,
		4, 2, 0,
		6, 4, 0,
		1, 3, 5,
		1, 5, 7
	};

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);
}

void Octree::setUseCulling(bool useCulling) {
	m_useCulling = useCulling;
}

void Octree::setUseOcclusionCulling(bool useOcclusionCulling) {
	m_useOcclusionCulling = useOcclusionCulling;
}