// For conditions of distribution and use, see copyright notice in License.txt

#pragma once
#include <atomic>
#include <functional>
#include <engine/scene/OctreeNode.h>
#include <engine/Frustum.h>
#include "AutoPtr.h"
#include "WorkQueue.h"

#define OCCLUSION_VERTEX "#version 410 core										 \n \
																				 \n \
						  layout(location = 0) in vec3 i_position;				 \n \
                                                                                 \n \
						  uniform mat4 u_vp = mat4(1.0);                         \n \
                          uniform mat4 u_model = mat4(1.0);                      \n \
																		         \n \
						  void main() {                                          \n \
						    gl_Position = u_vp * u_model *vec4(i_position, 1.0); \n \
						  }"


#define OCCLUSION_FRGAMENT	"#version 410 core                   \n \
                                                                 \n \
							 out vec4 color;                     \n \
                                                                 \n \
							 void main() {                       \n \
							   color = vec4(1.0, 1.0, 1.0, 0.5); \n \
							 }"    

static const size_t NUM_OCTANTS = 8;
static const unsigned char OF_DRAWABLES_SORT_DIRTY = 0x1;
static const unsigned char OF_CULLING_BOX_DIRTY = 0x2;
static const float OCCLUSION_QUERY_INTERVAL = 0.133333f; // About 8 frame stagger at 60fps
static unsigned randomSeed = 1;

struct ReinsertDrawablesTask;
/// %Octant occlusion query visibility states.
enum OctantVisibility
{
    VIS_OUTSIDE_FRUSTUM = 0,
    VIS_OCCLUDED,
    VIS_OCCLUDED_UNKNOWN,
    VIS_VISIBLE_UNKNOWN,
    VIS_VISIBLE
};
/// %Octree cell, contains up to 8 child octants.
class Octant
{
    friend class Octree;

public:
    /// Construct with defaults.
    Octant();
    /// Destruct. If has a pending occlusion query, free it.
    ~Octant();

    /// Initialize parent and bounds.
    void Initialize(Octant* parent, const BoundingBox& boundingBox, unsigned char level, unsigned char childIndex);
    /// Add debug geometry to be rendered.
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });
    /// React to occlusion query being rendered for the octant. Store the query ID to know not to re-test until have the result.
    void OnOcclusionQuery(unsigned queryId);
    /// React to occlusion query result. Push changed visibility to parents or children as necessary. If outside frustum, no operation.
    void OnOcclusionQueryResult(bool visible);

    /// Return the culling box. Update as necessary.
    const BoundingBox& CullingBox() const;
    /// Return drawables in this octant.
	const std::vector<OctreeNode*>& Drawables() const { return drawables; }
    /// Return whether has child octants.
    bool HasChildren() const { return numChildren > 0; }
    /// Return child octant by index.
    Octant* Child(size_t index) const { return children[index]; }
    /// Return parent octant.
    Octant* Parent() const { return parent; }
    /// Return child octant index based on position.
    unsigned char ChildIndex(const Vector3f& position) const { unsigned char ret = position[0] < center[0] ? 0 : 1; ret += position[1] < center[1] ? 0 : 2; ret += position[2] < center[2] ? 0 : 4; return ret; }
    /// Return last occlusion visibility status.
    OctantVisibility Visibility() const { return (OctantVisibility)visibility; }
    /// Return whether is pending an occlusion query result.
    bool OcclusionQueryPending() const { return occlusionQueryId != 0; }
    /// Set bit flag. Called internally.
    void SetFlag(unsigned char bit, bool set) const { if (set) flags |= bit; else flags &= ~bit; }
    /// Test bit flag. Called internally.
    bool TestFlag(unsigned char bit) const { return (flags & bit) != 0; }

    /// Test if a drawable should be inserted in this octant or if a smaller child octant should be created.
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
        const Octant* octant = this;

        while (octant && !octant->TestFlag(OF_CULLING_BOX_DIRTY))
        {
            octant->SetFlag(OF_CULLING_BOX_DIRTY, true);
            octant = octant->parent;
        }
    }

    /// Push visibility status to child octants.
    void PushVisibilityToChildren(Octant* octant, OctantVisibility newVisibility)
    {
        for (size_t i = 0; i < NUM_OCTANTS; ++i)
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
    void SetVisibility(OctantVisibility newVisibility, bool pushToChildren = false)
    {
        visibility = newVisibility;

        if (pushToChildren)
            PushVisibilityToChildren(this, newVisibility);
    }

    /// Return true if a new occlusion query should be executed. Use a time interval for already visible octants. Return false if previous query still pending.
    bool CheckNewOcclusionQuery(float frameTime)
    {
        if (visibility != VIS_VISIBLE)
            return occlusionQueryId == 0;

        occlusionQueryTimer += frameTime;

        if (occlusionQueryId != 0)
            return false;

        if (occlusionQueryTimer >= OCCLUSION_QUERY_INTERVAL)
        {
            occlusionQueryTimer = fmodf(occlusionQueryTimer, OCCLUSION_QUERY_INTERVAL);
            return true;
        }
        else
            return false;
    }

	inline static float Random() { return Rand() / 32768.0f; }
	inline static int Rand() {
		randomSeed = randomSeed * 214013 + 2531011;
		return (randomSeed >> 16) & 32767;
	}
	void updateCullingBox() const;
private:
    /// Combined drawable and child octant bounding box. Used for culling tests.
    mutable BoundingBox cullingBox;
    /// Drawables contained in the octant.
	std::vector<OctreeNode*> drawables;
    /// Expanded (loose) bounding box used for fitting drawables within the octant.
    BoundingBox fittingBox;
    /// Bounding box center.
    Vector3f center;
    /// Bounding box half size.
    Vector3f halfSize;
    /// Child octants.
    Octant* children[NUM_OCTANTS];
    /// Parent octant.
    Octant* parent;
    /// Last occlusion query visibility.
    OctantVisibility visibility;
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
class Octree{

	static const size_t NUM_OCTANT_TASKS = 9;

	struct CollectOctantsTask : public MemberFunctionTask<Octree> {
		/// Construct.
		CollectOctantsTask(Octree* object_, MemberWorkFunctionPtr function_) :MemberFunctionTask<Octree>(object_, function_) {
		}

		Octant* startOctant;
		size_t resultIdx;
	};

	struct OcclusionQueryResult {
		unsigned id;
		void* object;
		bool visible;
	};

public:

	struct ThreadOctantResult {
		void Clear();
		size_t drawableAcc;
		size_t taskOctantIdx;
		size_t batchTaskIdx;
		std::vector<std::pair<Octant*, unsigned char>> octants;
		std::vector<Octant*> occlusionQueries;
	};

    /// Construct. The WorkQueue subsystem must have been initialized, as it will be used during update.
    Octree(const Camera& camera, const Frustum& frustum);
    /// Destruct. Delete all child octants and detach the drawables.
    ~Octree();
   
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
	void QueueUpdate(OctreeNode* drawable);
    /// Remove a drawable from the octree.
	void RemoveDrawable(OctreeNode* drawable);
    /// Add debug geometry to be rendered. Visualizes the whole octree.
	//void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

    /// Query for drawables using a volume such as frustum or sphere.
    template <class T> void FindDrawables(std::vector<OctreeNode*>& result, const T& volume, unsigned short drawableFlags, unsigned layerMask = LAYERMASK_ALL) const { CollectDrawables(result, const_cast<Octant*>(&root), volume, drawableFlags, layerMask); }
    /// Return whether threaded update is enabled.
    bool ThreadedUpdate() const { return threadedUpdate; }
    /// Return the root octant.
    Octant* Root() const { return const_cast<Octant*>(&root); }

	void CollectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask = 0x3f);
	void AddOcclusionQuery(Octant* octant, ThreadOctantResult& result, unsigned char planeMask);
	void CheckOcclusionQueries();
	void RenderOcclusionQueries();
	void CollectOctantsWork(Task* task, unsigned threadIndex);

	Vector3f previousCameraPosition;
	std::vector<Octant*> rootLevelOctants;
	std::atomic<int> numPendingBatchTasks;
	AutoArrayPtr<ThreadOctantResult> octantResults;
	AutoPtr<CollectOctantsTask> collectOctantsTasks[NUM_OCTANT_TASKS];

	void updateOctree(float dt);
	void updateFrameNumber();
	unsigned BeginOcclusionQuery(void* object);
	void EndOcclusionQuery();
	void FreeOcclusionQuery(unsigned id);
	void CheckOcclusionQueryResults(std::vector<OcclusionQueryResult>& result);
	size_t PendingOcclusionQueries() const { return pendingQueries.size(); }
	std::vector<std::pair<unsigned, void*> > pendingQueries;
	std::vector<unsigned> freeQueries;

	bool m_useCulling = true;
	bool m_useOcclusion = true;
	unsigned short m_frameNumber;

private:
    /// Process a list of drawables to be reinserted. Clear the list afterward.
	void ReinsertDrawables(std::vector<OctreeNode*>& drawables);
    /// Remove a drawable from a reinsert queue.
    void RemoveDrawableFromQueue(OctreeNode* drawable, std::vector<OctreeNode*>& drawables);
    
    /// Add drawable to a specific octant.
	void AddDrawable(OctreeNode* drawable, Octant* octant)
	{
		octant->drawables.push_back(drawable);
		octant->MarkCullingBoxDirty();
		octant->updateCullingBox();
		drawable->m_octant = octant;

		if (!octant->TestFlag(OF_DRAWABLES_SORT_DIRTY))
		{
			octant->SetFlag(OF_DRAWABLES_SORT_DIRTY, true);
			sortDirtyOctants.push_back(octant);
		}
	}

	/// Remove drawable from an octant.
	void RemoveDrawable(OctreeNode* drawable, Octant* octant)
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
					Octant* parentOctant = octant->parent;
					DeleteChildOctant(parentOctant, octant->childIndex);
					octant = parentOctant;
				}
				return;
			}
		}
	}

	/// Create a new child octant.
	Octant* CreateChildOctant(Octant* octant, unsigned char index);
	/// Delete one child octant.
	void DeleteChildOctant(Octant* octant, unsigned char index);
	/// Delete a child octant hierarchy. If not deleting the octree for good, moves any nodes back to the root octant.
	void DeleteChildOctants(Octant* octant, bool deletingOctree);
	/// Return all drawables from an octant recursively.
	void CollectDrawables(std::vector<OctreeNode*>& result, Octant* octant) const;
	/// Return all drawables matching flags from an octant recursively.
	void CollectDrawables(std::vector<OctreeNode*>& result, Octant* octant, unsigned short drawableFlags, unsigned layerMask) const;
	/// Work function to check reinsertion of nodes.
	void CheckReinsertWork(Task* task, unsigned threadIndex);

	/// Collect nodes matching flags using a volume such as frustum or sphere.
	template <class T> void CollectDrawables(std::vector<OctreeNode*>& result, Octant* octant, const T& volume, unsigned short drawableFlags, unsigned layerMask) const
	{
		Intersection res = volume.IsInside(octant->CullingBox());
		if (res == OUTSIDE)
			return;

		// If this octant is completely inside the volume, can include all contained octants and their nodes without further tests
		if (res == INSIDE)
			CollectDrawables(result, octant, drawableFlags, layerMask);
		else
		{
			std::vector<OctreeNodeNew*>& drawables = octant->drawables;

			for (auto it = drawables.begin(); it != drawables.end(); ++it)
			{
				OctreeNodeNew* drawable = *it;
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
	std::vector<OctreeNode*> updateQueue;
    /// Octants which need to have their drawables sorted.
    std::vector<Octant*> sortDirtyOctants;
    /// Extents of the octree root level box.
    BoundingBox worldBoundingBox;
    /// Root octant.
    Octant root;
    /// Cached %WorkQueue subsystem.
    WorkQueue* workQueue;
    /// Tasks for threaded reinsert execution.
	std::vector<AutoPtr<ReinsertDrawablesTask>> reinsertTasks;
    /// Intermediate reinsert queues for threaded execution.
	AutoArrayPtr<std::vector<OctreeNode*>> reinsertQueues;
    /// Remaining drawable reinsertion tasks.
    std::atomic<int> numPendingReinsertionTasks;

	const Frustum& frustum;
	const Camera& camera;
	float m_dt;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	static const float OCCLUSION_MARGIN;
	static std::unique_ptr<Shader> ShaderOcclusion;

	void createCube();
	void callRebuild(Octant* octant);
};


/// %Task for octree drawables reinsertion.
struct ReinsertDrawablesTask : public MemberFunctionTask<Octree> {
	/// Construct.
	ReinsertDrawablesTask(Octree* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<Octree>(object_, function_) {
	}

	/// Start pointer.
	OctreeNode** start;
	/// End pointer.
	OctreeNode** end;
};