// For conditions of distribution and use, see copyright notice in License.txt

#include <cassert>
#include <algorithm>
#include <engine/DebugRenderer.h>
#include <engine/scene/OctreeNode.h>
#include "Octree.h"

#include "Globals.h"

const float Octree::OCCLUSION_MARGIN = 0.1f;
std::unique_ptr<Shader> Octree::ShaderOcclusion= nullptr;

static const float DEFAULT_OCTREE_SIZE = 1000.0f;
static const int DEFAULT_OCTREE_LEVELS = 8;
static const int MAX_OCTREE_LEVELS = 255;
static const size_t MIN_THREADED_UPDATE = 16;

static inline bool CompareDrawableDistances(const std::pair<OctreeNode*, float>& lhs, const std::pair<OctreeNode*, float>& rhs){
	return lhs.second < rhs.second;
}

static inline bool CompareDrawables(OctreeNode* lhs, OctreeNode* rhs){
	return true;
}

Octant::Octant() :
    parent(nullptr),
    visibility(VIS_VISIBLE_UNKNOWN),
    occlusionQueryId(0),
    occlusionQueryTimer(Random() * OCCLUSION_QUERY_INTERVAL),
    numChildren(0)
{
    for (size_t i = 0; i < NUM_OCTANTS; ++i)
        children[i] = nullptr;
}

Octant::~Octant(){
    if (occlusionQueryId){
        //Graphics* graphics = ObjectTu::Subsystem<Graphics>();
		//if (graphics)
			//graphics->FreeOcclusionQuery(occlusionQueryId);
    }
}

void Octant::Initialize(Octant* parent_, const BoundingBox& boundingBox, unsigned char level_, unsigned char childIndex_){
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

void Octant::OnOcclusionQuery(unsigned queryId)
{
    // Should not have an existing query in flight
    assert(!occlusionQueryId);

    // Mark pending
    occlusionQueryId = queryId;
}

void Octant::OnOcclusionQueryResult(bool visible)
{
    // Mark not pending
    occlusionQueryId = 0;

    // Do not change visibility if currently outside the frustum
    if (visibility == VIS_OUTSIDE_FRUSTUM)
        return;

    OctantVisibility lastVisibility = (OctantVisibility)visibility;
    OctantVisibility newVisibility = visible ? VIS_VISIBLE : VIS_OCCLUDED;

    visibility = newVisibility;

    if (lastVisibility <= VIS_OCCLUDED_UNKNOWN && newVisibility == VIS_VISIBLE)
    {
        // If came into view after being occluded, mark children as still occluded but that should be tested in hierarchy
        if (numChildren)
            PushVisibilityToChildren(this, VIS_OCCLUDED_UNKNOWN);
    }
    else if (newVisibility == VIS_OCCLUDED && lastVisibility != VIS_OCCLUDED && parent && parent->visibility == VIS_VISIBLE)
    {
        // If became occluded, mark parent unknown so it will be tested next
        parent->visibility = VIS_VISIBLE_UNKNOWN;
    }

    // Whenever is visible, push visibility to parents if they are not visible yet
    if (newVisibility == VIS_VISIBLE)
    {
        Octant* octant = parent;

        while (octant && octant->visibility != newVisibility)
        {
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

Octree::Octree(const Camera& camera, const Frustum& frustum) : threadedUpdate(false), workQueue(WorkQueue::Get()), frustum(frustum), camera(camera), m_dt(0.0f), m_frameNumber(0){
    assert(workQueue);

    root.Initialize(nullptr, BoundingBox(-DEFAULT_OCTREE_SIZE, DEFAULT_OCTREE_SIZE), DEFAULT_OCTREE_LEVELS, 0);

    // Have at least 1 task for reinsert processing
	reinsertTasks.push_back(new ReinsertDrawablesTask(this, &Octree::CheckReinsertWork));
	reinsertQueues = new std::vector<OctreeNode*>[workQueue->NumThreads()];

	octantResults = new ThreadOctantResult[NUM_OCTANT_TASKS];

	for (size_t i = 0; i < NUM_OCTANTS + 1; ++i) {
		collectOctantsTasks[i] = new CollectOctantsTask(this, &Octree::CollectOctantsWork);
		collectOctantsTasks[i]->resultIdx = i;
	}

	if (!ShaderOcclusion) {
		ShaderOcclusion = std::unique_ptr<Shader>(new Shader(OCCLUSION_VERTEX, OCCLUSION_FRGAMENT, false));
	}

	createCube();
}

Octree::~Octree(){
    // Clear octree association from nodes that were never inserted
    // Note: the threaded queues cannot have nodes that were never inserted, only nodes that should be moved
	for (auto it = updateQueue.begin(); it != updateQueue.end(); ++it){
		OctreeNode* drawable = *it;
		if (drawable){
			drawable->m_octant = nullptr;
			drawable->m_reinsertQueued = false;
		}
	}

    DeleteChildOctants(&root, true);
}

void Octree::updateFrameNumber() {
	++m_frameNumber;
	if (!m_frameNumber)
		++m_frameNumber;
}

void Octree::Update(){

    // Avoid overhead of threaded update if only a small number of objects to update / reinsert
    if (updateQueue.size()) {
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
		workQueue->QueueTasks(taskIdx, reinterpret_cast<Task**>(&reinsertTasks[0]));
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
        octant->SetFlag(OF_DRAWABLES_SORT_DIRTY, false);
    }

    sortDirtyOctants.clear();
}

void Octree::Resize(const BoundingBox& boundingBox, int numLevels){
    // Collect nodes to the root and delete all child octants
	updateQueue.clear();
	CollectDrawables(updateQueue, &root);
    DeleteChildOctants(&root, false);

    //allocator.Reset();
    root.Initialize(nullptr, boundingBox, (unsigned char)Math::Clamp(numLevels, 1, MAX_OCTREE_LEVELS), 0);
}

void Octree::OnRenderAABB(const Vector4f& color) {
	root.OnRenderAABB(color);
}

void Octree::QueueUpdate(OctreeNode* drawable){
	assert(drawable);

	if (drawable->m_octant) {
		drawable->m_octant->MarkCullingBoxDirty();
	}

	if (!threadedUpdate){
		updateQueue.push_back(drawable);
		drawable->m_reinsertQueued = true;
	}else{
		//drawable->lastUpdateFrameNumber = frameNumber;

		// Do nothing if still fits the current octant
		const BoundingBox& box = drawable->getWorldBoundingBox();
		Octant* oldOctant = drawable->getOctant();
		if (!oldOctant || oldOctant->fittingBox.isInside(box) != BoundingBox::INSIDE){
			reinsertQueues[WorkQueue::ThreadIndex()].push_back(drawable);
			drawable->m_reinsertQueued = true;
		}
	}
}

void Octree::RemoveDrawable(OctreeNode* drawable){
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

void Octree::ReinsertDrawables(std::vector<OctreeNode*>& drawables){
	for (auto it = drawables.begin(); it != drawables.end(); ++it){
		OctreeNode* drawable = *it;

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
			else {
				Vector3f center = box.getCenter();
				newOctant = CreateChildOctant(newOctant, newOctant->ChildIndex(center));
			}
		}
		drawable->m_reinsertQueued = false;
	}

	drawables.clear();
}

void Octree::RemoveDrawableFromQueue(OctreeNode* drawable, std::vector<OctreeNode*>& drawables){
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
		OctreeNode* drawable = *it;
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

void Octree::CollectDrawables(std::vector<OctreeNode*>& result, Octant* octant) const{
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

void Octree::CollectDrawables(std::vector<OctreeNode*>& result, Octant* octant, unsigned short drawableFlags, unsigned layerMask) const{
	std::vector<OctreeNode*>& drawables = octant->drawables;

	for (auto it = drawables.begin(); it != drawables.end(); ++it){
		OctreeNode* drawable = *it;
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

void Octree::CheckReinsertWork(Task* task_, unsigned threadIndex_){
	ReinsertDrawablesTask* task = static_cast<ReinsertDrawablesTask*>(task_);
	OctreeNode** start = task->start;
	OctreeNode** end = task->end;
	std::vector<OctreeNode*>& reinsertQueue = reinsertQueues[threadIndex_];

	for (; start != end; ++start){
		// If drawable was removed before reinsertion could happen, a null pointer will be in its place
		OctreeNode* drawable = *start;
		if (!drawable)
			continue;

		if (drawable->m_octreeUpdate)
			drawable->OnOctreeUpdate();

		//drawable->OnFrameNumberSet(m_frameNumber);

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

void Octree::updateOctree(float dt) {
	m_dt = dt;
	// Clear results from last frame
	rootLevelOctants.clear();

	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i)
		octantResults[i].Clear();

	// Process moved / animated objects' octree reinsertions
	Update();

	// Check arrived occlusion query results while octree update goes on, then finish octree update
	CheckOcclusionQueries();
	FinishUpdate();

	// Enable threaded update during geometry / light gathering in case nodes' OnPrepareRender() causes further reinsertion queuing
	SetThreadedUpdate(workQueue->NumThreads() > 1);

	// Find the starting points for octree traversal. Include the root if it contains drawables that didn't fit elsewhere
	Octant& rootOctant = root;
	if (rootOctant.Drawables().size())
		rootLevelOctants.push_back(&rootOctant);

	for (size_t i = 0; i < NUM_OCTANTS; ++i) {
		if (rootOctant.Child(i))
			rootLevelOctants.push_back(rootOctant.Child(i));
	}

	// Keep track of both batch + octant task progress before main batches can be sorted (batch tasks will add to the counter when queued)
	numPendingBatchTasks.store((int)rootLevelOctants.size());

	// Find octants in view and their plane masks for node frustum culling. At the same time, find lights and process them
	// When octant collection tasks complete, they queue tasks for collecting batches from those octants.
	for (size_t i = 0; i < rootLevelOctants.size(); ++i) {
		collectOctantsTasks[i]->startOctant = rootLevelOctants[i];
	}

	workQueue->QueueTasks(rootLevelOctants.size(), reinterpret_cast<Task**>(&collectOctantsTasks[0]));

	// Execute tasks until can sort the main batches. Perform that in the main thread to potentially run faster
	while (numPendingBatchTasks.load() > 0)
		workQueue->TryComplete();

	// Finish remaining view preparation tasks (shadowcaster batches, light culling to frustum grid)
	workQueue->Complete();

	// No more threaded reinsertion will take place
	SetThreadedUpdate(false);
}

void Octree::CollectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask)
{
	const BoundingBox& octantBox = octant->CullingBox();

	if (planeMask) {
		// If not already inside all frustum planes, do frustum test and terminate if completely outside
		//planeMask = m_frustum.isInsideMasked(octantBox, planeMask);
		planeMask = m_useCulling ? frustum.isInsideMasked(octantBox, planeMask) : 0x00;
		if (planeMask == 0xff) {
			// If octant becomes frustum culled, reset its visibility for when it comes back to view, including its children
			if (m_useOcclusion && octant->Visibility() != VIS_OUTSIDE_FRUSTUM)
				octant->SetVisibility(VIS_OUTSIDE_FRUSTUM, true);
			return;
		}
	}

	// Process occlusion now before going further
	if (m_useOcclusion) {
		// If was previously outside frustum, reset to visible-unknown
		if (octant->Visibility() == VIS_OUTSIDE_FRUSTUM)
			octant->SetVisibility(VIS_VISIBLE_UNKNOWN, false);

		switch (octant->Visibility()) {
			// If octant is occluded, issue query if not pending, and do not process further this frame
		case VIS_OCCLUDED:
			AddOcclusionQuery(octant, result, planeMask);
			return;

			// If octant was occluded previously, but its parent came into view, issue tests along the hierarchy but do not render on this frame
		case VIS_OCCLUDED_UNKNOWN:
			AddOcclusionQuery(octant, result, planeMask);
			if (octant != &root && octant->HasChildren()) {
				for (size_t i = 0; i < NUM_OCTANTS; ++i) {
					if (octant->Child(i))
						CollectOctants(octant->Child(i), result, planeMask);
				}
			}
			return;

			// If octant has unknown visibility, issue query if not pending, but collect child octants and drawables
		case VIS_VISIBLE_UNKNOWN:
			AddOcclusionQuery(octant, result, planeMask);
			break;

			// If the octant's parent is already visible too, only test the octant if it is a "leaf octant" with drawables
			// Note: visible octants will also add a time-based staggering to reduce queries
		case VIS_VISIBLE:
			Octant* parent = octant->Parent();
			if (octant->Drawables().size() > 0 || (parent && parent->Visibility() != VIS_VISIBLE))
				AddOcclusionQuery(octant, result, planeMask);
			break;
		}
	}
	else {
		// When occlusion not in use, reset all traversed octants to visible-unknown
		octant->SetVisibility(VIS_VISIBLE_UNKNOWN, false);
	}

	const std::vector<OctreeNode*>& drawables = octant->Drawables();
	std::for_each(drawables.begin(), drawables.end(), std::bind(std::mem_fn<void(unsigned short)>(&OctreeNode::OnPrepareRender), std::placeholders::_1, m_frameNumber));

	for (auto it = drawables.begin(); it != drawables.end(); ++it) {
		OctreeNode* drawable = *it;
		result.octants.push_back(std::make_pair(octant, planeMask));
		result.drawableAcc += drawables.end() - it;
		//drawable->OnPrepareRender(m_frameNumber);
		break;
	}

	// Root octant is handled separately. Otherwise recurse into child octants
	if (octant != &root && octant->HasChildren()) {
		for (size_t i = 0; i < NUM_OCTANTS; ++i) {
			if (octant->Child(i))
				CollectOctants(octant->Child(i), result, planeMask);
		}
	}
}

void Octree::AddOcclusionQuery(Octant* octant, ThreadOctantResult& result, unsigned char planeMask) {
	// No-op if previous query still ongoing. Also If the octant intersects the frustum, verify with SAT test that it actually covers some screen area
	// Otherwise the occlusion test will produce a false negative
	if (octant->CheckNewOcclusionQuery(m_dt) && (!planeMask || frustum.isInsideSAT(octant->CullingBox(), frustum.m_frustumSATData))) {
		result.occlusionQueries.push_back(octant);
	}
}

void Octree::CheckOcclusionQueries() {
	static std::vector<OcclusionQueryResult> results;
	results.clear();
	CheckOcclusionQueryResults(results);

	for (auto it = results.begin(); it != results.end(); ++it) {
		Octant* octant = static_cast<Octant*>(it->object);
		octant->OnOcclusionQueryResult(it->visible);
	}
}

void Octree::RenderOcclusionQueries() {
	Matrix4f boxMatrix(Matrix4f::IDENTITY);
	float nearClip = camera.getNear();

	// Use camera's motion since last frame to enlarge the bounding boxes. Use multiplied movement speed to account for latency in query results
	Vector3f cameraPosition = camera.getPosition();
	Vector3f cameraMove = cameraPosition - previousCameraPosition;
	Vector3f enlargement = (OCCLUSION_MARGIN + 4.0f * cameraMove.length()) * Vector3f::ONE;

	glDisable(GL_BLEND);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	ShaderOcclusion->use();
	ShaderOcclusion->loadMatrix("u_vp", camera.getPerspectiveMatrix() * camera.getViewMatrix());
	glBindVertexArray(m_vao);
	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i) {
		for (auto it = octantResults[i].occlusionQueries.begin(); it != octantResults[i].occlusionQueries.end(); ++it) {
			Octant* octant = *it;

			const BoundingBox& octantBox = octant->CullingBox();
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
			unsigned queryId = BeginOcclusionQuery(octant);
			
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
			
			EndOcclusionQuery();

			// Store query to octant to make sure we don't re-test it until result arrives
			octant->OnOcclusionQuery(queryId);
		}
	}
	glBindVertexArray(0);
	ShaderOcclusion->unuse();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	previousCameraPosition = cameraPosition;
}

void Octree::CollectOctantsWork(Task* task_, unsigned) {
	CollectOctantsTask* task = static_cast<CollectOctantsTask*>(task_);

	// Go through octants in this task's octree branch
	Octant* octant = task->startOctant;
	ThreadOctantResult& result = octantResults[task->resultIdx];

	CollectOctants(octant, result);
	numPendingBatchTasks.fetch_add(-1);
}

unsigned Octree::BeginOcclusionQuery(void* object)
{
	GLuint queryId;

	if (freeQueries.size())
	{
		queryId = freeQueries.back();
		freeQueries.pop_back();
	}
	else
		glGenQueries(1, &queryId);

	glBeginQuery(GL_ANY_SAMPLES_PASSED, queryId);
	pendingQueries.push_back(std::make_pair(queryId, object));

	return queryId;
}

void Octree::EndOcclusionQuery()
{
	glEndQuery(GL_ANY_SAMPLES_PASSED);
}


void Octree::FreeOcclusionQuery(unsigned queryId)
{
	if (!queryId)
		return;

	for (auto it = pendingQueries.begin(); it != pendingQueries.end(); ++it)
	{
		if (it->first == queryId)
		{
			pendingQueries.erase(it);
			break;
		}
	}

	glDeleteQueries(1, &queryId);
}

void Octree::CheckOcclusionQueryResults(std::vector<OcclusionQueryResult>& result) {
	GLuint available = 0;

	// To save API calls, go through queries in reverse order and assume that if a later query has its result available, then all earlier queries will have too
	for (size_t i = pendingQueries.size() - 1; i < pendingQueries.size(); --i)
	{
		GLuint queryId = pendingQueries[i].first;

		if (!available)
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT_AVAILABLE, &available);

		if (available)
		{
			GLuint passed = 0;
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &passed);

			OcclusionQueryResult newResult;
			newResult.id = queryId;
			newResult.object = pendingQueries[i].second;
			newResult.visible = passed > 0;
			result.push_back(newResult);

			freeQueries.push_back(queryId);
			pendingQueries.erase(pendingQueries.begin() + i);
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