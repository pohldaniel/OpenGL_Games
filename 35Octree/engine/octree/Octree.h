// For conditions of distribution and use, see copyright notice in License.txt

#pragma once
#include <atomic>
#include <functional>
#include <engine/scene/OctreeNode.h>
#include <engine/Frustum.h>
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

/// Acceleration structure for rendering.
class Octree{

	friend OctreeNode;

	struct CollectOctantsTask : public MemberFunctionTask<Octree> {
		CollectOctantsTask(Octree* object, MemberWorkFunctionPtr function) :MemberFunctionTask<Octree>(object, function) {}
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
	

    Octree(const Camera& camera, const Frustum& frustum, const float& dt);
    ~Octree();
   
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

    void update();
    void finishUpdate();
    void resize(const BoundingBox& boundingBox, int numLevels);
	void setThreadedUpdate(bool threadedUpdate);
	void queueUpdate(OctreeNode* drawable);
	void removeDrawable(OctreeNode* drawable);	
	template <class T> void findDrawables(std::vector<OctreeNode*>& result, const T& volume, unsigned short drawableFlags, unsigned layerMask = LAYERMASK_ALL) const;
	bool getThreadedUpdate() const;
	Octant* getRoot() const;

	void collectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask = 0x3f);
	void addOcclusionQuery(Octant* octant, ThreadOctantResult& result, unsigned char planeMask);
	void checkOcclusionQueries();
	void renderOcclusionQueries();
	void collectOctantsWork(Task* task, unsigned threadIndex);
	void updateOctree();
	void updateFrameNumber();

	unsigned int beginOcclusionQuery(void* object);
	void endOcclusionQuery();
	void checkOcclusionQueryResults(std::vector<OcclusionQueryResult>& result);
	size_t pendingOcclusionQueries() const;
	
	void setUseCulling(bool useCulling);
	void setUseOcclusionCulling(bool useOcclusionCulling);
	const std::vector<Octant*>& getRootLevelOctants() const;
	const ThreadOctantResult* getOctantResults() const;

	static void FreeOcclusionQuery(unsigned id);
	static std::vector<std::pair<unsigned, void*>> PendingQueries;
	
private:

	void reinsertDrawables(std::vector<OctreeNode*>& drawables);
    void removeDrawableFromQueue(OctreeNode* drawable, std::vector<OctreeNode*>& drawables);
	void addDrawable(OctreeNode* drawable, Octant* octant);
	void removeDrawable(OctreeNode* drawable, Octant* octant);
	Octant* createChildOctant(Octant* octant, unsigned char index);
	void deleteChildOctant(Octant* octant, unsigned char index);
	void deleteChildOctants(Octant* octant, bool deletingOctree);
	void collectDrawables(std::vector<OctreeNode*>& result, Octant* octant) const;
	void checkReinsertWork(Task* task, unsigned threadIndex);
	template <class T> void collectDrawables(std::vector<OctreeNode*>& result, Octant* octant, const T& volume, unsigned short drawableFlags, unsigned layerMask) const;

	void createCube();
	void callRebuild(Octant* octant);

    volatile bool m_threadedUpdate;
	std::vector<OctreeNode*> m_updateQueue;
    std::vector<Octant*> m_sortDirtyOctants;
    BoundingBox m_worldBoundingBox;
    Octant m_root;
	std::vector<Octant*> m_rootLevelOctants;
	ThreadOctantResult* m_octantResults;
	std::vector<ReinsertDrawablesTask*> m_reinsertTasks;
	std::vector<OctreeNode*>* m_reinsertQueues;
    std::atomic<int> m_numPendingReinsertionTasks;
	std::vector<unsigned> m_freeQueries;
	Vector3f m_previousCameraPosition;
	
	std::atomic<int> m_numPendingBatchTasks;
	
	CollectOctantsTask* m_collectOctantsTasks[NUM_OCTANT_TASKS];

	unsigned short m_frameNumber;
	bool m_useCulling;
	bool m_useOcclusionCulling;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	const Frustum& frustum;
	const Camera& camera;
	const float& m_dt;
	WorkQueue* workQueue;

	static const float OCCLUSION_MARGIN;
	static std::unique_ptr<Shader> ShaderOcclusion;

	
};

struct ReinsertDrawablesTask : public MemberFunctionTask<Octree> {
	ReinsertDrawablesTask(Octree* object, MemberWorkFunctionPtr function) : MemberFunctionTask<Octree>(object, function) {
	}

	OctreeNode** start;
	OctreeNode** end;
};

template <class T> void Octree::findDrawables(std::vector<OctreeNode*>& result, const T& volume, unsigned short drawableFlags, unsigned layerMask) const { 
	CollectDrawables(result, const_cast<Octant*>(&root), volume, drawableFlags, layerMask); 
}

template <class T> void Octree::collectDrawables(std::vector<OctreeNode*>& result, Octant* octant, const T& volume, unsigned short drawableFlags, unsigned layerMask) const{
	Intersection res = volume.IsInside(octant->CullingBox());
	if (res == OUTSIDE)
		return;

	if (res == INSIDE)
		CollectDrawables(result, octant, drawableFlags, layerMask);
	else{
		std::vector<OctreeNode*>& drawables = octant->m_octreeNodes;

		for (auto it = drawables.begin(); it != drawables.end(); ++it){
			OctreeNode* drawable = *it;
			if ((drawable->Flags() & drawableFlags) == drawableFlags && (drawable->LayerMask() & layerMask) && volume.isInsideFast(drawable->getWorldBoundingBox()) != OUTSIDE)
				result.push_back(drawable);
		}

		if (octant->numChildren){
			for (size_t i = 0; i < NUM_OCTANTS; ++i){
				if (octant->children[i])
					CollectDrawables(result, octant->children[i], volume, drawableFlags, layerMask);
			}
		}
	}
}