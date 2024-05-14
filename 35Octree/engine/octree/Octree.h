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
static const size_t NUM_OCTANT_TASKS = 9;
static const unsigned LAYERMASK_ALL = 0xffffffff;
static const float OCCLUSION_QUERY_INTERVAL = 0.133333f;

struct ReinsertDrawablesTask;
enum OctantVisibility{
    VIS_OUTSIDE_FRUSTUM = 0,
    VIS_OCCLUDED,
    VIS_OCCLUDED_UNKNOWN,
    VIS_VISIBLE_UNKNOWN,
    VIS_VISIBLE
};

class Octant{
    friend class Octree;

public:

    Octant();
    ~Octant();


    void Initialize(Octant* parent, const BoundingBox& boundingBox, unsigned char level, unsigned char childIndex);
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });
    void OnOcclusionQuery(unsigned queryId);
    void OnOcclusionQueryResult(bool visible);
	bool OcclusionQueryPending() const;

    const BoundingBox& getCullingBox() const;
	const std::vector<OctreeNode*>& getOctreeNodes() const;
	bool hasChildren() const;
	Octant* getChild(size_t index) const;
	Octant* getParent() const;
	unsigned char getChildIndex(const Vector3f& position) const;
	OctantVisibility getVisibility() const;

private:

	bool fitBoundingBox(const BoundingBox& box, const Vector3f& boxSize) const;
	void markCullingBoxDirty() const;
	void pushVisibilityToChildren(Octant* octant, OctantVisibility newVisibility);
	void setVisibility(OctantVisibility newVisibility, bool pushToChildren = false);
	bool checkNewOcclusionQuery(float dt);
	void updateCullingBox() const;

    mutable BoundingBox m_cullingBox;
	std::vector<OctreeNode*> m_octreeNodes;
    BoundingBox m_fittingBox;
    Vector3f m_center;
    Vector3f m_halfSize;
    Octant* m_children[NUM_OCTANTS];
    Octant* m_parent;
    OctantVisibility m_visibility;
    unsigned m_occlusionQueryId;
    float m_occlusionQueryTimer;
    unsigned char m_numChildren;
    unsigned char m_level;
    unsigned char m_childIndex;
	mutable bool m_cullingBoxDirty;
	mutable bool m_sortDrawables;
	bool m_drawDebug;
};

/// Acceleration structure for rendering. Should be created as a child of the scene root.
class Octree{

	friend OctreeNode;

	struct CollectOctantsTask : public MemberFunctionTask<Octree> {
		CollectOctantsTask(Octree* object_, MemberWorkFunctionPtr function_) :MemberFunctionTask<Octree>(object_, function_) {}
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
	template <class T> void FindDrawables(std::vector<OctreeNode*>& result, const T& volume, unsigned short drawableFlags, unsigned layerMask = LAYERMASK_ALL) const;
	// Return whether threaded update is enabled.
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

	void CheckOcclusionQueryResults(std::vector<OcclusionQueryResult>& result);
	size_t PendingOcclusionQueries() const { return PendingQueries.size(); }
	
	std::vector<unsigned> freeQueries;

	void setUseCulling(bool useCulling);
	void setUseOcclusionCulling(bool useOcclusionCulling);


	static std::vector<std::pair<unsigned, void*>> PendingQueries;
	static void FreeOcclusionQuery(unsigned id);

private:
    /// Process a list of drawables to be reinserted. Clear the list afterward.
	void ReinsertDrawables(std::vector<OctreeNode*>& drawables);
    /// Remove a drawable from a reinsert queue.
    void RemoveDrawableFromQueue(OctreeNode* drawable, std::vector<OctreeNode*>& drawables);
    
    /// Add drawable to a specific octant.
	void AddDrawable(OctreeNode* drawable, Octant* octant){
		octant->m_octreeNodes.push_back(drawable);
		octant->markCullingBoxDirty();
		octant->updateCullingBox();
		drawable->m_octant = octant;

		if (!octant->m_sortDrawables){
			octant->m_sortDrawables = true;
			sortDirtyOctants.push_back(octant);
		}
	}

	/// Remove drawable from an octant.
	void RemoveDrawable(OctreeNode* drawable, Octant* octant)
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
					DeleteChildOctant(parentOctant, octant->m_childIndex);
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
	unsigned short m_frameNumber;

	bool m_useCulling;
	bool m_useOcclusionCulling;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	static const float OCCLUSION_MARGIN;
	static std::unique_ptr<Shader> ShaderOcclusion;

	void createCube();
	void callRebuild(Octant* octant);
};

struct ReinsertDrawablesTask : public MemberFunctionTask<Octree> {
	ReinsertDrawablesTask(Octree* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<Octree>(object_, function_) {
	}

	OctreeNode** start;
	OctreeNode** end;
};

template <class T> void Octree::FindDrawables(std::vector<OctreeNode*>& result, const T& volume, unsigned short drawableFlags, unsigned layerMask) const { 
	CollectDrawables(result, const_cast<Octant*>(&root), volume, drawableFlags, layerMask); 
}
